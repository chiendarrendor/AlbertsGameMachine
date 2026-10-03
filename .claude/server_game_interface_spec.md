---
name: server-game-interface-spec
description: Concrete wire-protocol spec for out-of-process game instances (stdio + newline-delimited JSON-RPC-ish) — transport, message catalog, VariCast dropped via player/spectator split; three-phase rollout (process decoupling, then client-facing JSON re-encoding, then per-viewer full-state JSON blob), plus the Python-based mock-game-process test infrastructure this needs
metadata:
  type: project
---

Written 2026-09-19, directly continuing [[server-game-decoupling-investigation]]. That memory's
"How to apply" list left five open items; this spec resolves items 1-3 (transport/framing,
handshake shape, VariCast replacement) with a concrete design. Items 4-5 (process-per-instance,
preserve the global single-threaded invariant) were already resolved there and are treated here as
settled inputs.

**Corrections to that investigation's model, verified directly against current source before
designing on top of it:**
- `ServerGameInfo` (`gamecommon/ServerGameInfo.hpp/cpp`) is per game-*type*, not per instance —
  just `{Name, SaveDir, XMLLoc, XMLFile}`, constructed once by `GameCloset` from `gameconfig.txt`
  and reused (as the one `GameBox` object, which *is* the `ServerGameInfo`) across every room's
  instances of that type. It carries no roster or room identity at all.
- `OutputPort` (the room's `RoomOutputPort`) outlives any one `Game` — constructed once per
  `Room`, and survives `NEWGAME` deleting and replacing the active `Game`.
- `Game` never receives a player roster at construction; it only ever learns player names
  incidentally, from whichever `HandleAction`/`SendFullState` calls happen to arrive
  (`Room::AddPlayerToRoom` in `server/RoomManager.cpp` calls `SendFullState` directly on join).
- `VariCast` has **exactly one real call site in the entire codebase**, and it's generic engine
  code, not game-specific: `gamecommon/DLLGame.hpp:225`, inside `HandleAction`, broadcasting
  `LEGALACTION` to whichever players `Transition::IsLegal` currently allows, once per transition
  out of the new state. Verified by grepping all three real `<Game>Server.xml` files (Outpost,
  MerchantOfVenus, AOR — where hand-written `<body>`/`<auto>`/`<refresh>` snippets live) for
  `Varicast`: zero matches in all three, despite `transitioncompiler` (`ManualEvent.pm`/
  `GlobalEvent.pm`) mechanically generating a `Varicast<Event>` method for every Manual/Global
  event in every game. Cross-checked against the actual generated `.cpp` output
  (`Outpost/tca/OutpostDLL.cpp`, `MerchantOfVenus/tca/MerchantOfVenusDLL.cpp`): every occurrence
  there is a generated definition or one generated wrapper delegating to another, never a call
  from real game logic. Two vestigial `*EventGenerator.hpp` files (`Outpost/`, `AOR/`) also declare
  `Varicast*` methods with no `.cpp` implementation and no includers anywhere — dead output.
- `SendFullState` (`DLLGame.hpp:109-125`) already solves the identical "who can currently do what"
  problem *without* `VariCast` — it computes `GetLegalTransitionNames(player, ...)` directly per
  player and issues plain `Unicast`s. This is the pattern leaned on below.

**Server's residual per-game-type knowledge, once games are fully out-of-process — corrected
2026-09-23 after an earlier overreach in this section (Albert's original 2026-09-19 steer: the
server holds no knowledge of any game *construct*).** The distinction that actually matters, which
an earlier draft of this spec blurred: *game-internal* data — rules, board layout, save format,
anything intrinsic to the game's own logic — legitimately belongs to the game process alone, and
the server should never hold it. But *deployment topology* — where a static client-facing asset is
hosted, how this particular server instance is wired up — is a property of *this deployment*, not
of the game's logic, and stays true regardless of what the client technology eventually becomes
(Albert, 2026-09-23: even after the separately-tracked front-end rewrite replaces the Java client,
*something* will still need deployment-time properties telling it where to fetch that game's
client-side GUI descriptor from — that's inherent to deploying a client asset, not an artifact of
today's Java/XML-plugin mechanism specifically). So the server's residual per-game-type config is
closer to today's `gameconfig.txt` shape than earlier drafts of this spec claimed, and needs only
one real change instead of a redesign:
1. **A launch spec** — how to start/reach an instance. Stays an externally-configured registry,
   `gameconfig.txt`-shaped (settled 2026-09-19 — a self-discovered alternative only made sense as a
   way to avoid a per-game-type config file entirely, and this file has to exist regardless). The
   **only actual format change**: `name,XMLloc,XMLfile,DLLfile` becomes
   `name,XMLloc,XMLfile,launchCommand` — `XMLLoc`/`XMLFile` **stay exactly as they are today**
   (server-owned deployment config, relayed to clients uninterpreted via `NEWGUI`, unchanged from
   today's mechanism), and only `DLLfile` changes shape, from a `.so` path to an executable
   path/launch command. `Name` also stays server-owned, for a reason worth stating plainly: it
   already *is* just this registry key, echoed back out — `ServerGameInfo`'s `Name` field is
   constructed from the same config-line string used to route `NEWGAME`/`LOADGAME` in the first
   place, so there was never a real case for a game process to separately self-report its own name.
2. **A data-directory path** — **derived, not configured, exactly as today**: `GameCloset` computes
   it mechanically as `serverDataRoot/name`, never a listed config column. It does double duty
   exactly as it does today, though: the server needs it to list available saves (`<dir>/save/`) —
   the room-lobby save picker has to work generically, and *before* any game process exists for that
   room, so it can't be answered by asking a live process the way everything else can — but it's
   also the *only* thing a spawned process needs at launch to load whatever private, read-only,
   game-specific data it wants from the rest of that same directory — verified in code:
   `MerchantOfVenusSet`'s constructor does `new MapData(i_DataDir + "/MerchantOfVenusMap.xml")`
   (`MerchantOfVenusSet.cpp:11`), a filename hardcoded inside that one game's own code, under the
   identical `i_DataDir` already passed to `Initialize` today; `OutpostSet(i_DataDir)` takes the
   same argument and does nothing with it, matching Outpost having no shared board data at all.
   Neither the server nor the generic engine ever needs to know a file like
   `MerchantOfVenusMap.xml` exists — it's invisible outside that game's own `Set` code, today and in
   the new design alike. The server just passes the resolved path as a launch argument; the `save/`
   subtree is the part it also independently lists, everything else in it is the game's own private
   business.

Net effect: `ServerGameInfo`/`GameBox` barely change in shape from today at all — an earlier draft
of this spec claimed they'd shrink to just `{Name, SaveDir}`; that was wrong. The only real change
anywhere in this section is `DLLfile` → a launch command, and *how* a `Game` instance gets created
(process-spawn instead of `dlopen`+symbol-call) — not *what static config describes it*.

**Consequence worth naming, not a problem:** today's `pGameSet`/`pStateMachine` are DLL-load-time
globals (see the generated `Initialize()` in `MerchantOfVenus/tca/MerchantOfVenusDLL.cpp` and
`Outpost/tca/OutpostDLL.cpp`), shared across every concurrent instance of that game type within one
server process — one parse of `MerchantOfVenusMap.xml` serves all of them. Process-per-instance
loses that sharing: each spawned process parses its own copy at startup instead. Pure space/time
tradeoff (a presumably-small file reparsed once per instance rather than once per server run), and
the same tradeoff already implicitly accepted when process-per-instance was chosen for crash
isolation over process-per-game-type.

**`NEWGAME` vs. `LOADGAME` stay distinct, mapping onto the same two wire primitives (Albert,
2026-09-19):** a saved game can be loaded either as a fresh instance, or *into an already-running
instance*, replacing its in-progress state — matching today's exact capability
(`Room::HandleLoadGame` calls `m_pGame->Load(filename)` on a live `Game`). Both cases compose the
same two operations differently:
- `NEWGAME`: spawn a fresh instance, no `load` call.
- `LOADGAME`, no live instance: spawn, then send `load`.
- `LOADGAME`, live instance: send `load` directly to the running process, no spawn.

Whether to gate "replace a live game's state via `LOADGAME`" behind extra authority/guardrails is
a room/permission-layer policy question, deliberately left open — not something the wire interface
itself should enforce structurally.

## Wire-level mechanics

**Process model:** one OS process per room's game instance (per
[[server-game-decoupling-investigation]]'s resolution), spawned by the server on
`NEWGAME`/`LOADGAME` and owned by the server for its full lifetime — including staying alive after
`IsDone()` (so players can still see final state), until explicitly replaced by a later
`NEWGAME`/`LOADGAME` or the server itself shuts down.

**Abandoned/disconnected-from games are not a reaping problem for this interim step (Albert,
2026-09-19) — deliberately not designed for, not an oversight.** Today, a player disconnecting is
never even reported to the `Game` object at all (verified: `RoomManager::HandleDisconnect` →
`Room::RemovePlayerFromRoom` only touch server-side membership bookkeeping —
`m_Inhabitants`/`RoomOutputPort`'s `m_BroadcastTargets` — with no call into `m_pGame`); an abandoned
game just sits in memory, indistinguishable from an active one, for the life of the server process.
That's unchanged by decoupling: `event {target:"*"}` delivery is still entirely server-owned and
already skips anyone not currently connected, exactly like `RoomOutputPort::BroadCast` does today,
so no wire traffic to the game process is needed either way. The one thing that's *new* with
process-per-instance is that an abandoned game now costs a live OS process rather than a free
in-memory object — but this isn't a real problem given how the server is actually operated: a
single bounded, invite-only session (start the server, play, shut it down), never a long-running
always-on service strangers wander into. An abandoned process is bounded by that one run, same as
an abandoned object is today. This justification doesn't need to survive the eventual Lambda/
DynamoDB destination either — at that point there's no persistent process to abandon at all (every
action is a stateless invocation against externally-persisted state), so "abandoned game" stops
being a process-lifecycle question and becomes a pure storage-cost one (idle DynamoDB rows), a
different and much cheaper problem that the migration bypasses rather than solves. Confirmed
unchanged either way: a newcomer walking into a room with an abandoned-but-still-running game must
see its current state exactly like joining an active one — already covered by `sendFullState`
below, which has no notion of "abandoned," it just always catches up whoever arrives.

**Transport: stdio pipes, not a socket.** The server spawns the game executable as a child process
and communicates over its inherited stdin/stdout. Rationale: the server already owns the child's
full lifecycle — parent-owns-child is exactly what stdio inheritance is for — every candidate
future language reads/writes stdio trivially, and it avoids port allocation/bind races entirely.
Two lifecycle signals fall out for free: EOF on the pipe (paired with the OS process handle) is the
crash/exit signal; closing the child's stdin is the shutdown signal. **Hard constraint this
imposes:** stdout is reserved 100% for protocol traffic — any hand-written game code doing a stray
`std::cout`/`printf` (some already exists, e.g. `GameCloset.cpp`'s startup logging) would corrupt
framing. `GameServerMain` must redirect logging to stderr and treat stdout as sacred.

**Message format: JSON-RPC-ish, newline-delimited.** Not full JSON-RPC 2.0, not gRPC (assumes
HTTP/2 — too much tooling for one local child process), not an extension of today's comma +
`UnComma`/`ReComma` escaping scheme (every future non-C++ port would have to reimplement that from
scratch; JSON needs zero custom parsing code in any language). Framing is one JSON object per
line — safe because JSON already escapes embedded newlines inside string values, so no
`Content-Length`-prefix bookkeeping (LSP-style) is needed.

Three message shapes, matching the two traffic types the investigation already identified (calls
that block for a reply vs. fire-and-forget events):
- **Request** (server→game, expects a reply): `{"id":<n>,"method":"<name>","params":{...}}`
- **Response** (game→server, answers a specific request): `{"id":<n>,"result":{...}}` on success, or
  `{"id":<n>,"error":"..."}` instead of `result` if the request failed (see "ERROR is a special
  case" below for when this applies and why it's modeled this way, not as a notification).
- **Notification** (either direction, no reply expected): `{"method":"<name>","params":{...}}`

**Action-complete boundary:** a `handleAction` request may cause the game to emit any number of
`event` notifications while it's being processed (mirroring today's `UniCast`/`BroadCast` calls
happening inline inside `HandleAction`'s call stack) — the matching `{"id":<n>,"result":{}}`
response is the explicit "done" signal the investigation flagged as missing from today's in-process
model. The server must not consider the action complete, or send another request to that process,
until that response arrives — this is what preserves the single-threaded/single-writer invariant
already established as load-bearing.

**Message catalog:**

*Server → Game, requests (blocking round trip):*
- `handleAction {player, action}` → `{}` on success (any resulting `event` notifications for this
  call, per the full-state design below, precede this response), or a native RPC error instead of a
  result (`error:"..."`) — see "ERROR is a special case" below: an error means *zero* state change
  and *zero* FSM transition, so it goes back only to the connection that made this exact request,
  with no preceding `event` traffic of any kind. No roster param, no `registeredPlayer` field — see
  "VariCast is dropped" and "Full per-viewer state" below for why neither is needed.
- `sendFullState {player}` → `{}`, with the actual state delivered as one `event` notification
  beforehand, **always addressed to `target: <player>` specifically — even when that name isn't
  (yet) a recognized player and its content is therefore computed via the collective `SPECTATOR`
  view** (Albert, 2026-09-22: "all records should be effectively unicast to the requesting player
  even if they would normally be targeted to `SPECTATOR`"). This is the one place "who the content
  was computed for" and "who it's addressed to on the wire" genuinely differ — everywhere else in
  this spec they're the same name. See "Full per-viewer state" below for what the notification
  carries, and how its shape changes across the two migration stages. Wire equivalent of today's
  `Game::SendFullState(name)` — called once per name when a player joins a room with an active game
  (`Room::AddPlayerToRoom`), and once per current inhabitant when refreshing everyone at once
  (`Room::BroadcastFullState`, e.g. immediately after `NEWGAME` creates a fresh instance). This is
  what gets a `JOINGAME`-style action in front of every room occupant the moment a game starts —
  self-contained per name, so the game needs zero prior knowledge of anyone to answer it.
- `load {filename}` → `{success: bool}`
- `save {filename}` → `{success: bool}`
- `getStatusString {}` → `{status: string}`
- `isDone {}` → `{done: bool}`

*Game → Server, notifications:*
- `event {target, message}` — `target` is a player name (replaces `UniCast`), `"*"`/omitted for
  literally everyone, players and spectators alike (replaces `BroadCast`), or the reserved sentinel
  **`SPECTATOR`** — every current room occupant who isn't a recognized player, delivered via the
  same stateless per-batch inference rule described under "Full per-viewer state" below (no roster,
  no persistent tracking). `SPECTATOR` is a **phase-3-only** value (see "Full per-viewer state"'s
  "Staging" note, revised 2026-09-24 to three phases) — phases 1-2 only ever use a specific name or
  `"*"`. **`SPECTATOR` must be a login name no real player can ever hold** — see `TODO.md` for the
  `LoginManager`-side guard this requires; the reserved string alone isn't a real guarantee, just a
  reduction in accidental-collision risk. **`message`'s payload format tracks which phase is live,
  not this spec's own framing** — in phase 1 it's still today's `UnComma`-escaped comma line
  (the game process builds it exactly like today's `MakeXXXMessage` functions do, and the server
  relays it to the still-unmodified Java client verbatim); from phase 2 onward, once the
  client↔server wire itself has moved to JSON (see "Full per-viewer state"'s "Staging" note), it
  becomes a JSON value instead, still just relayed verbatim by the server either way.

**No handshake message at all, corrected 2026-09-23 — a `describe {name, xmlLoc, xmlFile}`
notification was here in an earlier draft, dropped entirely.** It existed to let the game process
self-report its name and client-GUI-descriptor location; per "Server's residual per-game-type
knowledge" above, none of those three fields ever needed self-reporting — the server already owns
all of them as deployment config, exactly as it does today via `gameconfig.txt`/`ServerGameInfo`.
With nothing left for a handshake to carry, there's no reason for one: a spawned process is simply
ready to receive `handleAction`/`sendFullState`/etc. requests immediately, with no "wait for an
initial message" step. This also resolves the spawn/handshake-sequence question this spec left open
all session (see "How to apply" below) — there wasn't a real handshake needed at all.

**Confirmed invariant (Albert, 2026-09-19): the game process never needs to target-message a name
it hasn't already been introduced to, and there's no scenario requiring it.** A name only ever
becomes addressable to the game because it showed up as `handleAction`'s `player` field or a
`sendFullState`'s `player` field — never invented or looked up independently. This is why no
"introduce this spectator" step is needed anywhere in this protocol: the moment a room occupant
takes their first action (typically *how* they join a game, e.g. a `JOINGAME`-style transition) or
gets `sendFullState`'d, the game has everything it needs to `event {target: <name>}` them from then
on. Before that point, the only traffic that can reach them is a broadcast-shaped `event` (fanned
out by the server to every live connection in the room, without the game needing to know who they
are), the `SPECTATOR`-targeted `event` (phase 3 only — same idea, still no names involved, see
"Full per-viewer state" below), or a `sendFullState` response (self-contained per name, no prior
introduction needed either).

**Launch-time config is argv, not a wire message** (fully known before spawn, so no handshake
message is needed for it): the server-assigned per-game-type data-directory path (see above — the
same directory the server lists `<dir>/save/` from, doubling as wherever the process wants to load
its own private read-only data, like MoV's map file, from). Loading a save — whether at startup or
mid-session — always goes over the wire as a `load` request rather than a spawn-time parameter; one
mechanism for both cases instead of two.

**Amended 2026-09-27 (found live, during Albert's first real end-to-end GUI test of Phase 1B):
argv also carries Name/XMLLoc/XMLFile, appended after DataDir — `execlp(launchCommand, DataDir,
Name, XMLLoc, XMLFile)`.** Root cause of what this fixes: `RESET`'s generated wire payload embeds
`ServerGameInfo::GetName()` directly (`MakeRESETMessage()` in every `<Game>DLL.cpp`) — with
`GameServerMain.hpp` constructing an empty-Name `ServerGameInfo` (per this section's original,
too-strong reading of "the game process never needs to self-report its name"), this produced a
literal `RESET,` with nothing after the comma. **Important distinction this surfaced (Albert,
2026-09-27), easy to conflate and worth keeping straight:** the C++-source-code identity a game's
`GAME_SERVER_NAME()` macro carries (which types/files this executable was compiled from) and the
front-end/GUI identity `gameconfig.txt`'s Name column carries (what the client sees via
`NEWGUI`/`GAMES`) are deliberately allowed to be different strings — they happen to be identical
for Outpost/MoV today, but a fix that derived RESET's Name from `GAME_SERVER_NAME()` instead of
the real front-end name would be building on that coincidence, not the actual design. Since the
front-end Name is exactly as fully-known-before-spawn as DataDir already was (same `GameBox`/
`ServerGameInfo` object, same moment), argv is the right mechanism for it too — not a new
handshake message, which would reopen the "no handshake needed" decision above rather than extend
what's already there. XMLLoc/XMLFile came along for the same reason as Name: no known game-side
consumer exists for them (they're resolved entirely at the `Room` level, server-side), but passing
them anyway makes the game-side `ServerGameInfo` a faithful reconstruction of the one object
server and game used to share before decoupling split it into two independently-constructed
copies, rather than a partial one built only from whatever's been discovered necessary so far.

## VariCast is dropped from the interface entirely, via a player/spectator identity split — resolved

**Two epiphanies from Albert (2026-09-19/2026-09-22) that fully resolve what was previously an open
problem here, in two successive passes — both superseded by the final formulation below, kept here
because each pass explains why the next one's simplification is actually safe.**

*Pass 1 (superseded):* the server hands the game an inline `roster` param so it can loop over real
room-membership names and evaluate legality per name, replicating `VariCast`. Unnecessary — the game
never needs external roster data at all, because of the identity model below.

*Pass 2 (superseded):* introduce a `"spectator"` wire sentinel plus a `registeredPlayer` field on
`handleAction`'s response, so the server can fan out spectator-only deliveries while excluding known
players. Also unnecessary — see why below.

**The identity model both passes were working toward: there are only ever two kinds of people who
send the game process anything.** "Players" — names the game itself has already recorded, by having
them successfully execute some transition. "Spectators" — everyone else, by definition unknown to
the game, and, also by definition, treated *identically* to each other: same `LEGALACTION` set if
any, same public state events, never any player-secret state. The only way to cross from spectator
to player is a new class of transition — **`open`** — legal for anyone regardless of identity, not
just existing players. `JOINGAME`/`JOIN` is the obvious/universal example (Albert: every completed
game has exactly one), but `open` transitions aren't restricted to game-start — Albert flagged a
plausible mid-game case (a game-show-style "bring in a volunteer from the audience" mechanic), so
the model has to support an `open` transition firing at any point, not just from `InitialState`.

**The final simplification (Albert, 2026-09-22): recast `UNICAST`/`BROADCAST`/`VARICAST` themselves,
at the point where game code decides how to send something, as three answers to one question — "is
this going to one particular player, or to everyone regardless of identity?"** `BROADCAST` = the
default, everyone, players and spectators alike, unchanged. `UNICAST` = one specific player.
`VARICAST` = one or more players — **never spectators, by definition of what `VARICAST` means here.**
Then wire the `LEGALACTION` generation itself through exactly these three, with no new concept:
an `open` transition's availability is sent `BROADCAST` (harmless to also tell existing players —
their client just won't act on it, and it costs nothing to skip tracking who's exempt); every other
transition's availability is sent `VARICAST`, restricted to the game's own known players. **This
makes `VARICAST` provably nothing more than syntactic sugar for a loop over the game's own player
list emitting individual `UNICAST`s** — provable, not just true of the one real historical use case,
because `VARICAST`'s audience is defined to always be a subset of players, and the game always has
its own player list in hand (it has to, to implement any per-player logic at all — whose turn it is,
private hands, etc.). That's what makes Pass 2's machinery unnecessary: there's no case, even
hypothetically, where a `VARICAST`-shaped send needs a spectator's name or the server's help
excluding one, because spectators were never a valid `VARICAST` target to begin with, and the
"everyone including players" case is already just `BROADCAST`. So the wire protocol needs nothing
beyond what it already had before Pass 1 — a specific player name, or everyone — and `VARICAST`
never needs to exist as a wire concept at all, only as an authoring-time convenience on the game
side that expands to a loop of `UNICAST`s before anything crosses the wire.

**Engine-side prerequisite this design depends on, not itself a wire-protocol concern (tracked as a
`TODO.md` item):** `transitioncompiler`'s XML schema needs an `open` attribute on `<transition>`,
and `stateengine`/`DLLGame::HandleAction`-equivalent logic needs to route each transition's
`LEGALACTION` through `BROADCAST` (if `open`) or a per-known-player loop of `UNICAST`s (otherwise) —
which also means `OutputPort::VariCast`/`NameBoolean` can be retired entirely, not just left unused,
even in today's in-process code, not only the decoupled design. None of this is new *wire* surface
at all — it's game/engine-side work this interface design assumes will exist, and it's strictly
less wire surface than either superseded pass above, not more.

**Revisited (Albert, 2026-09-22): the `BROADCAST`-for-`open`-transitions approach two paragraphs up
is cosmetically inaccurate, not dangerous — and that's an acceptable, deliberately-scoped-to-phase-1
(and phase 2) tradeoff, not a bug to design around yet.** Broadcasting an `open` transition's
availability to *everyone*, players included, does mean an already-joined player's client can be told
`JOIN` is still "legal" for them, which isn't true — `JOIN`'s own legality logic denies it to existing
players. But that's only a display inaccuracy, not a correctness hazard, **as long as the
`JOIN`-style transition itself defensively handles being attempted by an existing player** — either
rejecting it outright (an ordinary `ERROR`) or treating it as a harmless no-op/idempotent
re-confirmation. Either approach means no real state can ever be corrupted by this, so phases 1-2
keep plain `BROADCAST` for `open`-transition `LEGALACTION`s (no `SPECTATOR`-targeted routing needed
yet — see "Full per-viewer state"'s "Staging" note). The per-viewer full-state design below (phase 3)
fully fixes the *cosmetic* part too
(each player's own individually-computed state correctly omits `JOIN` once they're a known player,
so their client stops being told it's available at all) — at which point the defensive guard on
`JOIN` becomes dead code, safe to leave in place rather than needing removal.

**Game-process crash mid-round-trip — resolved (Albert, 2026-09-19), no protocol change needed.**
Today, if the process holding a game's active state dies, that state is lost outright (has happened
to Albert before; mitigated historically by just saving more often by hand). Decoupling doesn't
need to solve this at the wire-protocol level at all — every primitive required already exists in
this spec: a `save` request issued after every successful `handleAction` (autosave), and on an
unexpected child exit (the same pipe-EOF/process-exit signal already named under "Transport: stdio
pipes"), the server simply spawns a fresh instance and issues `load` against the last autosave,
exactly like any other spawn-then-load. This is a server-side feature to build (tracked in
`TODO.md`), not an interface concern — nothing here needs to change to support it.

## Full per-viewer state replaces individual Events — the bigger, separately-staged redesign

**This is a second, larger idea (Albert, 2026-09-22) that supersedes TODO.md's "dirty-tracking"
rearchitecture item with something much simpler, and it fully subsumes `LEGALACTION` generation
from the section above.** It's deliberately staged as a *later* migration on top of everything
above, not something the process-decoupling work has to deliver in one step — see "Staging" below.

**Two distinct concepts of "full state," where today there's only one.** (1) **Internal state**
(`<Game>State`) — exists purely in game code, can be whatever the developer wants, with the sole
constraint that it's serializable for `Load`/`Save`. Unchanged from today. (2) **Client-destined
state** — a separately-computed representation, "more or less the data currently embodied in all
the `Event`s sent out during the game" (Albert), but unified into one JSON value per viewer instead
of scattered across many hand-authored typed messages.

**The mechanism: one function per game, `GetClientState(playerName or SPECTATOR) → JSON`,** written
by the game developer, replacing both `<refresh>` and every ad hoc `Unicast`/`Broadcast`/`Varicast`
call scattered across every transition body. This is why it's a bigger win than it first sounds:
today, "what should this viewer learn" logic is scattered across every transition *and* separately,
manually kept in sync in `<refresh>`; the new design has exactly one place per game that answers
"what does this viewer currently know," called generically after every successful transition (and
by `sendFullState`, unifying that call with this same function too — Albert: "this is going to look
quite a bit like `<refresh>`").

**No separate "secrecy" concept is needed, and this is *why* the earlier secrecy-marker idea (an
Albert-originated approach, later abandoned by him before this reached me) got complicated and was
dropped.** Because the function is inherently per-viewer, hiding something is just *not writing it
into that viewer's JSON* — which also naturally handles cases a field-level secrecy marker can't:
Outpost's hidden hand isn't "elide the card," it's "emit a card-*back* descriptor instead of a
card-*front* one" — a substitution, not a redaction. Similarly, some internal state (draw-deck
order) is private to *everyone*, player and spectator alike, and some (MoV's full `Token` object vs.
the bare name/identifier sent for a token in a player's holds) is simply more detailed than any
client needs, secret or not — both fall out naturally as "the developer's function just doesn't
include that," with no separate mechanism required. A game with no hidden state at all (Chess) and
an internal representation that's already client-shaped could implement this function as "return a
copy of internal state" — but Albert is explicit that MoV and Outpost already made a real, necessary
internal→client transformation choice, so for them this function does real work, same as `<refresh>`
already does today.

**`SPECTATOR` is strictly the collective spectator state — never additionally sent to a named
player (Albert, 2026-09-22, correcting an ambiguity in this spec's own earlier phrasing, which used
the word "default" here — renamed below, see "Naming" note).** Every current player always gets
their own individually-computed state; `SPECTATOR` reaches only room occupants the game doesn't
(yet) recognize as players. This is what fully (not just cosmetically) fixes the `JOIN`-visibility
issue raised above: since each player's own state is computed separately, it correctly omits `JOIN`
once they're a player, while `SPECTATOR` correctly still shows it as available to whoever hasn't
joined yet — but this is a phase 3 property specifically (see "Staging" below), not something
phases 1-2 get for free.

**Naming (Albert, 2026-09-22): the sentinel needs a name no real login could ever collide with, not
the plain word "default"** — `LoginManager`'s flat-file store auto-creates an account for *any* name
on first login attempt, so nothing today would stop a real player from logging in as literally
`default`. This spec uses **`SPECTATOR`** instead (all-caps, matching this codebase's existing
convention for reserved protocol tokens like the implicit `RESET` event), but the name alone is only
a reduction in accidental-collision risk, not a real guarantee — `LoginManager` needs an explicit
check rejecting `SPECTATOR` as a login name (tracked in `TODO.md`).

**Player/spectator routing needs a "player register" — but it's entirely game-side, and the server
stays fully ignorant of who's a player (Albert, 2026-09-22).** The engine-generic post-transition
step needs to iterate "every currently known player" (to call `GetClientState` once per player) —
that's a small, game/engine-internal roster, not a wire concern, and not the same thing as the
`roster` param two sections above already rejected. **The server-side routing rule this produces is
purely mechanical and entirely stateless — no `registeredPlayer` field, no persistent tracking
carried between calls:** for every person currently connected in the room, if this response's batch
of `event` notifications included one individually addressed to them by name, deliver that; otherwise
deliver the `SPECTATOR` one. Since a fresh, individually-addressed `event` is emitted for *every
current player on every successful transition* (not just the rare join moment), this is always
correct and always freshly computed from that one response's traffic — nothing to keep in sync, and
nothing left over from Pass 2 above needs reviving. (`sendFullState`'s response is the one exception
to this routing rule — see its catalog entry above: it's always addressed to the specific asker by
name, never to `SPECTATOR`, even when its content was computed via the `SPECTATOR` view.)

**Idea for phase 3, recorded but not yet designed in (Albert, 2026-09-25): retire `GUIIAM`/`IAM` by
having the server silently stamp `"Name": "<name>"` onto every packet it delivers to a connection.**
Today there are two separate "remind the GUI who it is" mechanisms, for two separate reasons: the
room/lobby layer sends `GUIIAM,<name>` once at login because it has no structural link to
game-handling code at all; a game itself sends `IAM,<index>` once you join because it tracks players
by small integer index, not name. Both exist only because nothing downstream of login carries
identity forward automatically. Once messages are structured JSON (phase 2+) rather than opaque
comma lines, the server can fix this generically instead of per-event-type: at the point it writes
to one specific socket — which happens identically whether the payload is a normal state update, an
`ERROR`, or one copy of a `SPECTATOR` fan-out to several sockets — it already knows which login name
owns that connection, so it can add a `"Name"` field unconditionally, with zero game-side involvement
and no per-message-type special-casing. **Bonus possibility, not decided:** if phase 3's per-viewer
state ends up keyed by player name rather than by a game's internal integer index (natural in JSON,
awkward in flat comma-lines — probably why games use indices today), knowing your own `"Name"` might
be enough to find yourself in that structure directly, retiring the game-specific numeric `IAM` too,
not just `GUIIAM`. Left for whoever designs phase 3's exact JSON shape to decide.

**Client-side consumption design for the per-viewer blob — resolved 2026-09-27, after reading the
real Java client together (see [[component-java-client]] for the wire-dispatch/widget architecture
this builds on).** Initial worry going in: today's widgets (`ActionNode`, `CommodityHand`,
`ShipList`, `BidItemWatcher`, `StateSensitiveTextBox`, etc.) are all built around reacting to a
*stream* of small, individually-typed deltas, so swapping in one big per-transition JSON blob looked
like it might force rewriting every widget's internal model, not just the wire format. **Albert's
correction: overstated.** The existing GUI already has to handle "apply the complete current state
all at once" as a normal, exercised code path — `sendFullState`, fired whenever a player (re)joins a
room with a live game — and it already produces correct output doing that. Widgets aren't purely
differential; they're already built to be fully re-appliable from a clean slate. That shrinks the
real client-side task for Phase 3 down to: build the layer that unpacks `GetClientState`'s JSON into
the same *kind* of per-widget calls the full-refresh path already produces correctly today — not a
rewrite of every widget's own logic.

**Chosen design (Albert, 2026-09-27): minimize blast radius by changing what gets read, not how the
walk works.** `GameNode`'s existing recursive tree-walk (self via `MyHandleEvent`, then every child,
same order, same `<eventhandler>` show/hide/pass/block machinery) stays structurally unchanged. The
only thing that changes is what's threaded through it: today, one `GameEvent` object representing a
single wire message; under Phase 3, the current whole-state JSON blob. Each widget's own logic moves
from "is this the event named X I'm watching for, and if so what are its vars" to "read field X
directly out of the JSON I was just handed." **Deliberately no attempt to manufacture a synthetic
legacy-event-ordering/dispatch layer on top of the JSON** — walk the tree once per refresh, same
order as today, let each node pull what it needs. Exceptions are handled per-widget as they're
actually discovered to need one, not designed for upfront.

`LEGALACTION` is the concrete resolved example, and it was a special case on the server side too
(see "VariCast is dropped" above): today's `ActionNode` hides on every `NEWSTATE` and re-shows only
when a matching `LEGALACTION` arrives, purely because there's never been a "no longer legal" event to
hide it symmetrically. Under Phase 3 this collapses to a plain membership check against an explicit
current-legal-actions list field in the incoming state — present in the list, legal; absent, not —
with nothing to reset between refreshes. (This is also what fully resolves `ActionNode`'s
`STATEDALWAYSHIDDEN`-vs-`ALWAYSHIDDEN` distinction, which only exists today because legality has to
be explicitly re-armed to `false` on `NEWSTATE` for some `VisibilityType`s but not others — once
legality is a fresh membership check every refresh, there's nothing left to re-arm.)

**The `<eventhandler>` show/hide/pass/block DSL survives this shape change almost for free**, and
turns out to be low-blast-radius anyway. Structurally: it already just matches attribute name/value
pairs against "whatever the currently-dispatched thing's vars are" — swapping the data source from
an event's declared vars to the whole-state JSON's fields is the same kind of change as everywhere
else, no DSL redesign needed. Empirically: real usage turns out to be light. Grepped both real games'
actual client XML — MoV: zero uses of `<eventhandler>`/`showif`/`hideif` at all (consistent with MoV
being almost entirely one hand-written `javaclass`, see [[component-java-client]]); Outpost: a modest
11 total across `actions.xml` (4), `Players.xml` (1), and `Options.xml` (6), concentrated in a few
specific spots, not pervasive. Matches Albert's own account: an early design idea (static images
turned on/off purely by events passing by) that turned out "deeply clunky" in practice, and one he
doesn't think he leaned on much in either real game once hand-written `UserDefinedInterface` widgets
became the norm.

**One specific semantic risk flagged, deliberately not designed around now (per the same "don't
solve until it's a real problem" philosophy as the widget-flicker note below) — a candidate
exception to watch for, not a known bug. Corrected after actually reading `GameEventHandler.java`
(the "back of mind" version of this note, above, had the mechanism wrong even though the practical
conclusion mostly still holds):** `GameEventHandler.ParseEvent` runs on **every single event that
reaches a node**, not just ones relevant to it, and resets its show/hide/block flags at the top of
every call — there's no "skip, this event isn't mine" short-circuit anywhere. Each `HandlerUnit.
Matches` treats a var that's simply absent from the current event's vars as a non-match (`evval ==
null → return false`), same as a var that's present with the wrong value — no distinction between
"this event doesn't carry an opinion" and "this event actively disagrees." For one-directional units
(`showif`/`hideif`/`showifnot`/`hideifnot`), a non-match produces no verdict at all, and
`GameNode.HandleEvent`'s `if (IsVisible()) ... else if (IsInvisible()) ...` then does nothing,
leaving whatever visibility was last set standing — so the *practical effect* really is "irrelevant
event → visibility untouched," it's just achieved by "re-evaluate to no-verdict every time," not by
skipping evaluation. **But `showhide`/`hideshow` (the two-directional units) have a real `else`
branch that fires on any non-match** — meaning, already true today, not a future risk: a `showhide`
matched on some var that a given event simply doesn't declare (e.g. matching on a game-specific var
against a `NEWSTATE` event that doesn't carry it) actively flips to the opposite visibility on
*every* such event, not just ones that share that var name at all. If the new whole-state JSON is a
genuinely complete snapshot every refresh (every declared field always present, never absent), that
specific "absent counts as a real non-match" behavior for `showhide`/`hideshow` types would still
technically evaluate every walk, but the field would now virtually always be *present* going forward
— collapsing "genuinely absent" and "present but not matching" into one case going forward, which is
only actually a behavior change for the small number of `showhide`/`hideshow` units, if any, that
today rely on a var being *entirely absent* from most event types rather than present-with-a-specific
value. Given how lightly this DSL is used at all (above), and how few of its uses (if any) are the
two-directional forms in the first place, this is a small, contained thing to check for specifically
if/when eventhandler-based widgets are actually ported — not something to design a general solution
for now.

**Widget-level "memory" to avoid flicker across refreshes — flagged (Albert, 2026-09-27), explicitly
not designed for pre-emptively.** Some widget collections may need to remember something across a
full-state reapplication that isn't itself part of the state (to avoid visibly flickering off/on
where the old incremental model never would have). Treat this as something to watch for empirically
once real Phase 3 client work is underway, not a problem to solve in the abstract — only act on it if
it turns out to be a systemic issue, not per-widget as a matter of course.

**Layered JSON composition — the actual shape of `GetClientState`'s output, resolved 2026-09-27
(Albert: "I don't know if we stated this, but I was always envisioning it").** Each architectural
layer produces its own JSON object and includes the *next inner layer's* JSON as one field of its
own — not one function flattening everything into a single merged blob. Innermost to outermost:
1. The game developer's `GetClientState`-equivalent function (the `<refresh>` analogue) — game-
   specific content only. Deliberately does **not** include the current FSM state name/description
   or the legal-action list — neither is the game's own concern.
2. The generic statewalker/FSM layer — wraps the game's JSON as a field of its own, and is the one
   that adds current-state and the legal-action list, since it already knows both generically with
   zero game-specific knowledge (this is the layer `LEGALACTION`'s list, above, actually comes from).
3. Possibly an intermediate generic-game-code layer — floated as a maybe, not yet decided; left open.
4. The server's room layer, wrapping again.
5. The outermost server layer, at the point it's actually writing to one specific socket — the
   natural home for the `"Name"` field from the `IAM`/`GUIIAM`-retirement idea recorded above, since
   that's the one layer that actually knows which login name owns which connection.

Each outer layer's job is to wrap the inner layer's JSON **opaquely** — never inspect or transform
it — matching the same never-hold-inner-layer's-knowledge principle already established everywhere
else in this redesign (see "Server's residual per-game-type knowledge" above).

**LOGIN/LOGOUTOTHER traffic — scope corrected 2026-09-28, superseding the note below (kept for the
history, not as current guidance).** ~~Confirmed out of scope for Phase 2 (Albert, 2026-09-27): this
exchange is already its own separate island on both sides — client-side, `ClientManager.
SendLoginLikeString` never goes anywhere near `GameGui`/`ActionTransferManager`, the mechanism Phase
2's "wrap actions in JSON" plan actually targets. JSON-wrapping it too would be nice for consistency
eventually, but it's explicitly not part of the current Phase 2 scope — stays a separate,
lower-priority item.~~ **Reversed (Albert, 2026-09-28): explicitly in scope after all** — the
governing rule is now simply "every client→server message becomes JSON in phase 2," with no
per-message-category carve-outs, so `LOGIN`/`LOGOUTOTHER` (and whatever room-navigation traffic the
client sends — source not yet located, see the "Staging" note below) are included on exactly the same
footing as in-game actions. `ClientManager.SendLoginLikeString` never going near `GameGui`/
`ActionTransferManager` is still true structurally (it's a separate code path), it just no longer
means "separate scope" — both paths get the identical JSON-wrap treatment, independently.

**Staging — revised to three explicit phases (Albert, 2026-09-24), each independently shippable and
validatable, one variable changed at a time:**

1. **Process decoupling** (the bulk of this spec) — games become separate processes, talking
   stdio + JSON-RPC-ish to the server. The *client*↔server wire is deliberately untouched here,
   still today's hand-rolled comma-escaped lines, exactly as today's Java client already speaks. This
   is what let this session's test infrastructure (`server/tests/`) be built and validated against
   `mock_game.py` without needing any client-side change at all. `open`-transition `LEGALACTION`s
   stay plain `BROADCAST` at this phase (see the "Revisited" note above — a defensive guard on the
   transition itself is enough, not a real gap). **Split into two sub-parts (Albert, 2026-09-26):**
   - **Phase 1A — server side. Done and committed as of 2026-09-26**: `common/JsonLineProtocol`,
     `server/GameProcessProxy`, spawn-based `GameBox`/`GameCloset`, the `RoomManager`/build-hygiene/
     port-configurability fixes this work surfaced, and the full `server/tests/` pytest suite
     validating all of it against `mock_game.py` and the real compiled `gameserver` binary.
   - **Phase 1B — game side, in progress as of 2026-09-27.** `gamecommon/GameServerMain.hpp`: the
     glue that lets an existing game's `DLLGame`-based code become a standalone executable speaking
     the same protocol Phase 1A's server side already expects. **Statically linked, not `dlopen`'d
     (Albert, 2026-09-27):** once every game is its own spawned process (one `launchCommand` per
     game-type, decided before the process even starts), nothing needs runtime dynamic loading
     anymore — confirmed by grepping the actual codebase, not just architecturally: `common/
     SystemSpecificDynamicLoading`/`GenericLibraryLoader` already had zero callers (`server/GameBox.cpp`
     stopped using it in Phase 1A), and nothing outside `Outpost/tca/Makefile`/`MerchantOfVenus/tca/
     Makefile`'s own `.so` targets referenced either `.so` anymore. **Fully removed, not just left
     unused (Albert, 2026-09-27):** both games' `.so`/`LIBSUFFIX`/`SHLIBEXT`/`PICFLAG` Makefile
     machinery deleted (the executable targets are now just named `Outpost`/`MerchantOfVenus` — bare
     game names, no suffix — matching the launch-command convention already live in the real, gitignored
     `data/gameconfig.txt`, which Albert had already updated ahead of this work); `transitioncompiler`'s
     `FileWriter.pm` no longer generates the `extern "C" Initialize`/`CreateGame` functions or the
     `GameBoxDLL.hpp` include at all (this *did* require touching `transitioncompiler`'s codegen,
     unlike the Varicast removal above — a narrow, mechanical deletion of dead output, not the "Phase 4"
     architectural question); `gamecommon/GameBoxDLL.hpp` deleted entirely (`git rm`); `common/
     SystemSpecificDynamicLoading.{hpp,cpp}` deleted entirely too (`git rm`, per Albert: "it's not that
     it's wrong code, it just doesn't belong in this project any more" — not a soft/commented-out
     deprecation). `AOR/AORDLL.cpp` (the one other `GameBoxDLL.hpp` includer, frozen since this repo's
     first commit, no `tca/` directory or live regeneration path — confirmed via `git log`) is left
     broken by this; consistent with Albert's own framing of AOR as an abandoned early prototype, not
     a design reference, not worth protecting. Regenerated and reverified after this change: both
     games' `<Game>DLL.cpp`/`<Game>GameInfo.hpp` have zero remaining `Varicast`/`NameBoolean`
     references (confirmed by grep on the fresh output), MoV's 36-case Boost.Test suite passes, and
     the full `server/tests/` pytest suite (12 cases, including the real-game e2e tests below) passes
     against the renamed executables. This means the game side's own long-term fate (the "Phase 4"
     reframing in `TODO.md`'s XML/`transitioncompiler`-replacement item: C++ templates, or a
     non-C++ reimplementation of the same Pattern) is still genuinely undecided and untouched — only
     the dead dlopen-era output was removed, nothing about the codegen's actual approach changed. So Phase 1B
     doesn't invest in reshaping its codegen. `AOR/AORDLL.cpp` is the one remaining `GameBoxDLL.hpp`
     includer — an abandoned early prototype (predates Outpost, never updated to the current system's
     shape), explicitly not a design reference for anything here.

     **The roster-based `VariCast` replacement (see "VariCast is dropped" below) is actually
     implemented, not just designed:** `DLLGame::HandleAction`'s `LEGALACTION` loop now iterates the
     server-supplied roster directly, calling each transition's `IsLegal` per candidate name and
     `UnicastLEGALACTION` on a hit — `TransitionBoolean`/`VaricastLEGALACTION` deleted from
     `DLLGame.hpp`. Verified end-to-end against the *real*, statically-linked `outpostserver` binary
     (not `mock_game.py`): a player who has already joined correctly stops seeing `JOIN` as legal,
     while a roster name that hasn't joined yet correctly sees only `JOIN` — full original fidelity,
     no heuristic, no cosmetic bug, closing the "never validated against a real game process" gap
     this section used to flag.

     **Flagged, not resolved differently — a real discrepancy between this spec and 25-year-old code,
     preserved as-is (Albert confirmed, 2026-09-27):** `DLLGame::HandleAction`'s existing error path
     (`UnicastERROR`/`BroadcastERROR`) has always been a plain `event`, never the native-RPC-error
     shape "`ERROR` is a special case" (below) describes. `GameServerMain.hpp` preserves this real
     behavior rather than reinterpreting it — client-visible behavior is identical either way, since
     `GameProcessProxy` dispatches any interleaved `event` (error or not) before ever inspecting the
     final response for an `"error"` field.

     **File placement convention:** a game's `<Game>ServerMain.cpp` lives alongside `<Game>Set.cpp`
     in the game's own source directory (e.g. `Outpost/OutpostServerMain.cpp`), never in `tca/` —
     `tca/` holds only each game's `Makefile` and compiled/generated artifacts (Albert, 2026-09-27).

     **A preprocessor pattern worth remembering, found jointly with Albert 2026-09-27 (recorded here
     since it's genuinely reusable, not just a `GameServerMain.hpp`-specific detail):** a per-game
     `<Game>ServerMain.cpp` needs to name one thing — the game's name — and have everything else
     (which header to `#include`, which C++ type names to instantiate templates with) derived from
     it, not separately spelled out. Two obstacles stood in the way, both resolved:
     - **Plain object-like macro + `##` token-pasting cannot build a computed `#include` containing a
       literal `.`** — tested directly: `#define NAME a` then pasting `NAME` with `Set.hpp` fails
       with `pasting "a" and "." does not give a valid preprocessing token`, because `##` requires
       both operands to combine into one *valid* token, and an identifier immediately followed by a
       `.` never can. A GCC-manual-style "adjacent string literals get concatenated for `#include`"
       idiom (`#include SOMEMACRO ".hpp"`) was also tested and does **not** work on this toolchain —
       GCC treats the second literal as an "extra token" and errors.
     - **The fix: make the name macro a zero-argument *function-like* macro** (`#define
       GAME_SERVER_NAME() Outpost`), not an object-like one. Its explicit `()` is a real token
       boundary the preprocessor recognizes as ending the macro invocation — so `GAME_SERVER_NAME()`
       immediately followed, with *no* intervening whitespace, by literal `GameInfo.hpp` tokenizes as
       the macro call *plus* three separate literal tokens (`GameInfo`, `.`, `hpp`), not one attempted
       (and invalid) paste. Stringizing that whole sequence via the standard two-level `#`/`##`
       indirection trick (`STR2(x) #x` / `STR(x) STR2(x)`, and the analogous `PASTE2(a,b) a##b` /
       `PASTE(a,b) PASTE2(a,b)` for building C++ type names) then works correctly for *both* needs —
       `#include GAME_SERVER_STR(GAME_SERVER_NAME()GameInfo.hpp)` resolves and includes the real file,
       and `GAME_SERVER_PASTE(GAME_SERVER_NAME(),Set)` correctly yields the token `OutpostSet` —
       confirmed via `g++ -E` showing the exact resolved filename/token in each case, then via a real
       successful build and a real spawned-process smoke test. (An initial suggestion from Gemini,
       when Albert asked it whether this was possible, proposed a plain object-like macro plus `##`
       across a `.` — its own example, `xVERSION_MAJOR_##VERSION_MINOR` claiming to yield `"v2_5"`,
       was checked directly with `g++ -E` and does not do that at all; `##`'s operands are never
       macro-expanded before pasting, so it actually yields the literal, meaningless text
       `"VERSION_MAJOR_VERSION_MINOR"` — a hallucinated result, not a real mechanism. Worth remembering
       specifically as a reminder to verify any AI-suggested preprocessor trick with `-E` before
       trusting it, not just this one.)
     - Net effect: a real per-game file is just three lines — `#define GAME_SERVER_NAME() Outpost`,
       `#define GAME_SERVER_MAIN`, `#include "GameServerMain.hpp"` — see
       `Outpost/OutpostServerMain.cpp`/`MerchantOfVenus/MerchantOfVenusServerMain.cpp` for the real
       examples. `GAME_SERVER_AUTORECURSION_DEPTH` defaults to 60 inside `GameServerMain.hpp` (the
       common case) and is only overridden (`#define` before the `#include`) when a game genuinely
       needs a deeper bound (MoV needs 100, matching today's hardcoded `CreateGame()` literal).
2. **JSON re-encoding of the client↔server wire — its own standalone phase, not bundled into either
   neighbor. Revised 2026-09-27 (Albert) to cover actions, not events — see the dated note below for
   why — then revised again 2026-09-28 (Albert): the real dividing line is direction, not message
   category. **Phase 2 covers *every* client→server message, not just in-game actions.** **Server side
   (including the game-process side of the game↔server wire and `transitioncompiler`'s codegen) done
   and verified 2026-09-29 — see `TODO.md`'s matching item for the full file-by-file summary and test
   results; only the Java client's outgoing side (`GameGui.SendAction`/`ClientManager.
   SendLoginLikeString`) remains, not started.** This
   explicitly includes `LOGIN`/`LOGOUTOTHER` (today's `ClientManager.SendLoginLikeString`, previously
   carved out as its own separately-deferred island — that carve-out is superseded) and whatever
   room-navigation traffic (`CHANGEROOM`/`NEWROOM`/etc.) the client sends, source not yet located in
   this codebase as of this writing (see [[component-java-client]] — likely `roomgui/`, unexplored).
   Server→client traffic (events, the welcome string, the login response, the gui-management
   meta-layer) stays phase 3's side of the same clean split — see that bullet below. Actions (and now
   everything else crossing this direction) get the same message *type* and semantics as today, just
   a JSON payload instead of hand-rolled `UnCommaStringify`/`UnComma`/`ReComma` escaping. Deliberately
   *not* folded
   into phase 1 (which was scoped to leave the client boundary alone entirely, and bundling would
   double what needs validating at once) nor phase 3 (which bundles a much bigger message-*shape*
   change; re-encoding today's existing action messages is a fully separable, smaller,
   independently-valuable step). Concretely motivated, not just theoretical: investigating a real
   discovered bug (`Room::AddPlayerToRoom`'s `GAMES,` list gets `UnComma`'d once per game name *and*
   once again for the whole joined string, escaping the `,` separators themselves into `%C`, e.g.
   observed as `GAMES,MerchantOfVenus%COutpost` in a live smoke test) surfaced exactly the kind of
   latent fragility a hand-rolled escaping scheme invites — safe today only because no game name ever
   contains a comma or `%`, not provably safe in general (that specific bug is in an *event*, though,
   so this phase doesn't itself fix it — phase 3 does, by retiring the mechanism that produces it).
   Requires a real Java-client change (sending JSON instead of comma lines for every outgoing
   message, not just in-game actions) — "modest," per Albert, and distinct from the much larger,
   separately-tracked full front-end rewrite. Also requires updating `server/tests/client.py` (still
   sending legacy action lines, and legacy raw `LOGIN`/`LOGOUTOTHER` lines, as of this writing).
3. **Full per-viewer state** (see above) — the many-typed-events-to-one-blob redesign, plus
   `SPECTATOR`-targeted routing. **Revised 2026-09-27: events go straight from today's raw comma-lines
   to this, with no JSON-wrapped-event intermediate stage** — see the dated note below.
   **Revised again 2026-09-28 (Albert): phase 3 is also where every *other* remaining server→client
   message becomes JSON** — completing the direction-based split phase 2 now owns on the
   client→server side (see that bullet above). This is genuinely two different kinds of change
   bundled under one phase, worth keeping distinct: (a) the individual per-transition game Events,
   which get replaced *in shape* by the `GetClientState` blob redesign — this is the one place a
   JSON-wrapped-event intermediate stage would be pure throwaway work, per the revision above; (b)
   the welcome string, the login response (`WELCOME`/`ALREADYLOGGEDIN`/error text), and the
   `NEWGUI`/`ADDGUI`/`RESETGUI`/`DROPGUI`/`MESSAGE`/`SERVERERROR` gui-management meta-layer (see
   [[component-java-client]]'s `GuiPacketParser`) — these just get JSON-wrapped with their *existing*
   shape/semantics preserved, exactly like phase 2 does for actions, and nothing later throws that
   away. A genuine side-benefit of (b): it permanently retires `GuiPacketParser`'s un-escaped
   `split(",")` tokenizing (the same class of latent fragility as the `GAMES,` bug above, previously
   noted as out of scope for either phase — no longer true). Also unambiguously the moment to do the
   `TODO.md`-flagged `V1.0`→`V2.0` server version-string bump, since the welcome string carrying that
   version is itself one of the messages changing shape.

**Superseded 2026-09-29 — the "opaque string, avoid replumbing twice" reasoning below no longer
applies, and is kept only for the history of how this design evolved.** ~~Confirmed 2026-09-26,
revised 2026-09-27: phase 2 wraps today's *action* messages, it does not genuinely restructure them,
and it does not touch events at all — both deliberately, to avoid replumbing the same code twice.
Phase 2 could, in principle, re-encode actions as real per-field JSON
(`{"method":"MOVE","params":{"piece":3,"dest":7}}`) rather than an opaque comma-string carried as one
JSON field's value (`{"action":"MOVE,3,7"}`) — Albert's call: if phase 2 were the final destination,
genuine restructuring would be worth it, but phase 3 requires touching the same code again, more
deeply, to actually split state into per-field JSON, so doing the "real" restructuring once, in phase
3, avoids replumbing the same places twice.~~ **Why this no longer holds (Albert, 2026-09-29):** this
assumed phase 3 would eventually come back and restructure the *action* side "for real." Under the
direction-based phase split finalized 2026-09-28 (Phase 2 = the entire client→server direction,
permanently; Phase 3 = the entire server→client direction, permanently — see "Action namespacing"
above and the Staging section below), Phase 3 never touches actions at all. There is no future second
pass on the action encoding — whatever shape Phase 2 gives it is what this codebase lives with for
the foreseeable future, so the "avoid doing it twice" argument for picking the cheapest option
doesn't apply; there's no second time to avoid.

**Resolved 2026-09-29 (Albert): actions get real, natively-typed, named JSON fields — not an opaque
string, not even named-but-still-string fields.** E.g. `{"namespace":["game"],"action":"OPTIONS",
"params":{"discard":false,"robots":2,...}}`, with each param's JSON type (number/boolean/string)
matching its declared C++ type, not a stringified value needing `boost::lexical_cast`. This is a
direct, deliberate response to actually reading the real generated validation code together
(`Outpost/tca/OutpostDLL.cpp`'s `OutpostGameInfo::OPTIONS_ExecuteAction`): today's `ActionParser`
(`gamecommon/ActionParser.hpp`) hands every argument to `*_ExecuteAction` as a plain positional
`std::string`, and the transitioncompiler-generated body does one `boost::lexical_cast<T>(i_ap[N])`
per declared argument (plus separate min/max range validation, unaffected by this change either way).
Two things make the natively-typed option cheap enough to justify given it's now permanent, not a
placeholder to revisit later: (1) **no new dependency** — `boost::json` is already a live project
dependency (`common/JsonLineProtocol.hpp`, built for Phase 1's game↔server wire), so parsing typed
JSON values doesn't require adopting anything new; (2) **no new information to invent** — every
argument's target type (and its min/max) already lives in the `<var>` declarations in each
`<Game>Server.xml` at codegen time, `transitioncompiler` already knows it, it just currently emits a
`lexical_cast`-from-string call instead of a typed JSON-object accessor. The actual work is
localized and identified, not open-ended: `transitioncompiler/Transition.pm` and `FileWriter.pm` are
the two modules that currently emit the `lexical_cast` blocks (confirmed via `grep`) and are what need
rewriting to emit typed `boost::json` extraction instead. `ActionParser` itself either goes away or
becomes a thin JSON-object wrapper — no longer a comma-line splitter.

**A related, softer finding from the same conversation, recorded in `TODO.md`'s "Phase 4"
`transitioncompiler`-obsolescence item, not here:** reading this validation code directly reminded
Albert how much real sophistication `transitioncompiler` already generates (argument-count checks,
per-field typed casting, range validation, state-validity checks) — tempering, not cancelling, the
appetite for replacing it with template metaprogramming or a from-scratch reimplementation anytime
soon. See that `TODO.md` item for the full note.

**Second application of the same "don't replumb it twice" principle, this time to *which side* of
the wire phase 2 touches at all, not just how deep (Albert, 2026-09-27):** the original plan had
phase 2 also JSON-wrap events, symmetrically with actions. That would have required real work on
*both* ends of the event path — server-side code wrapping every outgoing `SENDLINE`/`RoomOutputPort`
string in a JSON envelope, and client-side code unwrapping it before handing the same comma-string to
its existing per-event-type dispatch logic. Since phase 3 doesn't send individual events *at all*
anymore (one `GetClientState` blob replaces the whole per-transition event mechanism), all of that
event-wrapping/unwrapping code would be built in phase 2 and then discarded almost immediately in
phase 3 — a complete throwaway on both ends of the wire, not hypothetical waste. So phase 2 leaves
events alone entirely: still today's raw, unwrapped comma-lines, both directions unchanged from
phase 1. **Real, deliberate consequence: phase 2's wire is asymmetric while it's deployed on its
own** — actions arrive JSON-wrapped, events leave as plain comma-lines, same connection, two
different framings in the two directions. Albert confirmed this is an acceptable consequence, not a
problem to design around (2026-09-27) — especially since the intent is to ship phases 2 and 3 the
same day, making the asymmetric-phase-2-alone window very short in practice. This doesn't remove the
still-outstanding prerequisite noted elsewhere (Albert walking through how the client's *generic*
wire-protocol handling works, distinct from the widget/rendering architecture) — if anything it's
slightly more load-bearing now, since this plan skips the shallow "practice run" a JSON-wrapped-events
pass in phase 2 would otherwise have given before phase 3's real restructuring.

**Scoping note:** this section (and the rest of this spec) governs the *game↔server* wire. The
*server↔client* leg either carries translated legacy lines (if the Java client is left alone for
now) or the same JSON payloads (given the "modest change" above) — those are independently staged;
nothing here forces the front-end rewrite to happen on any particular timeline.

## Action namespacing — resolved 2026-09-28, part of the Phase 2 JSON re-encoding

**The problem, found while walking the real server dispatch code (Albert + Claude, 2026-09-28):**
today's Action name is a single flat namespace with no scoping at all, and the server resolves it by
literal-string-match priority order, not by anything structural:
```cpp
// RoomManager::HandleAction
if (i_ap.GetActionName() == "NEWROOM") ...
else if (i_ap.GetActionName() == "CHANGEROOM") ...
else if (i_ap.GetActionName() == "ROOMTALK") ...
else if (i_ap.GetActionName() == "PLAYERTALK") ...
else { pRoom->HandleAction(i_Name, i_ap); }   // falls through to Room

// Room::HandleAction
if (i_ap.GetActionName() == "NEWGAME") ...
else if (i_ap.GetActionName() == "LOADGAME") ...
else if (i_ap.GetActionName() == "SAVEGAME") ...
else { m_pGame->HandleAction(i_Name, i_ap, m_Inhabitants); }  // only now reaches the live game
```
Seven reserved names at the time (`NEWROOM`/`CHANGEROOM`/`ROOMTALK`/`PLAYERTALK`/`NEWGAME`/`LOADGAME`/
`SAVEGAME`; **`ROOMTALK`/`PLAYERTALK` removed 2026-10-03** — the whole chat facility was dropped, see
`.claude/TODO.md` — leaving five: `NEWROOM`/`CHANGEROOM`/`NEWGAME`/`LOADGAME`/`SAVEGAME`)
are silently intercepted before ever reaching a game process — any game transition that happens to
share one of these names (`CHANGEROOM` is a plausible real collision: any game with its own in-game
"move between areas" concept) would never fire at all, with no error, just the wrong handler quietly
eating it. Confirmed real while exploring [[component-java-client]]'s `roomgui/` client, not
hypothetical.

**A second, distinct bug class this same gap enables, found in the same reading:** `Room::HandleAction`'s
final fallthrough forwards *unconditionally* to whatever `m_pGame` currently is, with no check that
the action was ever meant for *that* game. A stale action (e.g. arriving after `NEWGAME` has swapped
one game type for another in the same room) that happens to coincidentally also be a valid transition
name in the new game would silently misfire against the wrong rules instead of erroring.

**Resolution (Albert, 2026-09-28), rolled into the Phase 2 JSON re-encoding of the client→server
wire (see "Staging" above): every Action gets an explicit `namespace` field.** **No default/implicit
namespace is allowed; every action must carry one explicitly.** Two kinds of namespace value:
- **A small, hardcoded, structurally-fixed set for the server's own reserved layers** — one value
  per real dispatch layer (`RoomManager`'s own 4 actions, `Room`'s own 3), matching the two
  `HandleAction` chains above exactly.
- **For every other action, the namespace must equal the actual live game's own public `Name`**
  (`ServerGameInfo::GetName()`/`GameBox::GetName()` — the same string already threaded through
  `NEWGUI`/`GAMES`/`RESET`, see "Server's residual per-game-type knowledge" above — not a new
  identifier). **The server validates this against the room's actual current game and generates an
  explicit error on any mismatch** (wrong game name, or no live game at all) — this is what closes
  the second bug class above, not just the first: a stale or misdirected action can no longer
  silently reach the wrong game's rules engine.

**`namespace` is a plain string, not a path/array — settled 2026-09-29, after briefly considering and
rejecting an array.** The initial design (2026-09-28) made `namespace` an array of segments
specifically so a hypothetical future extra dispatch layer could be added without a wire-schema
change, modeled on filesystem paths/Java packages. **Albert, 2026-09-29, after being asked to weigh
that tradeoff directly: "I can't for the life of me see this getting complicated enough that we would
need in-JSON-object support for hierarchical namespaces"** — every concrete namespace value actually
specified above is exactly one segment deep (a single hardcoded reserved-layer value, or a single
game name), so the array was paying a real complexity cost against a hierarchy need that doesn't
exist and may never materialize; if it ever does, a plain string can move to a delimited convention
(`"room.something"`) or a schema bump same as any other wire format change would. Settled as a plain
string.

**Full resolved shape of an incoming Action (Albert, 2026-09-29) — envelope fields at the top level,
every argument nested one level down, eliminating any possibility of a name collision between the
two (see below for why nesting, not a prefix, was chosen):**
```json
{"namespace": "Outpost", "action": "BUYOREFACTORIES", "params": {"NumOre": 2, "DiscardString": "0010"}}
```
- **`namespace`** — a plain string, per above: one of the small hardcoded reserved values, or the
  live game's own public `Name`.
- **`action`** — the action's own name (matching today's `ActionParser::GetActionName()`).
  **Naming this took two rejected candidates first, worth recording since the reasoning generalizes:**
  `name` was rejected because it's already heavily used in the real dispatch code to mean the
  *player's* name (`Room::HandleAction(const std::string &i_Name, const ActionParser &i_ap)`,
  `RoomManager::HandleNewRoom(const std::string &i_Name, ...)`, etc. — confirmed directly in
  `RoomManager.cpp`); `method` was rejected because it borrows JSON-RPC vocabulary implying real
  method dispatch, when `RoomManager`/`Room::HandleAction` are actually plain if/else string chains —
  Albert didn't want a field name overclaiming an implementation shape the code doesn't have.
  `actionname` (mapping 1:1 onto the existing C++ accessor) was considered and rejected in favor of
  the shorter `action`.
- **`params`** — every argument declared by the transition's `<var>` directives in `<Game>Server.xml`,
  keyed by name, nested in its own object specifically so `namespace`/`action` can never collide with
  a user-defined argument name — reusing the exact same envelope/`params` split already established
  for the Phase 1 game↔server wire's `Request` shape (`{"id":<n>,"method":"<name>","params":{...}}`,
  see the message catalog above) rather than inventing a second, different collision-avoidance
  convention (e.g. a reserved-field prefix) for this leg of the wire.
- **C++ numeric types are represented as native JSON numbers, non-numeric C++ types as JSON strings**
  (Albert, 2026-09-29) — this is what Tier 3 (above) actually means concretely: `int`/similar become
  JSON numbers, not numeral strings needing `boost::lexical_cast`.

**What this makes `RoomManager`/`Room`'s dispatch look like** (Albert: "quite lovely" — handle
everything that's your own, pass on everything that isn't, unchanged in shape as more layers appear):
```cpp
void RoomManager::HandleAction(...) {
  if (i_ap.GetNamespace() == "roommanager") { /* dispatch by action name, as today */ }
  else { pRoom->HandleAction(i_Name, i_ap); }  // pass the whole thing on, untouched
}
void Room::HandleAction(...) {
  if (i_ap.GetNamespace() == "room") { /* dispatch by action name, as today */ }
  else { m_pGame->HandleAction(i_Name, i_ap, m_Inhabitants); }  // game validates namespace == its own Name
}
```

**Client-side consequence, concrete and small:** `GameGui` doesn't currently store its own gui-name
at all — `GUIUnit` sees it via `GuiPacketParser.GetGuiName()` at construction time (the same string
as the game's public `Name`, per how `NEWGUI` is built) but never passes it down. That needs
threading into `GameGui`'s constructor so `SendAction` can stamp `{"namespace": thatName, ...}` on
every outgoing action from that instance automatically, with no per-action-site bookkeeping anywhere
in the widget code documented in [[component-java-client]]/[[outpost-client-gui]]/
[[merchant-of-venus-client-gui]] — none of those call sites need to know or care about namespacing at
all, `GameGui.SendAction` is still the one and only place that changes.

**`LOGIN`/`LOGOUTOTHER` are explicitly excluded from this shape (Albert, 2026-09-29) — their own
design is a separate conversation, not yet had.** Albert wants them JSON too eventually, but since
their format is "absolutely fixed" (always exactly username/password, no schema-driven variability
the way a game transition's `<var>`s have), they don't need the generality of `namespace`/`action`/
`params` at all — whatever shape ends up simplest for a fixed two-field message is fine, and doesn't
need to reuse this envelope.

**Both the server and the individual game process unwrap this JSON — but not the same amount, and
not symmetrically. Confirmed 2026-09-29 by tracing the real code, both directions of both wires
involved:**
- **Client → server:** `RoomManager`/`Room::HandleAction` must parse the incoming JSON enough to read
  `namespace` — mandatory for every action, since routing depends on it. For the reserved actions
  (five since `ROOMTALK`/`PLAYERTALK` were removed 2026-10-03: `NEWROOM`/`CHANGEROOM`/`NEWGAME`/
  `LOADGAME`/`SAVEGAME`), the server also
  needs `action` and `params` fully, since it executes those itself. For a game-bound action, the
  server needs `namespace` (to validate against the room's live game) but never needs to interpret
  `action`/`params` at all — those get passed through opaquely, matching the standing principle that
  the server holds no game-internal knowledge.
- **Server → game process — this is the part that reaches back into already-shipped Phase 1 work, not
  just new Phase 2 surface.** Checked directly: `server/GameProcessProxy.cpp:192` builds today's
  outgoing `handleAction` JSON-RPC request with `params["action"] = i_ap.GetRawLine();` — the
  *entire original raw client line*, forwarded verbatim as one opaque string, exactly what
  `ActionParser::GetRawLine()` was added for in Phase 1. Inside the game process,
  `gamecommon/GameServerMain.hpp:172,182` pulls that string back out
  (`params.at("action").as_string()`) and reconstructs a **second** `ActionParser` from it
  (`ActionParser ap(action)`) before calling `game.HandleAction(player, ap, roster)` — which is what
  reaches the generated `*_ExecuteAction` methods' positional `boost::lexical_cast` calls. So today,
  both wires' `ActionParser`s run over the *identical* raw string, redundantly — that redundancy is
  exactly what let Phase 1 ship with zero coupling between the two wires' formats.
- **What Tier 3 forces to change here:** for the generated `*_ExecuteAction` code to actually stop
  doing `lexical_cast`-from-string and read typed JSON directly, the typed `params` object has to
  survive intact from the client all the way into the game process — a single raw string can't carry
  that. Concretely: `GameProcessProxy.cpp`'s outgoing `handleAction` request needs to carry `action`
  (the name) and `params` (the real JSON object, passed through by the server without transformation)
  instead of one opaque string, and `GameServerMain.hpp`'s `handleAction` case needs to stop building
  an `ActionParser` from a string and instead hand the generated dispatch code a `boost::json::object`
  directly. **Real implementation-scope consequence: Phase 2, as designed, requires touching
  `GameProcessProxy.cpp` and `GameServerMain.hpp` — both already-shipped, already-tested Phase 1
  code — not just `RoomManager`/`Room`/`ActionParser`/`transitioncompiler`.** `ActionParser` itself
  likely goes away entirely at that point, on both sides of both wires, replaced by direct
  `boost::json::object` access — it was purpose-built for the comma-line format this whole redesign
  is retiring.

## `ERROR` is a special case, handled specially on both client and server (Albert, 2026-09-22)

**An action that gets rejected causes zero internal state change and zero FSM transition — so the
*only* output is the error message, and it goes *only* to the sender**, never `BROADCAST`,
`VARICAST`, or `SPECTATOR`. This is why `handleAction`'s error case (see message catalog above) is
modeled as the RPC layer's own native error response (`{"id":n,"error":"..."}`), not an `event`
notification requiring an explicit target — the response is already inherently scoped to the one
request/connection that caused it, so no addressing is needed at all, and structurally nothing else
can be emitted alongside it.

**Open wrinkle, not yet resolved — flagged rather than papered over:** today's
`DLLGame::HandleAction` has a `RECURSION_LIMIT` case (the auto-transition cascade hit its depth
limit) that doesn't cleanly fit this rule. It `BroadcastERROR`s a warning but then *falls through*
(not `return`s) into the normal success path — state *did* change, up to the point the cascade was
cut off, so it's not "zero state change" the way `TRANSITION_NOT_FOUND`/`EXECUTE_FAILED` are. Also
worth a second look: today's `NO_STATE` case `BroadcastERROR`s (everyone, not just the sender) rather
than following the sender-only rule — arguably deliberate, since it represents the whole FSM being
corrupted rather than one rejected action, but worth confirming rather than assuming when this is
actually implemented.

**Investigated 2026-09-22 (Albert raised whether `RECURSION_LIMIT` is provably avoidable; decided
to leave it as an accepted, rare, autosave-mitigated risk rather than redesign around it now — this
is background for if that decision gets revisited later, not a call to action.)** Whether an
auto-cascade halts is undecidable in general (`<auto>` conditions are arbitrary C++ over arbitrary
state — literally Halting-Problem-equivalent, per Albert). There is a decidable, *sufficient*
(sound but conservative) condition that sidesteps the semantics entirely, though: if the subgraph
formed by only `auto`-flagged transitions' `from`→`to` edges is acyclic, the cascade is
structurally bounded regardless of what any condition computes — checkable by `transitioncompiler`
at compile time via ordinary cycle detection, no need to interpret any embedded C++. Checked what
this would actually mean against both real games' current XML (not hypothetically):
- **Outpost's apparent 6-state whole-turn cycle
  (`StartFinalManning→FinalManning→StartDiscard→DoDiscard→StartPurchase→Purchase→StartFinalManning`)
  is a false positive**, not a real cycle: every edge into/out of `StartDiscard` and `FinalManning`
  in that loop (`COMMODITYDONETODISCARD`/`COMMODITYDONETOPURCHASE`/`ENDDISCARDTOPURCHASE`/
  `ENDDISCARDTOTURNEND`/`ENDMANNINGTODISCARD`/`ENDMANNINGTOTURNEND`) is gated by
  `GetOptions().GetEarlyDiscard()` or its negation — a value fixed once at game start and never
  changed thereafter (Albert's correction, confirmed in `OutpostServer.xml`). For any single game,
  only one branch of each pair is ever live, giving a strictly linear phase order, not a cycle — the
  apparent cycle only exists if you naively union both option branches' edges into one graph.
- **Two real, structural cycles remain, and neither is resolved by the options point**, because both
  are bounded by quantities that *do* change mid-turn, not a startup-fixed option:
  Outpost's `AUTOSKIPDISCARD` (`DoDiscard`→`DoDiscard` self-loop, bounded by
  `IncrementTurnOrder()` advancing through the player list) and MoV's
  `ProcessPilotNumber→FinalizeMove→CompleteMove→ProcessPilotNumber` loop (`AUTOCHOOSEDEST`'s
  condition includes `GetDests().size()==1`, bounded by the shrinking number of remaining
  destination choices). Adopting a strict acyclic-auto-subgraph rule as-is would reject both of
  these currently-working mechanisms, not just hypothetical future ones — it's a real design
  constraint, not a free compile-time win, if pursued.
- Also worth remembering if this is revisited: MoV's `ENDMOVE` has *both* a real `<auto>` condition
  and a real `<allowed>` condition simultaneously — not every transition is cleanly auto-xor-player-
  triggered, which any future cycle-detection tooling would need to account for.
- One alternative not requiring a graph-structure rule at all, if this is ever revisited: move a
  "keep skipping until done" loop like `AUTOSKIPDISCARD` inside a single transition's `<body>` as an
  ordinary bounded C++ loop instead of expressing it as repeated FSM auto-hops — trades away that
  loop's visibility as FSM structure for sidestepping the recursion-limit question for that pattern
  specifically.

**How to apply:** this is the concrete wire-protocol answer to
[[server-game-decoupling-investigation]]'s open items 1-3. The spawn/handshake-sequence question
that stayed open all session is now resolved (2026-09-23): there is no handshake — a spawned
process is immediately ready to receive requests, since nothing was ever left for it to self-report
(see "No handshake message at all" above). This spec has no remaining open *design* items as of this
writing — what's left is the test infrastructure below, needed before implementation starts, not a
design gap.

## Test infrastructure this redesign needs (Albert, 2026-09-24)

**A redesign this size — new socket-adjacent process spawning, pipe-based IPC, a from-scratch
wire protocol — needs integration-level testing, not just the existing in-process Boost.Test unit
tests.** Today's tests (`MoveMediatorTest.cpp`, `outposttest`, etc.) verify game *rules* by calling
game code directly in-process, some against hand-written `OutputPort` test doubles (e.g.
`Outpost/tests/TestOutputPort.hpp`). None of that exercises real sockets, real process spawning, or
real pipe framing — which is exactly the new surface this whole spec adds. Two components:

**1. A scriptable mock game process** — a real, spawnable executable speaking the same stdio +
newline-delimited JSON-RPC-ish protocol as any real `GameServerMain`-based game, driven by a script
instead of running actual game rules: for each expected incoming request (in a fixed sequence to
start — simplest to build; pattern-matching can come later if a test needs it), the script says
what `event` notifications to emit and what to respond with. No handshake logic needed at all,
since `describe` no longer exists. The script file can piggyback on plumbing that already exists
rather than inventing a new argv convention — point the mock at a data directory (the same
mechanism every real game already gets at launch) containing the script. Worth building deliberate
*bad*-behavior instructions in from the start, not just happy-path scripts, since testing failure
handling matters as much as the happy path here: a "crash now" instruction (exit without
responding — exercises the pipe-EOF crash-detection and, once built, the autosave/auto-restore
path), a "malformed output" instruction (tests the server's parser robustness), a "hang" instruction
(never responds).

**2. A multi-connection client-side test driver** — opens several raw TCP connections to the
server, logs each in as a distinct user, sends arbitrary lines, and asserts on what comes back
per-connection. This barely touches anything *this* redesign changes (login/rooms/sockets stay
untouched throughout this whole spec) — its value is exercising the *new* surface (room→process
routing, `SPECTATOR`/`UNICAST`/`BROADCAST` delivery, the action-complete boundary, crash detection)
through the real stack end-to-end, not testing any one piece in isolation.

**Language recommendation: Python for both pieces, not C++.** This is test-only infrastructure, so
the language choice carries zero production risk, and it's exactly the "spin up sockets/processes,
script some JSON, assert on lines" profile Python is good at — much faster to write many test
scenarios in than C++, with no compile step per scenario change. It's also a good, low-stakes way to
get a real data point on the separately-raised "port the server itself to Python" question, without
having committed to that for production.

**Concrete scenarios this infrastructure should be able to cover**, grounding the abstract design in
what it actually needs to prove works:
- Basic single-player round trip: login, `NEWROOM`/`CHANGEROOM`, `NEWGAME` (spawns the mock),
  one `handleAction`, verify event delivery and the completion response.
- Player + spectator routing: a scripted transition that emits distinct per-player `event`s plus a
  `SPECTATOR` one; verify each connected user gets exactly the right one (phase 3 only, once built).
- `LOADGAME` both ways: no live instance (spawn-then-`load`) vs. a live instance (`load` directly,
  replacing in-progress state) — see "`NEWGAME` vs. `LOADGAME` stay distinct" above.
- Crash-and-recover: mock scripted to "crash" mid-round-trip; verify the server detects it
  (pipe EOF) and, once the autosave/auto-restore `TODO.md` item is built, respawns and reloads
  correctly.
- `ERROR` handling: mock responds with an RPC error; verify it reaches *only* the sender, with no
  preceding `event` traffic and no broadcast state change (see "`ERROR` is a special case" above).
- Concurrent-room isolation: two rooms' mock instances don't interfere with each other
  (process-per-instance).
- Disconnect doesn't reach the game process: a player disconnects mid-game; verify the mock never
  receives any notification of it (per the "Confirmed invariant" above) and other players are
  unaffected.
- (Phase 2, once built) JSON re-encoding round-trips every existing message type correctly,
  including ones a hand-rolled comma-escaping bug could hide: a list-valued field (like today's
  `GAMES,` list) with more than one entry, and a field value containing a literal comma or `%` —
  exactly the case the discovered `Room::AddPlayerToRoom` double-`UnComma` bug never got exercised
  by, since no game name today contains either character.
