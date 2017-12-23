#include "Players.hpp"
#include <stdexcept>
#include <algorithm>
#include "Utilities.hpp"
#include <iostream>
#include <sstream>

Players::Players() : m_Players()
{
}

Players::~Players()
{
}

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

Player &Players::GetPlayerByName(const std::string &i_Name)
{
  return (*this)[GetPlayerId(i_Name)];
}

const Player &Players::GetPlayerByName(const std::string &i_Name) const
{
  return (*this)[GetPlayerId(i_Name)];
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
  m_Players.push_back(Player(i_Name,m_Players.size()));
}

void Players::remove(const std::string &i_Name)
{
  m_Players.erase(m_Players.begin() + GetPlayerId(i_Name));
  size_t i;
  for (i = 0 ; i < m_Players.size() ; ++i)
  {
    m_Players[i].SetId(i);
  }
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


bool Players::AllOk() const
{
  for (size_t i = 0; i < m_Players.size() ; ++i)
  {
    if (!m_Players[i].OkStartup()) return false;
  }
  return true;
}

void Players::RandomizeTurnOrder()
{
  m_TurnOrder.clear();
  for (size_t i = 0 ; i < m_Players.size(); ++i)
  {
    m_TurnOrder.push_back(i);
  }
  myshuffle(m_TurnOrder.begin(),m_TurnOrder.end());
  m_CurPlayerIdx = 0;
}

std::string Players::GetTurnOrder() const
{
  std::ostringstream oss;
  for (size_t i = 0 ; i < m_TurnOrder.size(); ++i)
  {
    oss << m_TurnOrder[i];
  }
  return oss.str();
}

size_t Players::GetCurPlayerId() const
{
  return m_TurnOrder[m_CurPlayerIdx];
}

void Players::NextPlayer()
{
  m_CurPlayerIdx = (m_CurPlayerIdx+1)%m_Players.size();
}

  
Player& Players::GetCurPlayer()
{
  return (*this)[GetCurPlayerId()];
}

const Player& Players::GetCurPlayer() const
{
  return (*this)[GetCurPlayerId()];
}
