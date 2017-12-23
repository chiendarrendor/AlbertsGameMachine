#include "MapData.hpp"
#include "MapSpace.hpp"
#include <iostream>
#include <fstream>
#include <set>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include "MerchantOfVenus.hpp"
#include <boost/algorithm/string.hpp>


namespace
{
  std::map<std::string,std::string> ParseAttributes(pugi::xml_node node,
                                                    const std::set<std::string>& reqattrs,
                                                    const std::set<std::string>&  optattrs)
  {
    std::map<std::string,std::string> result;
    pugi::xml_node::attribute_iterator ait;
    for (ait = node.attributes_begin() ; ait != node.attributes_end() ; ++ait)
    {
      pugi::xml_attribute curattr = *ait;
      if (reqattrs.find(curattr.name()) == reqattrs.end() &&
          optattrs.find(curattr.name()) == optattrs.end())
      {
        THROW(std::runtime_error,"unknown attr " << curattr.name() << " in " << node.name());
      }
      result[curattr.name()] = curattr.value();
    }

    std::set<std::string>::const_iterator reqitr;
    for (reqitr = reqattrs.begin() ; reqitr != reqattrs.end() ; ++reqitr)
    {
      if (result.find(*reqitr) == result.end())
      {
        THROW(std::runtime_error,"missing required attr " << *reqitr << " in " << node.name());
      }
    }
    return result;
  }

  SpaceType ParseType(const std::string& i_type)
  {
    if (i_type == "dot") return DOT;
    if (i_type == "asteroid") return ASTEROID;
    if (i_type == "qbox") return QBOX;
    if (i_type == "penalty") return PENALTY;
    if (i_type == "city") return CITY;
    if (i_type == "station") return STATION;

    THROW(std::runtime_error,"illegal type " << i_type);
  }

  Color ParseColor(const std::string& i_color)
  {
    if (i_color == "blue") return BLUE;
    if (i_color == "red") return RED;
    if (i_color == "yellow") return YELLOW;

    THROW(std::runtime_error,"illegal color " << i_color);
  }

  std::ostream& operator<<(std::ostream& o, SpaceType st)
  {
    switch(st)
    {
    case DOT: o << "dot"; break;
    case ASTEROID: o <<  "asteroid"; break;
    case QBOX: o <<  "qbox"; break;
    case PENALTY: o <<  "penalty"; break;
    case CITY: o <<  "city"; break;
    case STATION: o <<  "station"; break;
    default: o <<  "???";
    }
    return o;
  }
}

MapData::MapData(const std::string &i_fname)
{
  pugi::xml_document doc;
  pugi::xml_parse_result status = doc.load_file(i_fname.c_str());

  if (!status)
  {
    throw std::runtime_error("pugi xml load failure");
  }

  pugi::xml_node root = doc.document_element();
  if (std::string(root.name()) != "board") throw std::runtime_error("root not 'board'");

  for(pugi::xml_node child = root.first_child() ; child ; child=child.next_sibling())
  {
    if (std::string(child.name()) == "line") ProcessLine(child);
    else if (std::string(child.name()) == "solarsystem") ProcessSolarSystem(child);
    else 
    {
      THROW(std::out_of_range,"Unknown Region Type " << child.name());
    }
  }

  // at this point, its all been loaded into memory.
  // we need to validate the following things:
  // a) every adjacency of every space points to a space that is 
  //    i) different
  //    ii) extant 
  // b) every adjacency of every space must also have an inverse adjacency from the other space
  // c) every flyable adjacency must
  //    i) start in a city
  //    ii) end in a city in the same region
  //    iii) 's inverse adjacency be also flyable
  //    iv) all cities in a region are flyable to each other.
  // d) pilot numbers 1-6 must be defined for each NAVCIRCLE space.

  // for c,iv ... a map keyed by Region id, of each CITY space in that region.
  std::map<std::string,std::vector<MapSpace *> > regioncities;


  for (std::map<std::string,MapSpace *>::iterator sit = m_spaces.begin() ; sit != m_spaces.end() ; ++sit)
  {
    MapSpace *origSpace = sit->second;

    if (origSpace->m_type == CITY) regioncities[origSpace->m_regionname].push_back(origSpace);

    std::vector<std::pair<std::string,bool> >::iterator adjit;
    for (adjit = origSpace->m_adjacentnames.begin() ; adjit != origSpace->m_adjacentnames.end() ; ++adjit)
    {
      std::pair<std::string,bool>& origAdj = *adjit;

      // d)
      if (origSpace->IsNavCircle())
      {
        for (size_t i = 1 ; i <= 6 ; ++i)
        {
          if (origSpace->m_pilotdirs[i] == -1)
            THROW(std::runtime_error, "Nav Circle Space " << origSpace->m_name << " missing pilot number " << i);
        }
      }

      // a,i
      if (origSpace->m_name == origAdj.first)
        THROW(std::runtime_error,"space " << origSpace->m_name << " points to itself!");

      std::map<std::string,MapSpace *>::iterator otherit = m_spaces.find(origAdj.first);
      // a,ii 
      if (otherit == m_spaces.end())
        THROW(std::runtime_error,"space " << origSpace->m_name << " has nonexistent adjacency " << origAdj.first);
      MapSpace* otherSpace = otherit->second;

      std::vector<std::pair<std::string,bool> >::iterator backit;
      for (backit = otherSpace->m_adjacentnames.begin() ; backit != otherSpace->m_adjacentnames.end() ; ++backit)
      {
        if (backit->first == origSpace->m_name) break;
      }

      // b)
      if (backit == otherSpace->m_adjacentnames.end())
        THROW(std::runtime_error,"space " << otherSpace->m_name << " fails to point back to " << origSpace->m_name);

      std::pair<std::string,bool>& backAdj = *backit;

      // c)
      if (origAdj.second != backAdj.second)
        THROW(std::runtime_error,"links between " << otherSpace->m_name << " and " << origSpace->m_name <<
              " fail to have matching flyable status");

      if (origAdj.second)
      {
        if (otherSpace->m_type != CITY || origSpace->m_type != CITY)
          THROW(std::runtime_error,"flyable links between " << otherSpace->m_name << " and " << origSpace->m_name <<
                " are not between two cities!");
        if (otherSpace->m_regionname != origSpace->m_regionname)
          THROW(std::runtime_error,"flyable links between " << otherSpace->m_name << " and " << origSpace->m_name <<
                " are between cities in different Solar Systems!");
      }
    }
  }

  // for c,iv ... a map keyed by Region id, of each CITY space in that region.
  //std::map<std::string,std::vector<MapSpace *> > regioncities;
  std::map<std::string,std::vector<MapSpace *> >::iterator regionit;
  for (regionit = regioncities.begin() ; regionit != regioncities.end() ; ++regionit)
  {
    std::vector<MapSpace *>& cities = regionit->second;
    for (size_t i = 0 ; i < cities.size() ; ++i)
    {
      MapSpace *curcity = cities[i];
      for (size_t j = i+1 ; j < cities.size() ; ++j)
      {
        const std::string& oname = cities[j]->m_name;
        
        std::vector<std::pair<std::string,bool> >::iterator adjit;
        for (adjit = curcity->m_adjacentnames.begin() ; adjit != curcity->m_adjacentnames.end() ; ++adjit)
        {
          if (adjit->first == oname) break;
        }
        
        if (adjit == curcity->m_adjacentnames.end())
          THROW(std::runtime_error,"cities " << curcity->m_name << " and " << 
                cities[j]->m_name << " are not linked!");
      }
    }
  }
}

void MapData::ProcessLine(pugi::xml_node linenode)
{
  std::set<std::string> optattrs;
  std::set<std::string> reqattrs;
  reqattrs.insert("name");
  std::map<std::string,std::string> attrs = ParseAttributes(linenode,reqattrs,optattrs);

  Region *reg = new Region(attrs["name"],false,false);

  if (m_regions.find(reg->m_name) != m_regions.end())
  {
    THROW(std::runtime_error,"duplicate region id " << reg->m_name);
  }

  m_regions[reg->m_name]=reg;

  std::string begin;
  std::string end;
  std::vector<MapSpace *> spaces;

  for(pugi::xml_node child = linenode.first_child() ; child ; child=child.next_sibling())
  {
    if (std::string(child.name()) == "begin") begin = ProcessTerminal(child);
    else if (std::string(child.name()) == "end") end = ProcessTerminal(child);
    else if (std::string(child.name()) == "space") 
    {
      MapSpace *newspace = ProcessSpace(*reg,child);

      if (newspace->m_type != DOT)
        THROW(std::runtime_error,"illegal space type " << newspace->m_type << " for line: " << newspace->m_name);

      spaces.push_back(newspace); // need these to work on the line-joins.

    }
    else THROW(std::runtime_error,"line node has unknown child " << child.name());
  }

  for (size_t i = 0 ; i < spaces.size() ; ++i)
  {
    if (i == 0) 
    {
      spaces[i]->AddAdjacency(begin);
    }
    else
    {
      spaces[i]->AddAdjacency(spaces[i-1]->m_name);
    }

    if (i == spaces.size()-1)
    {
      spaces[i]->AddAdjacency(end);
    }
    else
    {
      spaces[i]->AddAdjacency(spaces[i+1]->m_name);
    }
  }
}

void MapData::ProcessSolarSystem(pugi::xml_node solnode)
{
  std::set<std::string> optattrs;
  std::set<std::string> reqattrs;
  reqattrs.insert("name");
  reqattrs.insert("x");
  reqattrs.insert("y");
  optattrs.insert("inhabited");
  optattrs.insert("ishabitable");
  optattrs.insert("alpha");
  std::map<std::string,std::string> attrs = ParseAttributes(solnode,reqattrs,optattrs);

  bool habit = false;
  bool inhabit = false;
  if (attrs.find("ishabitable") != attrs.end() && attrs["ishabitable"] == "y") habit = true;
  if (attrs.find("inhabited") != attrs.end() && attrs["inhabited"] == "y") inhabit = true;


  Region *reg = new Region(attrs["name"],habit,inhabit);

  if (m_regions.find(reg->m_name) != m_regions.end())
  {
    THROW(std::runtime_error,"duplicate region id " << reg->m_name);
  }

  m_regions[reg->m_name]=reg;

  // if we discover any space name of the form <stuff>_loop_<number>
  // a) all of the loop spaces must have the same <stuff>
  // b) there must be <number>s starting from 1 and increment to each subsequent number to some maximum
  static const boost::regex r("(\\S+)_loop_(\\d+)");
  std::map<int,MapSpace *> loops;
  std::string loopprefix("");
  size_t maxloop = 0;

  for(pugi::xml_node child = solnode.first_child() ; child ; child=child.next_sibling())
  {
    if (std::string(child.name()) == "icon") continue;

    if (std::string(child.name()) != "space") 
      THROW(std::runtime_error,"unknown child " << child.name() << " of solarsystem");

    MapSpace *newspace = ProcessSpace(*reg,child);

    boost::smatch what;
    if (boost::regex_match(newspace->m_name,what,r))
    {
      if (loopprefix == "") 
      {
        loopprefix = what[1];
      }
      else if (loopprefix != what[1])
        THROW(std::runtime_error,"non-matching loop prefix: " << newspace->m_name);

      size_t index = boost::lexical_cast<size_t>(what[2]);

      if (index < 1)
        THROW(std::runtime_error,"illegal loop index: " << newspace->m_name);

      if (loops.find(index) != loops.end())
        THROW(std::runtime_error,"duplicate loop index: " << newspace->m_name);

      if (index > maxloop) maxloop = index;

      loops[index] = newspace;
    }
  }
  // ok...now we've processed all spaces, and found all loops;
  // all loop spaces 
  // a) have the same prefix
  // b) have unique numbers
  // c) the smallest legal number is 1
  
  // before we start, lets make sure that we have the right number
  // since we know the largest number, and the smallest number can
  // be no smaller than 1, a simple count will suffice to provide assurance.
  if (loops.size() != maxloop)
    THROW(std::runtime_error,"solar system " << reg->m_name << " has incomplete loop");

  for (size_t i = 1 ; i <= maxloop ; ++i)
  {
    MapSpace *pCur = loops[i];
    if (i == 1)
    {
      pCur->AddAdjacency(loops[maxloop]->m_name);
    }
    else
    {
      pCur->AddAdjacency(loops[i-1]->m_name);
    }

    if (i==maxloop)
    {
      pCur->AddAdjacency(loops[1]->m_name);
    }
    else
    {
      pCur->AddAdjacency(loops[i+1]->m_name);
    }
  }
}


// processes a node that should be of the form 
// <begin name="terminal"/>
// or 
// <end name="terminal"/>
// returns terminal or throws exception

std::string MapData::ProcessTerminal(pugi::xml_node termnode)
{
  std::set<std::string> optattrs;
  std::set<std::string> reqattrs;
  reqattrs.insert("name");
  std::map<std::string,std::string> attrs = ParseAttributes(termnode,reqattrs,optattrs);
  return attrs["name"];
}

MapSpace* MapData::ProcessSpace(Region& i_region,pugi::xml_node spacenode)
{
  std::set<std::string> optattrs;
  std::set<std::string> reqattrs;
  reqattrs.insert("name");
  reqattrs.insert("type");
  reqattrs.insert("x");
  reqattrs.insert("y");
  optattrs.insert("color");
  optattrs.insert("cost");
  
  std::map<std::string,std::string> attrs = ParseAttributes(spacenode,reqattrs,optattrs);
  SpaceType type = ParseType(attrs["type"]);

  MapSpace *result = new MapSpace(i_region.m_name,attrs["name"],type);

  switch(type)
  {
  case CITY:
  case QBOX:
  case ASTEROID:
  case STATION:
    optattrs.clear();
    attrs = ParseAttributes(spacenode,reqattrs,optattrs);
    break;
  case DOT:
    optattrs.clear();
    optattrs.insert("color");
    attrs = ParseAttributes(spacenode,reqattrs,optattrs);
    if (attrs.find("color") != attrs.end()) result->m_color = ParseColor(attrs["color"]);
    break;
  case PENALTY:
    reqattrs.insert("cost");
    optattrs.clear();
    optattrs.insert("color");
    attrs = ParseAttributes(spacenode,reqattrs,optattrs);
    result->m_cost = boost::lexical_cast<int>(attrs["cost"]);
    if (attrs.find("color") != attrs.end()) result->m_color = ParseColor(attrs["color"]);
    break;
  default:
    THROW(std::runtime_error,"Unprocessable type " << type);
  }

  if (m_spaces.find(result->m_name) != m_spaces.end()) 
    THROW(std::runtime_error,"duplicated space name " << result->m_name);
  m_spaces[result->m_name] = result;
    
  for(pugi::xml_node child = spacenode.first_child() ; child ; child=child.next_sibling())
  {
    if (std::string(child.name()) == "adjacent") ProcessAdjacent(*result,child);
    else if (std::string(child.name()) == "orbit") ProcessOrbit(*result,child);
    else THROW(std::runtime_error,"illegal child of " << result->m_name);
  }
  
  return result;
}

void MapData::ProcessAdjacent(MapSpace& ms,pugi::xml_node adj)
{
  std::set<std::string> adjreq;
  std::set<std::string> adjopt;
  adjreq.insert("dest");
  adjopt.insert("flyable");
  adjopt.insert("pnumbers");
  adjopt.insert("invisible");
  std::map<std::string,std::string> adjattrs = ParseAttributes(adj,adjreq,adjopt);
  
  bool flyable = (adjattrs.find("flyable") != adjattrs.end()) && (adjattrs["flyable"] == "y");
  std::set<int> pnumbers;

  if (adjattrs.find("pnumbers") != adjattrs.end())
  {
    std::vector<std::string> pstrings;
    boost::split(pstrings,adjattrs["pnumbers"],boost::is_any_of(","));
    for(std::vector<std::string>::iterator vit = pstrings.begin() ; vit != pstrings.end() ; ++vit)
    {
      pnumbers.insert(boost::lexical_cast<int>(*vit));
    }
  }
  
  ms.AddAdjacency(adjattrs["dest"],flyable,pnumbers);
}

void MapData::ProcessOrbit(MapSpace& ms,pugi::xml_node orbit)
{
  std::set<std::string> reqattrs;
  std::set<std::string> optattrs;
  reqattrs.insert("name");
  optattrs.insert("orient");
  std::map<std::string,std::string> attrs = ParseAttributes(orbit,reqattrs,optattrs);
  ms.m_orbit = attrs["name"];
}

const std::map<std::string,Region *>& MapData::GetRegionMap() const
{
  return m_regions;
}

const std::map<std::string,MapSpace *>& MapData::GetSpaceMap() const
{
  return m_spaces;
}

