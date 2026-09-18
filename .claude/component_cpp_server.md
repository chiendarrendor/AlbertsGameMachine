---
name: component-cpp-server
description: "C++ server component of The Game Machine — directory layout and responsibilities (server, common, gamecommon, stateengine)"
metadata: 
  node_type: memory
  type: project
  originSessionId: 416ca727-2f1a-4082-9558-d306f19709ba
  modified: 2026-09-12T07:34:17.199Z
---

Part of [[project-overview]]. This covers the C++ TCP/IP server component's internal directory structure, as described by Albert. All four directories below have since been fully code-verified: `server/` in [[server-directory-architecture]], `common/` in [[common-directory-architecture]], and `gamecommon/`/`stateengine/` in [[server-game-decoupling-investigation]].

- **`server/`** — the main loop and control engine. Also owns creation/management of virtual "rooms" — the spaces in which games are actually played (matchmaking/session container concept).
- **`common/`** — shared/general-purpose C++ utility code. Notably contains two libraries whose job is cross-platform abstraction: they encapsulate the differences between Windows and Linux so the rest of the codebase can compile/run on both without caring about platform specifics.
- **`gamecommon/`** — interfaces and objects shared between the server and the individual per-game DLLs (see [[component-game-dlls]]).
- **`stateengine/`** — also interfaces/objects shared between server and per-game DLLs (paired with gamecommon — Albert described these two together as "must be shared between the server and the individual game dll's").

**How to apply:** When working in this component, `server/` is the place for connection handling / room lifecycle logic; `common/` is where to look for (or add) platform-abstraction utilities rather than writing new #ifdef WIN32 blocks inline; `gamecommon/` and `stateengine/` define the contract that any game DLL must conform to, so changes there are cross-cutting and affect every game DLL.
