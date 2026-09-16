---
name: repo-untracked-dirs
description: "How to interpret untracked directories that show up in git status: tca/* subdirs are build artifacts, top-level GameMachine/ is an abandoned IntelliJ dev attempt"
metadata:
  type: project
---

Two recurring untracked-directory patterns Albert has explained, confirmed 2026-09-14:

- **Any untracked content inside a `tca/` directory is almost certainly a build artifact**, not in-progress work worth preserving carefully. `tca` dirs live under each game (see [[games-overview]], [[build-environment]]) and hold build output/staged client resources (images, compiled resources, etc.) — safe to treat as regeneratable.
- **`GameMachine/` (top-level, contains `GameMachine.iml`, `.idea/`, `out/`, `src/`)** is Albert's attempt at doing dev via IntelliJ — an IDE project scratch directory, not part of the shipped project layout described in [[project-overview]]. `*.iml` is already gitignored; the rest of the directory isn't tracked either.

**How to apply:** don't treat these as mysterious/unexplained state requiring investigation before touching — untracked `tca/*` content can be assumed disposable, and `GameMachine/` can be assumed to be IDE scratch rather than lost work. Still avoid deleting either without asking, per normal caution around untracked files, but no need to dig into their contents first.
