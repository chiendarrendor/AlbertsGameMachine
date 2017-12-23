#include "Serialize.hpp"
#include "CommodityDeck.hpp"

class CommodityManager
{
public:
  CommodityManager();
  
  void ApplyResearchMegaStatus(bool i_hasmega);

  CommodityDeck &GetDeck(CommodityType i_ctype);
  const CommodityDeck &GetDeck(CommodityType i_ctype) const;

private:
  SERIALIZE_FUNC
  {
    SERIALIZE(m_Commodities);
  }

  std::map<CommodityType,CommodityDeck> m_Commodities;
};
