#ifndef RANDOMHPP
#define RANDOMHPP

#include <algorithm>
#include <iterator>
#include <memory>
#include "RandomFactory.hpp"

// singleton seam for all game randomness. Production code gets a real,
// OS-entropy-seeded SystemRandomFactory by default; tests can call
// SetFactory() to swap in a FixedSequenceRandomFactory so outcomes don't
// depend on what a particular seed happens to produce on this platform.
class Random
{
public:
  static Random& Instance();

  void SetFactory(std::unique_ptr<RandomFactory> i_factory);
  void ResetToDefault();

  int MyRand(int i_upper);
  int Between(int i_lower,int i_upper);
  int DieRoll();

  template<class _It> void Shuffle(_It i_Begin,_It i_End)
  {
    while (i_Begin != i_End)
    {
      _It other = i_Begin;
      std::advance(other,MyRand(std::distance(i_Begin,i_End)));
      std::iter_swap(other,i_Begin);
      ++i_Begin;
    }
  }

private:
  Random();

  std::unique_ptr<RandomFactory> m_factory;
};

#endif
