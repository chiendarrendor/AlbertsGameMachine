#include "Players.hpp"
#include <stdexcept>
#include <algorithm>
#include "Utilities.hpp"
#include <iostream>

Players::Players() : m_Players(),m_TurnOrder(),m_CurTurnIndex(0),m_CurBidIndex(0)
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
  m_TurnOrder.push_back(0);
}

void Players::remove(const std::string &i_Name)
{
  m_Players.erase(m_Players.begin() + GetPlayerId(i_Name));
  m_TurnOrder.erase(m_TurnOrder.begin());
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
  size_t i;

  for (i = 0 ; i < m_Players.size() ; ++i)
  {
    if (!m_Players[i].OkStartup()) return false;
  }

  return true;
}

const std::vector<int> &Players::GetTurnOrder() const
{
  return m_TurnOrder;
}

struct TurnOrderSort
{
  TurnOrderSort(const Players &i_Players) : m_Players(i_Players) {}
  bool operator()(const int &i_left,const int &i_right)
  {
    if (m_Players[i_left].GetVP() != m_Players[i_right].GetVP()) 
      return m_Players[i_left].GetVP() > m_Players[i_right].GetVP();

    return 
      m_Players[i_left].GetOwnedItems().GetCostSum() >
      m_Players[i_right].GetOwnedItems().GetCostSum();
  }
  const Players &m_Players;
};

void Players::DetermineTurnOrder()
{
  size_t i;
  for (i = 0 ; i < size() ; ++i)
  {
    m_TurnOrder[i] = i;
  }
  myshuffle(m_TurnOrder.begin(),m_TurnOrder.end());
  std::stable_sort(m_TurnOrder.begin(),m_TurnOrder.end(),TurnOrderSort(*this));
}

int Players::GetPhase() const
{
  size_t i;
  int maxscore = 0;

  for (i = 0 ; i < m_Players.size() ; ++i)
  {
    int pvp = m_Players[i].GetVP();
    if (pvp > maxscore) maxscore = pvp;
  }

  if (maxscore < 10) return 1;
  if (maxscore < 30) return 2;
  return 3;
}

void Players::StartManningTurnOrder()
{
  size_t i;
  for (i = 0 ; i < m_Players.size() ; ++i)
  {
    PurchaseState ps = m_Players[i].GetPurchaseState();
    if (ps == FINAL_DONE || ps == INTERNAL_FINAL_DONE) continue;
    m_Players[i].SetPurchaseState(UNPLAYED);
  }

  m_CurTurnIndex = m_Players.size();
  for (i = 0 ; i < m_Players.size() ; ++i)
  {
    PurchaseState ps = m_Players[m_TurnOrder[i]].GetPurchaseState();
    if (ps == UNPLAYED)
    {
      m_Players[m_TurnOrder[i]].SetPurchaseState(ACTIVE);
      m_CurTurnIndex = i;
      break;
    }
  }
}

void Players::IncrementManningTurnOrder()
{
  if (AllPlayersDone()) return;
  GetCurTurnPlayer().SetPurchaseState(FINAL_DONE);

  while(!AllPlayersDone() && GetCurTurnPlayer().GetPurchaseState() != UNPLAYED)
  {
    m_CurTurnIndex++;
  }

  if (AllPlayersDone()) return;
  GetCurTurnPlayer().SetPurchaseState(ACTIVE);
}




  
void Players::StartTurnOrder()
{
  m_CurTurnIndex = 0;
  size_t i;
  for (i = 0 ; i < m_Players.size() ; ++i)
  {
    m_Players[i].SetPurchaseState(UNPLAYED);
    if (m_Players[i].GetId() == m_TurnOrder[0])
    {
      m_Players[i].SetPurchaseState(ACTIVE);
    }
  }
}

void Players::IncrementTurnOrder()
{
  if (AllPlayersDone()) return;
  GetCurTurnPlayer().SetPurchaseState(DONE);
  m_CurTurnIndex++;
  if (AllPlayersDone()) return;
  GetCurTurnPlayer().SetPurchaseState(ACTIVE);
}

bool Players::AllPlayersDone() const
{
  return m_CurTurnIndex >= m_Players.size();
}

const Player &Players::GetCurTurnPlayer() const
{
  if (AllPlayersDone()) throw std::out_of_range("can't get Player when all players done!");

  return m_Players[m_TurnOrder[m_CurTurnIndex]];
}

Player &Players::GetCurTurnPlayer()
{
  if (AllPlayersDone()) throw std::out_of_range("can't get Player when all players done!");

  return m_Players[m_TurnOrder[m_CurTurnIndex]];
}

void Players::StartPurchasePhase()
{
  StartTurnOrder();

  size_t i;
  for (i = 0 ; i < m_Players.size() ; ++i)
  {
    m_Players[i].SetHasPurchased(false);
  }
}

void Players::PurchaseIncrementTurnOrder(PurchaseState i_state)
{
  if (AllPlayersDone()) return;
  
  GetCurTurnPlayer().SetPurchaseState(i_state);
  m_CurTurnIndex++;

  while(!AllPlayersDone() && GetCurTurnPlayer().GetPurchaseState() != UNPLAYED)
  {
    m_CurTurnIndex++;
  }

  if (!AllPlayersDone())
  {
    GetCurTurnPlayer().SetPurchaseState(ACTIVE);
  }
}

bool Players::PurchaseAllPlayersDone() const
{
  size_t i;
  for (i = 0 ; i < m_Players.size() ; ++i)
  {
    PurchaseState ps = m_Players[i].GetPurchaseState();

    if (ps != DONE && ps != INTERNAL_DONE  &&
        ps != FINAL_DONE && ps != INTERNAL_FINAL_DONE) return false;
  }

  return true;
}

bool Players::IsPlayerInternalizing(const std::string &i_PlayerName) const
{
  return IsPlayer(i_PlayerName) && GetPlayerByName(i_PlayerName).GetPurchaseState() == INTERNALIZING;
}

bool Players::CanPlayerPurchase(const std::string &i_PlayerName,
                                PurchasableType i_Purchasable,
                                RobotMechanism i_mechanism) const
{
  bool isLegalPlayer = false;

  if (IsPlayerInternalizing(i_PlayerName)) isLegalPlayer = true;
  else if (!AllPlayersDone() && i_PlayerName == GetCurTurnPlayer().GetName()) isLegalPlayer = true;

  if (!isLegalPlayer) return false;

  if (GetPlayerByName(i_PlayerName).GetHighBid() != NOT_HIGH_BID) return false;

  return GetPlayerByName(i_PlayerName).GetMaxPurchasable(i_Purchasable,i_mechanism) > 0;
}

bool Players::CanPlayerMulligan(const std::string &i_PlayerName) const
{
  bool isLegalPlayer = false;

  if (IsPlayerInternalizing(i_PlayerName)) isLegalPlayer = true;
  else if (!AllPlayersDone() && i_PlayerName == GetCurTurnPlayer().GetName()) isLegalPlayer = true;

  if (!isLegalPlayer) return false;

  if (GetPlayerByName(i_PlayerName).GetHighBid() != NOT_HIGH_BID) return false;

  return GetPlayerByName(i_PlayerName).GetCommodityHand().GetHandValue() < 20 &&
    GetPlayerByName(i_PlayerName).HasPurchasedAnything() == false;
}

void Players::StartBidPhase(bool i_MustBeActive,bool i_OnePurchase,int i_InitialBid)
{
  size_t i;
  for (i = 0 ; i < m_Players.size() ; ++i)
  {
    Player &pl = m_Players[i];

    pl.SetHighBid(NOT_HIGH_BID);

    switch(pl.GetPurchaseState())
    {
    case UNPLAYED: 
      pl.SetBidState(UNBID);
      break;
    case INTERNALIZING:
    case INTERNAL_DONE:
    case INTERNAL_FINAL_DONE:
      pl.SetBidState(PASS_OUT);
      break;
    case ACTIVE:
      pl.SetBidState(HASBID);
      pl.SetHighBid(i_InitialBid);
      m_HighBidIndex = i;
      break;
    case DONE:
    case FINAL_DONE:
      if (i_MustBeActive) pl.SetBidState(PASS_OUT);
      else if (i_OnePurchase && pl.HasPurchased()) pl.SetBidState(PASS_OUT);
      else pl.SetBidState(UNBID);
      break;
    }
  }

  m_CurBidIndex = m_CurTurnIndex;
  IncrementBidIndex();
}

void Players::IncrementBidIndex()
{
  if (size() == 1) return;

  size_t count = 1;
  m_CurBidIndex = (m_CurBidIndex + 1) % size();

  while (count <= size() && GetCurBidPlayer().GetBidState() == PASS_OUT)
  {
    count++;
    m_CurBidIndex = (m_CurBidIndex + 1) % size();
  }
  if (count > size())
  {
    throw std::out_of_range("No non-PASS_OUT Players?");
  }
  GetCurBidPlayer().SetBidState(CURBID);
}

const Player &Players::GetCurBidPlayer() const
{
  return (*this)[GetTurnOrder()[m_CurBidIndex]];
}

Player &Players::GetCurBidPlayer()
{
  return (*this)[GetTurnOrder()[m_CurBidIndex]];
}

void Players::IncrementBid(BidState i_BidState,int i_HighBid)
{
  if (i_HighBid != NOT_HIGH_BID)
  {
    m_Players[m_HighBidIndex].SetHighBid(NOT_HIGH_BID);
    m_HighBidIndex = GetCurBidPlayer().GetId();
  }
  GetCurBidPlayer().SetBidState(i_BidState);
  GetCurBidPlayer().SetHighBid(i_HighBid);
  IncrementBidIndex();
}

int Players::GetCurrentHighBid() const
{
  return m_Players[m_HighBidIndex].GetHighBid();
}

const Player &Players::GetHighBidPlayer() const
{
  return m_Players[m_HighBidIndex];
}

Player &Players::GetHighBidPlayer()
{
  return m_Players[m_HighBidIndex];
}



std::set<size_t> Players::GetWinningPlayerIndices() const
{
  size_t i;
  std::set<size_t> result;
  
  int maxvp = 0;

  for (i = 0 ; i < m_Players.size() ; ++i)
  {
    if (maxvp < m_Players[i].GetVP()) maxvp = m_Players[i].GetVP();
  }
  
  if (maxvp < WINVP) return result;

  for (i = 0 ; i < m_Players.size() ; ++i)
  {
    if (maxvp == m_Players[i].GetVP()) result.insert(i);
  }

  return result;
}


      
      
    
  
