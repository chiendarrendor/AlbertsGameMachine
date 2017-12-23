#include "CommodityHand.hpp"
#include <stdexcept>
#include <boost/lexical_cast.hpp>

CommodityHand::CommodityHand() : m_Hand() 
{
}

CommodityHand::CommodityHand(const CommodityHand &i_right) : 
  m_Hand(i_right.m_Hand)
{
}

CommodityHand &CommodityHand::operator=(const CommodityHand &i_right)
{
  if (&i_right != this)
  {
    m_Hand = i_right.m_Hand;
  }
  return *this;
}

int CommodityHand::GetHandWeight(bool i_SmallResearch,bool i_SmallMicro) const
{
  int result = 0;
  size_t i;

  for (i = 0 ; i < m_Hand.size() ; ++i)
  {
    if (m_Hand[i].GetType() == RESEARCH_COMMODITY && i_SmallResearch) continue;
    if (m_Hand[i].GetType() == MICROBIOTICS_COMMODITY && i_SmallMicro) continue;

    result += (m_Hand[i].IsMega() ? 4 : 1);
  }
  return result;
}

int CommodityHand::GetHandValue() const
{
  int result = 0;
  size_t i;

  for (i = 0 ; i < m_Hand.size() ; ++i)
  {
    result += m_Hand[i].GetValue();
  }
  return result;
}

int CommodityHand::GetNumResearch() const
{
  int result = 0;
  size_t i;

  for (i = 0 ; i < m_Hand.size() ; ++i)
  {
    if (m_Hand[i].GetType() == RESEARCH_COMMODITY)
    {
      if (m_Hand[i].IsMega()) result += 4;
      else                    result += 1;
    }
  }
  return result;
}



struct CommSorter
{
  bool operator()(const Commodity &i_left,const Commodity &i_right)
  {
    if (i_left.GetType() != i_right.GetType())
    {
      return (int)i_left.GetType() > (int)i_right.GetType();
    }
    if (i_left.IsMega() != i_right.IsMega())
    {
      return i_left.IsMega();
    }
    if (i_left.IsAverage() != i_right.IsAverage())
    {
      return i_right.IsAverage();
    }
    return i_left.GetValue() > i_right.GetValue();
  }
};





void CommodityHand::AddCommodity(Commodity i_com)
{
  m_Hand.push_back(i_com);
  std::sort(m_Hand.begin(),m_Hand.end(),CommSorter());
}


// takes a string of 0 and 1, and deletes all records from
// m_Hand where the corresponding index at i_deletes is 1.
// -1 size mismatch
// -2 illegal string
//  0 ok.

int CommodityHand::ApplyDeletes(const std::string &i_deletes,std::vector<Commodity> &o_deletes)
{
  if (m_Hand.size() != i_deletes.size())
  {
    return -1;
  }
  size_t i;
  for (i = 0 ; i < i_deletes.size() ; ++i)
  {
    if (i_deletes[i] != '0' && i_deletes[i] != '1') return -2;
  }

  std::vector<Commodity> newhand;
  for (i = 0 ; i < i_deletes.size() ; ++i)
  {
    if (i_deletes[i] == '1')
    {
      o_deletes.push_back(m_Hand[i]);
    }
    else
    {
      newhand.push_back(m_Hand[i]);
    }
  }
  m_Hand = newhand;
  return 0;
}

std::string CommodityHand::GetHandDescription(bool i_showPrivate,bool i_showNew) const
{
  std::string result;
  size_t i;

  result += "|";

  for (i = 0 ; i < m_Hand.size() ; ++i)
  {
    if (i != 0) result += "-";

    switch(m_Hand[i].GetType())
    {
    case ORE_COMMODITY:              result += "OR"; break;
    case WATER_COMMODITY:            result += "WA"; break;
    case TITANIUM_COMMODITY:         result += "TI"; break;
    case RESEARCH_COMMODITY:         result += "RE"; break;
    case MICROBIOTICS_COMMODITY:     result += "MI"; break;
    case NEW_CHEMICALS_COMMODITY:    result += "NE"; break;
    case ORBITAL_MEDICINE_COMMODITY: result += "OM"; break;
    case RING_ORE_COMMODITY:         result += "RO"; break;
    case MOON_ORE_COMMODITY:         result += "MO"; break;
    default: throw std::out_of_range("Bad Commodity Type");
    }

    // megas and average always get shown.
    if (m_Hand[i].IsMega() || 
        m_Hand[i].IsAverage())
    {
      result += boost::lexical_cast<std::string>(m_Hand[i].GetValue());
    }
    // if it's not mega or average..
    // if we're not showing private, it gets a star
    else if (!i_showPrivate)
    {
      result += "*";
    }
    // if we get here, its neither mega nor average, and we're showing private
    // if show new is true, we show new (and we always show old)
    else if (i_showNew)
    {
      result += boost::lexical_cast<std::string>(m_Hand[i].GetValue());
    }
    // if we get here, its neither mega nor average, we're showing private, and we don't show new.
    else if (m_Hand[i].IsNew())
    {
      result += "*";
    }
    else
    {
      result += boost::lexical_cast<std::string>(m_Hand[i].GetValue());
    }      
    
    if (m_Hand[i].IsMega()) result += "M";
    if (m_Hand[i].IsAverage()) result += "A";
  }
  result += "|";
  return result;
}

const std::vector<Commodity>& CommodityHand::GetHand() const
{
  return m_Hand;
}

void CommodityHand::AgeHand()
{
  size_t i;
  for (i = 0 ; i < m_Hand.size() ; ++i)
  {
    m_Hand[i].MakeOld();
  }
}
