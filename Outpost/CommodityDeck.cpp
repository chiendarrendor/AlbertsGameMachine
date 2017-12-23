
#include "CommodityDeck.hpp"
#include "Utilities.hpp"

CommodityDeck::CommodityDeck() :
  m_ctype(ORE_COMMODITY),
  m_megavalue(0),
  m_averagevalue(0)
{
}

CommodityDeck::CommodityDeck(CommodityType i_type,int i_megavalue,int i_averagevalue) :
  m_ctype(i_type),
  m_megavalue(i_megavalue),
  m_averagevalue(i_averagevalue)
{
}

CommodityDeck::CommodityDeck(const CommodityDeck &i_right) :
  m_deck(i_right.m_deck),
  m_discard(i_right.m_discard),
  m_ctype(i_right.m_ctype),
  m_megavalue(i_right.m_megavalue),
  m_averagevalue(i_right.m_averagevalue)
{
}

CommodityDeck &CommodityDeck::operator=(const CommodityDeck &i_right)
{
  if (this != &i_right)
  {
    m_deck = i_right.m_deck;
    m_discard = i_right.m_discard;
    m_ctype = i_right.m_ctype;
    m_megavalue = i_right.m_megavalue;
    m_averagevalue = i_right.m_averagevalue;
  }
  return *this;
}

void CommodityDeck::AddCards(int i_CardValue,int i_NumCards)
{
  int i;

  for (i = 0 ; i < i_NumCards ; ++i)
  {
    m_deck.push_back(Commodity(m_ctype,i_CardValue,false,false));
  }

  myshuffle(m_deck.begin(),m_deck.end());
}

Commodity CommodityDeck::DrawCommodity(bool i_IsMega)
{
  if (i_IsMega)
  {
    return Commodity(m_ctype,m_megavalue,false,true);
  }

  if (m_deck.empty() && m_discard.empty())
  {
    return Commodity(m_ctype,m_averagevalue,true,false);
  }

  if (m_deck.empty())
  {
    m_deck = m_discard;
    m_discard.clear();
    myshuffle(m_deck.begin(),m_deck.end());
  }

  Commodity c = m_deck.front();
  m_deck.erase(m_deck.begin());
  c.MakeNew();
  return c;
}

void CommodityDeck::DiscardCommodity(Commodity i_Commodity)
{
  if (i_Commodity.IsAverage() || i_Commodity.IsMega()) return;

  m_discard.push_back(i_Commodity);
}

bool CommodityDeck::HasMega() const
{
  return m_megavalue != 0;
}

void CommodityDeck::ZeroMegaValue()
{
  m_megavalue = 0;
}

int CommodityDeck::GetDeckSize() const
{
  return m_deck.size();
}

int CommodityDeck::GetDiscardSize() const
{
  return m_discard.size();
}

std::ostream &operator<<(std::ostream &o,const CommodityDeck &i_deck)
{
  size_t i;

  o << "Type: " << i_deck.m_ctype << std::endl;
  o << "Mega: " << i_deck.m_megavalue << std::endl;
  o << "Avg: " << i_deck.m_averagevalue << std::endl;

  o << "Deck: " << std::endl;

  for (i = 0 ; i < i_deck.m_deck.size() ; ++i)
  {
    const Commodity &c = i_deck.m_deck[i];
    o << "  " << "Type: " << c.GetType();
    o << " " << "Value: " << c.GetValue();
    o << " " << "Average? " << c.IsAverage();
    o << " " << "Mega? " << c.IsMega() << std::endl;
  }

  return o;
}

int CommodityDeck::GetMegaValue() const
{
  return m_megavalue;
}

int CommodityDeck::GetAverageValue() const
{
  return m_averagevalue;
}
