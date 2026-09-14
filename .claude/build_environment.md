---
name: build-environment
description: "The Game Machine's C++ build is intentionally dual-environment (Windows/MSYS local dev + Linux/EC2 deploy) — preserve both, don't collapse to one"
metadata: 
  node_type: memory
  type: feedback
  originSessionId: 416ca727-2f1a-4082-9558-d306f19709ba
  modified: 2026-09-13T22:00:48.096Z
---

Albert has a general professional dislike of legacy projects carrying build/deploy support for environments other than the single final deployment target — but explicitly prides himself that **this particular personal project is dual-environment-capable on purpose**: it builds both locally on Windows/MSYS and on the Linux EC2 deployment target (see [[aws-deployment]]).

The mechanism (found in `server/Makefile`) is an `OSTYPE`-conditioned `BOOSTINC`:
```makefile
ifeq (${OSTYPE},msys)
BOOSTINC=/c/Boost/include/boost-1_46
endif
ifeq (${OSTYPE},linux)
BOOSTINC=/usr/include
endif
```

**Discovered 2026-09-14, during a MerchantOfVenus Boost build-error investigation:** during Albert's recent Makefile refresh for EC2 deployment (server, generic client, Outpost backend+frontend), `Outpost/tca/Makefile` was simplified to hard-code `BOOSTINC=/usr/include` — dropping the `OSTYPE` conditional entirely, i.e. it silently lost local-Windows buildability. `MerchantOfVenus/tca/Makefile` was left out of the refresh altogether and never had the conditional at all (predates it, or lost it), which is the immediate cause of MoV's build error (`BOOSTINC` resolves empty). Also: no Boost install of any kind currently exists on this Windows machine — the `msys` branch's path (`/c/Boost/include/boost-1_46`) doesn't exist, so even a syntactically-fixed Makefile can't build MoV locally until Boost 1.46 (MinGW-compatible) is (re)installed.

**Gotcha confirmed 2026-09-14 — `OSTYPE` must be explicitly exported:** bash sets `$OSTYPE` as a built-in shell parameter, but does NOT export it to child processes by default — `make` (a separate process) sees it as completely undefined unless something does `export OSTYPE=...` first, which silently defeats every `ifeq (${OSTYPE},...)` branch (leaves `BOOSTINC` fully unset, not just empty). Albert's fix: `export OSTYPE=msys` is now in `~/.bash_profile` on this MSYS/MinGW machine (and `export OSTYPE=linux` in `.bashrc` on his Linux/EC2 host). **Tool-specific trap:** each Bash-tool invocation in this Claude Code session starts a fresh shell — exported vars from one Bash call do NOT persist to the next one (only the working directory does) — so `source ~/.bash_profile` must be chained in the *same* command as any `make` invocation (e.g. `source ~/.bash_profile && make`), not run as a separate prior step.

**Also confirmed 2026-09-14:** the installed local MinGW compiler is GCC 6.3.0, while the Boost libs the Makefiles reference are named for `mgw45` (GCC 4.5) — even after fixing `BOOSTINC`, old prebuilt Boost 1.46 `.a` files won't ABI-match this compiler. No Boost install of any kind currently exists on this machine at all. Albert has said sourcing/building a GCC-6.3-compatible Boost locally is his own task, not something to hand to Claude.

**How to apply:** When fixing any per-game `tca/Makefile`, restore/preserve the `OSTYPE`-conditioned dual-path pattern from `server/Makefile` — do not follow `Outpost/tca/Makefile`'s simplified single-path version as the template, since that itself is a (likely unintentional) regression against Albert's stated intent. Worth proactively flagging to Albert if `Outpost/tca/Makefile`'s dropped Windows-buildability comes up, since he may want it restored too.
