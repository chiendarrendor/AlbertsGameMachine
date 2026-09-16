#include "SystemRandomFactory.hpp"

SystemRandomFactory::SystemRandomFactory() :
  m_engine(std::random_device()())
{
}

int SystemRandomFactory::Next(int i_upper)
{
  std::uniform_int_distribution<int> dist(0,i_upper-1);
  return dist(m_engine);
}
