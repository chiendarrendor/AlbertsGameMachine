#include "GameBoard.hpp"
#include "CommodityInfo.hpp"
#include "StringUtilities.hpp"
#include <fstream>
#include <iostream>
#include "BoardParsingUtilities.hpp"
    



GameBoard::GameBoard(const std::string &i_DataFile,const CommodityInfo &i_rComLog) : m_loadok(true)
{
	std::ifstream mapstream(i_DataFile.c_str());

  std::map<SpaceInfo *,std::string> linkStrings;

	if (!mapstream)
	{
    std::cout << "Map couldn't be loaded from " << i_DataFile << std::endl;
		m_loadok = false;
		return;
	}

	std::string linebuf;
	int linenum = 0;
	int area = 0;

	while(std::getline(mapstream,linebuf))
	{
		linenum++;
    // correctly condition line.
		RemoveWhitespace(linebuf);
		if (linebuf.size() == 0) continue;
		if (linebuf[0] == '#') continue;

    // is line an Area?
    AreaParser aparse(linebuf);
    if (aparse.IsValid()) { area = aparse.GetArea() ; continue; }

    // guess not...is it a legal space designation?
    SpaceParser sparse(linebuf);
    if (!sparse.IsValid()) 
    { 
      m_loadok = false; 
      std::cout << "bad map line: " << linenum << std::endl ; 
      continue; 
    }

    // does it exist already?
		if (m_spaces.GetSpace(sparse.GetName()))
		{
			std::cout << "Duplicate Space: " << sparse.GetName() << ", line " << linenum << std::endl;
			m_loadok = false;
			continue;
		}

    // time to create.
		SpaceInfo *pNewSpace = new SpaceInfo(
                                       sparse.GetName(),
                                       sparse.GetSpaceType(),
                                       area,
                                       sparse.GetMarketSize());
		m_spaces.Insert(*pNewSpace);
    linkStrings[pNewSpace] = sparse.GetLinkString();

    // COAST processing
    if (sparse.GetSpaceType() == COAST)
    {
      SpaceInfo *pLandSpace = m_spaces.GetSpace(sparse.GetLandName());
      if (!pLandSpace)
      {
        std::cout << "COAST space " << sparse.GetName() << " is not described after its LAND space." << std::endl;
        m_loadok = false;
      }
      else
      {
        pLandSpace->AddLink(*pNewSpace);
        pNewSpace->AddLink(*pLandSpace);
      }
    }

    // commodity parsing.
    if (sparse.GetMarketSize() > 1)
    {
      CommodityParser commparse(sparse.GetCommodityString(),pNewSpace,i_rComLog);

      if (!commparse.IsValid())
      {
        std::cout << "Error processing commodity on line " << linenum << std::endl;
        m_loadok = false;
        continue;
      }
    }
	}
  // end of file load.

	// link spaces.
  std::map<SpaceInfo *,std::string>::iterator linkit;

  for (linkit = linkStrings.begin() ; linkit != linkStrings.end() ; ++linkit)
  {
    SpaceInfo *pSpaceInfo = linkit->first;
    const std::string &linkString = linkit->second;

    LinkParser linkparser(pSpaceInfo,linkString,m_spaces);
    if (!linkparser.IsValid())
    {
      std::cout << "Error parsing Link Info for space " << pSpaceInfo->GetName() << std::endl;
      m_loadok = false;
    }
  }

	// Link Verification

  SpaceInfoMap::iterator spaceit;

	for (spaceit = m_spaces.begin() ; spaceit != m_spaces.end() ; ++spaceit)
	{
		SpaceInfo *pSpace = spaceit->second;

		const SpaceInfoMap &linkVec = pSpace->GetLinks();

		SpaceInfoMap::const_iterator linkit;

		for (linkit = linkVec.begin() ; linkit != linkVec.end() ; linkit++)
		{
			const SpaceInfo *pLink = linkit->second;

			if (!pLink->HasLinkTo(*pSpace))
			{
        std::cout << "Space " << pSpace->GetName() << " is not back-linked from " << pLink->GetName() << std::endl;
				m_loadok = false;
			}
		}
	}

	// build Satellites
  //	BuildSatellites();

	// Determine which spaces are 'overseas'
  //	SetOverseas();

}




bool GameBoard::LoadOk() const
{
	return m_loadok;
}


void GameBoard::PrintAttributes() const
{
  SpaceInfoMap::const_iterator spaceit;

  for(spaceit = m_spaces.begin() ; spaceit != m_spaces.end() ; ++spaceit)
  {
    const std::string &name = spaceit->first;
    const SpaceInfo *pSpace = spaceit->second;

    std::cout << "Map Name: " << name << std::endl;
    std::cout << "  Name: " << pSpace->GetName() << " is a ";
    switch (pSpace->GetSpaceType())
    {
    case LAND:
      std::cout << "LAND" << std::endl;
      break;
    case COAST:
      std::cout << "COAST" << std::endl;
      break;
    case SEA:
      std::cout << "SEA" << std::endl;
      break;
    case OCEAN:
      std::cout << "OCEAN" << std::endl;
      break;
    default:
      std::cout << "????" << std::endl;
    }

    std::cout << "  Area: " << pSpace->GetArea() << std::endl;
    std::cout << "  Market: " << pSpace->GetMarketSize() << std::endl;
    std::cout << "  Is Overseas: " << pSpace->IsOverseas() << std::endl;

    std::cout << "  Links: " << std::endl;
    SpaceInfoMap::const_iterator linkit;
    for (linkit = pSpace->GetLinks().begin() ; linkit != pSpace->GetLinks().end() ; ++linkit)
    {
      std::cout << "    " << linkit->second->GetName() << std::endl;
    }

    std::cout << "  Satellites: " << std::endl;
    for (linkit = pSpace->GetSatellites().begin() ; linkit != pSpace->GetSatellites().end() ; ++linkit)
    {
      std::cout << "    " << linkit->second->GetName() << std::endl;
    }

    std::cout << "  Commodities: " << std::endl;
    std::vector<const Commodity *>::const_iterator comit;
    for (comit = pSpace->GetCommodities().begin() ; comit != pSpace->GetCommodities().end() ; ++comit)
    {
      const Commodity *pCom = *comit;
      std::cout << "    " << pCom->GetName() << "(" << pCom->GetValue() << "," << pCom->GetSSNum() << ")" << std::endl;
    }
  }
} 

const SpaceInfoMap &GameBoard::GetSpaceInfoMap() const
{
  return m_spaces;
}

