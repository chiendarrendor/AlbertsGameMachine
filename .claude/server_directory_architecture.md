---
name: server-directory-architecture
description: Code-verified walkthrough of server/ — the socket layer, login, room orchestration, and the DLL-loading glue between the generic server and per-game code
metadata:
  type: project
---

Read in full 2026-09-18 (all 12 source files, ~1,400 lines) as part of [[cpp-server-glue-session]].
Supersedes the "not yet code-verified" caveat in [[component-cpp-server]] for the `server/` half of
that note. Related: [[component-game-dlls]], [[game-server-xml]], [[protocol-terminology]].

**Layer 1 — raw sockets (`ServerSocket.hpp/cpp`):** single-threaded, `select()`-based event loop on
a hardcoded port (4356). `ServerSocket` owns the listener and dispatches readable sockets;
`ConnectionHandler` is an abstract per-connection base that buffers bytes until a `\n`, then calls
the virtual `HandleLine`. Deletion is deferred via a "doomed" list swept once per loop iteration, to
avoid touching a handler mid-iteration.

**Layer 2 — session/login (`GameServerConnectionHandler.hpp/cpp`, `LoginManager`):** the concrete
`ConnectionHandler`. Before a connection is named, it only accepts `LOGIN,name,password` or
`LOGOUTOTHER,name,password`; everything else errors. `LoginManager` is a flat-file credential
store with a notable quirk — there's no separate signup step: the *first* login attempt for an
unknown name silently creates the account with whatever password was given, appended to the file.
Once named, all lines forward to `ActionManager::HandleAction`.

**The generic/game-specific seam — two abstract interfaces:**
- `ActionManager` (`ActionManager.hpp`, implemented once, by `RoomManager`): `HandleInitialConnection`
  / `HandleAction` / `HandleDisconnect`. This is the boundary between "network session" and "game
  orchestration."
- `OutputPort` (declared in gamecommon, not in `server/`; implemented here by `RoomOutputPort`):
  `UniCast`/`BroadCast`/`VariCast`. This is how a `Game` object pushes state back out;
  `RoomOutputPort` adapts it to a set of named socket connections within one room.

**Room orchestration (`RoomManager`/`Room`):** rooms are lightweight chat-room-like containers;
each hosts at most one active `Game`. `RoomManager` handles `NEWROOM`/`CHANGEROOM`/`ROOMTALK`/
`PLAYERTALK`, and forwards anything else to the player's current `Room`, which itself intercepts
`NEWGAME`/`LOADGAME`/`SAVEGAME` and forwards everything else straight into the active
`Game::HandleAction`.

**The actual DLL-loading glue (`GameBox`/`GameCloset`) — this is the real mechanism connecting
generic server code to per-game code:** `GameCloset` reads a config file (`name,XMLloc,XMLfile,
DLLfile` per line) and constructs a `GameBox` per entry. `GameBox` dynamically loads the DLL/.so via
`GenericLibraryLoader` (from `common/`), looks up two C-ABI symbols by name — `Initialize(std::string)`
and `CreateGame(const ServerGameInfo &, OutputPort &) -> Game *` — and calls them. No vtable/COM-style
interface at the loading boundary itself, just a raw function-pointer typedef (`CreateGamePointer`)
resolved by symbol name at load time. `GameBox` itself derives from `ServerGameInfo` (from
gamecommon, not yet read) and holds the loaded library + resolved `CreateGame` pointer.

**`ServerMain.cpp` wiring:** takes a single data-dir argument on the command line. Sets up logging
(`LogManager`/`ServerLogger`), `LoginManager` (`<datadir>/passwd`), `GameCloset` (`<datadir>/
gameconfig.txt`, loads every game's DLL up front), reads `<datadir>/mainloc.txt` for the lobby's own
XML descriptor (2 lines: XML location, XML filename), builds `RoomManager`, wraps it in a
`GameServerConnectionHandlerFactory` together with the `LoginManager`, and hands off to
`ServerSocket::RunConnections()` on port 4356, which runs until all sockets close.

**Protocol shape (reinforces [[protocol-terminology]]):** comma-separated ASCII lines terminated by
`\n`, parsed by `ActionParser` (declared elsewhere, not yet read) into an action name + args.

**Update 2026-09-18:** `gamecommon`/`stateengine` have since been read too — see
[[server-game-decoupling-investigation]] for the full writeup of `Game`, `OutputPort`,
`ServerGameInfo`, `ActionParser`, and the `StateWalker` FSM engine, done in service of exploring
whether games could become separate, language-agnostic processes. `GenericLibraryLoader` itself
(the actual dlopen/LoadLibrary wrapper) lives in `common/` and is still unread.

**How to apply:** when reasoning about how a game DLL plugs into the server, the load-time contract
is exactly `Initialize(std::string) -> bool` and `CreateGame(const ServerGameInfo&, OutputPort&) ->
Game*`, resolved by symbol name — that's the entire ABI surface at the dynamic-loading boundary.
Everything else (the `Game` interface itself: `HandleAction`, `SendFullState`, `Load`/`Save`,
`IsDone`, `GetName`, `GetStatusString`) is defined by the `Game` base class the DLL's `CreateGame`
returns — see [[server-game-decoupling-investigation]] for the detailed breakdown of that interface.
