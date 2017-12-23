#include "Serialize.hpp"
#include "Commodity.hpp"

class CommodityDeck
{
public:
  CommodityDeck();
  CommodityDeck(CommodityType i_type,int i_megavalue,int i_averagevalue);

  CommodityDeck(const CommodityDeck &i_right);
  CommodityDeck &operator=(const CommodityDeck &i_right);

  int GetDeckSize() const;
  int GetDiscardSize() const;
  

  void ZeroMegaValue();

  void AddCards(int i_CardValue,int i_NumCards);

  Commodity DrawCommodity(bool i_IsMega);
  void DiscardCommodity(Commodity i_Commodity);

  bool HasMega() const;
  int GetMegaValue() const;
  int GetAverageValue() const;

private:
  friend std::ostream &operator<<(std::ostream &o,const CommodityDeck &i_deck);
  SERIALIZE_FUNC
  {
    SERIALIZE(m_deck);
    SERIALIZE(m_discard);
    SERIALIZE(m_ctype);
    SERIALIZE(m_megavalue);
    SERIALIZE(m_averagevalue);
  }

  std::vector<Commodity> m_deck;
  std::vector<Commodity> m_discard;

  CommodityType m_ctype;
  int m_megavalue;
  int m_averagevalue;
};

std::ostream &operator<<(std::ostream &o,const CommodityDeck &i_deck);
