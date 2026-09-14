---
name: project-overview
description: "High-level architecture of \"The Game Machine\" (formerly \"OpenZorz\") — Albert's personal board-game platform project"
metadata: 
  node_type: memory
  type: project
  originSessionId: 416ca727-2f1a-4082-9558-d306f19709ba
  modified: 2026-09-12T09:22:03.283Z
---

"The Game Machine" is Albert's personal project embodying a custom-designed programming/spec language that lets a developer specify the rules of board games, then play those games with other people over the internet. Formerly named "OpenZorz" — Albert now considers that name bad, but remnants of it may still appear in old code/files/comments.

Major components (each a different language/tech, kept as separate memories — see [[component-cpp-server]], [[component-game-dlls]], [[component-perl-compiler]], [[component-java-gui]], [[component-java-game-guis]] as they're filled in):

1. **C++ TCP/IP server** — the networking/server backbone that lets players connect and play across the internet.
2. **Game DLLs (C++)** — one dynamically-linked library per game, handling that game's state/rules logic.
3. **Perl compiler** (`transitioncompiler` dir, see [[component-perl-compiler]]) — translates the XML game-rule spec files into C++ feeding the game DLLs. Note: this is templating/scaffolding, not a real custom-language parser — the "coded intelligence" in the XML is literally embedded C++ snippets (Albert initially described the project as "a programming language of my own design," but later clarified this was a bit of a fib — see [[component-perl-compiler]] for the correction).
4. **Java common GUI front end** — the shared client-side GUI application used across all games.
5. **Java game-specific GUI DLL-equivalents** — per-game GUI presentation modules, analogous in role to the C++ game DLLs but on the Java client side.

**Why this structure:** Albert wants per-component memory files (since components differ by language) rather than one big memory, to keep future sessions cheap — only load the component(s) relevant to the task at hand, without ingesting the whole codebase.

**How to apply:** Before diving into code in any one component, check if a dedicated component memory exists and read that first rather than re-deriving architecture from scratch. Update this overview file if Albert describes new components or corrects this breakdown.

**Git history caveat:** The project itself is ~25 years old, but Albert didn't start using git/GitHub for it until ~8 years ago (as of 2026, so roughly ~2018). The full `git log` for this repo is very short (as of 2026-09-12, just 5 commits total) — it is NOT a representative "recent activity" sample, it's close to the entire tracked history, and it captures only the last small slice of this project's real lifetime. Do not infer "this project is new/small" from git log, and do not treat git blame/log as authoritative for design history or rationale predating ~2018 — most of the project's history has no git record at all.
