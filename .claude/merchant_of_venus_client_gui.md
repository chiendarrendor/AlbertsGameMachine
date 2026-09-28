---
name: merchant-of-venus-client-gui
description: "MerchantOfVenus-specific client GUI architecture (hand-painted board, custom floating-panel windowing, client-derived trade legality) — concrete illustration of, and sharp contrast with, the generic client architecture in component-java-client and Outpost's declarative style in outpost-client-gui"
metadata:
  type: project
---

Written 2026-09-27/28, continuing the same client-architecture deep-dive session as
[[component-java-client]] and [[outpost-client-gui]]. Where those files describe the generic
framework and Outpost's (mostly declarative) use of it, this file is entirely about how MoV's client
uses that same framework almost entirely by hand instead. Cross-reference both for the underlying
mechanisms this file assumes (`ActionTransferManager`, `GameNode`, the wire-dispatch chain).

**The whole client is (from the XML's point of view) one `<javaclass>`.**
`MerchantOfVenusClient.xml` declares a full event schema (~27 events) and a full action list
(~20 actions, **every single one** `onstate="statedalwayshidden"`) but exactly one visible node:
`<window title="Merchant Of Venus" ...><javaclass classname="MerchantOfVenus"/></window>`. Real size:
`MerchantOfVenus/client/MerchantOfVenus/` is ~7600 lines across 45 files — a large, complete, custom
Swing application, not a thin wrapper.

## Manual, asymmetric event fan-out (contrast with `GameNode`'s uniform recursive walk)

`MerchantOfVenus.HandleEvent` hand-calls `HandleEvent` on 6 fixed children in a fixed order
(`plData`/`baseData`/`spaceData`/`dbo`/`dd`/`cp`) — a hand-rolled reimplementation of "broadcast, let
each decide relevance," not the generic `GameNode` tree-walk. One level down, `ControlPanel.
HandleEvent` does the same for only **6 of the 10** panels it actually holds (`ipanel`/`switches`/
`eog`/`mulliganPanel`/`pilotNumberPanel`/`tradePanel`) — the other four (`jfp`/`cfp`/`cmp`,
`relicPanel`... actually `relicPanel` *is* wired in construction but not fanned via `HandleEvent`
either) are shown only via **direct method calls from a sibling widget** (`ControlPanel.
raiseJumpFly`/`raiseCostlyFirst`/`raiseCostlyMove`, called by the board's own click handler — see
below), and hidden via one ad hoc inline check at the top of `ControlPanel.HandleEvent`
(`NEWSTATE` → `jfp.lower(); cfp.lower(); cmp.lower();`). So MoV's dispatch tree mixes wire-event-driven
nodes and peer-triggered nodes side by side, decided per-widget by whoever wrote it — there is no
single uniform mechanism the way `GameNode` provides one generically.

## Every action hidden by design — hand-built panels drive `ActionTransferManager` directly

Since every action is `statedalwayshidden`, `GameXMLWalker`'s own auto-built `ActionTabNode`/
`ActionNode`/`GoButton` UI is constructed (still doing real work internally — assembling
`{actionName, vars...}`, calling `SendAction`) but **never shown to the user.** Real UI panels drive
that hidden machinery by hand, using the exact same public seams the framework's own widgets use —
this isn't a bypass, it's the identical mechanism operated manually:
- **Legality → visibility:** a hand-written panel implements `ActionGenerator` directly and
  self-registers via `atm.AddButtonPresser(actionName, this)` (the same call `GoButton` makes), then
  wires `ActionLegalityChanged(boolean)` to whatever "become visible" means for that panel — usually
  `setVisible(...)`, but see `DynamicBoardOverlay.MoveManager` below, where it drives a paint flag
  instead. Concrete examples: `DieChoicePanel` (reused generically for **both** `MULLIGAN` and
  `SELECTPILOTNUMBER`, parameterized purely by constructor args — the same "one class, many action
  names" reuse trick as Outpost's `StateSensitiveTextBox`), `RelicPanel` (`GETRELIC`), `SwitchablePanel`
  (`SELECTSWITCHABLES`), `InitialPanel`'s inner `DoButton` (`JOIN`/`UNJOIN`/`VALIDATESETUP`/
  `LIKEOPTIONS`/`DONTLIKEOPTIONS`, one small reusable inner class per button).
- **Firing an action:** manually replicates `GoButton`'s two-step dance —
  `atm.ReportNewValueToConsumers(varName, value)` (as if some `ActionVariableGenerator` produced it)
  immediately followed by `atm.ButtonPressed(actionName)` (exactly what `GoButton.actionPerformed`
  calls). Every hand-built trigger in this client does this same pair of calls, whether it's a die-face
  button, a relic pickup/leave choice, a trade-item click, or a board click.
- **A nice secondary use of the same callback:** `RelicPanel.ActionLegalityChanged` does double duty
  — toggling visibility *and*, only when becoming legal, reaching into `PlayerData`/`OverrideSpaceData`
  to look up and display the actual relic icon at the current player's location. "Just became legal"
  is a convenient, reliable trigger for a content refresh too, not just a show/hide switch.

## The board: one hand-painted canvas, not a widget tree

`DynamicBoardOverlay` overrides `paintComponent` and draws everything itself with `Graphics2D` —
space-override icons, player ships (fanned out in a circle if several share a space), destination
highlights — and does its own hit-testing on raw mouse clicks via
`NearestSpaceSelector.GetNearestSpace(x,y)` (pixel → nearest board space). A genuinely different
widget-authoring style than anything in Outpost, where every interactive element is a discrete Swing
component.

**`MoveManager`** (an inner class) is the click handler and the source of `raiseJumpFly`/
`raiseCostlyFirst`/`raiseCostlyMove`: registers three separate inline anonymous `ActionGenerator`s
(`SELECTDESTINATION`/`SELECTPILOTNUMBER`/`ENDMOVE`) purely to track three visibility-adjacent
booleans, each toggle just triggering `repaint()`. `mouseReleased` finds the nearest board space and
branches: clicking your own space while `ENDMOVE` is legal ends your turn (with a `raiseCostlyFirst`
confirmation detour if stopping there costs a penalty on your first move); clicking a currently-legal
destination builds a small `Functor` callback (`ExecuteMoveToNextFunctor`) encapsulating "commit to
this move," executed immediately or, if the move has a penalty and isn't your first, only after
`raiseCostlyMove`'s confirmation panel accepts it. The functor itself resolves jump-vs-fly ambiguity
(`Destinations.Destination.IsJump() == -1` → pop `raiseJumpFly` and let *that* panel decide; otherwise
fire `SELECTDESTINATION` directly) — ties back to the Switch Switch/dual-telegate relic mechanics in
`MerchantOfVenus/IMPLEMENTATION_STATUS.md`.

## `TradePanel`: a third visibility-driving pattern, and real evidence for the Phase 3 conversation

Unlike every panel above, `TradePanel` doesn't use `ActionGenerator`/`LEGALACTION` at all — it drives
its own visibility straight off `NEWSTATE.statename == "TradePhase"`. This makes sense at this
granularity: `TradePanel` represents an entire *phase* (`BUY`/`SELL`/`PICKUP`/`DROPOFF`/`JETTISON`/
`TRADEINSHIP`/`REDEEMIOU`/`GETRELIC`/`ENDTURN` all at once), not one action, so a coarse state-name
check is the natural granularity rather than per-action legality — a real, shipped precedent for
exactly the kind of "check one field in the incoming state directly" simplification the Phase 3
per-viewer-state design (see [[server-game-interface-spec]]) is aimed at, one level up (phase-level,
not action-level).

**`TradePanel.rebuild()` fully reconstructs its item panels from scratch on every entry into
`TradePhase`** (`removeAll()` then rebuild every row fresh from current `PlayerData`/`TradeBaseData`/
`OverrideSpaceData`) — real, routine evidence that this client already treats "read current state,
render fully" as normal operation for its most complex phase, not something only exercised by a rare
full-game refresh. Directly supports the corrected, less-alarming Phase 3 client-impact framing
already recorded in [[server-game-interface-spec]].

**`TradeItemFactory`/`TradeItem` — per-token buy/sell affordability is derived entirely client-side**
from already-delivered state (hold space, money+barter, buys/sells remaining, home-race discounts,
proprietor/factory-owner commission splits, fare-destination matching), not from a per-item
`LEGALACTION`. **Correctly framed (Albert, 2026-09-27, correcting an overstated first pass at this):
this is not a different trust model from Outpost's** — both `LEGALACTION` and this derived
client-side logic are expressions of the same one invariant, that the client must be given a
genuinely complete picture of everything the player has a right to know. `LEGALACTION` is a
convenience precomputation the server can optionally offer on top of that completeness (saving a
client from re-deriving one specific piece of FSM logic); MoV's trade UI just does more of its own
last-mile derivation from the same complete underlying data. **Historical design note, same
conversation:** Albert originally envisioned this completeness requirement partly because a
non-human (AI) player might one day drive a `Player` instead of a GUI — needing the real picture to
reason from, not a pre-computed per-action flag — written well before LLMs existed to be that AI. (A
real instance of exactly this scenario is already logged in `TODO.md`: Claude playing a live game of
Outpost over raw sockets.)

## `PopAndLockPanel`: a bespoke floating-panel windowing system, not `TabbedWindow`

A bare `JLayeredPane` glass-overlay on top of the board's scroll view, holding draggable "handle"
icons (`LockLabel`) that snap to the nearest viewport edge (`calcLabelPosition`'s min-distance
heuristic) via `SpringLayout` constraints, each owning one or more content panels. Ships with a
hover-to-show/mouse-exit-to-hide mode (`defaultPop=true`, a 750ms timer) that turns out to be used
selectively, not universally:
- **`ControlPanel`'s ~10 panels all share one single handle**, added with `defaultPop=false` — so
  none of them use the hover mechanism at all; each decides its own visibility entirely through the
  mechanisms above (`ActionGenerator`/`LEGALACTION`, `NEWSTATE.statename`, or direct peer method
  calls). **Per Albert (2026-09-27): safe because MoV's rules make at most one of these ten panels
  legal/relevant at any given moment** (movement decisions and buy/sell decisions are governed by
  disjoint rule sets and never coincide) — consistent with everything found in each panel's own
  visibility logic, though not independently verified by walking the full `MerchantOfVenusServer.xml`
  transition graph (a real to-do if this ever needs to be relied on structurally, not just observed).
- **`PlayerPops` gives each player their own separate handle** (`defaultPop=true`, stacked vertically
  down the left edge, created lazily the first time that player is seen) — hover any one to see a
  fully hand-painted dashboard (race, money, barter, net worth, ship stats, hull/cargo-hold token
  icons, hand-laid-out pixel grid). Confirms hovering to inspect any player's full status is real,
  intentional functionality, just not used for the Control Panel cluster.

## Why MoV's architecture diverged this sharply from Outpost's — design history, not just contrast

**Resolved 2026-09-27/28 (Albert):** the difference between Outpost's declarative, generic-widget,
cross-frame GUI and MoV's hand-painted-canvas, custom-panel GUI isn't primarily a difference in
engineering skill or in the underlying framework's capability — both games sit on the exact same
`ActionTransferManager`/`GameGui`/wire-protocol foundation (see [[component-java-client]]). **It's
who built each one and with what collaboration.** Outpost was built by Albert alone, a self-described
non-UI-designer (see [[user-profile]]), reaching for the framework's own generic widgets. MoV had a
graphic designer helping craft its *core* GUI — and per Albert, she specifically disliked Outpost's
cross-frame, no-graphics style, which is a real causal reason (not just correlation) MoV's client
abandoned `GameXMLWalker`'s declarative tree almost entirely in favor of one custom-painted board and
a bespoke floating-panel system instead. This also sharpens the corrected "MoV ~70% done" framing in
[[games-overview]]: the gap was never missing engineering (this file exists precisely because the
engineering turned out to be substantial and complete) — it was missing art/design polish, which
depended on that same collaboration. **Designer status as of 2026-09-28:** a new graphic designer has
volunteered, unblocking this; the original designer is still around and wants to see the results, but
is no longer interested in doing the art (panel backgrounds/images) herself.

**How to apply:** when touching MoV's client, expect hand-wiring to `ActionTransferManager`
everywhere, not XML-declared widgets — grep for `implements.*ActionGenerator`/
`AddButtonPresser`/`ReportNewValueToConsumers` in `MerchantOfVenus/client/MerchantOfVenus/*.java`
before assuming a mechanism lives in the XML. When the eventual front-end rewrite reaches MoV, the
graphic-design collaboration (not just the wire protocol) is a real dependency to plan around.
