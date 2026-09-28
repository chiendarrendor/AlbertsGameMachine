---
name: outpost-client-gui
description: "Outpost-specific client GUI case studies (factory purchase UX, bidding widgets, dynamic tabs) — concrete illustrations of the generic client architecture in component-java-client, not the architecture itself"
metadata:
  type: project
---

Written 2026-09-27, alongside [[component-java-client]]'s big generic-architecture rewrite, during a
session dedicated to Albert walking Claude through the real client code as prep for the Phase 2/3
wire-protocol replumbing (see [[server-game-interface-spec]]). Everything here is Outpost-specific —
either a real illustration of a generic mechanism documented in [[component-java-client]], or a
genuine Outpost-only quirk/gotcha. Cross-reference that file for the architecture itself.

**Outpost sits mixed on the declarative-XML vs. hand-written-Java spectrum** (see the corrected "GUI
architecture" section of [[component-java-client]]) — a real declarative tree of `window`/`panel`/
`button`/`textbox`/etc. tags, plus 12 confirmed `<javaclass>` widgets, all `implements
UserDefinedInterface`, all under `Outpost/client/Outpost/`: `BidItemWatcher`, `CommodityCard`,
`CommodityHand`, `Factories`, `ItemCard`, `OptionSelector`, `PlayerWindow`, `ResourceDecks`,
`ShipList`, `StateSensitiveTextBox`, `TurnOrder`, `PlayerDisplay`. (`HandListener` is a related but
separate internal helper — an `ActionVariableGenerator` constructed *by* `CommodityHand`, never
itself loaded via `<javaclass>`.) Contrast MoV, which has essentially one (`MerchantOfVenus.java`
alone).

**Client XML file layout**: `Outpost/client/OutpostClient.xml` is the one file the server's single
`NEWGUI` names; it's a `cpp`-spliced template (see [[build-environment]]) pulling in `events.xml`,
`actions.xml`, `Options.xml`, `Players.xml`, `ResourceRules.xml` via literal `#include` lines, plus
one inline `<tab title="Purchase History"><javaclass classname="Spends"/></tab>` of its own.

## The factory-purchase UX — one action per item type, by design

Buying anything (`BUYMEN`/`BUYROBOTS`/`BUYOREFACTORIES`/`BUYWATERFACTORIES`/
`BUYTITANIUMFACTORIES`/`BUYRESEARCHFACTORIES`/`BUYNEWCHEMFACTORIES`) is a *separate* action per item
type in `actions.xml`, each just instantiating a shared `BuyThing`/`BuyMen` XML macro
(`<define>`/`<named>`) for the quantity-picker + cost text. **Deliberate, per Albert (2026-09-27):**
since which of these `LEGALACTION`s are currently broadcast is entirely server/FSM-computed, having
one action per item type means the set of currently-visible buy buttons *itself* communicates exactly
which purchases are allowed right now — the same design intent behind the whole `ActionNode`
visibility mechanism in [[component-java-client]]. Albert's own retrospective critique: workable, but
one of the "bizarre UI choices" of this client — not a great UX, by his own admission (not a
professional UI designer, worked alone on this game's client).

**The `DiscardString` case study — the clearest real proof that action-argument generation is
independent of GUI placement.** Every `BUY*`/`DISCARD` action declares a `hidden="y" noreset="y"`
`DiscardString` var in `actions.xml`, but none of them contain any widget for it. The real generator
is `HandListener` (`ActionVariableGenerator` for `"DiscardString"`), constructed lazily by
`CommodityHand` (a `<javaclass>` in the wholly separate `Players.xml`, one instance per player slot,
distinguished by a `playernum` option) the moment an `IAM` event confirms this is really your own
hand. Each hand-card renders as a plain `JCheckBox`; toggling any of them recomputes a bit-per-card
string ("1"/"0" in hand order) and reports it live via `ActionTransferManager`. Since
`ActionTransferManager`'s registries are keyed purely by var name with zero action-scoping, there is
really only **one** live `"DiscardString"` value in the whole GUI at any moment, shared across every
buy/discard action — whichever cards are currently checked rides along as a hidden argument on
whichever button you happen to press. `noreset="y"` is why pressing "Buy Ore Factories" doesn't clear
your checked cards afterward (the framework's normal post-`ActionGo` var-reset is explicitly skipped
for this var) — matching the actual workflow: pick cards in one window, go find quantity + the buy
button in a completely different one.

**`CommodityHand` is also a real, concrete client-side consumer of `IAM`** — relevant directly to
[[server-game-interface-spec]]'s planned Phase 3 retirement of `IAM`/`GUIIAM`. It switches between
read-only face-down rendering (`PLAYERCARDBACKS`, checkboxes disabled) and interactive real-card
rendering (`PLAYERCARDS`, checkboxes enabled) purely by comparing an incoming `IAM` event's
`PlayerNumber` against its own construction-time `playernum` option. Whatever replaces `IAM` needs a
real replacement for this specific check, not just a deletion.

## The bidding widget cluster (`OPENBID`/`BID`)

- **`ShipList`** (`Players.xml`, one shared instance for the whole visible fleet) is the real
  `ActionVariableGenerator` for `"ShipSlot"` — lazily grows one `ShipItem` widget per slot as
  `SUPPLYSHIP` events arrive; clicking a ship reports that slot as `"ShipSlot"`. `OPENBID`'s own XML
  declares `<var name="ShipSlot" .../>` with **no children at all** — same placement-independence
  pattern as `DiscardString`, one layer removed (a completely different top-level window than either
  the action panel *or* the hand widget).
- **`BidItemWatcher`** — a pure display widget (an item icon), reused in two modes distinguished only
  by one XML attribute (`ConsumeVariable="true"` on `OPENBID`'s copy, absent on `BID`'s): in `BID`
  (raising an already-open bid) it passively reads the live `BIDITEM` event, since the slot is already
  server-confirmed; in `OPENBID` (picking a slot to begin with) there's no such event yet, so it
  registers as an `ActionVariableConsumer` for `"ShipSlot"` instead and previews the local selection
  live. Either way it maintains its own small passive slot→item cache purely from watching
  `SUPPLYSHIP` events go by, so it can render regardless of which path fed it.
- **`StateSensitiveTextBox`** — a generic, reusable digits-only numeric input, parameterized purely by
  an XML `varname` option (serves both `OpeningBid` and `Bid`). Tracks FSM state via a small
  `ServerStateHistory` helper (also used independently by `ShipList`) and clears itself the instant
  the state changes mid-bid — a small deliberate UX safeguard against a stale half-typed value.

## Dynamic vs. static tabs, side by side

`ResourceRules.xml` mixes both directly: `<tab title="Resource Decks"><javaclass
classname="ResourceDecks"/></tab>` is genuinely dynamic — `ResourceDecks` renders an HTML table of
per-resource draw/discard-pile counts, built purely from caching `COMMODITYSTOCK` events
(`CommodityNumber`→count) in two plain `HashMap`s, re-rendered on every update. `<tab title="Quick
Reference">` right next to it is genuinely static — hardcoded HTML rules text, no event handling at
all. Confirms Albert's recollection exactly ("one of the tabbed windows tells you how many cards are
left in each resource deck").

**Gotcha, worth remembering to avoid repeating the mix-up:** `Spends.java` (the `<javaclass>` in
`OutpostClient.xml`'s own inline "Purchase History" tab) is **not** the resource-spend-selection
widget its name suggests — that's `HandListener`/`CommodityHand`, above. `Spends` is a passive,
read-only purchase/bid/discard **history log** (a scrolling `JTextArea`), appending one line per
`SPENDS` event it observes.

**Real `<eventhandler>`/`showif`/`hideif` usage is light, confirmed by grep (2026-09-27):** 11 total
across live files — `actions.xml` (4: the `BuyMen` macro's per-player cost display, `MegaCard`'s
`showhide`), `Players.xml` (1), `Options.xml` (6). (`oldOptions.xml` also has 6, but is a superseded,
non-live file — not included in the 11.) See [[server-game-interface-spec]]'s Phase 3 section for why
this count matters (it's the basis for calling this DSL's Phase 3 blast radius small). Matches
Albert's own account: an early design idea (static images toggled purely by passing events) that
turned out "deeply clunky," and one he doesn't think he leaned on much in either real game once
hand-written `UserDefinedInterface` widgets became the norm.

**How to apply:** when a Phase 2/3 design question needs a concrete "does this actually happen in a
real game" check, Outpost is the richer of the two real games for this purpose (MoV is almost all one
`javaclass`, so it doesn't exercise the declarative-tree/placement-independence machinery much at
all) — check here first before assuming a mechanism is only theoretical.
