#include "SpaceInfo.hpp"


class AreaParser
{
public:
  AreaParser(const std::string &i_Line)
  {
    std::vector<std::string> linetokens;

    Tokenize(linetokens,i_Line," ");
    if (linetokens.size() == 2 && linetokens[0] == "Area")
    {
      m_IsValid = true;
      m_Area = atoi(linetokens[1].c_str());
    }
    else
    {
      // while not valid, it's not really an error for AreaParser to fail
      // (could be a land line)
      m_IsValid = false;
      m_Area = -1;
    }
  }
  
  bool IsValid() { return m_IsValid; }
  int GetArea() { return m_Area; }
private:
  bool m_IsValid;
  int m_Area;
};

class SpaceParser
{
public:
  SpaceParser(const std::string &i_Line) : m_IsValid(false)
  {
    std::vector<std::string> linetokens;
    Tokenize(linetokens,i_Line,",",true);
    if (linetokens.size() != 3 && linetokens.size() != 5) 
    { 
      std::cout << "Illegal # of tokens on line." << std::endl;
      return; 
    }

		m_Name = linetokens[0];
		std::string typestring = linetokens[1];

		m_LinkString = linetokens[2];

    m_MarketSize = -1;

    if      (typestring == "Sat")    {   m_SpaceType =  LAND;   m_MarketSize = 1;		}
		else if (typestring == "Coast")  {   m_SpaceType = COAST;		m_MarketSize = 0;		}
		else if (typestring == "Land")   {	 m_SpaceType = LAND; /* stay tuned */     }  
		else if (typestring == "Sea")    {   m_SpaceType = SEA;			m_MarketSize = 0;   }
		else if (typestring == "Ocean")  {   m_SpaceType = OCEAN;		m_MarketSize = 0;   }
		else 
    {  
      std::cout << "Unknown land type: " << typestring << std::endl;
      return; 
    }

    if (m_MarketSize == -1 && linetokens.size() != 5) 
    { 
      std::cout << "Land line needs Market size and Commodity" << std::endl;
      return; 
    } 
    if (m_MarketSize != -1 && linetokens.size() != 3) 
    { 
      std::cout << "non-land line has too many tokens." << std::endl;
      return; 
    }

    // special parsing for COAST names
    if (m_SpaceType == COAST)
    {
      Tokenize(linetokens,m_Name,":");
      if (linetokens.size() > 2) 
      { 
        std::cout << "Illegal coast name " << m_Name << std::endl;
        return ; 
      }

      m_LandName = linetokens[0];

      if (linetokens.size() == 1)
      {
        m_Name += ":DEFAULT";
      }
    }

    if (m_MarketSize != -1) { m_IsValid = true ; return; }

    m_MarketSize = atoi(linetokens[3].c_str());
    if (m_MarketSize <= 1 || m_MarketSize > 6) 
    { 
      std::cout << "non-satellite market size not between 2 and 6" << std::endl;
      return; 
    }

    m_CommodityString = linetokens[4];

    m_IsValid = true;
  }

  bool IsValid() { return m_IsValid; }
  const std::string &GetName() { return m_Name; }
  const std::string &GetLandName() { return m_LandName; }
  SpaceType GetSpaceType() { return m_SpaceType; }
  const std::string &GetLinkString() { return m_LinkString; }
  int GetMarketSize() { return m_MarketSize; }
  const std::string &GetCommodityString() { return m_CommodityString; }
private:
  bool m_IsValid;
  std::string m_Name;
  std::string m_LandName;
  SpaceType m_SpaceType;
  std::string m_LinkString;
  int m_MarketSize;
  std::string m_CommodityString;
};

class CommodityParser
{
public:
  CommodityParser(const std::string &i_CommString,
                  SpaceInfo *i_pSpaceInfo,
                  const CommodityInfo &i_rComLog)
  {
    m_IsValid = true;
    std::vector<std::string> linetokens;
    Tokenize(linetokens,i_CommString,"/");
    bool seenOne = false;
    
    for (size_t i = 0 ; i < linetokens.size() ; ++i)
    {
      const Commodity *pCom = i_rComLog.GetCommodityByName(linetokens[i]);
      if (!pCom)
      {
        std::cout << "Unknown commodity " << linetokens[i] << std::endl;
        m_IsValid = false;
        continue;
      }

      i_pSpaceInfo->AddCommodity(*pCom);
      seenOne = true;
    }
    
    if (!seenOne)
    {
      std::cout << "line needs at least one commodity." << std::endl;
      m_IsValid = false;
    }
  }

  bool IsValid() { return m_IsValid; }
private:
  bool m_IsValid;
};


class LinkParser
{
public:
  LinkParser(SpaceInfo *i_pSpaceInfo,const std::string &i_LinkString,SpaceInfoMap &m_Spaces)
  {
    std::vector<std::string> linetokens;
    Tokenize(linetokens,i_LinkString,"/");

    for (size_t i = 0 ; i < linetokens.size() ; ++i)
    {
      SpaceInfo *pLinkTarget = m_Spaces.GetSpace(linetokens[i]);
      if (!pLinkTarget)
      {
        std::cout << "Can't find " << linetokens[i] << " (putative link of " << i_pSpaceInfo->GetName() << std::endl;
        m_IsValid = false;
        continue;
      }
      
      // COAST and LAND spaces are auto-connected, so if a non-land space appears to connect
      // to a land space, assume what they meant was the default coast space of that land space.
      if (i_pSpaceInfo->GetSpaceType() != LAND && pLinkTarget->GetSpaceType() == LAND)
      {
        std::string coaststring = linetokens[i] + ":DEFAULT";
        
        pLinkTarget = m_Spaces.GetSpace(coaststring);
        if (!pLinkTarget)
        {
          std::cout << "Can't find Default Coast of " << linetokens[i] << std::endl;
          m_IsValid = false;
          continue;
        }
      }

      if (i_pSpaceInfo->HasLinkTo(*pLinkTarget))
      {
        std::cout << "Duplicate Link from " << i_pSpaceInfo->GetName() << 
          " to " << pLinkTarget->GetName() << std::endl;
        m_IsValid = false;
        continue;
      }

      if (i_pSpaceInfo->GetSpaceType() == LAND &&
          pLinkTarget->GetSpaceType() != LAND)
      {
        std::cout << "land " << i_pSpaceInfo->GetName() << 
          " linked to non-land " << pLinkTarget->GetName() << std::endl;
        m_IsValid = false;
        continue;
      }

      i_pSpaceInfo->AddLink(*pLinkTarget);
    }
  }

  bool IsValid() { return m_IsValid; }

private:
  bool m_IsValid;
};
