#include "FixedSequenceRandomFactory.hpp"
#include <stdexcept>

FixedSequenceRandomFactory::FixedSequenceRandomFactory(const std::vector<int>& i_sequence) :
  m_sequence(i_sequence),
  m_idx(0)
{
}

int FixedSequenceRandomFactory::Next(int i_upper)
{
  if (m_idx >= m_sequence.size())
    throw std::runtime_error("FixedSequenceRandomFactory: sequence exhausted");

  int result = m_sequence[m_idx++];
  if (result < 0 || result >= i_upper)
    throw std::runtime_error("FixedSequenceRandomFactory: scripted value out of range");

  return result;
}
