#ifndef MAPDATAHPP
#define MAPDATAHPP

#include <string>
#include <map>
#include "pugixml.hpp"

class Region;
class MapSpace;
class NextInfo;

class MapData
{
public:
  MapData(const std::string &i_fname);

  const std::map<std::string,Region *>& GetRegionMap() const;
  const std::map<std::string,MapSpace *>& GetSpaceMap() const;

private:
  std::map<std::string,Region *>m_regions;
  std::map<std::string,MapSpace *>m_spaces;

  void ProcessLine(pugi::xml_node linenode);
  void ProcessSolarSystem(pugi::xml_node solnode);
  std::string ProcessTerminal(pugi::xml_node termnode);
  MapSpace *ProcessSpace(Region& i_region,pugi::xml_node spacenode);
  void ProcessAdjacent(MapSpace& ms,pugi::xml_node adjacent);
  void ProcessOrbit(MapSpace& ms,pugi::xml_node orbit);


};

#endif

