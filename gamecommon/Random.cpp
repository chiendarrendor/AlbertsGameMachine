#include "Random.hpp"
#include "SystemRandomFactory.hpp"

Random::Random() :
  m_factory(new SystemRandomFactory())
{
}

Random& Random::Instance()
{
  static Random instance;
  return instance;
}

void Random::SetFactory(std::unique_ptr<RandomFactory> i_factory)
{
  m_factory = std::move(i_factory);
}

void Random::ResetToDefault()
{
  m_factory.reset(new SystemRandomFactory());
}

int Random::MyRand(int i_upper)
{
  return m_factory->Next(i_upper);
}

int Random::Between(int i_lower,int i_upper)
{
  return MyRand(i_upper-i_lower+1) + i_lower;
}

int Random::DieRoll()
{
  return MyRand(6) + 1;
}
