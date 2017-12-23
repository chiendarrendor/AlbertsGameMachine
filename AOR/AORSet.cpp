#include "AORSet.hpp"

AORSet::AORSet(const std::string &i_DataDir) :
  m_DeckInfo(),
  m_CommodityInfo(),
  m_GameBoard(i_DataDir + std::string("/AorMap.txt"),m_CommodityInfo)
{
};

bool AORSet::IsValid() const 
{ 
  return m_GameBoard.LoadOk(); 
}

const DeckInfo &AORSet::GetDeckInfo() const 
{ 
  return m_DeckInfo; 
}

const CommodityInfo &AORSet::GetCommodityInfo() const 
{ 
  return m_CommodityInfo; 
}

const GameBoard &AORSet::GetGameBoard() const
{
  return m_GameBoard;
}
