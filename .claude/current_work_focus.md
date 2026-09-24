---
name: current-work-focus
description: Albert's current work allocation across games/initiatives — MoV primary, Outpost refresh deferred, plus some cross-project "systemic" tasks
metadata:
  type: project
---

As of 2026-09-19, Albert's primary active work is [[merchant_of_venus_random_refactor|MerchantOfVenus]] (see also [[mapdata_pugixml_migration]]). Confirmed directly by Albert: "We'll go back and fix the Outpost when we do its refresh...we're mostly working on MoV right, although there are some systemic tasks I'm looking at."

**Why this matters:** when a fix or gap is found in code shared across games (build system patterns, common/gamecommon library issues, etc.), don't reflexively fix it everywhere — check whether it's in scope for the current task, note it for later, and defer game-specific instances outside MoV to that game's future refresh. Example: the `.dll`-hardcoded-on-Linux Makefile bug (see [[mapdata_pugixml_migration]]) was fixed for MoV but deliberately left alone in `Outpost/tca/Makefile` and `AOR/Makefile` — Outpost's fix is intended to land as part of its "2nd gen rules" refresh mentioned in [[games_overview]], not as a drive-by now.

**How to apply:** default assumption for new work is MoV unless Albert says otherwise. "Systemic tasks" (his term) means cross-cutting/infrastructure work that isn't game-specific — e.g. the [[build_environment]] Linux migration, [[server_game_decoupling_investigation]] — those are fair game to work on directly rather than deferring. Game-specific cleanup outside MoV (Outpost, AOR, RoadsAndBoats) should generally wait for that game's own dedicated pass unless it's blocking current MoV work.
