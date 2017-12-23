#ifndef MAPOVERLAYHPP
#define MAPOVERLAYHPP

#include "MapData.hpp"
#include "Serialize.hpp"
#include "MapSpace.hpp"
#include "Token.hpp"
#include "Secret.hpp"

// this class is the interface to the map, differing from MapData
// in that it is aware of the game-state specific aspects of the map
// * contents of qbox spaces
// * what relics are on which qbox asteroids
// * whether the relic has been picked up from an asteroid qbox
// * whether an orbit box has become a station
// * who the owner is of a station
// * who knows about the contents of the qboxes.

class MapOverlay
{
public:
  MapOverlay(const MapData& i_mapdata);

  // argument is station name
  // return value is name of space station got placed on.
  std::string PlaceStation(const std::string& i_stationname,int i_owner);
  void PlaceFactory(const std::string& i_solarsystem,int i_owner);

  const std::pair<MapSpace,Secret> GetMapSpace(const std::string& i_name) const;

  std::string GetSpaceString(const std::string& i_name) const;
  const std::map<std::string,std::pair<MapSpace,Secret> >& GetOverrideMap() const;

  // returns NULL if telegate is either not present or not public
  const MapSpace* GetTeleGate(int i_pnum) const;

  void MakeSpacePublic(const std::string& i_name);
  void AddPlayerToSecret(const std::string& i_name,int i_plid);

  const std::vector<Token>& GetRelicList() const;

  bool HasRelic(const std::string& i_space) const;
  Token TakeRelic(const std::string& i_space);
 
  // keyed by space name
  const std::map<std::string,size_t>& GetStationOwners() const;
  // keyed by solar system name
  const std::map<std::string,size_t>& GetFactoryOwners() const;

private:
  const MapData& m_mapdata; // set in constructor call, not serialized.

  // map by space name of MapSpace objects where we are
  // overriding what is in the MapData map.
  // (qboxes and orbits)
  std::map<std::string,std::pair<MapSpace,Secret> > m_overrides;
  // map by space name of Tokens for Asteroid QBoxes (relics)
  std::map<std::string,Token> m_relics;
  // map by space name of the owner id of that station
  std::map<std::string,size_t> m_stationowners;
  // map by solar system name of the owner id of that solar system's factory.
  std::map<std::string,size_t> m_factoryowners;
  // map by orbit name of all spaces on the board that have orbits.
  std::map<std::string,std::string> m_orbitlocations;

  std::vector<Token> m_allrelics;

  SERIALIZE_FUNC
  {
    SERIALIZE(m_overrides);
    SERIALIZE(m_relics);
    SERIALIZE(m_stationowners);
    SERIALIZE(m_factoryowners);
    SERIALIZE(m_allrelics);
    SERIALIZE(m_orbitlocations);
  }

};

#endif
