---
name: protocol-terminology
description: "Client/server messaging vocabulary in The Game Machine — \"Action\" vs \"Event\""
metadata: 
  node_type: memory
  type: project
  originSessionId: 416ca727-2f1a-4082-9558-d306f19709ba
  modified: 2026-09-12T07:54:39.893Z
---

Part of [[project-overview]]. Cross-cutting terminology used throughout the codebase (not specific to [[component-cpp-server]] or [[component-java-client]] or any one game) for client↔server communication:

- **"Action"** — a message from the client to the server. Represents an attempted state change on the server: moving between rooms, starting a game in a room, making a move in a game being played, etc. Anything the client asks the server to do is an Action.
- **"Event"** — a message from the server back to the client. Represents a delta to observable game state that the client should render to the user.

**How to apply:** When reading or writing code, treat any symbol/class/message named `*Action*` as client→server intent, and `*Event*` as server→client notification. This directional convention should hold everywhere in the codebase — server code, game DLLs, client code, and the Perl-generated C++ from XML game specs.
