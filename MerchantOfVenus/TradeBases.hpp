#include "TradeBase.hpp"
#include "Options.hpp"

class MapData;

class TradeBases
{
public:
  TradeBases(const MapData& i_mapdata,const Options& i_options);

  TradeBase& GetTradeBaseBySolarSystemName(const std::string& i_ssname);
  const std::string& GetSolarSystemOfRace(const std::string& i_raceid) const;

  const std::map<std::string,TradeBase>& GetBaseMap() const;
  const std::map<std::string,std::string>& GetSolarSystemByRaceMap() const;

  bool RegionHasTradeBase(const std::string& i_regionname) const;

private:
  enum Technology
    { PRIMITIVES,INDUSTRY,TECHNOLOGY,BIOENGINEERS,METAPHYSICS,GALACTICBASE };

  TradeBase MakeBase(const std::string& i_id,const std::string& i_name,
                     Technology i_tech,const std::string& i_buyers,
                     int i_iou,int i_factorycost,
                     int i_goodcount,const std::string& i_goodname,
                     int i_goodbuy,int i_goodsell,
                     const std::string& i_fgoodname,
                     int i_fgoodbuy,int i_fgoodsell,
                     const std::string& i_regionname) const;

  const MapData& m_mapdata; // set in constructor call, not serialized.
  bool m_hideunusedweapons; // set in constructor call, not serialized.

  std::map<std::string,TradeBase> m_bases;
  std::map<std::string,std::string> m_solarsystembyrace;

  SERIALIZE_FUNC
  {
    SERIALIZE(m_bases);
    SERIALIZE(m_solarsystembyrace);
  }
};


  
