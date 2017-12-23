#include <vector>
#include <string>
#include <set>
#include "Serialize.hpp"


enum SpaceType { DOT,ASTEROID,QBOX,PENALTY,CITY,STATION,TELEGATE};
enum Color { BLUE,RED,YELLOW};

class Region
{
public:
  Region(const std::string& i_name,bool i_habitable,bool i_inhabited);
  std::string m_name;
  bool m_habitable;
  bool m_inhabited;
};

class MapSpace
{
public:
  MapSpace();
  MapSpace(const std::string& i_regionname,const std::string& i_name,
           SpaceType i_type,Color i_color = BLUE);
  MapSpace(const MapSpace& i_right);
  MapSpace& operator=(const MapSpace& i_right);

  bool IsNavCircle() const;

  // a) an adjacency can be flyable only if type = CITY
  // no two adjacencies of a NAVCIRCLE can share a pilot number
  void AddAdjacency(const std::string& i_name,bool isFlyable,const std::set<int>& i_pnumbers);
  // short form...for non-pnumber,non-flyable adjacencies.
  void AddAdjacency(const std::string& i_name);
   
public:
  std::string m_regionname;
  std::string m_name;
  SpaceType m_type;
  std::vector<std::pair<std::string,bool> > m_adjacentnames; // the boolean is for flyable.
  int m_pilotdirs[7]; // only for NAVCIRCLE
  std::string m_orbit; // non-empty string is orbit (only for DOT)
  Color m_color; // only for DOT/PENALTY
  int m_cost; // only for PENALTY
  int m_gateid; // only for TELEGATE

  SERIALIZE_FUNC
  {
    SERIALIZE(m_regionname);
    SERIALIZE(m_name);
    SERIALIZE(m_type);
    SERIALIZE(m_adjacentnames);
    SERIALIZE(m_pilotdirs);
    SERIALIZE(m_orbit);
    SERIALIZE(m_color);
    SERIALIZE(m_cost);
    SERIALIZE(m_gateid);
  }
};
  
