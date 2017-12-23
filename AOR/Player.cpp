#include "Player.hpp"

Player::Player() :
  m_Name(""),
  m_CurCash(0),
  m_CapitalBid(NO_BID),
  m_Hand(),
  m_Capital(""),
  m_ShipType(GALLEYS),
  m_ShipGrade(0),
  m_MiseryIndex(0),
  m_NumDominations(STARTING_NUM_DOMINATIONS),
  m_NumStockTokens(STARTING_NUM_TOKENS),
  m_NumExpansionTokens(0)
{
}

Player::Player(const std::string &i_Name) : 
  m_Name(i_Name),
  m_CurCash(0),
  m_CapitalBid(NO_BID),
  m_Hand(),
  m_Capital(""),
  m_ShipType(GALLEYS),
  m_ShipGrade(0),
  m_MiseryIndex(0),
  m_NumDominations(STARTING_NUM_DOMINATIONS),
  m_NumStockTokens(STARTING_NUM_TOKENS),
  m_NumExpansionTokens(0)
{
}

Player::Player(const Player &i_Player) 
{
  *this = i_Player;
}

Player &Player::operator=(const Player &i_Player)
{
  m_Name = i_Player.m_Name;
  m_CurCash = i_Player.m_CurCash;
  m_CapitalBid = i_Player.m_CapitalBid;
  m_Hand = i_Player.m_Hand;
  m_Capital = i_Player.m_Capital;
  m_ShipType = i_Player.m_ShipType;
  m_ShipGrade = i_Player.m_ShipGrade;
  m_MiseryIndex = i_Player.m_MiseryIndex;
  m_NumDominations = i_Player.m_NumDominations;
  m_NumStockTokens = i_Player.m_NumStockTokens;
  m_NumExpansionTokens = i_Player.m_NumExpansionTokens;

  return *this;
}



const std::string &Player::GetName() const
{
  return m_Name;
}

const CardPile &Player::GetHand() const
{
  return m_Hand;
}

void Player::SetCash(int i_CurCash)
{
  m_CurCash = i_CurCash;
}

int Player::GetCash() const
{
  return m_CurCash;
}

void Player::SetCapitalBid(int i_CapitalBid)
{
  m_CapitalBid = i_CapitalBid;
}

int Player::GetCapitalBid() const
{
  return m_CapitalBid;
}

size_t Player::GetNumCards() const
{
  return m_Hand.size();
}

void Player::AddCard(std::string i_CardName)
{
  m_Hand.push_back(i_CardName);
}

const std::string &Player::GetCapital() const
{
  return m_Capital;
}

void Player::SetCapital(const std::string &i_Capital)
{
  m_Capital = i_Capital;
}

void Player::SetShipType(ShipType i_ShipType)
{
  m_ShipType = i_ShipType;
}

ShipType Player::GetShipType() const
{
  return m_ShipType;
}

void Player::SetShipGrade(int i_ShipGrade)
{
  m_ShipGrade = i_ShipGrade;
}

int Player::GetShipGrade() const
{
  return m_ShipGrade;
}

void Player::SetMiseryIndex(int i_MiseryIndex)
{
  m_MiseryIndex = i_MiseryIndex;
}

int Player::GetMiseryIndex() const
{
  return m_MiseryIndex;
}

void Player::IncrementDominations(int i_Num)
{
  m_NumDominations += i_Num;
}

void Player::DecrementDominations(int i_Num)
{
  m_NumDominations -= i_Num;
}

int Player::GetNumDominations() const
{
  return m_NumDominations;
}

void Player::IncrementStockTokens(int i_Num)
{
  m_NumStockTokens += i_Num;
}

void Player::DecrementStockTokens(int i_Num)
{
  m_NumStockTokens -= i_Num;
}

int Player::GetNumStockTokens() const
{
  return m_NumStockTokens;
}

void Player::IncrementExpansionTokens(int i_Num)
{
  m_NumExpansionTokens += i_Num;
}

void Player::DecrementExpansionTokens(int i_Num)
{
  m_NumExpansionTokens -= i_Num;
}

int Player::GetNumExpansionTokens() const
{
  return m_NumExpansionTokens;
}

void Player::SetExpansionTokens(int i_Num)
{
  if (i_Num < 0) return;

  m_NumExpansionTokens = (m_NumExpansionTokens > m_NumStockTokens) ? 
    m_NumStockTokens : m_NumExpansionTokens;

  m_NumStockTokens -= m_NumExpansionTokens;
}

void Player::ClearExpansionTokens()
{
  m_NumStockTokens += m_NumExpansionTokens;
  m_NumExpansionTokens = 0;
}

void Player::ExtendTurnLog(int i_TurnToExtend)
{
  m_TurnLog.push_back(TurnLogEntry(i_TurnToExtend,m_CurCash));
}

int Player::GetNumBoardDominations() const
{
  return STARTING_NUM_DOMINATIONS - m_NumDominations;
}

int Player::GetNumBoardTokens() const
{
  return STARTING_NUM_TOKENS - m_NumStockTokens - m_NumExpansionTokens;
}

const std::vector<TurnLogEntry> &Player::GetTurnLog() const
{
  return m_TurnLog;
}

TurnLogEntry &Player::GetLastTurnLogEntry()
{
  if (m_TurnLog.size() == 0)
  {
    throw std::out_of_range("No Turn Logs available!");
  }

  return m_TurnLog[m_TurnLog.size() - 1];
}


