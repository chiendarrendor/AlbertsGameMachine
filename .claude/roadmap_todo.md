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
5. **(Biggest)** Port the entire server off C++ to a language with first-class JSON support (Java/Python candidates) — full rewrite of server + all games.
6. Complete MerchantOfVenus client + server (see [[games-overview]]).
7. Start "Interstellar" once boards/rules are supplied (new game, not yet started — see [[games-overview]]).
8. Migrate server to AWS serverless (Lambda + API Gateway, with DynamoDB for state). This is unstarted. **Correction:** `awsnotes.txt` is NOT related to this — it documents the *current* deployment (EC2 + CloudFront), a separate/earlier effort. See [[aws-deployment]].

`TODO.md` also has a **MerchantOfVenus-specific** section (added 2026-09-12, during the implementation-status deep dive — see [[games-overview]]): validate relic behavior, validate buy/sell vs. barter, add an option to strip the unused combat items (Laser/Nova Ball) from play, and check whether space cities and land cities both count as "cities" for the one-buy/one-sell-per-landing rule. (A fifth item — reconciling `MerchantOfVenusMap.xml`'s 24 qbox spaces against a rulebook figure implying 25 — was resolved and removed: Albert confirmed by checking the physical board that 24 is correct and the rulebook's "11 leftover" figure is simply a misprint. `MerchantOfVenusMap.xml` needed no change.)

**How to apply:** These items have real dependency/ordering relationships (e.g. #5 likely subsumes #3 and #4; #3's JSON-state idea pairs naturally with both #5's language choice and #8's DynamoDB storage — a JSON-shaped game state is a natural fit for a document store, so #3/#5/#8 could plausibly be planned as one coherent direction rather than three separate efforts). When Albert brings up one of these, it's worth surfacing related items from this list rather than treating them in isolation. None of these are started/scheduled as of this writing — they're captured intent, not active work. (Do not confuse #8 with the already-existing EC2/CloudFront deployment described in [[aws-deployment]] — that's current infrastructure, not the serverless migration target.)
