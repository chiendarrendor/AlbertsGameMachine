#ifndef SCOPEDFIXEDRANDOMHPP
#define SCOPEDFIXEDRANDOMHPP

#include <memory>
#include <vector>
#include "FixedSequenceRandomFactory.hpp"
#include "Random.hpp"

// RAII helper for tests that need Random::Instance() to play back a specific,
// known sequence of draws (see FixedSequenceRandomFactory / AppendShuffleDraws).
//
// Random is a single process-wide singleton, and a Boost test binary runs all
// of its test cases in one process. If a test installs a scripted factory via
// Random::Instance().SetFactory() and forgets to put it back afterward, that
// script leaks into whichever test case happens to run next -- which then
// either gets nonsensical scripted values it never asked for, or an
// exception when the leftover script runs out.
//
// ScopedFixedRandom ties the scripted factory's lifetime to a local
// variable's scope: construct it with the draws a test needs, and it resets
// Random::Instance() back to the default SystemRandomFactory in its
// destructor -- which C++ guarantees runs on every path out of the test
// (normal return, a failed BOOST_REQUIRE, or an exception from the scripted
// factory itself running out of draws), so there's no "don't forget to reset
// this at the end" step to miss.
//
//   std::vector<int> draws;
//   AppendShuffleDraws(draws, originalOrder, desiredOrder);
//   ScopedFixedRandom guard(draws);
//   MapOverlay mo(md); // consumes the scripted draws instead of real randomness
//   ...
//   // Random::Instance() is back to normal once guard goes out of scope
class ScopedFixedRandom
{
public:
  explicit ScopedFixedRandom(const std::vector<int>& i_draws)
  {
    Random::Instance().SetFactory(
      std::unique_ptr<RandomFactory>(new FixedSequenceRandomFactory(i_draws)));
  }

  ~ScopedFixedRandom()
  {
    Random::Instance().ResetToDefault();
  }

private:
  ScopedFixedRandom(const ScopedFixedRandom&);
  ScopedFixedRandom& operator=(const ScopedFixedRandom&);
};

#endif
