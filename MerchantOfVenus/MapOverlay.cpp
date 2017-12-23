#include "MapOverlay.hpp"
#include <boost/lexical_cast.hpp>
#include "Utilities.hpp"
#include "MerchantOfVenus.hpp"

namespace
{
  MapSpace TeleGate(int i_gateid)
  {
    MapSpace result("unknown",std::string("TeleGate ") + 
                    boost::lexical_cast<std::string>(i_gateid),
                    TELEGATE);
    result.m_gateid = i_gateid;
    return result;
  }

  MapSpace OpenPort()
  {
    MapSpace result("unknown","Open Port",STATION);
    return result;
  }

  MapSpace Penalty(Color i_color,int i_penalty)
  {
    MapSpace result("unknown",std::string("Penalty ") + 
                    boost::lexical_cast<std::string>(i_penalty),
                    PENALTY,i_color);
    result.m_cost = i_penalty;
    return result;
  }
  
  MapSpace Asteroid()
  {
    MapSpace result("unknown","asteroid",ASTEROID);
    return result;
  }

  void MakeQBoxList(std::vector<MapSpace>& i_qboxes)
  {
    i_qboxes.push_back(TeleGate(1));
    i_qboxes.push_back(TeleGate(2));
    i_qboxes.push_back(TeleGate(3));
    i_qboxes.push_back(TeleGate(4));
    i_qboxes.push_back(TeleGate(5));
    i_qboxes.push_back(TeleGate(6));
    i_qboxes.push_back(OpenPort());
    i_qboxes.push_back(OpenPort());
    i_qboxes.push_back(OpenPort());
    i_qboxes.push_back(Penalty(RED,10));
    i_qboxes.push_back(Penalty(RED,20));
    i_qboxes.push_back(Penalty(RED,20));
    i_qboxes.push_back(Penalty(RED,30));
    i_qboxes.push_back(Penalty(RED,30));
    i_qboxes.push_back(Penalty(RED,40));
    i_qboxes.push_back(Penalty(YELLOW,10));
    i_qboxes.push_back(Penalty(YELLOW,20));
    i_qboxes.push_back(Penalty(YELLOW,30));
    i_qboxes.push_back(Penalty(YELLOW,40));
    i_qboxes.push_back(Penalty(BLUE,10));
    i_qboxes.push_back(Penalty(BLUE,10));
    i_qboxes.push_back(Penalty(BLUE,20));
    i_qboxes.push_back(Penalty(BLUE,20));
    i_qboxes.push_back(Penalty(BLUE,30));
    i_qboxes.push_back(Penalty(BLUE,30));
    i_qboxes.push_back(Penalty(BLUE,40));
    for (int i = 0 ; i < 10 ; ++i)
    {
      i_qboxes.push_back(Asteroid());
    }
  }

  void MakeRelicList(std::vector<Token>& i_reliclist)
  {
    i_reliclist.push_back(Token::Relic("Jump Start",120));
    i_reliclist.push_back(Token::Relic("Gate Lock",100));
    // is red, is yellow, is relic
    i_reliclist.push_back(Token::Drive(false,true,true,80));
    i_reliclist.push_back(Token::Relic("Switch Switch",100));
    i_reliclist.push_back(Token::Shield(true,60));
    i_reliclist.push_back(Token::Relic("Auto Pilot",80));
    i_reliclist.push_back(Token::Relic("Spy Eye",100));
    i_reliclist.push_back(Token::Relic("Air Foil",80));
    i_reliclist.push_back(Token::Relic("Mulligan Gear",120));
    i_reliclist.push_back(Token::Laser(true,100));
  }
}

MapOverlay::MapOverlay(const MapData& i_mapdata) :
  m_mapdata(i_mapdata),
  m_overrides(),
  m_relics(),
  m_stationowners(),
  m_factoryowners(),
  m_orbitlocations()
{
  // get list of all qbox names and orbit locations.
  std::map<std::string,MapSpace *>::const_iterator spaceit;
  std::vector<std::string> qboxnames;
  for (spaceit = i_mapdata.GetSpaceMap().begin() ; 
       spaceit != i_mapdata.GetSpaceMap().end() ; ++spaceit)
  {
    if (spaceit->second->m_type == QBOX) qboxnames.push_back(spaceit->first);
    if (spaceit->second->m_orbit != "") m_orbitlocations[spaceit->second->m_orbit] = spaceit->first;
  }

  // make list of all qbox objects.
  std::vector<MapSpace> qboxes;
  MakeQBoxList(qboxes);
  myshuffle(qboxes.begin(),qboxes.end());

  // make list of all relics (go on qbox asteroids)
  MakeRelicList(m_allrelics);
  myshuffle(m_allrelics.begin(),m_allrelics.end());

  size_t vecidx;
  size_t relicidx = 0;

  for (vecidx = 0 ; vecidx < qboxnames.size() ; ++vecidx)
  {
    MapSpace qbox = qboxes[vecidx];
    const std::string& qboxname = qboxnames[vecidx];

    std::map<std::string,MapSpace *>::const_iterator fit;
    fit = m_mapdata.GetSpaceMap().find(qboxname);
    if (fit == m_mapdata.GetSpaceMap().end()) 
      THROW(std::runtime_error,"unknown qbox name?");
    
    const MapSpace& origspace = *(fit->second);

    qbox.m_regionname = origspace.m_regionname;
    qbox.m_name = origspace.m_name;
    qbox.m_adjacentnames = origspace.m_adjacentnames;
    for (size_t i = 1 ; i <= 6 ; ++i )
      qbox.m_pilotdirs[i] = origspace.m_pilotdirs[i];

    m_overrides[qboxname] = std::pair<MapSpace,Secret>(qbox,Secret());
    if (qbox.m_type == ASTEROID)
    {
      m_relics[qboxname] = m_allrelics[relicidx++];
    }
  }
}

std::string MapOverlay::PlaceStation(const std::string& i_station,int i_owner)
{
  std::string i_space = m_orbitlocations[i_station];

  std::map<std::string,MapSpace *>::const_iterator fit;
  fit = m_mapdata.GetSpaceMap().find(i_space);
  if (fit == m_mapdata.GetSpaceMap().end()) 
    THROW(std::runtime_error,"unknown qbox name?");
    
  const MapSpace& oldspace = *(fit->second);
  MapSpace newspace(oldspace.m_regionname,i_space,STATION);

  newspace.m_adjacentnames = oldspace.m_adjacentnames;
  for (size_t i = 1 ; i <= 6 ; ++i )
    newspace.m_pilotdirs[i] = oldspace.m_pilotdirs[i];

  m_overrides[i_space] = std::pair<MapSpace,Secret>(newspace,Secret());
  // Space Stations are public.
  m_overrides[i_space].second.MakePublic();

  m_stationowners[i_space] = i_owner;

  return i_space;
}

void MapOverlay::PlaceFactory(const std::string& i_solarsystem,int i_owner)
{
  m_factoryowners[i_solarsystem] = i_owner;
}



  
const std::vector<Token>& MapOverlay::GetRelicList() const
{
  return m_allrelics;
}

const std::pair<MapSpace,Secret> MapOverlay::GetMapSpace(const std::string& i_name) const
{
  std::map<std::string,std::pair<MapSpace,Secret> >::const_iterator fit = m_overrides.find(i_name);
  if (fit == m_overrides.end())
  {
    std::map<std::string,MapSpace*>::const_iterator rfit = m_mapdata.GetSpaceMap().find(i_name);
    std::pair<MapSpace,Secret> result(*(rfit->second),Secret());
    result.second.MakePublic();
    return result;
  }
  return  fit->second;
}

const MapSpace* MapOverlay::GetTeleGate(int i_pnum) const
{
  std::map<std::string,std::pair<MapSpace,Secret> >::const_iterator overit;
  for(overit = m_overrides.begin() ; overit != m_overrides.end() ; ++overit)
  {
    if (overit->second.first.m_type != TELEGATE) continue;
    if (overit->second.first.m_gateid != i_pnum) continue;
    // if we get here, we found it.
    if (overit->second.second.IsPublic()) return &(overit->second.first);
    else return NULL;
  }
  return NULL;
}

void MapOverlay::MakeSpacePublic(const std::string& i_name)
{
  std::map<std::string,std::pair<MapSpace,Secret> >::iterator overit =
    m_overrides.find(i_name);
  if (overit == m_overrides.end()) return;

  overit->second.second.MakePublic();
}

void MapOverlay::AddPlayerToSecret(const std::string& i_name,int i_plid)
{
  std::map<std::string,std::pair<MapSpace,Secret> >::iterator overit =
    m_overrides.find(i_name);
  if (overit == m_overrides.end()) return;

  overit->second.second.PlayerLearnsSecret(i_plid);

}

std::string MapOverlay::GetSpaceString(const std::string& i_name) const
{
  std::map<std::string,std::pair<MapSpace,Secret> >::const_iterator fit = 
    m_overrides.find(i_name);
  if (fit == m_overrides.end())
  {
    THROW(std::runtime_error,"gave a non-override space to GetSpaceString");
  }
  std::ostringstream oss;
  switch(fit->second.first.m_type)
  {
  case TELEGATE: oss << "Telegate " << fit->second.first.m_gateid; break;
  case STATION: 
    {
      oss << "Station";
      std::map<std::string,size_t>::const_iterator sit = m_stationowners.find(i_name);
      if (sit != m_stationowners.end()) 
      {
        oss << " " << sit->second;
      }
      break;
    }
  case PENALTY:
    oss << "Penalty ";
    switch(fit->second.first.m_color)
    {
    case BLUE: oss << 'B'; break;
    case YELLOW: oss << 'Y'; break;
    case RED: oss << 'R'; break;
    }
    oss << ' ' << fit->second.first.m_cost;
    break;
  case ASTEROID:
    {
      oss << "Asteroid";
      std::map<std::string,Token>::const_iterator rit = m_relics.find(i_name);
      if (rit != m_relics.end())
      {
        oss << ' ' << rit->second.Encoding();
      }
      break;
    }
  default:
    THROW(std::runtime_error,"unknown type " << fit->second.first.m_type);
  }
   
  return oss.str();
  
}

const std::map<std::string,std::pair<MapSpace,Secret> >& MapOverlay::GetOverrideMap() const
{
  return m_overrides;
}

bool MapOverlay::HasRelic(const std::string& i_space) const
{
  return m_relics.find(i_space) != m_relics.end();
}

Token MapOverlay::TakeRelic(const std::string& i_space)
{
  std::map<std::string,Token>::iterator fit = m_relics.find(i_space);
  Token result = fit->second;
  m_relics.erase(fit);
  return result;
}

const std::map<std::string,size_t>& MapOverlay::GetStationOwners() const
{
  return m_stationowners;
}

const std::map<std::string,size_t>& MapOverlay::GetFactoryOwners() const
{
  return m_factoryowners;
}


