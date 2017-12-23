#include "TradeBases.hpp"
#include "MerchantOfVenus.hpp"
#include "Utilities.hpp"
#include "Ship.hpp"
#include "MapData.hpp"
#include "MapSpace.hpp"

namespace
{

  enum Technology 
    { PRIMITIVES,INDUSTRY,TECHNOLOGY,BIOENGINEERS,METAPHYSICS,GALACTICBASE };


  TradeBase MakeGalacticBase()
  {
    TradeBase result("base","Galactic Base");
    result.AddToken(Ship::GetShipOfClass(SCOUT));
    result.AddToken(Ship::GetShipOfClass(TRANSPORT));
    result.AddToken(Ship::GetShipOfClass(CLIPPER));
    result.AddToken(Ship::GetShipOfClass(FREIGHTER));
    return result;
  }

  TradeBase MakeBase(const std::string& i_id,const std::string& i_name,
                     Technology i_tech,const std::string& i_buyers,
                     int i_iou,int i_factorycost,
                     int i_goodcount,const std::string& i_goodname,
                     int i_goodbuy,int i_goodsell,
                     const std::string& i_fgoodname,
                     int i_fgoodbuy,int i_fgoodsell,
                     const std::string& i_regionname,
                     const std::map<std::string,MapSpace *>i_spaces)
  {
    TradeBase result(i_id,i_name);
    
    switch(i_tech)
    {
    case PRIMITIVES:
    case GALACTICBASE:
      break;
    case INDUSTRY:
      result.AddToken(Ship::GetShipOfClass(TRANSPORT));
      result.AddToken(Ship::GetShipOfClass(FREIGHTER));
      result.AddToken(Token::Drive(false,true,false,80));
      result.AddToken(Token::Laser(false,100));
      break;
    case TECHNOLOGY:
      result.AddToken(Ship::GetShipOfClass(SCOUT));
      result.AddToken(Ship::GetShipOfClass(CLIPPER));
      result.AddToken(Token::Drive(true,false,false,120));
      result.AddToken(Token::NovaBall());
      break;
    case METAPHYSICS:
      result.AddToken(Token::Drive(true,true,false,300));
      break;
    case BIOENGINEERS:
      result.AddToken(Token::Shield(false,60));
      break;
    }

    result.AddToken(Token::IOU(i_id,i_iou));

    std::ostringstream fname;
    fname << i_fgoodname << " ($" << i_fgoodbuy << "/$" << i_fgoodsell << ")";

    result.AddToken(Token::Deed(fname.str(),FACTORY_DEED,i_factorycost,i_id));
    Token good = Token::Good(i_goodname,i_id,i_buyers,i_goodbuy,i_goodsell,false);
    for (int i = 0 ; i < i_goodcount ; ++i)
    {
      result.AddToken(good);
    }
    result.SetFactoryGood(Token::Good(i_fgoodname,i_id,i_buyers,i_fgoodbuy,i_fgoodsell,true));
    
    std::map<std::string,MapSpace *>::const_iterator spaceit;
    for (spaceit = i_spaces.begin() ; spaceit != i_spaces.end() ; ++spaceit)
    {
      if (spaceit->second->m_regionname != i_regionname) continue;
      if (spaceit->second->m_orbit.size() == 0) continue;
      result.AddSpaceportDeed(Token::Deed(spaceit->second->m_orbit,SPACEPORT_DEED,200,""));
    }

    return result;
  }
}

TradeBases::TradeBases(const MapData& i_mapdata)
{
  std::vector<std::string> habitablenames;
  std::string galbasename;

  std::map<std::string,Region *>::const_iterator regit;
  for (regit = i_mapdata.GetRegionMap().begin() ; 
       regit != i_mapdata.GetRegionMap().end() ; ++regit)
  {
    const Region* reg = regit->second;
    if (reg->m_inhabited)
    {
      if (galbasename != "") 
        THROW(std::runtime_error,"duplicate inhabited region");
      galbasename = reg->m_name;
    } 
    else if (reg->m_habitable)
    {
      habitablenames.push_back(reg->m_name);
    }
  }
  if (habitablenames.size() != 14)
    THROW(std::runtime_error,"map has invalid number of habitable systems!");

  myshuffle(habitablenames.begin(),habitablenames.end());

  m_bases[galbasename] = MakeGalacticBase();
  m_bases[habitablenames[0]] =
    MakeBase("1a","Nillis",BIOENGINEERS,"2,3,4a,4b",
             80,100,
             5,"Bionic Perfume",80,140,
             "Guard Plants",60,160,
             habitablenames[0],i_mapdata.GetSpaceMap());
  m_bases[habitablenames[1]] = 
    MakeBase("1b","Volois",METAPHYSICS,"2,3,4a,4b",
             120,200,
             6,"Voil Silk",140,220,
             "Canned Traits",120,240,
             habitablenames[1],i_mapdata.GetSpaceMap());
  m_bases[habitablenames[2]] = 
    MakeBase("2","Graw",PRIMITIVES,"3,4a,4b,5",
             90,200,
             6,"Space Spice",30,80,
             "Glorious Junk",100,200,
             habitablenames[2],i_mapdata.GetSpaceMap());
  m_bases[habitablenames[3]] = 
    MakeBase("3","Niks",BIOENGINEERS,"4a,4b,5,6",
             80,200,
             6,"Mulch Wine",20,60,
             "Living Toys",80,180,
             habitablenames[3],i_mapdata.GetSpaceMap());
  m_bases[habitablenames[4]] = 
    MakeBase("4a","Dell",INDUSTRY,"5,6,7a,7b",
             60,200,
             6,"Finest Dust",10,50,
             "Dribble Glass",120,200,
             habitablenames[4],i_mapdata.GetSpaceMap());
  m_bases[habitablenames[5]] = 
    MakeBase("4b","Humans",TECHNOLOGY,"5,6,7a,7b",
             90,100,
             5,"Rock Videos",120,200,
             "Primitive Art",60,160,
             habitablenames[5],i_mapdata.GetSpaceMap());
  m_bases[habitablenames[6]] = 
    MakeBase("5","Shenna",PRIMITIVES,"6,7a,7b,8",
             90,100,
             6,"Melf Pelts",50,110,
             "Pet Monsters",80,150,
             habitablenames[6],i_mapdata.GetSpaceMap());
  m_bases[habitablenames[7]] = 
    MakeBase("6","YXKLYX",TECHNOLOGY,"7a,7b,8,9a,9b",
             100,200,
             6,"Immortal Grease",50,100,
             "Shining Slime",100,200,
             habitablenames[7],i_mapdata.GetSpaceMap());
  m_bases[habitablenames[8]] = 
    MakeBase("7a","Zum",INDUSTRY,"8,9a,9b,10",
             100,200,
             5,"Chicle Liquor",40,90,
             "Custom Hives",140,220,
             habitablenames[8],i_mapdata.GetSpaceMap());
  m_bases[habitablenames[9]] =
    MakeBase("7b","EeepEeep",TECHNOLOGY,"8,9a,9b,10",
             100,100,
             4,"Servo Mechanism",200,300,
             "Pedigree Bolts",100,200,
             habitablenames[9],i_mapdata.GetSpaceMap());
  m_bases[habitablenames[10]] =
    MakeBase("8","Whynoms",INDUSTRY,"9a,9b,10,1a,1b",
             80,100,
             4,"Impossible Furniture",110,180,
             "Other Shoes",80,160,
             habitablenames[10],i_mapdata.GetSpaceMap());
  m_bases[habitablenames[11]] =
    MakeBase("9a","Cholos",BIOENGINEERS,"10,1a,1b,2",
             90,200,
             6,"Designer Genes",60,120,
             "Life Projects",160,240,
             habitablenames[11],i_mapdata.GetSpaceMap());
  m_bases[habitablenames[12]] = 
    MakeBase("9b","Wollow",PRIMITIVES,"10,1a,1b,2",
             90,100,
             5,"Megalith Paperweight",90,160,
             "Portable Pipe Organ",80,160,
             habitablenames[12],i_mapdata.GetSpaceMap());
  m_bases[habitablenames[13]] = 
    MakeBase("10","Qosssuth",METAPHYSICS,"1a,1b,2,3",
             120,200,
             4,"Psychotic Sculpture",160,250,
             "Infinite Puzzles",120,250,
             habitablenames[13],i_mapdata.GetSpaceMap());

  std::map<std::string,TradeBase>::iterator tbit;
  for (tbit = m_bases.begin() ; tbit != m_bases.end() ; ++tbit)
  {
    m_solarsystembyrace[tbit->second.GetId()] = tbit->first;
  }
}

TradeBase& TradeBases::GetTradeBaseBySolarSystemName(const std::string& i_ssname)
{
  return m_bases[i_ssname];
}

const std::map<std::string,TradeBase>& TradeBases::GetBaseMap() const
{
  return m_bases;
}

const std::map<std::string,std::string>& TradeBases::GetSolarSystemByRaceMap() const
{
  return m_solarsystembyrace;
}

const std::string& TradeBases::GetSolarSystemOfRace(const std::string& i_raceid) const
{
  std::map<std::string,std::string>::const_iterator findit;
  findit = m_solarsystembyrace.find(i_raceid);
  return findit->second;
}

bool TradeBases::RegionHasTradeBase(const std::string& i_regionname) const
{
  std::map<std::string,TradeBase>::const_iterator findit = 
    m_bases.find(i_regionname);

  return (findit != m_bases.end());
}

