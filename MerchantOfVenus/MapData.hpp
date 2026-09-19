#ifndef MAPDATAHPP
#define MAPDATAHPP

#include <string>
#include <map>
#include <boost/property_tree/ptree.hpp>

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

  void ProcessLine(const boost::property_tree::ptree& linenode);
  void ProcessSolarSystem(const boost::property_tree::ptree& solnode);
  std::string ProcessTerminal(const std::string& tagname, const boost::property_tree::ptree& termnode);
  MapSpace *ProcessSpace(Region& i_region,const boost::property_tree::ptree& spacenode);
  void ProcessAdjacent(MapSpace& ms,const boost::property_tree::ptree& adjacent);
  void ProcessOrbit(MapSpace& ms,const boost::property_tree::ptree& orbit);


};

#endif

