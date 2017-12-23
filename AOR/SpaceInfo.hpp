#ifndef SPACEINFOHPP
#define SPACEINFOHPP

#include "SpaceInfoMap.hpp"
#include <vector>
#include "CommodityInfo.hpp"

typedef int SpaceType;
const SpaceType LAND = 0;
const SpaceType COAST = 1;
const SpaceType SEA = 2;
const SpaceType OCEAN = 3;

class SpaceInfo
{
public:
  SpaceInfo(const std::string &i_Name,
            SpaceType i_Type,
            int i_Area,
            int i_MarketSize);

  const std::string &GetName() const;
  int GetSpaceType() const;
  int GetArea() const;
  int GetMarketSize() const;

  bool IsOverseas() const;
  bool HasLinkTo(const SpaceInfo &i_rLink) const;
  
  const SpaceInfoMap &GetSatellites() const;
  const std::vector<const Commodity *> &GetCommodities() const;
  const SpaceInfoMap &GetLinks() const;

  void AddLink(SpaceInfo &i_rLink);
  void AddSatellite(SpaceInfo &i_rLink);
  void AddCommodity(const Commodity &i_rCommodity);
  void SetOverseas();

private:
  std::string m_Name;
  SpaceType m_Type;
  int m_MarketSize;
  int m_Area;

  bool m_IsOverseas;

  SpaceInfoMap m_Links;
  SpaceInfoMap m_Satellites;

  std::vector<const Commodity *>m_Commodities;
};

#endif

