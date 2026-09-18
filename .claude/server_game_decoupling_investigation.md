---
name: server-game-decoupling-investigation
description: Investigation into decoupling game implementations from the core server process, so games aren't forced to be C++; findings on the DLL boundary and what would/wouldn't decouple easily
metadata:
  type: project
---

Started 2026-09-18. Related to [[project-overview]], [[component-game-dlls]], [[component-cpp-server]],
[[server-directory-architecture]].

**The idea:** the server<->game interface today is an in-process `.so`/`.dll` dynamically loaded
directly into the server (see [[component-game-dlls]]) — which forces every game to be written in
C++. Albert notes this was an artifact of a design choice made in the early 2000s, not a fundamental
requirement, and C++ was never going to be the easiest language to write a game's rules in even back
then. He's exploring what it would take to separate each game into its own OS process, talking to
the server over a language-agnostic wire protocol (something JSON-RPC-ish over a pipe/socket, or
gRPC), so future games (or rewrites of existing ones) could be written in whatever language suits
them, without needing to retrofit the whole server.

This is a smaller, more incremental cousin of [[roadmap-todo]]'s biggest item ("port the entire
server off C++") — it decouples *games* from the server's implementation language without requiring
the *server itself* to be rewritten too. Worth keeping that connection in mind: this could be a
staging step toward the bigger item, or an alternative to it, depending on how far Albert wants to
take it.

**Session history:** started 2026-09-18 with intent to keep it in a session deliberately separate
from MoV-specific work, briefed by Albert directly rather than an agent exploring bottom-up first.
That same day, Albert redirected [[cpp-server-glue-session]] (already scoped to `server/`, `common/`,
`gamecommon/`, `stateengine/` for cost-management reasons) at this exact question, after that session
had already read all of `server/` (see [[server-directory-architecture]]) and then all of
`gamecommon/`+`stateengine/` with this decoupling question specifically in mind. So as of now this
is the session carrying the investigation forward — the findings below came from bottom-up code
reading, but under Albert's direct steer rather than independently.

**The DLL boundary today is not really a C ABI.** `gamecommon/GameBoxDLL.hpp` declares
`extern "C" bool Initialize(const std::string&)` and
`extern "C" Game *CreateGame(const ServerGameInfo&, OutputPort&)` (resolved by symbol name at
dynamic-load time in `server/GameBox.cpp` — see [[server-directory-architecture]]). But
`std::string`, `ServerGameInfo`, `OutputPort`, and the returned `Game*` are all full C++ objects
passed by reference/pointer. `extern "C"` here only suppresses name-mangling for symbol lookup; the
actual calling convention still requires the DLL to be built with the same compiler, STL, and ABI as
the server. **This — not the shape of the interface — is the real reason today's games have to be
C++.**

**The interface shape itself is already message-shaped, not API-shaped.** Once a `Game` exists, the
server's entire interaction with it (`gamecommon/Game.hpp`) is:
- `HandleAction(playerName, ActionParser)` — and `ActionParser` (`gamecommon/ActionParser.hpp`) just
  parses a comma-separated line, the *exact same wire format* the client already speaks to the
  server. The game isn't handed a rich in-memory call; it's handed a string line.
- `SendFullState(playerName)`, `GetName()`, `GetStatusString()`, `IsDone()` — small, coarse,
  stateless-looking queries.
- `Load(filename)` / `Save(filename)` (implemented in `Game.cpp`, calling the abstract
  `LoadFile`/`SaveFile`) — and critically, the `Game` owns its own file format entirely. The
  in-DLL `DLLGame` template happens to use Boost.Serialization (XML archives, via `common/
  Serialize.hpp` — corrected 2026-09-18, previously mis-stated here as binary), but that's
  `DLLGame`'s implementation choice, not part of the `Game` contract. An out-of-process game could
  save however it likes.
- Output flows back the same way, in reverse, through `OutputPort::UniCast/BroadCast/VariCast`
  (`gamecommon/OutputPort.hpp`) — again just string lines to named players, already abstracted away
  from raw sockets by `RoomOutputPort` on the server side.

So the actual runtime traffic across the boundary — action lines in, event lines out, plus
load/save by filename — maps cleanly onto a real IPC/socket protocol with very little semantic
translation. The hard part isn't the steady-state protocol, it's `CreateGame`'s live object
references at construction time (`ServerGameInfo&`, `OutputPort&`), which would need to become an
init-time message (config values) plus a callback/RPC channel respectively.

**One real wrinkle: `VariCast`.** It takes a `NameBoolean` — an arbitrary C++ predicate object —
which has no wire equivalent. Tracing its one call site (`DLLGame::HandleAction`, sending each
player their legal-action list via `TransitionBoolean` wrapping `Transition::IsLegal`), the
predicate logic only exists inside the game anyway. This collapses cleanly: give the game process
the room's player-name list (it doesn't have this today, but easily could), let it evaluate
legality itself, and have it emit N individual `UniCast`s instead of one `VariCast`. Not a
fundamental blocker, just a restructuring.

**The part that doesn't decouple for free: the FSM engine itself — and this is deliberate, per
Albert.** `stateengine/`'s `StateMachine`/`StateWalker`/`State`/`Transition` are C++ templates,
parameterized on each game's own `T_GameInfo` type — compile-time generic, not runtime-shared code.
The Perl `transitioncompiler` generates the concrete instantiation (state names, transition wiring)
per game from its `<Game>Server.xml` (see [[game-server-xml]], [[component-perl-compiler]]), and
that generated code + the templated `StateWalker` engine both get compiled directly into each game's
DLL.

Albert's framing (2026-09-18): **this is the real "secret sauce" of The Game Machine, and it's
intentionally non-DRY.** `StateWalker` — the code that actually walks the FSM: matching an incoming
action to a legal transition, executing it, then chasing any resulting "auto" transitions
recursively (`StateWalker::ExecuteAuto`, bounded by a recursion-depth limit) — is duplicated (via
template instantiation) into every single game DLL rather than being a single shared, dynamically-
dispatched implementation in the server. The payoff of accepting that duplication is exactly what
makes today's server/game split easy to decouple further: **the server itself never has to know
anything about FSM-walking, legal-move logic, or game state at all** — it only ever passes opaque
message lines back and forth (`HandleAction` in, `UniCast`/`BroadCast`/`VariCast` out) and asks
coarse yes/no/string questions (`IsDone`, `GetStatusString`, `GetName`). All FSM "magic" is fully
self-contained on the game side of the boundary already. That's precisely why the message-shaped
analysis above works: the boundary was already drawn at "pass messages," not "share FSM machinery,"
so moving the game to a different process (or a different language, reimplementing an equivalent
walker) doesn't require the server to gain any new capability — it just moves what's already an
opaque message-passing counterparty from an in-process pointer to a network peer.

**The wire protocol's escaping scheme (`common/StringUtilities.cpp`):** every comma-delimited line
— client↔server *and* the same `ActionParser`-shaped calls into a `Game` — relies on `UnComma`/
`ReComma` to let field content safely contain literal commas or percent signs: `UnComma` rewrites
`,` → `%C` and `%` → `%%` before a value is dropped into a comma-joined line; `ReComma` reverses it
on parse. Any replacement wire protocol for out-of-process games either inherits this same escaping
(if reusing the line format) or needs its own equivalent — either way it's a small, already-solved
problem, not a new one.

**The other half of "what wouldn't decouple for free": `transitioncompiler` itself
(2026-09-18, closes the last gap between this session's server-side understanding and the
MoV-side session's game-implementation understanding — see [[component-perl-compiler]] for the full
code-verified breakdown).** The Perl compiler's XML input is only *partly* a portable spec: state
names, the transition graph, and each transition's action-argument schema (types, min/max/list) are
genuine structural metadata a reimplementation could regenerate mechanically. But a transition's
`<allowed>`/`<auto>`/`<body>` blocks, a `<refresh>` block (a game's `SendFullState`), a `<preface>`
block, and any `global`/`private` event's `source=` value are all **raw, verbatim C++ snippets**
pasted directly into the generated `.cpp`/`.hpp` — there is no portable representation of actual
game *rules* here, only of FSM topology and message shape. So on top of the `StateWalker`
reimplementation cost already noted above, porting an existing game to a new language means
hand-translating every one of those embedded-C++ blocks by reading the XML as source code, not
data. This doesn't change the core finding (the runtime message-passing boundary is still cleanly
decouplable) — it just means "decoupling" and "porting an existing game to a new language" are two
separable costs: the former is an architecture change with a small, well-understood protocol
surface; the latter is a manual line-by-line rewrite regardless of what process boundary exists.

**Side benefit worth naming:** because games run in-process today, any game DLL crash takes down the
entire server — every room, every player, every other game — since it's all one address space.
Out-of-process games would get crash isolation as a free side effect of the same change that gets
you language independence.

**Interim step (Albert's idea, 2026-09-18): decouple the process, not the language, first.**
Instead of jumping straight to "games in other languages," wrap each existing game's
transitioncompiler-generated + hand-written C++ in a standalone executable instead of a `.so`/
`.dll`. This is a much smaller move than it first sounds: `DLLGame`, `StateWalker`, every
`FileWriter.pm`-generated `<Name>GameInfo`/`<Name>StateMachine`/`<Name>_Transition` class, and the
game's own hand-written `<Name>Set`/`<Name>State` piece classes are **all still just C++, compiled
exactly as today** — the only thing that changes is what final binary they link into (an exe with
a new `main()`, instead of a `.so` loaded via `dlopen`). The new `main()` still calls the exact
same generated `Initialize(dataDir)` and `CreateGame(ServerGameInfo&, OutputPort&)` — no change to
either.

This collapses almost all of the earlier "game-side requirements" list for this interim step:
`StateWalker` reimplementation, hand-porting `<allowed>`/`<auto>`/`<body>`/`<refresh>`/`<preface>`,
and reimplementing line-parsing/escaping (`ActionParser`, `UnComma`/`ReComma`) all evaporate —
they're reused unchanged. What's genuinely new, and only needs writing **once, generically, not
per-game** (since it only ever touches the already-generic `Game`/`OutputPort`/`ServerGameInfo`/
`ActionParser` interfaces):
1. A network-backed `OutputPort` subclass — `UniCast`/`BroadCast`/`VariCast` serialize onto the
   wire instead of calling back into an in-process `RoomOutputPort`.
2. A driver loop: read a wire line, build an `ActionParser` (unchanged, reused), call
   `Game::HandleAction` — relocating what `Room::HandleAction` does today.
3. A handshake step building a `ServerGameInfo` from wire-delivered config instead of a live
   reference (trivial — its constructor already takes plain strings).
4. Wire-level exposure of `GetName`/`GetStatusString`/`IsDone`/`Load`/`Save`.
5. The `VariCast` roster-propagation problem from earlier — still real, but now fully contained in
   this one generic piece, never touching game-specific code.

Server-side requirements from the earlier list are essentially unchanged by this interim framing —
still need the transport/framing choice, still need to receive and route the `OutputPort` traffic,
still need the process-lifecycle/crash-handling model, still need the process-per-room vs.
process-per-game-type fork resolved. The simplification is entirely on the game side.

**Mechanism for wiring up that generic wrapper — modeled on Boost.Test's `main()`-reification
trick, but simpler.** Boost.Test needs an actual runtime *registry*: many independently-written
`BOOST_AUTO_TEST_CASE` blocks across files, each self-registering via a static-init side effect,
because the set of test cases isn't known in advance. This project doesn't need that, because
each game already has exactly one, fixed-name pair of global functions (`Initialize`/`CreateGame`)
per `FileWriter.pm`'s convention — so ordinary linker symbol resolution does the whole job, no
registry required:
- A shared `GameServerMain.hpp` holds the entire generic wrapper (listen loop, `OutputPort`
  subclass, driver loop, handshake, `Load`/`Save`/status exposure), with the actual
  `int main(int argc, char **argv) {...}` guarded behind `#ifdef GAMESERVERMAIN`.
- Per game, one trivial, otherwise-empty `.cpp` (e.g. `MerchantOfVenusGameServerMain.cpp`):
  ```cpp
  #define GAMESERVERMAIN
  #include "GameServerMain.hpp"
  ```
- That file links together with the game's existing generated + hand-written `.o`s, unchanged. The
  linker resolves `main()`'s calls to `Initialize`/`CreateGame` against whichever game's object
  files are in that particular link — exactly one `.cpp` per link may define `GAMESERVERMAIN`, same
  discipline as `BOOST_TEST_MODULE` needing to appear in exactly one translation unit.
- Later refinement (not needed to prototype the header itself): since `FileWriter.pm` already
  emits its two files purely off `$tcom->{NAME}`, it could just as easily emit this third
  boilerplate file too, so a game author never hand-writes it.

**Load-bearing invariant (Albert, 2026-09-18): the whole system is single-threaded and fully
synchronous, and this is depended on, not incidental.** Verified against the code already read,
not just asserted: `ServerSocket::RunConnections()`'s inner `for` loop finds the *first* ready
socket, handles it, then `break`s — nothing else in the server is serviced until that call fully
unwinds. And "fully unwinds" is a deep synchronous chain: `RoomManager::HandleAction` →
`Room::HandleAction` → `Game::HandleAction` → `StateWalker::Execute` → the raw `<body>` C++ → any
`ExecuteAuto` cascade → the `BroadcastNEWSTATE`/`VaricastLEGALACTION` calls after it — all nested in
one call stack before `HandleLine` returns and the next `select()` even happens. **There is zero
locking or thread-safety anywhere in `gamecommon`/`stateengine`/the generated code, because this
invariant *is* the concurrency control.** Every `<body>`/`<allowed>`/`<auto>`/`<refresh>` snippet
across every game, written over ~25 years, assumes nothing else can possibly touch game state while
it runs.

**The same invariant, and the same lack of any locking, governs room/player bookkeeping too — and
it isn't cleanly separable from the game boundary (Albert's addition).** `RoomManager::
HandleNewRoom`/`HandleChangeRoom`/`HandleInitialConnection` mutate `RoomManager`'s own shared state
(`m_Rooms`, `m_Inhabitants`) with no locking, relying on the exact same one-event-at-a-time
guarantee. And they reach directly into a room's `Game` instance without any special-casing:
`Room::AddPlayerToRoom` calls `m_pGame->SendFullState(i_Name)` inline when a player joins a room
with an active game, and `HandleChangeRoom` calls `RemovePlayerFromRoom`/`AddPlayerToRoom` — meaning
room/player operations and game-action processing are not independent subsystems that happen to
share a process; they touch each other directly, synchronously, today.

**Protocol implications for `GameServerMain` (following from the above):**
1. **The `HandleAction`-equivalent must stay a blocking round trip.** The server can't fire an
   action at the game process and go service other sockets — at minimum, whatever room/game
   instance sent it must be fully blocked until the response returns.
2. **The wire protocol needs an explicit "action complete" boundary**, distinct from the stream of
   `OutputPort` events. Today, `UniCast`/`BroadCast`/`VariCast` happen inline inside `HandleAction`'s
   single call stack, so there's no existing signal for "still working" vs. "done" — over a wire,
   output events arrive as async messages *while* the server waits on the action's response, so a
   terminating sentinel (or equivalent framing) is required or the server can't tell an in-progress
   cascade from a finished one.
3. **The same blocking-round-trip requirement applies to every other `Game`-facing call** — not
   just `HandleAction`: `SendFullState`, `GetStatusString`, `IsDone`, `Load`, `Save` are all called
   synchronously today too.
4. **Open question, not yet decided:** does the interim design preserve the exact global invariant
   (the whole server blocks during any one room's in-flight action, exactly like today), or relax
   to per-room serialization (block only that room; let unrelated rooms' round trips proceed
   concurrently)? Per-room-only looks safe on the *game* side in isolation — different rooms have
   fully separate `Game` instances with no shared state between them — **but the room/player-
   bookkeeping wrinkle above raises the bar for that relaxation significantly**: `AddPlayerToRoom`/
   `RemovePlayerFromRoom`/room creation touch cross-room shared state (`m_Rooms`, `m_Inhabitants`)
   *and* reach into a specific room's `Game` (the `SendFullState` call on join). Allowing per-room
   concurrency means explicitly deciding what happens if a player joins/leaves a room while that
   room's game has an action in flight — a scenario that has never been possible, and so has never
   been exercised or defended against, by any existing hand-written game code. Preserving the exact
   global invariant sidesteps all of this and is the safer default for the interim step; relaxing it
   should be a deliberate, later decision, not a byproduct of how the wrapper happens to get written.

**Resolution for the game side of that open question (Albert, 2026-09-18): process-per-instance
makes cross-room concurrency safe "for free," structurally.** If the process model is one game
process per game *instance* (not per game *type* serving many rooms), there is no shared memory
between any two rooms' game state at all — they're different address spaces. The concurrency-naive
embedded C++ is then protected not by any scheduling discipline, but by simple structural fact:
there's nothing else in that process to race against, as long as `GameServerMain`'s own
request-handling loop stays single-threaded and blocking per connection (trivial to guarantee — it
just never needs worker threads). Two things this does *not* automatically resolve, though:
- **The room/player-bookkeeping risk is orthogonal and unaffected.** `RoomManager`'s `m_Rooms`/
  `m_Inhabitants` and the `AddPlayerToRoom`→`SendFullState` call live entirely in the *server*
  process, not any game process — this decision doesn't touch them. They stay exactly as safe as
  today for exactly the same reason as today: the server's own main loop remains single-threaded,
  independent of how many game processes exist.
- **Safety is not the same as realized throughput.** If the server's own main loop still blocks
  fully on one outstanding game round trip before touching any other socket (the simplest thing to
  build first), you get exactly today's fully-serialized behavior — nothing gained yet structurally,
  but nothing lost either. Actually getting cross-room concurrency would additionally require the
  server's *own* loop to support multiple outstanding round trips at once (real async I/O or
  multiplexing server-side) — separate, later work. This decision just makes attempting that later
  safe, rather than making it happen now.

**Ultimate destination, kept in view (Albert, 2026-09-18): eventually run game-state handling as an
AWS Lambda responding to Game Actions via API Gateway.** Not a near-term concern, but it reframes
why the process-per-instance/single-writer discipline above matters beyond just the interim step.
Albert's framing is explicitly Hadoop-shaped: the game logic (a mapper/reducer, in that analogy) is
written naively, assuming it's the only thing operating on its data — exactly what today's
concurrency-naive embedded C++ already assumes — and it's the *runtime engine*'s job to guarantee
that invariant actually holds (partitioning work, and ensuring no two workers touch the same
partition concurrently) while still running many partitions' work in parallel. That maps directly
onto this investigation: "one process per game instance" is just today's mechanism for providing
that single-writer guarantee (OS process isolation); a Lambda-based future would need to provide
the *same* guarantee through a different mechanism (a per-instance lock/lease, optimistic
concurrency on the persisted state, etc.), while different instances' invocations still run fully
in parallel. **The payoff: the same concurrency-naive game logic could, in principle, ride
unchanged all the way from in-process DLL → `GameServerMain` executable → eventual Lambda handler,
as long as whatever hosts it keeps providing "exactly one active writer per instance at a time."**
One forward-looking divergence worth naming now even though it's explicitly not today's problem:
Lambda invocations are stateless/ephemeral between calls, so that future move requires game state to
be persisted externally and rehydrated per invocation — a real departure from today's model (and
the interim `GameServerMain` model), where state simply lives in a long-running process's memory
across many actions.

**The concrete mechanism Albert has in mind for that Lambda future (2026-09-18): optimistic
concurrency via CAS writes against a NoSQL store (e.g. DynamoDB), not a lock/lease.** Read the
current persisted state (with its version/condition token) into memory, run the naive,
unchanged state-transition logic against that in-memory copy exactly as today, then attempt a
conditional ("compare-and-swap") write back. On CAS failure, don't merge or patch — **restart the
whole invocation from scratch**: re-read current state, re-run the transition, re-attempt the
write. Albert's argument for why this is a good fit here rather than a contention nightmare: unlike
general-purpose optimistic-concurrency use cases, this application already has the property (borne
out by everything read in `Transition.pm`/`IsLegal`'s per-player gating) that only the players of a
given game instance may make *any* change to its state, and typically only one specific player is
even legally allowed to act at a given moment — so true concurrent writers to the *same* instance's
state should be rare in practice, making the occasional CAS-retry cost worth avoiding the cost/
complexity of a lock or lease service entirely.

**Open implication this raises, not yet resolved:** if a CAS write fails and the whole invocation
restarts, what happens to the `OutputPort` events (`UniCast`/`BroadCast`/`VariCast`-equivalent) the
*failed* attempt already produced before its write was rejected? Ties directly to the
"action-complete" framing question raised earlier for the interim `GameServerMain` protocol — the
natural answer is that output events must be buffered/held and only actually delivered to players
*after* the CAS write commits successfully, with a failed attempt's buffered events discarded
entirely before re-running from fresh state. Otherwise a retried transition risks giving players two
different, inconsistent narrations of "what just happened," one of which was silently thrown away.
Worth deciding deliberately when this mechanism is actually designed, not left implicit.

**Bigger move, prompted by the output-buffering question above: replace `UNICAST`/`BROADCAST`/
`VARICAST` and embedded per-piece state entirely with a generic data-model + engine-broadcast
design (Albert, 2026-09-18).** Instead of game code calling `Unicast`/`Broadcast`/`Varicast`
imperatively mid-transition (today's model), and instead of state living scattered across
hand-written `<Name>Set`/`<Name>State` piece classes, the mechanism becomes:
`<read data model into memory>` → `<validate move legality on data model>` → `<transition updates
data model>` → `<engine attempts CAS write of model>` → `<engine broadcasts new data model to all
interested parties>`. Legality-checking and transition logic become pure functions over the data
model with **no side effects at all** — delivery becomes solely the generic engine's job, done once,
after a successful commit.

This doesn't just answer the CAS/output-buffering question above, it dissolves it: since nothing is
emitted during computation, a failed CAS attempt has nothing to buffer or discard — it just throws
away a scratch value and re-reads. It also independently resolves the `VariCast`
predicate-portability wrinkle noted earlier (delivery-targeting logic moves generically into the
engine, no game-specific predicate needs to cross any boundary), and shrinks the language-porting
cost identified in the `transitioncompiler` findings — the entire `Event`/`GlobalEvent`/
`ManualEvent`/`PrivateEvent`/`SourceEvent` message-crafting machinery (`Unicast<Event>`/
`Broadcast<Event>`/`Make<Event>Message` generation) becomes unnecessary for a ported game to
reimplement at all.

**Three open questions this raises, and Albert's answers (2026-09-18):**

1. **Hidden and per-player-*different* data — the hardest piece, by Albert's own estimate.** Both
   Outpost and MoV have fields hidden from some players (e.g. secret hands) *and* fields that are
   genuinely different per viewer, not just redacted (e.g. the `IAM` message telling each player
   their own player index). So this isn't only an ACL/redaction problem (hide field X from player
   Y) — it's also a personalization problem (field X's value depends on who's asking), and any
   mechanism needs to support both, not just masking. **Hard constraint, non-negotiable per
   Albert:** this must be computed and enforced entirely server-side/engine-side. Shipping the full
   data model to every client and trusting it to filter what it displays is basic-infosec-101
   unacceptable ("send the whole state, and client, pinky-swear that you won't show the player some
   of it"). Still genuinely open which mechanism carries this (model-level visibility/personalization
   annotations the engine applies mechanically, vs. the transition step computing per-player views
   directly) — but wherever the logic lives, it must execute on the trusted side of the boundary.
2. **Game state must be hierarchical — confirmed, and this is where `StateWalker` fits.**
   `StateWalker` operates at an outer, generic layer (current FSM state name+desc, legal actions per
   viewer, error/message — i.e. what today's four built-in `ManualEvent`s carry) — what a specific
   game considers "the game state" (Outpost's board/factories, MoV's map/ships/etc.) is a **child**
   of that outer layer, opaque to the generic engine/`StateWalker` machinery.
3. **The `<auto>` cascade mechanics are unchanged; only the persistence *atomicity* is new —
   confirmed.** `StateWalker` still chains as many transitions as needed, sequentially, each
   operating on the data model left by the previous one, exactly as `ExecuteAuto` does today, until
   landing in a state with no further auto-transitions. What's new: the engine wraps the **entire
   chain** (the player-triggered transition plus every subsequent auto-transition) in **one atomic
   CAS write** — either the whole chain commits, or it all fails and the whole chain re-runs from
   the original pre-chain state (not a partial/per-transition commit, not a resume-partway retry).
   One CAS write and one broadcast per player-initiated action, never one per intermediate auto-hop.

**How to apply:** if you're continuing this investigation — the recommended path is the interim
step above (process decoupling via `GameServerMain.hpp`, keeping every game in C++ unchanged)
before tackling language decoupling. The steady-state protocol (action-in/event-out/
load-save-by-filename) is basically already designed by the existing `Game`/`OutputPort`
interfaces; remaining open design work is (1) picking the transport/framing, (2) how `CreateGame`'s
init-time references become a handshake message + a bidirectional RPC/callback channel, with an
explicit action-complete boundary per the synchronicity invariant above, (3) how to replace
`VariCast`'s predicate object with the player-list-plus-repeated-UniCast pattern described above
(this lives once in `GameServerMain.hpp`, not per-game), (4) the process-per-room vs.
process-per-game-type fork, and (5) whether to preserve the exact global single-threaded invariant
or relax to per-room serialization — default to preserving it unless there's a specific reason to
take on the room/player-bookkeeping entanglement described above. Only if/when language decoupling
is pursued on top of this does a new
implementation need its own `StateWalker`-equivalent and hand-ported game rules, per the
`transitioncompiler` findings above — that cost is unrelated to and unlocked independently of the
process-decoupling step.
