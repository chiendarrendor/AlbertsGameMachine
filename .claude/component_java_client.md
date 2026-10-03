---
name: component-java-client
description: "Java client component of The Game Machine — directory layout, Applet origins, connects to C++ server over TCP/IP"
metadata: 
  node_type: memory
  type: project
  originSessionId: 416ca727-2f1a-4082-9558-d306f19709ba
  modified: 2026-09-12T08:04:06.227Z
---

Part of [[project-overview]]. Covers the Java common GUI front-end client.

- Lives in the `client/` directory, with various subdirectories.
- Most notable subdirectory: `TheGameMachine` (renamed 2026-09-28 from `OpenZorz`, a holdover from the project's old name — see [[project-overview]] and `TODO.md`/[[roadmap-todo]] for the resolved rename item).
- Designed originally as an in-browser Java Applet.
- Connects to the backend C++ server (see [[component-cpp-server]]) via TCP/IP.

**Current execution model (as of 2026-09-12):** Originally an in-browser Applet, but in-browser Java is no longer viable. Since a Java Applet is just a subclass of `java.awt.Panel`, Albert was able to make it runnable as a standalone desktop application with a small change, rather than a rewrite. The script that currently launches the client is `../runclient` (relative to this component — i.e. a top-level `runclient` file in the project). This standalone mode is a stopgap to keep the project alive/playable, not the desired end state.

**Long-term goal:** Albert wants to eventually rewrite the entire front-end using modern/current state-of-the-art browser GUI tech (framework TBD). He is not a front-end developer by background and expects to need significant help designing/implementing that rewrite. Tracked in `TODO.md` (see [[roadmap-todo]]). This is a big, not-yet-started initiative — distinct from the smaller "expunge OpenZorz naming" cleanup task, which is now resolved (2026-09-28).

**GUI architecture — corrected 2026-09-27 after actually reading the code together (Albert: the
original version of this section was written from his recollection, in a session before either of
us had read this code; where it conflicted with the code, the code wins).**

There is only **one** bootstrap path, with no exceptions: every per-game GUI, for every game, is
loaded the same way. `ClientManager.HandleOneLine` sees a `NEWGUI`/`ADDGUI` wire packet, `GUIUnit`
fetches `<XMLLoc>/<GuiName>.jar` over the network via a `URLClassLoader` and *unconditionally*
constructs a `GameXMLWalker` to parse `<XMLLoc>/<XMLFile>` and build the widget tree
(`GUIUnit.ParseXML`, no branch, no alternate path). `GameXMLWalker`/XML is not legacy or
vestigial — it is the mandatory entry point for 100% of client GUI code, including MoV's.

What actually varies per game is **how much of the widget tree is declared via `GameXMLWalker`'s
own built-in XML vocabulary vs. handed off to hand-written Java**, and that hand-off happens via a
single, real escape-hatch tag: **`<javaclass classname="...">`** (`GameXMLWalker.WalkUserDefinedNode`
→ `UserDefinedNode`). This reflectively loads a class (via the same per-game jar's `ClassLoader`)
that implements **`UserDefinedInterface`** (`NodeInterfacePackage/UserDefinedInterface.java`) and has
a specific 4-arg constructor: `(HashMap<String,String> options, String dataLoc, JPanel panel,
ActionTransferManager atm)`. Verified directly in both real games:
- **MerchantOfVenus leans almost entirely on this side**: `MerchantOfVenusClient.xml` has exactly
  one `<javaclass classname="MerchantOfVenus">` at the top, and `MerchantOfVenus.java` (`implements
  UserDefinedInterface`) is the *entire* hand-written GUI — effectively one big escape hatch with a
  near-trivial declarative shell around it.
- **Outpost mixes both throughout**: its XML (`OutpostClient.xml`/`Players.xml`/`actions.xml`/
  `Options.xml`/`ResourceRules.xml`) is a real declarative tree of `window`/`panel`/`button`/
  `textbox`/etc., with numerous individual `<javaclass>` widgets scattered inside it (`OptionSelector`,
  `ItemCard`, `ShipList`, `TurnOrder`, `PlayerDisplay`, `PlayerWindow`, `CommodityCard`,
  `StateSensitiveTextBox`, `BidItemWatcher`, `Spends`, `ResourceDecks` — all `Outpost/client/Outpost/
  *.java`, all `implements UserDefinedInterface`).

**`ActionGeneratorNode` is *not* the hand-authored-code seam** (this was the wrong guess in the
original version of this note) — it's an internal abstract `GameNode` base class
(`TheGameMachine/ActionGeneratorNode.java`) used only by `GameXMLWalker`'s own *built-in* widget-generator
node types (`ButtonNode`, `CheckBoxNode`, `SelectionNode`, `MinMaxSelectionNode`, `SpinnerNode`,
`TextBoxNode` — the concrete classes behind `<button>`/`<checkbox>`/`<fixedselection>`/etc.), giving
them their shared `ActionVariableGenerator` registration with `ActionTransferManager`. Hand-authored
`UserDefinedInterface` classes that need to act as an action-variable source implement
`ActionVariableGenerator` directly instead (e.g. `Outpost/OptionSelector.java` implements
`UserDefinedInterface,ActionVariableGenerator,ActionListener` side by side) — they don't go through
`ActionGeneratorNode` at all.

**How to apply:** don't treat `GameXMLWalker`/XML as legacy or `ActionGeneratorNode` as the
"modern" per-game-GUI pattern — neither is true. The real per-game authoring axis is *declarative
XML tree* (using `GameXMLWalker`'s built-in tags) vs. *hand-written Java* (via `<javaclass>` +
`UserDefinedInterface`), and real games sit at different points on that spectrum (MoV near the
hand-written end, Outpost mixed) rather than belonging to two chronological "generations." When
working on any per-game GUI, check `implements UserDefinedInterface` in that game's own
`client/<Game>/*.java` to find its hand-authored widgets, and its `*Client.xml` for the declarative
shell around them. **The "OpenZorz" naming cleanup is done (2026-09-28)**: the package/directory is
now `TheGameMachine` (was `OpenZorz`) — don't expect to find that old name in source anymore; if it
does turn up somewhere unexpected, that's stale and worth flagging/fixing, not a sign of an
incomplete rename. Don't assume the client still runs as an in-browser Applet — check
`runclient` for the actual current launch mechanism (this script lives only on Albert's Windows/MSYS
laptop, not in this Linux checkout — it separately references the old fully-qualified
`OpenZorz.ClientApplication` class name and still needs updating there, by Albert, not tracked here). When front-end rewrite discussions come up,
lean in with more front-end guidance/explanation than usual since Albert has explicitly said this is
a gap in his own background, unlike his general seniority (see [[user-profile]]).

---

**The rest of this file (added 2026-09-27) is a from-scratch, code-verified walkthrough of the
generic client architecture** — done deliberately as prep for the Phase 2/3 wire-protocol
replumbing (see [[server-game-interface-spec]]), reading the real code together rather than from
recollection. Outpost-specific findings that are illustrations of this generic architecture, not
the architecture itself, are kept in a separate file — see [[outpost-client-gui]].

## Wire-dispatch chain: how one raw line becomes GUI behavior

`ClientApplication.main` → `ClientApplet.start()` (spins a thread running `ClientManager.mainloop()`,
the only real work either class does) → `ClientManager`:
- `mainloop()`: connect the socket, read one unparsed welcome line, `DoLogin` (see below), then loop
  `HandleOneLine()` forever.
- **Login is structurally its own separate code path, though not out of scope for the wire-protocol
  replumbing (scope corrected 2026-09-28 — see below).** `SendLoginLikeString` writes a raw
  `LOGIN,<user>,<mangled-pass>` (or `LOGOUTOTHER,...`) comma line directly via `m_out.print` (same
  `UnComma` escaping convention as actions, but a completely separate code path from `GameGui`/
  `ActionTransferManager`); `SendLogin` reads one raw response line and compares it *literally* to
  the strings `"WELCOME"` / `"ALREADYLOGGEDIN"`. **Originally carved out of Phase 2 as a separately-
  deferred island (Albert, 2026-09-27); reversed 2026-09-28: Phase 2 covers every client→server
  message with no category exceptions, so `LOGIN`/`LOGOUTOTHER` get the same JSON-wrap treatment as
  in-game actions — just via this separate code path, not through `GameGui.SendAction`.** Likewise,
  `GetWelcome()`'s currently-unparsed welcome line and `SendLogin`'s literal-string response
  comparison both become real JSON parsing under Phase 3 (server→client direction) — see
  [[server-game-interface-spec]] for the full corrected phase split.
- `HandleOneLine()`: reads one raw line, wraps it in `GuiPacketParser`, which classifies it into a
  fixed set of **gui-management** packet types — `NEWGUI`/`ADDGUI`/`DROPGUI`/`RESETGUI`/`MESSAGE`/
  `SERVERERROR` — or the fallthrough **`NOTGUI`** (everything else, i.e. actual game Events).
  `GuiPacketParser`'s tokenizer is a bare `i_string.split(",")` with **no `UnComma`/`ReComma`
  unescaping applied to any of its own fields** (guiname/xmlloc/xmlfile) — same class of latent
  fragility as the server-side `GAMES,` double-escaping bug documented in
  [[server-game-interface-spec]], currently safe only because no gui name/xmlloc/xmlfile has ever
  contained a comma or `%`. **Scope corrected 2026-09-28: this is now covered, and permanently fixed
  in the process** — Phase 3's corrected scope (see [[server-game-interface-spec]]) is every
  remaining server→client message, including this whole gui-management meta-layer, not just the
  per-game event stream. Once this is real JSON there's no bare `split(",")` left to be fragile.
  - `NOTGUI` lines get broadcast to **every** currently-registered `GUIUnit` (`GUIUnitMap.ForEach`) —
    routing to the *right* one isn't done centrally here at all, it's pushed down to each `GUIUnit`.
  - `NEWGUI`/`ADDGUI` construct (`NEWGUI`) or re-parse into (`ADDGUI`) a `GUIUnit`, keyed by "gui
    name" in a `TreeMap` (`GUIUnitMap`). `RESETGUI,<name>` synthesizes a bare `"RESET"` line and
    re-injects it through the exact same per-unit `HandleLine` path an ordinary broadcast `RESET`
    line would also take — two different triggers converging on identical behavior; legacy
    duplication, not something either phase needs to preserve carefully.

`GUIUnit` (one per live game/gui instance): on `NEWGUI`, builds a `URLClassLoader` that fetches
**`<XMLLoc>/<GuiName>.jar` over the network** — i.e. a per-game client GUI is a remotely-downloaded
plugin jar, not code baked into the client build. `ParseXML` then unconditionally constructs a
`GameXMLWalker` (see below) on the Swing EDT (`invokeAndWait`) and calls `RealizeWindows()`. Its
`HandleLine(String)` does no filtering of its own — just `SwingUtilities.invokeLater` into
`GameGui.HandleEvent`, deferring "is this line relevant" entirely to the layer below.

`GameGui` (one per `GUIUnit`, the real per-instance hub) owns: `m_EventTypes` (a `GameEventType`
schema map, name → declared ordered var names, populated by `GameXMLWalker` parsing `<event>`
blocks), `m_Windows` (a **flat list of only the top-level** `GameNode`s — one per `<window>`/`<tab>`
block), and the one `ActionTransferManager` for this instance.
- **`HandleEvent(String s)` — the sole incoming-Event entry point per instance:** split on `,` (limit
  `-1`, so trailing empty fields survive), `StringUtility.ReComma` each token (un-escaping the
  server's `UnComma`), look up the first token in `m_EventTypes`. Unknown name → silently dropped;
  wrong arg count → dropped with a debug message. Otherwise zips values against declared var names
  into a `GameEvent`, special-cases `RESET`/`ERROR`, and otherwise calls `SendEventToWindows` — a
  flat loop handing the *same* `GameEvent` to every top-level `GameNode`, which each recurse into
  their own children (see "GameNode" below). **Filtering is fully decentralized all the way down**:
  `ClientManager` broadcasts to every `GUIUnit`, `GUIUnit` forwards to its one `GameGui`, `GameGui`
  broadcasts to every top-level `GameNode` — nobody upstream ever routes to a single addressee by
  name; each leaf widget decides for itself whether an event's name/values are relevant to it.
  - **Baseline schema every game gets for free**, registered before any XML is parsed: `NEWSTATE
    (statename,statedesc)`, `LEGALACTION(actionname)`, `RESET`, `MESSAGE(messagetext)`,
    `ERROR(errortext)`.
- **`SendAction(String[] s)` — the sole outgoing-Action entry point, full stop, for every widget in
  every game:** joins the given fields with `,`, `UnComma`-escaping each one first, writes the line +
  `\n`, flushes. Confirmed exhaustively — every widget we traced (built-in and hand-written alike)
  funnels through this one method (see "Action-authoring pipeline" below); nothing else in the whole
  client writes an in-game action to the socket. This is why Phase 2's client-side change really is
  as narrow as the spec claims: one method to touch, nothing upstream of it needs to change.

## `GameXMLWalker`: five jobs bundled into one class

A single recursive-descent parser/builder for a bespoke declarative UI language rooted at
`<gameboard>`, run once per `GUIUnit.ParseXML` call:
1. **Event schema declaration** (`<event name>`/`<var name>`) — populates the same `GameEventType`
   map `GameGui.HandleEvent` consults.
2. **A widget-tree builder**, one `Walk*Node` method per tag — `image`, `text`, `textbox`,
   `checkbox`, `fixedselection`/`varselection`/`minmaxvarselection`, `fixedspinner`/`varspinner`,
   `button`, `panel`, `window`, `tab` — each validated against a hand-declared attribute set
   (`AttributeChecker`) before construction.
3. **The `<eventhandler>` sub-language** — `showif`/`hideif`/`showifnot`/`hideifnot`/`showhide`/
   `hideshow` (visibility) and `passif`/`passifnot`/`blockif`/`blockifnot` (event pass-through,
   mutually exclusive with the show/hide units in the same handler). See "GameNode" below for exactly
   how this evaluates — it's more subtle than "matches or doesn't" for the two-directional forms.
4. **A crude XML-level macro system, built entirely on DOM cloning, not a real templating engine:**
   `<define name>` registers a raw subtree; `<named name attr=val...>` clones it and does literal
   string substitution into the clone (`PriorityMap`/`DOMUtil.RecursiveValueReplace`); `<forupto>`/
   `<foreach>` do the same clone-and-substitute once per value in a range or list.
5. **The Action-authoring side** — `<actions><action name onstate text color alerts><var name text
   .../></action></actions>` — see "Action-authoring pipeline" below.

**The one true hand-written-code escape hatch: `<javaclass classname="...">`**
(`WalkUserDefinedNode` → `UserDefinedNode`). Reflectively loads a class (via the same per-game jar's
`ClassLoader`) implementing `NodeInterfacePackage.UserDefinedInterface` (`HandleEvent`/`Refresh`/
`Reset`), requiring an exact 4-arg constructor `(HashMap<String,String> options, String dataLoc,
JPanel panel, ActionTransferManager atm)`. This — not `ActionGeneratorNode` — is the real seam
hand-authored per-game GUI code plugs into; see the corrected section above for why.

## The Action-authoring pipeline: XML declaration → `SendAction` call

- **`ActionTabNode`** — one auto-built "`<Name>` Action Selection" tab per game (constructed in
  `GameXMLWalker`'s own constructor, before any XML content is walked), holding a "Current State"
  label (kept in sync off the built-in `NEWSTATE` event) and a stacked list of `ActionNode`s. This is
  the literal implementation of Albert's original design intent: one panel per possible Action, shown
  or hidden by the FSM's own `LEGALACTION` broadcasts.
- **`ActionNode`** — one per `<action>`: a real `JButton` ("`GoButton`") plus a row of `ActionVarNode`
  argument-picker children. `VisibilityType` (`ALWAYSON`/`NEWSTATEHIDES`/`STATEDALWAYSHIDDEN`/
  `ALWAYSHIDDEN`) governs both panel visibility and whether external `ActionGenerator` listeners get
  told about legality changes:
  - `NEWSTATEHIDES` (default): hides on every `NEWSTATE`, re-shows only on a matching `LEGALACTION` —
    the only variant whose *panel* is ever actually shown or hidden by this code.
  - `ALWAYSON`: opts out of the hide/show machinery entirely.
  - `STATEDALWAYSHIDDEN` vs `ALWAYSHIDDEN` — **both permanently hidden, `AlterVisibility` never
    called for either.** The one real difference: on `NEWSTATE`, `STATEDALWAYSHIDDEN` explicitly
    resets legality to `false` (`ActionLegalityChanged(name, false)`); `ALWAYSHIDDEN` does not — so an
    `ALWAYSHIDDEN` action's legality, once told `true`, stays `true` from an external listener's point
    of view until some other `LEGALACTION` batch happens not to include it, with no explicit reset.
    Both get the unconditional `ActionLegalityChanged(name, true)` call on a matching `LEGALACTION`.
    Since `GoButton.ActionLegalityChanged` is a no-op ("nothing happens here because the containing
    panel goes invisible or not" — stale comment for these two, since their panel never goes
    invisible either way), this distinction only matters to an external `ActionGenerator` (i.e.
    something built by `<javaclass>`) actually subscribed to this action's legality.
  - **`ActionGo()` is the real `SendAction` call site**: assembles `{actionName, var1, var2, ...}` in
    child-declaration order, calls `SendAction`, resets all vars, then immediately re-queries each
    generator's current value (`GetCurVarVals`) so e.g. a spinner with a natural default doesn't need
    a fresh click to re-arm.
- **`ActionVarNode`** — one per action arg. Registers itself as an `ActionVariableConsumer` under its
  var name; whatever concrete widget lives inside it (built-in or `<javaclass>`) is expected to
  register as the matching `ActionVariableGenerator` and report value changes.
- **`ActionTransferManager`** — pure pub/sub, zero wire/protocol knowledge: four name-keyed
  registries (variable consumers, variable generators, one action-handler per action name, a list of
  "button pressers" per action name). **Keyed purely by string name, with zero scoping to which
  action or which window/panel declared it** — this is the mechanism that lets an action's hidden
  argument be generated by a widget living in a completely different top-level window than the
  button that sends it (see [[outpost-client-gui]] for concrete real examples: `DiscardString`,
  `ShipSlot`). This is the clearest evidence in the whole client that event/action processing is
  fully independent of GUI placement.
- **`ActionGeneratorNode`** — an internal abstract `GameNode` base class used only by the framework's
  own built-in generator widgets (`ButtonNode`/`CheckBoxNode`/`SelectionNode`/
  `MinMaxSelectionNode`/`SpinnerNode`/`TextBoxNode`), giving them shared `ActionVariableGenerator`
  registration. **Not** the hand-authored-code seam (see corrected section above) — hand-written
  `UserDefinedInterface` classes that need to act as a generator implement `ActionVariableGenerator`
  directly instead, side by side with `UserDefinedInterface`.

## `GameNode`: a placement-independent event/visibility/reset tree-walk

`GameNode.HandleEvent(GameEvent)`, `.refresh()`, and `.reset()` are all pure recursive walks: call
`MyHandleEvent`/`RefreshMe`/`ResetMe` on self, then unconditionally recurse into every child, in
declaration order — completely decoupled from what Swing container a node ends up rendered in.
`GameGui` only ever tracks a flat list of **top-level** nodes (one per `<window>` or `<tab>`) and
walks from there.

**`GameEventHandler.ParseEvent` — the mechanism behind `<eventhandler>`'s show/hide/pass/block
semantics, and more subtle than "matches or doesn't":** it runs on **every** event a node receives
(no relevance pre-filter), resetting its internal show/hide/block flags at the top of every call.
`HandlerUnit.Matches` treats "this event simply doesn't declare this var at all" the same as "this
event declares it with the wrong value" — both are non-matches. For one-directional units (`showif`/
`hideif`/`showifnot`/`hideifnot`), a non-match produces no verdict, and `GameNode.HandleEvent`'s
`if (IsVisible()) ... else if (IsInvisible()) ...` then does nothing at all — so the *practical*
effect of an unrelated event is "visibility left exactly as it was," achieved by re-evaluating to
"no opinion" every time, not by skipping evaluation. **But the two-directional units (`showhide`/
`hideshow`) have a real `else` branch that fires on *any* non-match** — including an event that
simply doesn't carry the matched var at all — so a `showhide` handler already actively flips
visibility on every unrelated event today, not just ones that share its var name. Real usage of this
whole DSL turns out to be light in practice in both games (see [[outpost-client-gui]] for counts) —
Albert's own account: an early design idea (static images toggled purely by passing events) that
turned out "deeply clunky," largely abandoned once hand-written `UserDefinedInterface` widgets became
the norm.

**Room-navigation traffic is not a separate mechanism — `roomgui/rooms.xml` is just another ordinary
`<gameboard>` GUI instance** (checked 2026-09-28, closing a gap flagged during the Phase 2 scope
correction above), loaded through the identical `NEWGUI`→`GUIUnit`→`GameGui`→`GameXMLWalker` pipeline
as any per-game client, under its own gui name (distinct `GUIUnit`/`GameGui`/`ActionTransferManager`
instance, per the normal one-`GUIUnit`-per-gui-name rule). `roomgui/rooms.xml` itself nicely
demonstrates both authoring styles side by side: `NEWROOM`/`NEWGAME`/`LOADGAME`/`SAVEGAME` are
`onstate="alwayson"` (genuinely using the framework's visible auto-built button UI, Outpost-style),
while `CHANGEROOM` is `alwayshidden` and hand-driven by a custom `<javaclass>`
widget (`RoomWidget`, MoV-style). (`ROOMTALK`/`PLAYERTALK` were a second `alwayshidden`/`TalkPanel`
example here too, until the whole chat facility was removed 2026-10-03 — see `.claude/TODO.md`.)
One new `ActionTransferManager` mechanic this surfaced:
`RoomWidget` (one instance per known room) registers as generator/button-presser under the *same*
names (`"TargetRoom"`/`"CHANGEROOM"`) for every room simultaneously — confirming
`GetVariableGeneratorValue`'s `if (v.size() != 1) return null` guard is a real, exercised case, not
just defensive code: with multiple competing generators there's no single "current" value to
re-prime from, so `RoomWidget.actionPerformed` must actively report its value on click every time.
Also confirmed: `GUIIAM` is just an ordinary declared event in this gui's own schema, not a special
mechanism (relevant to the `IAM`/`GUIIAM` retirement idea in [[server-game-interface-spec]]), and the
built-in `MESSAGE` event type (one of `GameGui`'s five free baseline events) is exercised here for
both room-wide and player-to-player chat text via `TalkPanel`.

**`TabbedWindow`/`TabNode`/`WindowNode` — the concrete proof placement doesn't matter:** `<window>`
(`WindowNode`) creates its own independent floating `JFrame`. `<tab>` (`TabNode`) instead adds its
panel as one page inside a **single `TabbedWindow`, shared across every `GUIUnit`/game instance in
the whole client**, constructed once in `ClientManager`'s own constructor. A tab can even be popped
out into its own floating `JFrame` at runtime and dragged back in (`TabbedWindow.MoveTabToWindow`/
`MoveWindowToTab`, via a small export button baked into every tab) — pure Swing container
reparenting, touching zero `GameNode`/event/action logic at all. A widget three panels deep inside a
`<tab>` sees events, visibility rules, and action wiring identically to one three panels deep inside
a `<window>`.
