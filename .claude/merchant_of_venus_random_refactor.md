---
name: merchant-of-venus-random-refactor
description: Replaced libc rand()-based randomness with an injectable Random singleton / RandomFactory seam in gamecommon, to fix MoveMediatorTest.cpp's platform-dependent srand() test failures
metadata:
  type: project
---

Part of [[project-overview]] / [[component-game-dlls]]. Landed 2026-09-16.

**The problem:** `MoVunittests.exe` had 15 failing tests in `MerchantOfVenus/tests/MoveMediatorTest.cpp`.
Root cause: `gamecommon/Utilities.hpp`'s old `myrand`/`RandomBetween`/`DieRoll`/`myshuffle` all called
libc `rand()` directly. The tests hardcoded `srand(5)` (occasionally `srand(7)`) and asserted the
exact downstream outcome (which telegate lands where, which die value comes up) as literal expected
strings, with comments like "srand seed 5 has the following telegates: ...". libc `rand()`'s sequence
for a given seed is platform-defined — MSYS/mingw and glibc (this EC2 box) produce different
sequences for the same seed, so those hardcoded expectations only ever matched the MSYS side. See
[[build-environment]] for the related (and prompting) decision to drop MSYS support entirely.

**The fix (architectural, not just re-capturing expected values for Linux):** game code should never
depend on the exact sequence a particular `rand()` implementation produces for a seed. Added an
injectable seam in `gamecommon/`:
- `RandomFactory.hpp` — abstract interface, `int Next(int upper)` returns a value in `[0,upper)`.
- `SystemRandomFactory.hpp`/`.cpp` — production default, `std::mt19937` seeded once from
  `std::random_device` (replaces libc `rand()`/`srand()` entirely — this is also why
  `server/RoomManager.cpp`'s old `srand(time(NULL))` was deleted as dead code, and why the leftover
  `srand(time(NULL))` calls in a few test files are now harmless no-ops rather than real seeding).
- `FixedSequenceRandomFactory.hpp`/`.cpp` — test double, constructed from a `std::vector<int>`; each
  `Next(upper)` call pops the front value and throws `std::runtime_error` if the list is exhausted or
  the value doesn't fit `[0,upper)`. Lets a test script an exact, known outcome instead of reverse-
  engineering what a seed happens to produce.
- `Random.hpp`/`.cpp` — the singleton (`Random::Instance()`) that holds the current factory
  (`SetFactory()`/`ResetToDefault()`) and exposes `MyRand`/`Between`/`DieRoll`/`Shuffle` (same
  Fisher-Yates `Shuffle` algorithm as the old `myshuffle`, just sourcing randomness through the
  factory instead of raw `rand()`).

**Deliberately zero call-site churn:** `gamecommon/Utilities.hpp` keeps `myrand()`, `RandomBetween()`,
`DieRoll()`, `myshuffle()` as one-line forwarders to `Random::Instance()` — Albert's explicit choice,
to avoid touching all ~15 call sites across MoV/Outpost/AOR (`Cup.cpp`, `MapOverlay.cpp`,
`Players.cpp`, `TradeBases.cpp`, `MoveMediator.cpp`, `Outpost/CommodityDeck.cpp`,
`Outpost/UnownedItems.cpp`, `Outpost/Players.cpp`, `AOR/CardPile.cpp`, `AOR/Players.cpp`) for no
behavioral difference.

**Build wiring:** `gamecommon/Makefile`'s `OBJS` gained `Random.o SystemRandomFactory.o
FixedSequenceRandomFactory.o` (first `.cpp` files gamecommon ever needed — `Utilities.hpp` used to be
pure header-only). `Makefile.depend` was regenerated. No changes needed to the three games'
Makefiles — they already link the whole `libGameCommon.a` archive.

**Follow-up landed 2026-09-17:** `MoveMediatorTest.cpp`'s 11 `srand(5)`/`srand(7)`-seeded test cases
have been converted to use `FixedSequenceRandomFactory`. `MoVunittests.exe` now passes all 31 cases,
confirmed deterministic across repeated runs (not luck). Key findings from that conversion, worth
knowing before touching this test file again:
- `MapOverlay`'s qbox shuffle is a 36→24 draw, not a 24-item reorder: `MakeQBoxList()` builds 36
  candidates (6 telegates, 3 open ports, 17 penalties, 10 asteroids), shuffles all 36, and only the
  first 24 (post-shuffle) end up on the real board (`MerchantOfVenusMap.xml` has exactly 24
  `type="qbox"` spaces) — the other 12 are discarded that game. Scripting this shuffle means
  supplying a full 36-element target order, not just a 24-element one.
- `qboxnames` (the real 24 positions) is populated by walking `std::map<std::string,MapSpace*>`, so
  it's in sorted-by-name order — that order is now encoded as `g_QboxDeckOrder`/`g_QboxLayout` at the
  top of `MoveMediatorTest.cpp`.
- The relic shuffle (10 items, right after the qbox one in the same constructor) is untested by
  anything in this file — scripted as an identity/no-op (10 zero draws).
- 9 of the 11 tests never depend on the *specific* dice values `StartMove()` draws (their
  `SpendMP(mm.GetMP()-N)` calls are self-normalizing to leave exactly `N` MP regardless of the
  original roll, and nothing else checks `GetMP()` against a fixed threshold below the minimum
  possible 3-die-Scout roll) — arbitrary draws work for those. Only
  `TestCullByPilotNumberNoPilotNumbers`/`WithPilotNumbers` genuinely need the dice set `{1,4,3}`
  (traced through `CullByPilotNumber()`'s set-intersection logic to confirm), matching the original
  "causes the dice roll to be 143" comment.
- `Random` is a process-wide singleton shared across every Boost test case in one binary, so each
  test wraps its scripted factory in `gamecommon/ScopedFixedRandom.hpp` (RAII: resets to
  `SystemRandomFactory` in its destructor) rather than calling `SetFactory`/`ResetToDefault` by hand —
  otherwise one test's leftover script would leak into whichever test runs next.

**Helper used for the conversion:** `gamecommon/AppendShuffleDraws.hpp` (header-only template, added
2026-09-16) converts "the order I actually want" into the raw `FixedSequenceRandomFactory` draws
`Random::Shuffle`'s Fisher-Yates needs to produce it — `AppendShuffleDraws(draws, originalOrder,
desiredOrder)` appends onto an existing `std::vector<int>`, so a test can build one flat script mixing
shuffle draws with plain `DieRoll()`/other `Next()` values in call order (see the header's own
extensive comment for the full derivation and a worked example). `gamecommon/ScopedFixedRandom.hpp`
(also added 2026-09-17) is the RAII wrapper described above.

**How to apply:** When adding new game randomness anywhere in MoV/Outpost/AOR, keep using
`myrand`/`RandomBetween`/`DieRoll`/`myshuffle` as before (they're free — the forwarding is invisible
to callers). When writing or fixing a test that needs a specific random outcome, use
`Random::Instance().SetFactory(std::unique_ptr<RandomFactory>(new FixedSequenceRandomFactory({...})))`
before the code under test runs, and `Random::Instance().ResetToDefault()` after — never reach for
`srand()` again, it no longer does anything (nothing calls libc `rand()` anymore, confirmed via
full-repo grep).
