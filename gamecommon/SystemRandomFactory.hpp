#ifndef SYSTEMRANDOMFACTORYHPP
#define SYSTEMRANDOMFACTORYHPP

#include <random>
#include "RandomFactory.hpp"

// production default: real, OS-entropy-seeded randomness
class SystemRandomFactory : public RandomFactory
{
public:
  SystemRandomFactory();

  int Next(int i_upper);

private:
  std::mt19937 m_engine;
};

#endif
