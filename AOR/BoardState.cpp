#include "BoardState.hpp"
#include "Players.hpp"
#include "AOREventGenerator.hpp"
#include "BoardCounter.hpp"
#include "CommodityInfo.hpp"
#include "SpaceInfo.hpp"
#include <iostream>

BoardState::BoardState(const GameBoard &i_GameBoard) :
  m_GameBoard(i_GameBoard)
{
}

SpaceState &BoardState::GetSpaceState(const std::string &i_Space)
{
 std::map<std::string,SpaceState>::iterator findit = m_SpaceStates.find(i_Space);
 if (findit != m_SpaceStates.end()) return findit->second;

 const SpaceInfo *pSpaceInfo = m_GameBoard.GetSpaceInfoMap().GetSpace(i_Space);
 
 if (!pSpaceInfo) throw std::out_of_range("Unknown Space Name!");

 m_SpaceStates[i_Space] = SpaceState(i_Space,pSpaceInfo);
 return m_SpaceStates[i_Space];
}

const SpaceState &BoardState::GetSpaceState(const std::string &i_Space) const
{
 std::map<std::string,SpaceState>::const_iterator findit = m_SpaceStates.find(i_Space);
 if (findit != m_SpaceStates.end()) return findit->second;

 const SpaceInfo *pSpaceInfo = m_GameBoard.GetSpaceInfoMap().GetSpace(i_Space);
 
 if (!pSpaceInfo) throw std::out_of_range("Unknown Space Name!");

 m_SpaceStates[i_Space] = SpaceState(i_Space,pSpaceInfo);
 return m_SpaceStates[i_Space];
}

const std::map<std::string,SpaceState> &BoardState::GetSpaceStates() const
{
  return m_SpaceStates;
}






  
void BoardState::SetDominance(const std::string &i_Capital, const std::string &i_Space,bool i_IsOld)
{
  SpaceState &spaceref = GetSpaceState(i_Space);
  spaceref.Dominate(i_Capital,i_IsOld);
}


// this broadcasts the current info about the current space.
void BoardState::BroadcastSpaceInfo(const AOREventGenerator &i_egen,
                                    const std::string &i_Space) const
{
  std::cout << "Getting Space State" << std::endl;
  const SpaceState &sstate = GetSpaceState(i_Space);
  // empty the space
  i_egen.BroadcastSPACEEMPTY(i_Space);

  std::cout << "Space Dominated?" << std::endl;
  if (sstate.IsDominated())
  {
    std::cout << "Yes!" << std::endl;
    i_egen.BroadcastDOMINANCE(i_Space,sstate.GetDomination().GetCapital(),
                              sstate.GetDomination().GetAgeString());
  }
  else
  {
    std::cout << "No!" << std::endl;
    size_t i;
    for (i = 0 ; i < sstate.GetNumTokens() ; ++i)
    {
      i_egen.BroadcastTOKEN(i_Space,i,sstate[i].GetCapital(),sstate[i].GetAgeString());
    }
  }
}


// this broadcasts the commodity log info about the commodity(ies) of the
// space listed.
void BoardState::BroadcastCommodityLogInfo(const AOREventGenerator &i_egen,
                                           const std::string &i_Space,
                                           const Players &i_PlayerList,
                                           size_t i_numCapitals) const
{
  BoardCounter bcount(*this,i_PlayerList);

  const SpaceState &sstate = GetSpaceState(i_Space);

  // for each player in the game.
  for (size_t playerindex = 0 ; playerindex < i_numCapitals ; ++playerindex)
  {
    const std::string &pCap = i_PlayerList[playerindex].GetCapital();
    // for each commodity of the space.
    const std::vector<const Commodity *> &comms = sstate.GetSpaceInfo().GetCommodities();
    for (size_t comidx = 0 ; comidx < comms.size() ; ++comidx)
    {
      const std::string &pCap = i_PlayerList[playerindex].GetCapital();
      const Commodity *pComm = comms[comidx];
      i_egen.BroadcastCOMMODITYNUM(pComm->GetName(),
                                   pCap,
                                   bcount.GetNumSpaces(pComm->GetName(),
                                                       pCap),
                                   bcount.GetNumNewSpaces(pComm->GetName(),
                                                          pCap));
    }
    // and the totals for each player.
    i_egen.BroadcastCOMMODITYNUM("Total",
                                 pCap,
                                 bcount.GetNumSpaces(pCap),
                                 bcount.GetNumNewSpaces(pCap));
    
  }
}
                            


// this unicasts to the specified player the entire board state.
// (can assume the board was reset prior to the call)
void BoardState::SendAllSpaceInfo(const AOREventGenerator &i_egen,
                                  const std::string &i_Player) const
{
  std::map<std::string,SpaceState>::const_iterator spaceit;
  
  for (spaceit = m_SpaceStates.begin() ; spaceit != m_SpaceStates.end() ; ++spaceit)
  {
    const SpaceState &sstate = spaceit->second;

    if (sstate.IsDominated())
    {
      i_egen.UnicastDOMINANCE(i_Player,sstate.GetName(),sstate.GetDomination().GetCapital(),
                              sstate.GetDomination().GetAgeString());
    }
    else
    {
      size_t i;
      for (i = 0 ; i < sstate.GetNumTokens() ; ++i)
      {
        i_egen.UnicastTOKEN(i_Player,sstate.GetName(),i,sstate[i].GetCapital(),sstate[i].GetAgeString());
      }
    }
  }
}

// this unicasts to the specified player all the Commodity info
// can assume the board was reset prior to the call
void BoardState::SendAllCommodityLogInfo(const AOREventGenerator &i_egen,
                                         const std::string &i_Player,
                                         const Players &i_PlayerList,
                                         size_t i_numCapitals,
                                         const CommodityInfo &i_CommodityInfo) const
{
  BoardCounter bcount(*this,i_PlayerList);

  // for each player in the game.
  for (size_t playerindex = 0 ; playerindex < i_numCapitals ; ++playerindex)
  {
    const std::string &pCap = i_PlayerList[playerindex].GetCapital();
    // for each commodity of the space.
    const std::map<std::string,Commodity *> &comms = i_CommodityInfo.GetCommodities();
    std::map<std::string,Commodity *>::const_iterator comit;
    for (comit = comms.begin() ; comit != comms.end() ; ++comit)
    {
      const Commodity *pComm = comit->second;
      if (bcount.GetNumSpaces(pComm->GetName(),pCap) == 0) continue;
      i_egen.BroadcastCOMMODITYNUM(pComm->GetName(),
                                   pCap,
                                   bcount.GetNumSpaces(pComm->GetName(),
                                                       pCap),
                                   bcount.GetNumNewSpaces(pComm->GetName(),
                                                          pCap));
    }
    // and the totals for each player.
    i_egen.BroadcastCOMMODITYNUM("Total",
                                 pCap,
                                 bcount.GetNumSpaces(pCap),
                                 bcount.GetNumNewSpaces(pCap));
    
  }
  
}
