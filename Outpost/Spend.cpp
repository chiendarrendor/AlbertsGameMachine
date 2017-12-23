#include "Spend.hpp"

Spend::Spend() :
  m_TurnNum(-1),
  m_PlayerNum(3000),
  m_SpendType(ITEMPURCHASE),
  m_SpendItem(0),
  m_SpendCost(0),
  m_Spends()
{
}

Spend::Spend(const Spend &i_right) :
  m_TurnNum(i_right.m_TurnNum),
  m_PlayerNum(i_right.m_PlayerNum),
  m_SpendType(i_right.m_SpendType),
  m_SpendItem(i_right.m_SpendItem),
  m_SpendCost(i_right.m_SpendCost),
  m_Spends(i_right.m_Spends)
{
  
}

Spend& Spend::operator=(const Spend& i_right)
{
  if (&i_right != this)
  {
    m_TurnNum = i_right.m_TurnNum;
    m_PlayerNum = i_right.m_PlayerNum;
    m_SpendType = i_right.m_SpendType;
    m_SpendItem = i_right.m_SpendItem;
    m_SpendCost = i_right.m_SpendCost;
    m_Spends = i_right.m_Spends;
  }
  return *this;
}




Spend::Spend(int i_TurnNum,size_t i_PlayerNum,SpendType i_SpendType,int i_SpendItem) :
  m_TurnNum(i_TurnNum),
  m_PlayerNum(i_PlayerNum),
  m_SpendType(i_SpendType),
  m_SpendItem(i_SpendItem),
  m_SpendCost(0),
  m_Spends()
{
}

void Spend::AddCommodity(Commodity i_com)
{
  m_Spends.AddCommodity(i_com);
}

void Spend::SetSpendCost(int i_Cost)
{
  m_SpendCost = i_Cost;
}

const int Spend::GetTurnNum() const { return m_TurnNum; }
const size_t Spend::GetPlayerNum() const { return m_PlayerNum; }
const SpendType Spend::GetSpendType() const { return m_SpendType; }
const int Spend::GetSpendItem() const { return m_SpendItem; }
const int Spend::GetSpendCost() const { return m_SpendCost; }
const CommodityHand& Spend::GetCommodityHand() const { return m_Spends; }
