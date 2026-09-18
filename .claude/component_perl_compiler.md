---
name: component-perl-compiler
description: "The transitioncompiler Perl CLI — reads <Game>Server.xml, emits C++ .hpp/.cpp for the game DLL"
metadata: 
  node_type: memory
  type: project
  originSessionId: 416ca727-2f1a-4082-9558-d306f19709ba
  modified: 2026-09-13T07:27:14.957Z
---

Part of [[project-overview]]. Lives in the top-level `transitioncompiler` directory. This is a Perl CLI application that reads a game's `<Game>Server.xml` (see [[game-server-xml]]) and writes out the full C++ `.hpp`/`.cpp` source files, which are then compiled into that game's DLL (see [[component-game-dlls]]).

**Important correction to the "custom programming language" framing:** Albert originally described this project as embodying "a programming language of my own design." In practice, he decided against building a real language with its own expressions/function-call semantics (his words: "my compiler class in university was plenty enough, thank you"). Instead, the "coded intelligence" inside Events, Actions, and Transitions in `<Game>Server.xml` (the `allowed` conditions, argument verification, transition bodies, `refresh` body, etc.) is literally embedded snippets of C++ source code within the XML. The Perl compiler's job is templating/scaffolding: it reads the XML structure (states, Events, Transitions) and stitches the embedded C++ snippets into complete, compilable `.hpp`/`.cpp` files — not parsing/interpreting a novel language grammar.

**Possible future replacement (tracked in `TODO.md`, see [[roadmap-todo]], server-side only):** Albert is considering dropping the XML + `transitioncompiler` pipeline entirely, in favor of expressing the FSM/game-state-manipulation logic as native C++ classes/libraries directly (possibly using template metaprogramming). Rationale: he wrote the original XML-driven approach ~25 years ago when he knew C++ less well, and both his skill and the language have grown considerably since. He considers the real "invention" of this project to be the FSM-and-game-state model (see [[game-spec-language]]), not the XML syntax currently used to express it — so this rearchitecture is about changing *how the FSM is authored*, not what it conceptually does. This would eliminate the need for this component entirely if pursued.

**`tca/` directories are generated output, never source:** Each game's `tca/` subdirectory (e.g. `MerchantOfVenus/tca/MerchantOfVenusDLL.cpp`) holds `transitioncompiler`'s generated C++ output — confirmed 2026-09-13 by cross-checking that its transition-body logic matches embedded C++ snippets in `<Game>Server.xml` verbatim. **Do not read, grep, or otherwise query code under any `tca/` directory when investigating game logic or hunting for where something is implemented** — it's a compiled artifact, not source, and Albert has explicitly asked this be remembered. The real, editable source for anything found there is the corresponding `<Game>Server.xml` (for transition/Event/FSM logic) or the game's own `.cpp`/`.hpp` files (for board-component logic) — see [[game-server-xml]] and [[component-game-dlls]].

**How to apply:** Don't go looking for a custom grammar, parser, or expression evaluator — there isn't one. When a change is needed to how `<Game>Server.xml` behaves, first figure out whether it's a change to the *embedded C++* (edit the XML's C++ snippets directly, per-game) or a change to the *scaffolding/structure* the compiler generates (edit `transitioncompiler`'s Perl code, which affects every game). The bigger the change to Events/Transitions' shape (e.g. the dirty-tracking rearchitecture idea in [[game-server-xml]]'s TODO), the more likely it requires touching `transitioncompiler` itself rather than just per-game XML.

**Code-verified 2026-09-18** (all 15 non-`.iml` files read, as part of [[cpp-server-glue-session]] /
[[server-game-decoupling-investigation]]) — confirms everything above and adds detail on exactly
which pieces of a game are structural/portable vs. raw embedded C++:
- **Portable/declarative, extractable straight from the XML by `TransitionCompiler.pm`/`Transition.pm`:**
  state names+descriptions, the legal from→to transition graph (`<transit>`/`<cyclic>`), each
  transition's action-argument *schema* (name, type, optional min/max or list-membership — parsed
  via a `trans` cast expression, default `boost::lexical_cast<Type>`), and each event's *shape*
  (name, arg list, and for `global`/`private` events an iterator spec for looping over some
  server-side collection). This is genuine structural metadata, not C++ — a decoupled/rewritten
  game in another language could mechanically regenerate the equivalent of this part.
- **Not portable — raw C++ pasted verbatim into the generated `.cpp`/`.hpp` by `FileWriter.pm`:**
  a transition's `<allowed>` (legality body), `<auto>` (auto-fire condition), and `<body>` (the
  actual effect of the move) blocks; a `global`/`private` event's per-var `source=` attribute (a
  C++ expression, e.g. `RESET`'s built-in var sourced from `m_ServerGameInfo.GetName()`); the
  top-level `<preface>` (verbatim C++ dumped at the top of the DLL's `.cpp`, typically helpers the
  `<body>` snippets rely on); and `<refresh>` (verbatim C++ body of `SendFullState` — usually a
  game's single biggest hand-written block, looping over game entities and calling the generated
  Unicast/Broadcast event methods). **Porting a game to another language means hand-translating all
  of this — the XML does not carry portable game-logic semantics, only FSM topology + argument
  schema + message shape**, which matters directly for [[server-game-decoupling-investigation]].
- **Confirms the DLL-boundary closes exactly here:** `FileWriter.pm`'s `WriteFiles` emits the
  literal `extern "C" bool Initialize(const std::string&)` and
  `extern "C" Game *CreateGame(const ServerGameInfo&, OutputPort&)` function bodies that
  `server/GameBox.cpp` looks up by symbol name (see [[server-directory-architecture]]) —
  `Initialize` constructs the game's `<Name>Set`/`<Name>StateMachine` singletons, `CreateGame`
  constructs a `DLLGame<<Name>Set,<Name>GameInfo>` and returns it as a `Game*`.
- Every generated event-message builder (`Make<Event>Message`, in `Event.pm`'s
  `GetPrivateCodeBody`) runs each argument through `UnCommaStringify`, which calls the
  `UnComma` escaping helper from [[common-directory-architecture]] — confirming that's where/how
  the wire protocol's comma-escaping is actually applied for outgoing events, not just for
  incoming `ActionParser` lines.
