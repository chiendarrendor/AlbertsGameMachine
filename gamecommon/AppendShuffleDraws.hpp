#ifndef APPENDSHUFFLEDRAWSHPP
#define APPENDSHUFFLEDRAWSHPP

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

// WHY THIS FUNCTION EXISTS:
//
// Random::Shuffle (gamecommon/Random.hpp) shuffles a sequence of N items with
// a Fisher-Yates-style algorithm: it calls MyRand(N), then MyRand(N-1), ...,
// down to MyRand(1), swapping as it goes. Every one of those MyRand() calls
// bottoms out in a single call to RandomFactory::Next(upper) (gamecommon/
// RandomFactory.hpp).
//
// This project used to make tests deterministic by calling libc srand(5) (or
// some other magic seed) and hardcoding, as a comment, whatever telegate
// layout/dice sequence that seed happened to produce -- see
// MerchantOfVenus/tests/MoveMediatorTest.cpp and .claude/
// merchant_of_venus_random_refactor.md for the full story of why that broke:
// libc rand()'s sequence for a given seed is platform-defined, so the exact
// same seed produced different, "wrong" results once development moved from
// MSYS/mingw to this Linux/glibc box. The fix was an injectable
// Random/RandomFactory seam: FixedSequenceRandomFactory is the test double
// that plays back an exact, known list of Next()-call answers instead of
// relying on what a seed happens to produce.
//
// But FixedSequenceRandomFactory only understands raw draws (the r_k values
// Fisher-Yates itself consumes) -- it has no notion of "shuffle this list
// into that order". Scripting a *specific* shuffle outcome by hand would mean
// a person re-deriving, step by step, exactly what Fisher-Yates does with a
// pencil and paper -- precisely the "reverse-engineer what a seed happens to
// produce" anti-pattern this whole refactor exists to eliminate. And an
// earlier version of this helper that took an index permutation instead of
// actual values had the same problem one layer up: a person would still have
// to hand-translate "I want AS_3_4 first" into "that's original index 3" by
// cross-referencing wherever the original order came from.
//
// AppendShuffleDraws does ALL of that translation once, mechanically, so
// nobody ever has to do it by hand again: give it the sequence Random::
// Shuffle will actually start from (i_original) and the sequence you want it
// to produce (i_desired), using the real, meaningful values on both sides --
// no indices, no manual seed-hunting -- and it appends the exact sequence of
// Next()-call answers that will make that happen.
//
// It APPENDS to io_draws (rather than returning a fresh vector) specifically
// so a test can build one flat, linear script covering everything the code
// under test will ask Random::Instance() for, in call order -- e.g.
// MapOverlay's constructor does one qbox shuffle immediately followed by one
// relic shuffle, and the rest of a MoveMediatorTest.cpp case is plain
// DieRoll() calls. Since a shuffle and a die roll both bottom out in the same
// RandomFactory::Next(upper), they can all be scripted into one
// FixedSequenceRandomFactory, in the exact order the test needs them:
//
//   std::vector<int> draws;
//   AppendShuffleDraws(draws, qboxOriginalOrder, qboxDesiredOrder);
//   AppendShuffleDraws(draws, relicOriginalOrder, relicDesiredOrder);
//   draws.push_back(3); // a DieRoll() call right after the shuffles
//   Random::Instance().SetFactory(std::make_unique<FixedSequenceRandomFactory>(draws));
//
// i_original is taken BY VALUE (not by reference) because it's consumed as
// scratch space while matching -- the caller's own copy of the original
// order is left untouched.
//
// Duplicate values in i_original (e.g. several interchangeable "asteroid"
// placeholders) are fine: the first remaining match at or after position k is
// used, and since duplicates are by definition indistinguishable, any valid
// tie-break reproduces the same observable outcome.
//
// Throws std::runtime_error if i_original and i_desired aren't the same
// size, or if some i_desired[k] has no matching value left in i_original --
// both indicate the desired order doesn't describe an actual permutation of
// the original one, which means the test's script is wrong, not this
// function.
template<class T>
void AppendShuffleDraws(std::vector<int>& io_draws,std::vector<T> i_original,const std::vector<T>& i_desired)
{
  if (i_original.size() != i_desired.size())
    throw std::runtime_error("AppendShuffleDraws: original and desired sequences differ in size");

  for (size_t k = 0 ; k < i_original.size() ; ++k)
  {
    size_t j = k;
    while (j < i_original.size() && !(i_original[j] == i_desired[k])) ++j;

    if (j >= i_original.size())
      throw std::runtime_error("AppendShuffleDraws: desired value not found in remaining original sequence");

    io_draws.push_back(static_cast<int>(j-k));
    std::swap(i_original[k],i_original[j]);
  }
}

#endif
