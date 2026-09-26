---
name: roadmap-todo
description: "Pointer to the project's TODO.md roadmap file and a summary of the major pending initiatives"
metadata: 
  node_type: memory
  type: reference
  originSessionId: 416ca727-2f1a-4082-9558-d306f19709ba
  modified: 2026-09-13T05:36:16.367Z
---

There is a `TODO.md` at `.claude/TODO.md` (repo-relative — this whole `.claude/` directory, including this memory, is checked into the repo itself so it travels with the code across machines, e.g. Windows laptop and EC2) tracking Albert's roadmap/wishlist items, built up across conversations. Check it at the start of any substantial work session for current priorities — it's a plain checklist, not memory content itself, so read it fresh rather than trusting a stale summary here.

As of 2026-09-12, major items (roughly small→huge):
1. Expunge "OpenZorz" naming from the codebase (see [[project-overview]]).
2. Rewrite the Java client front-end using modern browser GUI tech (see [[component-java-client]]).
3. Rearchitect game state/Events to use automatic dirty-tracking instead of manual Events + `refresh` (see [[game-server-xml]]).
4. Replace the XML + `transitioncompiler` pipeline with native C++ FSM code (see [[component-perl-compiler]]).
5. **Corrected 2026-09-26 — split in two, no longer "biggest":** porting the *server* off C++ (Java/Python candidates) is now small/tractable once the [[server-game-interface-spec]] process-decoupling work (phase 1) lands, since the server no longer needs to know anything about game logic. Porting *individual games* off C++ remains the genuinely large, separable, per-game effort — see that memory's `transitioncompiler` findings for why (embedded C++ rule bodies have no portable representation).
6. Complete MerchantOfVenus client + server (see [[games-overview]]).
7. Start "Interstellar" once boards/rules are supplied (new game, not yet started — see [[games-overview]]).
8. Migrate server to AWS serverless (Lambda + API Gateway, with DynamoDB for state). This is unstarted. **Correction:** `awsnotes.txt` is NOT related to this — it documents the *current* deployment (EC2 + CloudFront), a separate/earlier effort. See [[aws-deployment]].

`TODO.md` also has a **MerchantOfVenus-specific** section (added 2026-09-12, during the implementation-status deep dive — see [[games-overview]]): validate relic behavior, validate buy/sell vs. barter, add an option to strip the unused combat items (Laser/Nova Ball) from play, and check whether space cities and land cities both count as "cities" for the one-buy/one-sell-per-landing rule. (A fifth item — reconciling `MerchantOfVenusMap.xml`'s 24 qbox spaces against a rulebook figure implying 25 — was resolved and removed: Albert confirmed by checking the physical board that 24 is correct and the rulebook's "11 leftover" figure is simply a misprint. `MerchantOfVenusMap.xml` needed no change.)

**How to apply:** These items have real dependency/ordering relationships. **Correction 2026-09-26:** #5 no longer subsumes #3/#4 — that assumed porting the server meant porting every game's logic along with it, which the process-decoupling work (see [[server-game-decoupling-investigation]], [[server-game-interface-spec]]) disproved; #5's server-half and #3/#4 (game-side representation, message shape) are now understood to be orthogonal. #3's JSON-state idea still pairs naturally with #5's server-language choice and #8's DynamoDB storage in spirit. When Albert brings up one of these, it's worth surfacing related items from this list rather than treating them in isolation. Server-side process-decoupling work (a smaller, incremental step distinct from all of the above) is now actively in progress as of 2026-09-26 — see [[server-game-interface-spec]] for the full design and its three-phase rollout. (Do not confuse #8 with the already-existing EC2/CloudFront deployment described in [[aws-deployment]] — that's current infrastructure, not the serverless migration target.)
