#include "CardInfo.hpp"
#include <sstream>

CardInfo::CardInfo(const std::string &i_Commodity,int i_Epoch) :
  m_Type(COMMODITYCARD),
  m_Recycles(i_Epoch != 3),
  m_Epoch(i_Epoch)
  
{
  std::ostringstream oss;
  oss << i_Commodity << "_" << i_Epoch;
  m_Name = oss.str();

  m_Commodities.push_back(i_Commodity);
}

CardInfo::CardInfo(const std::string &i_Commodity1,
                   const std::string &i_Commodity2,
                   int i_Epoch) :
  m_Type(COMMODITYCARD),
  m_Recycles(i_Epoch != 3),
  m_Epoch(i_Epoch)
  
{
  std::ostringstream oss;
  oss << i_Commodity1 << i_Commodity2 << "_" << i_Epoch;
  m_Name = oss.str();

  m_Commodities.push_back(i_Commodity1);
  m_Commodities.push_back(i_Commodity2);
}

CardInfo::CardInfo(const std::string &i_Name,
                   int i_Epoch,
                   int i_Discount,
                   const char *i_Advances) :
  m_Type(LEADERCARD),
  m_Name(i_Name),
  m_Recycles(false),
  m_Epoch(i_Epoch),
  m_Discount(i_Discount)
{
  size_t i;
  for (i = 0 ; i < strlen(i_Advances) ; ++i)
  {
    m_Advances.push_back(i_Advances[i]);
  }
}

CardInfo::CardInfo(const std::string &i_Name,
                   int i_Epoch,
                   bool i_Recycles) :
  m_Type(EVENTCARD),
  m_Name(i_Name),
  m_Recycles(i_Recycles),
  m_Epoch(i_Epoch)
{
}
  
const std::string &CardInfo::GetName() const
{
  return m_Name;
}

CardType CardInfo::GetType() const
{
  return m_Type;
}

int CardInfo::GetEpoch() const
{
  return m_Epoch;
}

bool CardInfo::Recycles() const
{
  return m_Recycles;
}

const std::vector<std::string> &CardInfo::GetCommodityVector() const
{
  return m_Commodities;
}

const std::vector<char> &CardInfo::GetAdvanceVector() const
{
  return m_Advances;
}

int CardInfo::GetDiscount() const
{
  return m_Discount;
}
