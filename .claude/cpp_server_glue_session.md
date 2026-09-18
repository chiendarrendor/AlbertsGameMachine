---
name: cpp-server-glue-session
description: Dedicated session scoped to the generic C++ server and its glue contract with per-game DLLs (server/, common/, gamecommon/, stateengine/), kept separate to manage token/context cost
metadata:
  type: project
---

Started 2026-09-18. Related to [[project-overview]], [[component-cpp-server]], [[component-game-dlls]],
[[game-server-xml]], [[protocol-terminology]].

**Why this session exists:** Albert wants to partition Claude sessions by codebase subsection so
each one's context/memory/token usage stays focused on what it actually needs, rather than every
session loading the whole project's memory (all games' implementation status, MoV-specific logic,
the decoupling investigation, etc.). This session is the "generic server + glue" partition — see
[[server-game-decoupling-investigation]] for the precedent of deliberately scoping a session this
way for a different reason (clean framing rather than cost).

**Scope:** `server/` (main loop, room/session lifecycle — see [[component-cpp-server]]), `common/`
(cross-platform abstraction libs), and `gamecommon/`/`stateengine/` (the shared interfaces/objects
that define the contract every per-game DLL must implement, per [[component-game-dlls]]). Out of
scope: any individual game's gameplay logic (MoV, Outpost, AOR, RoadsAndBoats) except where a
question is really about how a game exercises the generic contract.

**Consulting MoV specifics:** there is a peer session named "MoV Server" that deeply understands
MerchantOfVenus's game-specific code. When a question is about how a *specific* game DLL uses the
gamecommon/stateengine contract (as opposed to what the contract itself requires), use `SendMessage`
to ask MoV Server rather than reading MoV's game logic bottom-up.

**How to apply:** if you're the session picking this up — stay in `server/`, `common/`,
`gamecommon/`, `stateengine/`, and the Perl `transitioncompiler` only as far as it generates the
DLL-interface scaffolding (`DLLGame.hpp`/`GameBoxDLL.hpp`). Don't pull in per-game implementation
files or per-game `.claude/` notes (e.g. MoV's `IMPLEMENTATION_STATUS.md`) unless a question
specifically requires cross-referencing them.
