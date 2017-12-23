#include "BoardCounter.hpp"
#include "BoardState.hpp"
#include "Players.hpp"
#include "CommodityInfo.hpp"
#include "SpaceInfo.hpp"
#include <iostream>

BoardCounter::BoardCounter(const BoardState &i_bs,const Players &i_PlayerList) :
  m_Totals(i_PlayerList.size()),
  m_PerCommodity(i_PlayerList.size())
{
  // 1. make a mapping of player capital to player id;
  size_t playerid;
  for (playerid = 0 ; playerid < i_PlayerList.size() ; ++playerid)
  {
    m_CapitalToIdMapper[i_PlayerList[playerid].GetCapital()] = playerid;
  }

  const std::map<std::string,SpaceState> &spaces = i_bs.GetSpaceStates();
  std::map<std::string,SpaceState>::const_iterator spaceit;

  for (spaceit = spaces.begin() ; spaceit != spaces.end() ; ++spaceit)
  {
    const SpaceState &sstate = spaceit->second;
    const SpaceInfo &sinfo = sstate.GetSpaceInfo();
    
    if (!sstate.IsDominated()) continue;

    playerid = m_CapitalToIdMapper[sstate.GetDomination().GetCapital()];
    
    // add to totals
    m_Totals[playerid].first++;
    if (!sstate.GetDomination().IsOld()) m_Totals[playerid].second++;

    const std::vector<const Commodity *> &commodities = sinfo.GetCommodities();
    std::vector<const Commodity *>::const_iterator comit;
    for (comit = commodities.begin() ; comit != commodities.end() ; ++comit)
    {
      const Commodity *pCom = *comit;

      m_PerCommodity[playerid][pCom->GetName()].first++;
      if (!sstate.GetDomination().IsOld()) m_PerCommodity[playerid][pCom->GetName()].second++;
    }
  }

  std::map<std::string,int>::iterator capit;
  std::cout << "Capital to Id:" << std::endl;
  for (capit = m_CapitalToIdMapper.begin() ; capit != m_CapitalToIdMapper.end() ; ++capit)
  {
    std::cout << "  " << capit->first << " -> " << capit->second << std::endl;
  }
  std::cout << "Per Commodity: " << std::endl;
  for (size_t i = 0 ; i < m_PerCommodity.size() ; ++i)
  {
    std::cout << "  PLID: " << i << std::endl;
    std::map<std::string,std::pair<int,int> >::iterator comit;
    for (comit = m_PerCommodity[i].begin() ; comit != m_PerCommodity[i].end() ; ++comit)
    {
      std::cout << "    COM: " << comit->first << "(" << comit->second.first << "," 
                << comit->second.second << ")" << std::endl;
    }
  }
  std::cout << "Totals: " << std::endl;
  for (size_t i = 0 ; i < m_Totals.size() ; ++i)
  {
    std::cout << "  PLID: " << i << std::endl <<
      m_Totals[i].first << "," <<
      m_Totals[i].second << ")" << std::endl;
  }
}

int BoardCounter::GetNumSpaces(const std::string &i_CommodityName,const std::string &i_CapitalName)
{
  return m_PerCommodity[m_CapitalToIdMapper[i_CapitalName]][i_CommodityName].first;
}

int BoardCounter::GetNumNewSpaces(const std::string &i_CommodityName,const std::string &i_CapitalName)
{
  return m_PerCommodity[m_CapitalToIdMapper[i_CapitalName]][i_CommodityName].second;
}

int BoardCounter::GetNumSpaces(const std::string &i_CapitalName)
{
  return m_Totals[m_CapitalToIdMapper[i_CapitalName]].first;
}

int BoardCounter::GetNumNewSpaces(const std::string &i_CapitalName)
{
  return m_Totals[m_CapitalToIdMapper[i_CapitalName]].second;
}

  
