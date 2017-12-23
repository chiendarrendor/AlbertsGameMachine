#include "DeckInfo.hpp"
#include "CommodityInfo.hpp"
#include "CardInfo.hpp"

const std::string DeckInfo::ALCHEMISTSGOLD("AlchemistsGold");
const std::string DeckInfo::ANDREASVESALIUS("AndreasVesalius");
const std::string DeckInfo::ARMOR("Armor");
const std::string DeckInfo::BARTOLOMEDELASCASAS("BartolomedeLasCasas");
const std::string DeckInfo::BLACKDEATH("BlackDeath");
const std::string DeckInfo::CHARLEMAGNE("Charlemagne");
const std::string DeckInfo::CHRISTOPHERCOLUMBUS("ChristopherColumbus");
const std::string DeckInfo::CIVILWAR("CivilWar");
const std::string DeckInfo::DESIDERIUSERASMUS("DesideriusErasmus");
const std::string DeckInfo::DIONYSUSEXIGUUS("DionysusExiguus");
const std::string DeckInfo::ENLIGHTENEDRULER("EnlightenedRuler");
const std::string DeckInfo::FAMINE("Famine");
const std::string DeckInfo::GALILEOGALILEI("GalileoGalilei");
const std::string DeckInfo::GUNPOWDER("Gunpowder");
const std::string DeckInfo::HENRYOLDENBURG("HenryOldenburg");
const std::string DeckInfo::IBNMAJID("IbnMajid");
const std::string DeckInfo::JOHANNGUTENBERG("JohannGutenberg");
const std::string DeckInfo::LEONARDODAVINCI("LeonardoDaVinci");
const std::string DeckInfo::LONGBOW("LongBow");
const std::string DeckInfo::MARCOPOLO("MarcoPolo");
const std::string DeckInfo::MONGOLARMIES("MongolArmies");
const std::string DeckInfo::MYSTICISMABOUNDS("MysticismAbounds");
const std::string DeckInfo::NICOLAUSCOPERNICUS("NicolausCopernicus");
const std::string DeckInfo::PAPALDECREE("PapalDecree");
const std::string DeckInfo::PIRATESVIKINGS("PiratesVikings");
const std::string DeckInfo::PRINCEHENRY("PrinceHenry");
const std::string DeckInfo::RASHIDADDIN("RashidadDin");
const std::string DeckInfo::REBELLION("Rebellion");
const std::string DeckInfo::RELIGIOUSSTRIFE("ReligiousStrife");
const std::string DeckInfo::REVOLUTIONARYUPRISINGS("RevolutionaryUprisings");
const std::string DeckInfo::SIRISAACNEWTON("SirIsaacNewton");
const std::string DeckInfo::STBENEDICT("StBenedict");
const std::string DeckInfo::STIRRUPS("Stirrups");
const std::string DeckInfo::THECRUSADES("TheCrusades");
const std::string DeckInfo::WALTERTHEPENNILESS("WalterthePenniless");
const std::string DeckInfo::WAR("War");
const std::string DeckInfo::WILLIAMCAXTON("WilliamCaxton");

DeckInfo::DeckInfo()
{
  CardInfo *pNewCard;

  pNewCard = new CardInfo(CommodityInfo::STONE,1);
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(2,pNewCard);

  pNewCard = new CardInfo(CommodityInfo::WOOL,1);
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(2,pNewCard);

  pNewCard = new CardInfo(CommodityInfo::TIMBER,1);
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(CommodityInfo::CLOTH,CommodityInfo::WINE,1);
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(CommodityInfo::METAL,1);
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(CommodityInfo::FUR,1);
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(CommodityInfo::SILK,1);
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(CommodityInfo::SPICE,1);
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(CommodityInfo::IVORY,CommodityInfo::GOLD,1);
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(CommodityInfo::TIMBER,2);
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(2,pNewCard);

  pNewCard = new CardInfo(CommodityInfo::GRAIN,2);
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(2,pNewCard);
  
  pNewCard = new CardInfo(CommodityInfo::CLOTH,2);
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(CommodityInfo::WINE,2);
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(CommodityInfo::METAL,2);
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(CommodityInfo::SILK,2);
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(CommodityInfo::SPICE,2);
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(CommodityInfo::CLOTH,3);
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(CommodityInfo::WINE,3);
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(CommodityInfo::METAL,3);
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(CommodityInfo::FUR,3);
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(CommodityInfo::SILK,3);
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(CommodityInfo::SPICE,3);
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(CommodityInfo::GOLD,3);
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);


  pNewCard = new CardInfo(ANDREASVESALIUS,3,20,"BD");
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(BARTOLOMEDELASCASAS,3,30,"Y");
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(CHARLEMAGNE,1,20,"W");
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(CHRISTOPHERCOLUMBUS,2,30,"UT");
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(DESIDERIUSERASMUS,2,20,"OQ");
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(DIONYSUSEXIGUUS,1,20,"N");
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(GALILEOGALILEI,3,20,"AQ");
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(HENRYOLDENBURG,3,30,"D");
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(IBNMAJID,2,20,"TY");
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(JOHANNGUTENBERG,2,30,"O");
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(LEONARDODAVINCI,3,20,"PBQ");
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(MARCOPOLO,2,20,"YR");
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(NICOLAUSCOPERNICUS,2,20,"AX");
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(PRINCEHENRY,2,20,"TX");
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(RASHIDADDIN,1,10,"NR");
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(SIRISAACNEWTON,3,20,"CD");
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(STBENEDICT,1,10,"NE");
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(WALTERTHEPENNILESS,1,20,"R");
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(WILLIAMCAXTON,2,20,"O");
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(ALCHEMISTSGOLD,1,true);
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(ARMOR,1,false);
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(BLACKDEATH,2,true);
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(CIVILWAR,1,true);
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(ENLIGHTENEDRULER,1,true);
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(FAMINE,1,true);
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(GUNPOWDER,2,false);
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(LONGBOW,2,false);
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(MONGOLARMIES,2,false);
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(MYSTICISMABOUNDS,1,true);
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(PAPALDECREE,1,true);
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(PIRATESVIKINGS,1,true);
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(REBELLION,1,true);
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(RELIGIOUSSTRIFE,2,true);
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(REVOLUTIONARYUPRISINGS,1,true);
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(STIRRUPS,1,false);
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(THECRUSADES,1,true);
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);

  pNewCard = new CardInfo(WAR,1,true);
  m_Cards[pNewCard->GetName()] = std::pair<int,CardInfo *>(1,pNewCard);
}

DeckInfo::~DeckInfo()
{
  std::map<std::string,std::pair<int,CardInfo *> >::iterator doomed;

  for (doomed = m_Cards.begin() ; doomed != m_Cards.end() ; ++doomed)
  {
    delete doomed->second.second;
  }
}


const CardInfo *DeckInfo::GetCardInfo(const std::string &i_Name) const
{
  std::map<std::string,std::pair<int,CardInfo *> >::const_iterator findit;

  findit = m_Cards.find(i_Name);
    
  if (findit == m_Cards.end())
  {
    return NULL;
  }

  return findit->second.second;
}

void DeckInfo::FillCardNameVector(std::vector<std::string> &io_Vector) const
{
  io_Vector.clear();

  std::map<std::string,std::pair<int,CardInfo *> >::const_iterator cardit;

  for (cardit = m_Cards.begin() ; cardit != m_Cards.end() ; ++cardit)
  {
    const std::string &cName = cardit->first;
    int cNum = cardit->second.first;
    
    int i;

    for (i = 0 ; i < cNum ; ++i)
    {
      io_Vector.push_back(cName);
    }
  }
}




