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
- Most notable subdirectory: `OpenZorz` — a holdover from the project's old name (see [[project-overview]]). Slated for renaming — tracked in `TODO.md` (see [[roadmap-todo]]) ("expunge the term OpenZorz from all code").
- Designed originally as an in-browser Java Applet.
- Connects to the backend C++ server (see [[component-cpp-server]]) via TCP/IP.

**Current execution model (as of 2026-09-12):** Originally an in-browser Applet, but in-browser Java is no longer viable. Since a Java Applet is just a subclass of `java.awt.Panel`, Albert was able to make it runnable as a standalone desktop application with a small change, rather than a rewrite. The script that currently launches the client is `../runclient` (relative to this component — i.e. a top-level `runclient` file in the project). This standalone mode is a stopgap to keep the project alive/playable, not the desired end state.

**Long-term goal:** Albert wants to eventually rewrite the entire front-end using modern/current state-of-the-art browser GUI tech (framework TBD). He is not a front-end developer by background and expects to need significant help designing/implementing that rewrite. Tracked in `TODO.md` (see [[roadmap-todo]]). This is a big, not-yet-started initiative — distinct from the smaller "expunge OpenZorz naming" cleanup task.

**GUI architecture (two generations, both in use):**

1. **Original/legacy approach:** Most classes in the `OpenZorz` directories subclass corresponding Java Swing widgets, forming the standard Swing tree (Frame → Panels → nested Panels/widgets, each panel with a Layout Manager). [[protocol-terminology]] Events are routed from the top-level frame down through this tree, and individual widgets are configured to respond to specific data in specific Events. The entire panel/widget/event-response configuration is data-driven: stored as XML in the game-specific jar and read at runtime by the `GameXMLWalker` class.
   - Albert's retrospective take: seemed clever at the time, but proved cumbersome and incredibly limiting in practice.
2. **Later/preferred approach — `ActionGeneratorNode`:** Written to replace the XML-driven approach. An `ActionGeneratorNode` embodies an arbitrary panel, subclassable to any arbitrary Java widget collection, with arbitrary code for both rendering incoming Events and reacting to widget input by sending Actions back to the server. Almost all game-specific GUIs built after this class existed use `ActionGeneratorNode` almost exclusively, rather than the `GameXMLWalker`/XML approach.

**How to apply:** When working on any per-game GUI, expect `ActionGeneratorNode`-based code to be the norm and the `GameXMLWalker`/XML-config approach to be legacy/vestigial — favor understanding and extending via `ActionGeneratorNode` patterns rather than the older XML system unless the specific game predates that class. This split may also inform the eventual browser front-end rewrite: `ActionGeneratorNode`'s per-widget Action/Event handling model is likely the better pattern to carry forward conceptually. Expect legacy "OpenZorz" naming in this directory (package names, class names, file paths) — when touching this code, prefer renaming toward "GameMachine" naming per the TODO item rather than perpetuating "OpenZorz", but don't do a mass rename unprompted since it's a tracked but not-yet-scheduled task. Don't assume the client still runs as an in-browser Applet — check `runclient` for the actual current launch mechanism. When front-end rewrite discussions come up, lean in with more front-end guidance/explanation than usual since Albert has explicitly said this is a gap in his own background, unlike his general seniority (see [[user-profile]]).
