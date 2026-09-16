#ifndef FIXEDSEQUENCERANDOMFACTORYHPP
#define FIXEDSEQUENCERANDOMFACTORYHPP

#include <cstddef>
#include <vector>
#include "RandomFactory.hpp"

// test double: plays back a scripted, known sequence of outcomes instead of
// relying on a particular seed producing a particular platform-dependent
// sequence. Throws std::runtime_error if the script runs out, or if a
// scripted value doesn't fit the requested [0,upper) range -- either means
// the test's script doesn't match what the code under test actually does.
class FixedSequenceRandomFactory : public RandomFactory
{
public:
  explicit FixedSequenceRandomFactory(const std::vector<int>& i_sequence);

  int Next(int i_upper);

private:
  std::vector<int> m_sequence;
  size_t m_idx;
};

#endif
