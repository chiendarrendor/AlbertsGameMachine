---
name: games-overview
description: "Per-game directory layout convention and completion status for each game in The Game Machine (Outpost, MerchantOfVenus, AOR, RoadsAndBoats)"
metadata: 
  node_type: memory
  type: project
  originSessionId: 416ca727-2f1a-4082-9558-d306f19709ba
  modified: 2026-09-13T05:56:22.645Z
---

Part of [[project-overview]]. Covers the games implemented on top of the [[component-cpp-server]] / [[component-java-client]] architecture, and their relative completion.

**Standard per-game directory layout** (Outpost is the exemplar; expect this pattern for every game):
- `<GameName>/` — top-level game directory contains the server-side C++ game logic (the game DLL, see [[component-cpp-server]]'s gamecommon/stateengine) plus `<GameName>Server.xml`.
- `<GameName>/client/` — Java client GUI code and graphics resources for this game, plus `<GameName>Client.xml`.
- So each game has one XML file describing server-side config/rules data and one describing client-side GUI config, alongside the actual server (C++) and client (Java) code.

**Game completion status (as of 2026-09-12):**
- **Outpost** — 100% complete, the only fully-done game. However: there are user requests to update it to "2nd gen" rules, and Albert considers its GUI "a travesty" (i.e. built on the legacy XML/GameXMLWalker approach per [[component-java-client]], likely a rewrite candidate).
- **MerchantOfVenus** — was described as ~70% complete, but a 2026-09-12 code survey found the server-side rules engine (all 24 Transitions in `MerchantOfVenusServer.xml`) fully implemented with no stubs, and every open question from that survey has since been resolved with Albert (see `MerchantOfVenus/IMPLEMENTATION_STATUS.md`, the living status doc). Notably: the optional player-vs-player combat ruleset (forts, Nova Balls/lasers, ship attacks, stealing cargo, starbase/factory revolts) is real in the physical game but was deliberately never implemented — attack values, buyable weapons, and relic weapons exist in the data model as inert placeholders so it could be added later without a retrofit; Shields are the exception, since their defense value already has real utility reducing Penalty Oval movement costs. Nothing outstanding was found in the server rules engine itself. **Confirmed with Albert (2026-09-12):** the earlier "~70% done" figure was tracking client-side completeness, not the server — work on MoV stalled specifically because Albert lost the services of his graphic designer, so the client GUI/art was left unfinished while the server rules engine (as this survey found) was essentially completed. **Update (same day):** Albert has a new graphic designer lined up, so this is no longer a blocker — MoV client work can proceed. **Naming quirk:** the rulebook term is "Culture" (e.g. Nillis, Volois, Humans...), but the code calls this field `Race` (`Player::GetRace()`) — search the code for `Race`, not `Culture`.
- **AOR (Age of Renaissance)** — skeleton only, far from done.
- **RoadsAndBoats** — skeleton only, far from done.
- **Interstellar** — not yet started; blocked on Albert supplying boards and rules first. Tracked in `TODO.md` (see [[roadmap-todo]]).

**How to apply:** Outpost is the best reference implementation to study for "how a complete game is structured" — but its GUI specifically should be treated as legacy/low-quality, not a pattern to replicate for new work. When Albert mentions wanting to improve Outpost's GUI or rules, that aligns with the broader [[component-java-client]] rewrite goal and the "2nd gen rules" request — worth checking whether these are meant to be tackled together. MerchantOfVenus, AOR, and RoadsAndBoats are in-progress/unfinished — don't assume feature parity with Outpost when working in those directories.
