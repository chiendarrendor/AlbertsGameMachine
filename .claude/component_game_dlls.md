---
name: component-game-dlls
description: "Structure of a per-game C++ DLL in The Game Machine — component objects, and the special Set/State classes"
metadata: 
  node_type: memory
  type: project
  originSessionId: 416ca727-2f1a-4082-9558-d306f19709ba
  modified: 2026-09-12T08:25:18.361Z
---

Part of [[project-overview]]. Covers the C++ game DLL component (one per game, e.g. [[games-overview]]'s Outpost/MerchantOfVenus/AOR/RoadsAndBoats), living in each game's top-level directory alongside its `<Game>Server.xml`. Complements [[game-spec-language]] (state vs. FSM) and conforms to the shared interfaces in [[component-cpp-server]]'s gamecommon/stateengine.

**Typical file contents of a game directory:**
- Most `.cpp`/`.hpp` files are code embodiments of the physical components that go on the game board — e.g. code for adding, removing, and moving a given piece/component type on the board. A handful of files instead hold utility functions for more complex actions that don't belong to one specific component type.
- **Two specially-named files required by the DLL interface** (generic server code looks them up by this naming convention, so they must be named exactly `<GameName>Set.cpp/.hpp` and `<GameName>State.cpp/.hpp` — e.g. `OutpostSet.hpp`, `OutpostState.hpp`):
  - **`<GameName>Set`** — instantiated once, the first time that particular game is ever started on a server. Holds read-only structures relevant to the game (e.g. board/rule constants). All concurrent gameplays (sessions) of that same game share this single `Set` object.
  - **`<GameName>State`** — instantiated once per individual gameplay/session of the game. Holds all the actual mutable data for that specific playthrough: board positions, everything that changes during play. This is the "game state" side of [[game-spec-language]].

**How to apply:** When adding a new game or a new board-piece type, expect one `.cpp`/`.hpp` pair per component type, following the add/remove/move pattern. When something needs to be shared read-only across all games-in-progress of the same game type (e.g. precomputed board topology), it belongs in `<GameName>Set`; anything that varies per playthrough belongs in `<GameName>State`. The naming convention (`<GameName>Set`/`<GameName>State`) is load-bearing — the DLL loader depends on it, so don't rename these files/classes without also updating the loader code.
