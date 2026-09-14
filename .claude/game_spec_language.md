---
name: game-spec-language
description: "Core concepts of Albert's game-rule specification language/DSL — game state vs. game finite state machine"
metadata: 
  node_type: memory
  type: project
  originSessionId: 416ca727-2f1a-4082-9558-d306f19709ba
  modified: 2026-09-12T08:16:56.383Z
---

Part of [[project-overview]]. This is the heart of the whole project: Albert's custom-designed language for specifying board game rules (authored as XML, compiled to C++ by the [[component-perl-compiler]] — see that memory once written — and executed via [[component-cpp-server]]'s gamecommon/stateengine, per [[protocol-terminology]] Actions/Events).

Two major components of any game specification:

1. **Game state** — the actual data describing a game in progress: whose turn it is, physical/logical position of all pieces on the board, the exact cards each player holds in hand, etc. This is the "nouns" of the game.
2. **Game finite state machine (FSM)** — the core rules engine concept. Games are modeled with nested time-based structural concepts: **turns**, **phases**, and **steps**. At any given moment, which Actions a player is allowed to take is restricted by where the game currently sits in this turn/phase/step hierarchy. Conversely, the Actions a player takes may themselves cause a transition — changing the current phase or turn. This is the "verbs and control flow" of the game.

**How to apply:** When reasoning about any game's rules implementation (Outpost, MerchantOfVenus, etc.), expect to find these two concerns cleanly separated: state data vs. FSM (turn/phase/step) logic. When debugging "why can't the player do X right now," check the FSM's current turn/phase/step first — legality of Actions is gated by FSM position, not just by game state data. When extending the spec language itself, changes likely touch both the Perl compiler's XML→C++ translation and the shared gamecommon/stateengine interfaces.
