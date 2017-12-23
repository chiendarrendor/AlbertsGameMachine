#include "MapSpace.hpp"
#include "MerchantOfVenus.hpp"

Region::Region(const std::string& i_name,bool i_habitable,bool i_inhabited) :
  m_name(i_name),
  m_habitable(i_habitable),
  m_inhabited(i_inhabited)
{
}

MapSpace::MapSpace(const MapSpace& i_right)
{
  *this = i_right;
}

MapSpace::MapSpace() :
  m_regionname(""),
  m_name(""),
  m_type(DOT),
  m_adjacentnames(),
  m_orbit(""),
  m_color(BLUE),
  m_cost(0),
  m_gateid(-1)
{
  for (size_t i = 1 ; i <= 6 ; ++i) m_pilotdirs[i] = -1;
}

MapSpace& MapSpace::operator=(const MapSpace& i_right)
{
  if (this == &i_right) return *this;
  m_regionname = i_right.m_regionname;
  m_name = i_right.m_name;
  m_type = i_right.m_type;
  m_adjacentnames = i_right.m_adjacentnames;
  for (int i = 0 ; i < 7 ; ++i)  m_pilotdirs[i] = i_right.m_pilotdirs[i];
  m_orbit = i_right.m_orbit;
  m_color = i_right.m_color;
  m_cost = i_right.m_cost;
  m_gateid = i_right.m_gateid;
  return *this;
}

MapSpace::MapSpace(const std::string& i_regionname,const std::string& i_name,
                   SpaceType i_type,Color i_color) :
  m_regionname(i_regionname),
  m_name(i_name),
  m_type(i_type),
  m_adjacentnames(),
  m_pilotdirs(),
  m_orbit(""),
  m_color(i_color),
  m_cost(0),
  m_gateid(-1)
{
  for (size_t i = 1 ; i <= 6 ; ++i) m_pilotdirs[i] = -1;
}

void MapSpace::AddAdjacency(const std::string& i_name)
{
  std::set<int> dummy;
  AddAdjacency(i_name,false,dummy);
}
 
void MapSpace::AddAdjacency(const std::string& i_name,bool isFlyable,const std::set<int>& i_pnumbers)
{
  // only insert one
  for (size_t i = 0 ; i < m_adjacentnames.size() ; ++i)
  {
    if (m_adjacentnames[i].first == i_name) return;
  }

  int nameindex = m_adjacentnames.size();
  m_adjacentnames.push_back(std::pair<std::string,bool>(i_name,isFlyable));
  if (m_type != CITY && isFlyable)
    THROW(std::runtime_error,"space " << m_name << " is not a city and cannot be flown from!");

  for (std::set<int>::const_iterator pit = i_pnumbers.begin() ; pit != i_pnumbers.end() ; ++pit)
  {
    int pnumber = *pit;
    if (pnumber < 1 || pnumber > 6)
      THROW(std::runtime_error,"space " << m_name << " has illegal pilot number " << pnumber);
    
    if (m_pilotdirs[pnumber] != -1)
      THROW(std::runtime_error,"space " << m_name << " has duplicate pilot number " << pnumber);

    m_pilotdirs[pnumber] = nameindex;
  }
}

bool MapSpace::IsNavCircle() const
{
  for (size_t i = 1 ; i <= 6 ; ++i)
  {
    if (m_pilotdirs[i] != -1) return true;
  }
  return false;
}

  
