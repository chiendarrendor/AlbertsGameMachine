#include "TradeBase.hpp"

class MapData;

class TradeBases
{
public:
  TradeBases(const MapData& i_mapdata);

  TradeBase& GetTradeBaseBySolarSystemName(const std::string& i_ssname);
  const std::string& GetSolarSystemOfRace(const std::string& i_raceid) const;

  const std::map<std::string,TradeBase>& GetBaseMap() const;
  const std::map<std::string,std::string>& GetSolarSystemByRaceMap() const;

  bool RegionHasTradeBase(const std::string& i_regionname) const;

private:
  std::map<std::string,TradeBase> m_bases;
  std::map<std::string,std::string> m_solarsystembyrace;

  SERIALIZE_FUNC
  {
    SERIALIZE(m_bases);
    SERIALIZE(m_solarsystembyrace);
  }
};


  
