---
name: common-directory-architecture
description: Code-verified walkthrough of common/ — cross-platform abstraction libs and general string/file/serialization utilities shared by server and game DLLs
metadata:
  type: project
---

Read in full 2026-09-18 (9 files) as part of [[cpp-server-glue-session]]. Completes the
code-verification of [[component-cpp-server]] alongside [[server-directory-architecture]] and the
gamecommon/stateengine half of [[server-game-decoupling-investigation]].

**Cross-platform abstraction (the two libraries [[component-cpp-server]] referred to):**
- `SystemSpecificDynamicLoading.hpp/cpp` — `GenericLibraryLoader`, the actual dlopen/LoadLibrary
  wrapper used by `server/GameBox.cpp` (see [[server-directory-architecture]]). `#ifdef msys` picks
  `LoadLibrary`/`GetProcAddress`/`FreeLibrary` and appends `.dll`; `#elif FreeBSD || linux` picks
  `dlopen(..., RTLD_NOW)`/`dlsym`/`dlclose` and appends `.so`. This is the literal mechanism behind
  the `Initialize`/`CreateGame` symbol-name lookups discussed in
  [[server-game-decoupling-investigation]].
- `SystemSpecificTCPIP.hpp` — platform macros only (no .cpp): winsock vs. BSD-socket
  `SOCKET`/`ERRSTRING`/`CLOSESOCKET`/`ACCEPT`/`IPC_INITIALIZE` defines, consumed by
  `server/ServerSocket.hpp`.

**General utilities:**
- `FileUtilities.hpp/cpp` — `FileExists`, `MakeDirectory`, `GetDirectoryContents` (`opendir`/
  `readdir`), plus the `DIR_SEP` macro (`\` under msys, `/` otherwise). Used by `ServerGameInfo`
  (creating `<datadir>/save`) and `Room::GetSavedGameString` (listing save files).
- `StringUtilities.hpp/cpp` — `Tokenize` (splits on a set of separator characters, with an
  `AllowEmpties` flag), used by both `GameCloset`'s config-file parsing and `ActionParser`'s
  line parsing. Also `UnComma`/`ReComma` — the wire protocol's escaping scheme: `,` → `%C` and
  `%` → `%%` on the way out, reversed on the way in, so a field's content can safely contain a
  literal comma or percent sign inside an otherwise comma-delimited line. Every comma-delimited
  line in this codebase (client↔server *and* `ActionParser`-shaped calls into a `Game`) relies on
  this.
- `Serialize.hpp` — thin macro wrapper around Boost.Serialization, hardcoded to **XML archives**
  (`boost::archive::xml_oarchive`/`xml_iarchive`, aliased `SaveArchive`/`LoadArchive`) — not binary,
  correcting an earlier mis-statement in [[server-game-decoupling-investigation]]. Provides
  `SERIALIZE_FUNC`/`SERIALIZE_LOAD_FUNC`/`SERIALIZE_SAVE_FUNC`/`SERIALIZE`/`SERIALIZE_PARENT` macros
  used throughout the game DLLs' `DLLGame`/`StateWalker` save/load path.
- `SerTest.cpp` — a standalone scratch demo of the `Serialize.hpp` macros (a `foo<T>`/`bar` toy
  hierarchy round-tripped through save/load); not production code, just a worked example.

**How to apply:** if code elsewhere references `GenericLibraryLoader`, `Tokenize`, `UnComma`/
`ReComma`, `DIR_SEP`, or the `SERIALIZE*` macros, this is where they live and what they actually do
— no need to re-derive behavior from call sites.
