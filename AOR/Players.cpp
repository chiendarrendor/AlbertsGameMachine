#include "Players.hpp"
#include <stdexcept>
#include <algorithm>
#include "Utilities.hpp"

size_t Players::size() const
{
  return m_Players.size();
}

const Player &Players::operator[](size_t i_Num) const
{
  if (i_Num < 0 || i_Num >= size())
  {
    throw std::out_of_range("Players list");
  }
  return m_Players[i_Num];
}

Player &Players::operator[](size_t i_Num)
{
  if (i_Num < 0 || i_Num >= size())
  {
    throw std::out_of_range("Players list");
  }
  return m_Players[i_Num];
}

size_t Players::GetPlayerId(const std::string &i_Name) const
{
  size_t i;

  for (i = 0 ; i < size() ; ++i)
  {
    if ((*this)[i].GetName() == i_Name)
    {
      return i;
    }
  }
  
  return NO_PLAYER;
}
  



void Players::add(const std::string &i_Name)
{
  m_Players.push_back(i_Name);
}

bool Players::IsPlayer(const std::string &i_Name) const
{
  std::vector<Player>::const_iterator findit;

  for (findit = m_Players.begin() ; findit != m_Players.end() ; ++findit)
  {
    const Player &rPlayer = *findit;
    if (rPlayer.GetName() == i_Name)
    {
      return true;
    }
  }
  return false;
}

bool Players::AllBid() const
{
  size_t i;
  for (i = 0 ; i < m_Players.size() ; ++i)
  {
    if (m_Players[i].GetCapitalBid() == NO_BID)
    {
      return false;
    }
  }
  return true;
}

bool largerBid(const Player &pl1,const Player &pl2)
{
  return pl1.GetCapitalBid() > pl2.GetCapitalBid();
}



void Players::ExecuteCapitalBid()
{
  // quick and dirty way to make sure that 
  // ties are handled...if we stable sort
  // a random ordering, then ties will 
  // maintain that new random ordering :-)
  myshuffle(m_Players.begin(),m_Players.end());

  // now, order with largest bid first.
  std::stable_sort(m_Players.begin(),m_Players.end(),largerBid);
}

void Players::ExtendTurnLog(int i_TurnToExtend)
{
  size_t i;

  for (i = 0 ; i < m_Players.size() ; ++i)
  {
    m_Players[i].ExtendTurnLog(i_TurnToExtend);
  }
}
