#include "AORState.hpp"
#include "AORSet.hpp"
#include <sstream>
#include <iostream>

AORState::AORState(const AORSet &i_rAORSet) :
  m_Players(),
  m_CardManager(i_rAORSet.GetDeckInfo()),
  m_BoardState(i_rAORSet.GetGameBoard()),
  m_AORSet(i_rAORSet),
  m_CurTurn(0)
{
}

Players &AORState::GetPlayers()
{
  return m_Players;
}

const Players &AORState::GetPlayers() const
{
  return m_Players;
}

CardManager &AORState::GetCardManager()
{
  return m_CardManager;
}

const CardManager &AORState::GetCardManager() const
{
  return m_CardManager;
}

std::string AORState::GetLegalBidString(int i_Min,int i_Max) const
{
  std::ostringstream oss;
  bool first = true;

  int i;

  for (i = i_Min ; i <= i_Max ; ++i)
  {
    if (!first)
    {
      oss << ",";
    }
    first = false;

    oss << i;
  }

  return oss.str();
}


void AORState::PrepareCapitalChoices()
{
  size_t nPlayers = GetPlayers().size();

  m_CapitalChoices.insert("Venice");
  m_CapitalChoices.insert("Genoa");
  m_CapitalChoices.insert("Barcelona");

  if (nPlayers > 3) m_CapitalChoices.insert("Paris");
  if (nPlayers > 4) m_CapitalChoices.insert("London");
  if (nPlayers > 5) m_CapitalChoices.insert("Hamburg");
}

// returns the index of the next person to choose their
// capital (# of players - # of unchosen capitals)
size_t AORState::GetCapitalChooser() const
{
  return GetPlayers().size() - m_CapitalChoices.size();
}

std::string AORState::GetCapitalChoiceList() const
{
  std::ostringstream oss;

  std::set<std::string>::const_iterator capit;

  for (capit = m_CapitalChoices.begin() ; capit != m_CapitalChoices.end() ; ++capit)
  {
    if (capit != m_CapitalChoices.begin())
    {
      oss << ",";
    }
    oss << *capit;
  }
  return oss.str();
}

void AORState::ClearCapitalChoice(const std::string &i_CapitalChoice)
{
  m_CapitalChoices.erase(i_CapitalChoice);
}

const std::set<std::string> &AORState::GetCapitalChoiceSet() const
{
  return m_CapitalChoices;
}

BoardState &AORState::GetBoardState()
{
  return m_BoardState;
}

const BoardState &AORState::GetBoardState() const
{
  return m_BoardState;
}

const AORSet &AORState::GetAORSet() const
{
  return m_AORSet;
}

int AORState::GetCurTurn() const
{
  return m_CurTurn;
}

void AORState::SetCurTurn(int i_CurTurn)
{
  m_CurTurn = i_CurTurn;
}

