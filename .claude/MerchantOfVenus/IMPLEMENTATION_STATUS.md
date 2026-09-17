# MerchantOfVenus Implementation Status

Living status doc — update this as more gets implemented. Last full survey: 2026-09-12.
Scope of this survey: server-side only (`MerchantOfVenus/*.hpp`, `MerchantOfVenus/*.cpp`,
`MerchantOfVenusServer.xml`). The `client/` subdirectory (Java GUI) was **not** examined —
that's a separate pass.

**Terminology note:** the physical game's rulebook term is **"Culture"** (e.g. Nillis, Volois,
Humans, etc.). The code instead calls this field **`Race`** (`Player::GetRace()`,
`TradeBase::GetId()` compared against it, etc.) — same concept, different name. When
grepping/searching this codebase for culture-related logic, search for `Race`, not `Culture`.

## Bottom line

The server-side rules engine is **surprisingly close to complete**, not ~30% remaining as
might be expected from "70% done." Every transition in `MerchantOfVenusServer.xml` has a
real, non-stub body (no `TODO`/`FIXME`/stub markers found anywhere in the `.cpp`/`.hpp`
files). The core turn loop — join/setup, tech switches, movement (dice, pilot numbers,
mulligan, telegates, backtrack prevention, drives/Air Foil/Auto Pilot effects), the full
trade phase (buy/sell/pickup/dropoff/jettison/trade-in-ship/redeem-IOU, demands, factory
& station commissions, factory/spaceport deeds), relic/asteroid handling, secrecy/discovery
of solar systems and qboxes, turn advancement, and the win condition — all appear fully wired
end-to-end, including a comprehensive `refresh` body for reconnecting/observing clients.

**As of 2026-09-12, every open question from the initial survey has been resolved with Albert**
(implicit `InitialState`/`TerminalState`, the implicit `RESET` event, the intentional 6-race
`STARTGAME` list, and the optional/never-implemented-by-design combat ruleset — see below).
Nothing outstanding was found in the server-side rules engine itself. What plausibly remains
to reach "complete" is the entirely-separate client GUI work — **still to confirm with Albert**
whether that's what the earlier "70%" estimate was tracking.

## FSM States

Declared in XML: `ValidateSetup`, `StartTurn`, `TechSelect`, `TechSelected`, `SelectMove`,
`Mulligan`, `ProcessPilotNumber`, `SelectPilotNumber`, `FinalizeMove`, `CompleteMove`,
`TradePhase`, `AutoTradePhaseSelect`, `AsteroidPhase`, `TurnEnd`.

Referenced but **never declared** with a `<state>` tag: `InitialState` (used as the game's
starting state) and `TerminalState` (used as the game-over state). These are presumably
framework-provided implicit states (see Open Questions).

## Transitions

All 24 transitions below have real bodies (or intentionally no body — see notes). None are
stubs.

| Transition | From → To | Gate | Status |
|---|---|---|---|
| JOIN | cyclic InitialState | allowed | done |
| UNJOIN | cyclic InitialState | allowed | done |
| OPTIONS | cyclic InitialState | allowed | done (sets `m_winmoney` only — see Board State notes) |
| VALIDATESETUP | InitialState → ValidateSetup | allowed | done |
| DONTLIKEOPTIONS | ValidateSetup → InitialState | allowed | done (no body needed — no state to mutate) |
| LIKEOPTIONS | cyclic ValidateSetup | allowed | done |
| STARTGAME | ValidateSetup → StartTurn | auto | done — see Open Questions re: hardcoded race list |
| GOTOTECHSELECT | StartTurn → TechSelect | auto | done |
| SELECTSWITCHABLES | TechSelect → TechSelected | allowed | done |
| SKIPTECHSELECT | StartTurn → TechSelected | auto | done |
| GOTOCHOOSE | TechSelected → SelectMove | auto | done |
| ENDMOVE | SelectMove → AutoTradePhaseSelect | auto + allowed | done |
| SELECTDESTINATION | SelectMove → Mulligan | allowed | done |
| CANTMULLIGAN | Mulligan → ProcessPilotNumber | auto | done (no body needed) |
| MULLIGAN | Mulligan → ProcessPilotNumber | allowed | done |
| NOMULLIGAN | Mulligan → ProcessPilotNumber | allowed | done |
| REJECTDESTINATION | ProcessPilotNumber → SelectMove | auto | done |
| AUTOPILOT | ProcessPilotNumber → FinalizeMove | auto | done |
| GOTOSELECTPILOTNUMBER | ProcessPilotNumber → SelectPilotNumber | auto | done |
| SELECTPILOTNUMBER | SelectPilotNumber → FinalizeMove | allowed | done |
| EXECUTEMOVE | FinalizeMove → CompleteMove | auto | done (largest body — movement, discovery, Spy Eye, commissions) |
| AUTOCHOOSEDEST | CompleteMove → ProcessPilotNumber | auto | done |
| MANUALCHOOSEDEST | CompleteMove → SelectMove | auto | done (no body — `DESTINATION` event was already sent by EXECUTEMOVE's body) |
| ISTRADEBASE | AutoTradePhaseSelect → TradePhase | auto | done |
| ISRELIC | AutoTradePhaseSelect → AsteroidPhase | auto | done (no body needed) |
| INSPACE | AutoTradePhaseSelect → TurnEnd | auto | done (no body needed) |
| GETRELIC | AsteroidPhase → TurnEnd | allowed | done |
| LEAVERELIC | AsteroidPhase → TurnEnd | allowed | done (no body needed) |
| REDEEMIOU | cyclic TradePhase | allowed | done |
| TRADEINSHIP | cyclic TradePhase | allowed | done |
| BUY | cyclic TradePhase | allowed | done (handles factory/spaceport deed side effects; **confirmed:** applies the 20% own-culture discount on Deed purchases — `buycost -= buycost/5` when `curp.GetRace() == tb.GetId()`, XML lines ~1161-1166) |
| PICKUP | cyclic TradePhase | allowed | done |
| DROPOFF | cyclic TradePhase | allowed | done |
| JETTISON | cyclic TradePhase | allowed | done |
| SELL | cyclic TradePhase | allowed | done (handles demand bonuses) |
| ENDTURN | TradePhase → TurnEnd | allowed | done |
| GAMENOTOVER | TurnEnd → StartTurn | auto | done |
| GAMEOVER | TurnEnd → TerminalState | auto | done |

`refresh` body: present and comprehensive — handles every state including the TechSelect/
move-family states and TerminalState. Looks complete.

## Events

All events declared in the XML's `<event>` blocks were confirmed emitted somewhere in the
transition bodies or the `refresh` body (cross-checked, not just declared-and-forgotten):

`MOVERROR`, `IAM`, `PLAYER`, `OPTIONS`, `PLAYEROKSTARTUP`, `TURNORDER`, `PLAYERMONEY`,
`PLAYERRACE`, `ADDTOPLAYER`, `SUBTRACTFROMPLAYER`, `ADDTOTRADEBASE`, `SUBTRACTFROMTRADEBASE`,
`SOLARSYSTEMPRIVACY`, `BASELOCATION`, `QBOXPRIVACY`, `QBOXCONTENTS`, `PLAYERLOCATION`,
`SWITCHABLES`, `SELECTEDSWITCHABLES`, `DESTINATION`, `DICE`, `MPLEFT`, `PLAYERWINS`,
`CURPLAYER`, `PILOTNUMBERCHOICE`, `PILOTNUMBERCHOSEN`, `REMAININGTRANSCOUNT`,
`FACTORYOWNER`, `AGEPLAYER`.

**Anomaly:** `RESET` is broadcast/unicast in several places (`UNJOIN`, and the `refresh` body)
but has **no `<event name="RESET">` declaration** anywhere in this XML. Either it's a
framework-provided built-in event (same category as the undeclared `InitialState`/
`TerminalState`), or it's a real gap. Flagged as an open question below.

## Fixed 2026-09-13: space cities incorrectly exempt from one-buy/one-sell limit

`MerchantOfVenusState::HasRemainingBuy()`/`HasRemainingSell()` exempted any `m_type == STATION`
space from the one-buy/one-sell-per-landing rule. `STATION` was overloaded for two different
things: (a) true Starbases (random qbox "Open Port" tokens, and player-bought Spaceport Deeds
via `MapOverlay::PlaceStation`) which *should* be unlimited, and (b) 6 fixed, pre-printed
"space city" locations (`Smoke Ring`, `Galactic Base`, `cloud_body_3`, `MGS_loop_3`, `AS_3_1`,
`AS_6_3`) which — after Albert and a fellow player re-read the rulebook correctly — should be
subject to the *same* one-buy/one-sell limit as land cities (`type="city"`). Movement cost was
unaffected by this bug: `MoveMediator`'s 2-MP-to-enter-a-city rule only checks `m_type == CITY`,
so these fixed stations already correctly cost 1 MP like a real Starbase — that part of the
distinction (land city = 2 MP, space city = 1 MP) was already right.

**Fix:** added a new `SpaceType` value, `SPACECITY` (`MapSpace.hpp`, parsed/stringified in
`MapData.cpp`, `MerchantOfVenusMap.xml` type string `"spacecity"`), used for exactly those 6
fixed locations (including Galactic Base, per Albert's explicit call). It behaves like `CITY`
for the buy/sell limit (no exemption — `HasRemainingBuy`/`HasRemainingSell` needed no code
change, since they only special-case `STATION`) and like `STATION` for movement cost (no
special-case in `MoveMediator`, since only `CITY` triggers the 2-MP cost). Updated the XML's
`ISTRADEBASE` and `INSPACE` auto-conditions and the IOU-reveal-on-landing check in the
`EXECUTEMOVE` body to treat `SPACECITY` alongside `CITY`/`STATION` (entering trade phase,
not counting as "in space", and revealing/redeeming IOUs) — these three needed the explicit
addition since they enumerate types rather than defaulting.

**Outstanding:** `MerchantOfVenus/tca/` (transitioncompiler's generated C++ output — NOT
source, do not hand-edit or query it) still reflects the *old* XML and needs a
`transitioncompiler` re-run to regenerate before this fix takes effect in an actual build.

## Board State / Components

Very rich data model — this is not a shallow implementation:

- **Players**: name, id, money, barter, race, location, per-turn buy/sell counts, full token
  inventory, `Advances` (tech).
- **Tokens**: ships, goods (incl. factory goods), fares, demands, relics, drives (red/yellow/
  combined), equipment (shield, laser, nova ball), deeds (factory/spaceport), IOUs — each with
  buy/sell price, sell type (cash/barter/none), sell destination (cup/culture/nowhere), and a
  wire `Encoding()`.
- **TradeBases**: all 14 habitable cultures hard-coded by name/race-id/technology-tier/goods
  (matches what looks like the real game's culture roster: Nillis, Volois, Graw, Niks, Dell,
  Humans, Shenna, YXKLYX, Zum, EeepEeep, Whynoms, Cholos, Wollow, Qosssuth) plus Galactic Base.
- **Cup**: pre-seeded with a specific hard-coded set of fares/demands per culture (looks
  hand-transcribed from the physical game's cup contents).
- **MapOverlay / MapData**: full board graph loaded from `MerchantOfVenusMap.xml`, with
  qbox randomization (telegates, penalty spaces of 3 colors, asteroids w/ relics, open ports),
  station/factory placement, and per-player secrecy tracking for solar systems and qboxes.
  `MapData`'s constructor does real graph-consistency validation (adjacency symmetry, flyable
  links only between same-region cities, nav-circle pilot numbers 1-6 all present, etc).
- **MoveMediator**: dice rolling (incl. Auto Pilot's virtual "4"), pilot-number/backtrack/
  telegate culling, movement-point accounting, mulligan.

Only one game-setup **option** is modeled (`winmoney`, the net-worth win threshold) — if the
physical game has more setup variants, those aren't represented, though this may be
intentional simplification rather than a gap.

## Possible gaps (inference — not confirmed against the physical rulebook)

- Nothing outstanding — see "Combat / pirates" resolution below. No other component-level gap
  stood out from this survey.

## Resolved: Combat / pirates (optional ruleset) — NOT a gap, by design

Merchant of Venus has an **optional player-vs-player combat ruleset** (forts, buying Nova
Balls/lasers, attacking ships, stealing cargo, inciting starbase/factory revolts) that Albert
has never played and deliberately has not implemented, while keeping the door open to add it
later without a retrofit. Verified against the code:

1. **Buyable from cultures, tech-tier-gated:** `TradeBases.cpp::MakeBase()` — Industry-tier
   cultures sell Laser, Technology-tier sell Nova Ball, Bioengineers-tier sell Shield.
2. **Laser/Nova Ball are mechanically inert; Shield is not:** `Token::GetAttackValue()` is set
   and serialized (`Encoding()`, for client display) but **never read** by any rules logic —
   no combat resolution consumes it anywhere in the codebase. Shields are different:
   `Advances::GetShieldStrength()` sums `GetDefenseValue()` across a player's inventory, and
   `MoveMediator::GetPenaltyCost()` subtracts that from Penalty Oval landing costs — so Shields
   have real utility (movement-penalty reduction) despite the absent combat system.
3. **Relic Shield / Relic Laser can appear on asteroids:** `MapOverlay.cpp::MakeRelicList()`
   includes `Token::Shield(true,60)` and `Token::Laser(true,100)` alongside the other 8 asteroid
   relics, shuffled together and seeded onto asteroid qboxes; `GETRELIC` just adds whatever
   relic is present to the player's inventory, no special-casing. Both relic variants have
   `selldestination = CULTURE` (barterable) — but only Relic Shield does anything if *kept*
   (feeds `GetShieldStrength()`); keeping Relic Laser is a no-op.

**Bottom line on this item:** not unfinished work, just an unimplemented optional module,
correctly left as inert-but-present data (attack values, buyable weapons, relic weapon) rather
than half-wired combat logic. Revisit only if/when Albert decides to actually implement the
optional combat rules.

## Relics — full validation (2026-09-17)

All 10 relics (`MapOverlay.cpp::MakeRelicList()`) individually validated against their specific
rules, relic by relic, with Albert. One real bug found and fixed along the way (Jump Start); the
other nine are confirmed correct as-is. Background mechanics this all rests on:

- **Nav Circles / pilot numbers:** a space's `m_pilotdirs[7]` array (only meaningfully set on
  plain, uncolored "blue dot" spaces — confirmed never on a Penalty oval or qbox) tags specific
  adjacent directions with pilot numbers 1-6. `MoveMediator::AddAdjacents()` reads this to tag
  `Dest`s; `CullByPilotNumber()` restricts destinations to the player's chosen/rolled pilot
  number(s) once dice are visible. A destination with an empty `pnumbers` set is always kept
  regardless of any pilot-number lock — free choice at ordinary (non-junction) dots is correctly
  unaffected by pilot-number mechanics.
- **Pilot-number "permanence":** once a player commits to a pilot number (`m_pnumber != 0`), it's
  locked for the rest of the turn's movement (`IsPilotNumberPermanent()`), restricting further
  Nav Circle choices to that same number — this is the mechanism Switch Switch (below) lifts.
- **Telegates dynamically become a Nav Circle:** `MoveMediator::AddTelegates()` — when standing on
  a telegate (or, for Jump Start, on the first move regardless of position), every visible
  telegate elsewhere becomes a jump destination tagged with its own pilot number, and normal walk
  destinations get retagged with whichever numbers aren't claimed by a telegate (dropped entirely
  if none are left). The telegate a player is standing on is always excluded from its own jump
  list, unconditionally, regardless of Jump Start.
- **Blind first move (rulebook 5.3):** "the player must specify the first dot he will move to
  before he rolls any dice" is implemented not by delaying `StartMove()`'s actual RNG call, but by
  hiding the roll's *values* — `GetDiceString()` renders `"?"` per die until `MakeDiceVisible()`
  fires (inside `SELECTDESTINATION`, the first time it's called that turn). If the player's blind
  guess doesn't survive dice-based pilot-number culling once revealed, `REJECTDESTINATION`
  auto-fires and bounces them back to `SelectMove` with a freshly recomputed, now dice-aware list.
  Confirmed this is a correct implementation of 5.3, not a bug.

Per-relic findings:

| Relic | Confirmed behavior |
|---|---|
| **Shield / Laser / Yellow Drive** (equipment-type relics) | `Token::Drive/Shield/Laser`'s `i_isRelic` flag sets `SizeClass=RELIC_SIZE` ("always 0 units") instead of `EQUIPMENT_SIZE`/`DRIVE_SIZE`; `Player::GetStuffSize()` correctly counts `RELIC_SIZE` as 0 toward ship-hold capacity, and `GETRELIC` doesn't even check capacity. Defense/attack/skip-color values are identical to the purchasable versions — functionally equivalent, just free of hold space. Sell destination (`CULTURE`, not `NOWHERE`) and `m_isunlimited=false` also correctly modeled: a sold relic remains listed at that culture until someone else buys it (a unique physical piece changing hands), unlike unlimited manufactured equipment.
| **Air Foil** | `AddAdjacents()`: unlocks `flyable`-tagged adjacencies (map convention: only ever between two cities on the same planet). `ApplyMovementPoints()`: removes the city entry/exit MP surcharge (`mpused` stays at the baseline 1 instead of jumping to 2). Without it, flyable links aren't hidden-but-costly — they don't appear as options at all.
| **Auto Pilot** | `StartMove()`: forces one die to a guaranteed 4 (`m_dicerolled.push_back(4)`), rolling one fewer real die. Confirmed interaction with Mulligan Gear below.
| **Mulligan Gear** | One reroll per turn, by *value* not position (`DoMulligan()` rerolls the first die matching the named value). Offered in the `Mulligan` FSM state, positioned specifically *before* `ProcessPilotNumber`'s validation of the player's blind first-move guess — meaning it can rescue a first guess that would otherwise get `REJECTDESTINATION`-bounced. Confirmed correct per rule text ("can choose any one of the dice ... including the Auto Pilot ... cannot choose to activate Auto Pilot as his reroll" — the Auto-Pilot-forced die is legitimately targetable, and `DoMulligan` always performs a genuine `DieRoll()`, never re-forces a 4).
| **Spy Eye** | `EXECUTEMOVE` (fires every hop, not just once/turn): reveals an inhabited solar system's culture from any space in it (not just an orbit spot), and auto-reveals every not-yet-known adjacent qbox on arrival. Confirmed it also correctly lets a player redirect after peeking at a colored penalty dot they're only "skipping" (0 MP) via a matching drive — since `ApplyMovementPoints()`'s color-discount only ever changes cost, never removes a destination from the list, that dot was always a normal, fully choosable stop to begin with; no special Spy-Eye-specific code was needed or is missing.
| **Switch Switch** | `IsPilotNumberPermanent()`/`CullByPilotNumber()`: lifts the pilot-number lock, so `activepnum` is the player's *full* rolled set at every subsequent Nav Circle, not just the one number they first committed to. Not a distinct player action — it's implicit in the normal per-hop destination list simply containing more options (each tagged with a different rolled value), and picking one naturally reassigns `m_pnumber` via the ordinary `SetPilotNumber()` flow.
| **Gate Lock** | `AddTelegates()`'s gate (`curspace.m_type==TELEGATE && !gl`): suppresses the entire "telegate becomes a Nav Circle" mechanic while active — no jump options, and (since `gl` is a switchable, fixed for the whole turn and re-checked fresh every call) *every* telegate the player encounters that turn is inert, matching "a non-functioning telegate acts as a blue dot" without needing separate persistent state. `ManualStop()` already treats `TELEGATE` identically to `DOT` (both non-stoppable mid-move) independent of Gate Lock, which is already consistent with "acts as a blue dot."
| **Jump Start** | **Bug found and fixed 2026-09-17.** Reuses the telegate-becomes-Nav-Circle logic for the turn's first move regardless of current position (`js && m_first`). Missing piece: per the rule, the first (pre-dice-visible) move must be restricted to Telegates only if any are visible (falling back to normal walking if none exist) — the code was instead including normal walk options in that blind list too. **Fixed** in `MoveMediator::AddTelegates()`: a new branch (`if (js && m_first && !AreDiceVisible()) { m_dests = jumpdests; return; }`) wholesale-replaces the destination list with telegates-only during the blind phase; once dice become visible, `PrepareForStep()` re-runs `AddAdjacents()` from scratch and the existing combined-list logic naturally recomputes (not filters) the full walk+jump set — matching "if you don't roll the pilot number for your chosen Telegate, but did roll one for another, you must prefer a Telegate over walking only up to the point dice are revealed, after which normal walking is a legal fallback." Confirmed self-exclusion (never a destination to itself) stays unconditional even under Jump Start. Also confirmed (new test `TestAddTelegatesJumpStartFromCityMPCost`) that a Jump Start telegate hop launched from a ground city already correctly cost 2 MP (1 with Air Foil) with zero code changes needed — `ApplyMovementPoints()`'s city surcharge already applies uniformly to jump destinations, not just walks.

## Open questions for Albert (surfaced, not guessed at)

1. ~~`InitialState`/`TerminalState` undeclared~~ **RESOLVED:** these are auto-added by the
   state-engine machinery generated at DLL compile time, not authored per-game. `InitialState`
   is where the `StateWalker` points on gameplay instantiation; entering `TerminalState` tells
   the state engine no further actions will ever occur on that gameplay. Not a gap.
2. ~~`RESET` event undeclared~~ **RESOLVED:** `RESET` is one of several events auto-created by
   the event compiler; most such implicit events are internal core-server↔core-client plumbing,
   but `RESET` is the one exception game code is allowed to use directly. Not a gap.
3. ~~`STARTGAME` hard-coded 6-race list~~ **RESOLVED:** intentional and matches the physical
   game — Merchant of Venus ships with six player mats, each printed with one specific culture
   (including that culture's purchase discount), so players can only ever be one of those six
   cultures, never any of the other eight in the full roster. Not a gap.
4. ~~Combat/pirates~~ **RESOLVED:** optional ruleset in the physical game, never implemented
   by design (to avoid a future retrofit), verified consistent with the code — see the
   dedicated section above. Not a gap.
