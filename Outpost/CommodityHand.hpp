#ifndef COMMODITYHANDHPP
#define COMMODITYHANDHPP

#include "Commodity.hpp"

class CommodityHand
{
public:
  CommodityHand();
  CommodityHand(const CommodityHand &i_right);
  CommodityHand &operator=(const CommodityHand &i_right);

  void AddCommodity(Commodity i_com);

  int GetHandWeight(bool i_SmallResearch,bool i_SmallMicro) const;
  int GetHandValue() const;
  int GetNumResearch() const;
  
  // takes a string of 0 and 1, and deletes all records from
  // m_Hand where the corresponding index at i_deletes is 1.
  // returns deleted Commodities in o_deletes
  // -1 size mismatch
  // -2 illegal string
  //  0 ok.
  int ApplyDeletes(const std::string &i_deletes,std::vector<Commodity> &o_deletes);

  void AgeHand();

  std::string GetHandDescription(bool i_showPrivate,bool i_showNew) const;
  const std::vector<Commodity> &GetHand() const;

private:
  std::vector<Commodity> m_Hand;

  SERIALIZE_FUNC
  {
    SERIALIZE(m_Hand);
  }
};

#endif

