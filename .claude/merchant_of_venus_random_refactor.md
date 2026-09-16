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

**Explicitly deferred (see `.claude/TODO.md`):** `MoveMediatorTest.cpp`'s 11 `srand(5)`/`srand(7)`-
seeded test cases still hardcode the old approach and are left untouched — Albert wants them rewritten
against `FixedSequenceRandomFactory` in a follow-up commit, one at a time, working out each test's
actually-intended `Shuffle`/`DieRoll` sequence. Until that lands, `MoVunittests.exe` has 15 known failures
in those cases (confirmed still 15 after this refactor, same as before — nothing new broken).
`TestStartMove`'s two "retry until desired die roll" loops were NOT touched and don't need to be —
they check variability/consistency across up to 10 tries rather than hardcoding an outcome, so they
work fine against any factory.

**Helper for the deferred test conversion:** `gamecommon/AppendShuffleDraws.hpp` (header-only
template, added 2026-09-16) converts "the order I actually want" into the raw `FixedSequenceRandomFactory`
draws `Random::Shuffle`'s Fisher-Yates needs to produce it — `AppendShuffleDraws(draws, originalOrder,
desiredOrder)` appends onto an existing `std::vector<int>`, so a test can build one flat script mixing
shuffle draws with plain `DieRoll()`/other `Next()` values in call order (see the header's own
extensive comment for the full derivation and a worked example). This is what the follow-up pass
converting `MoveMediatorTest.cpp`'s 11 cases should use instead of hand-deriving sequences.

**How to apply:** When adding new game randomness anywhere in MoV/Outpost/AOR, keep using
`myrand`/`RandomBetween`/`DieRoll`/`myshuffle` as before (they're free — the forwarding is invisible
to callers). When writing or fixing a test that needs a specific random outcome, use
`Random::Instance().SetFactory(std::unique_ptr<RandomFactory>(new FixedSequenceRandomFactory({...})))`
before the code under test runs, and `Random::Instance().ResetToDefault()` after — never reach for
`srand()` again, it no longer does anything (nothing calls libc `rand()` anymore, confirmed via
full-repo grep).
