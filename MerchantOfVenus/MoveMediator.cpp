
#include "MoveMediator.hpp"
#include "Players.hpp"
#include "MapOverlay.hpp"
#include <sstream>
#include "Utilities.hpp"

namespace
{
  void InPlaceIntersect(std::set<size_t>& place, const std::set<size_t>& other)
  {
    std::set<size_t>::iterator it1 = place.begin();
    std::set<size_t>::const_iterator it2 = other.begin();
    while(it1 != place.end() && it2 != other.end())
    {
      if (*it1 < *it2) place.erase(it1++);
      else if (*it2 < *it1) ++it2;
      else { ++it1; ++it2; }
    }
    place.erase(it1,place.end());
  }
}



MoveMediator::MoveMediator(Players& i_players,MapOverlay& i_mapoverlay) :
  m_players(i_players),
  m_mapoverlay(i_mapoverlay),
  m_dests(),
  m_canmulligan(false),
  m_dicerolled(),
  m_pnumbers(),
  m_pnumber(0),
  m_first(false),
  m_mp(0),
  m_backs(),
  m_dicevisible(false),
  m_destination(""),
  m_destisjump(false)
{
}

void MoveMediator::AddToBacks(const std::string& i_sourcename,
                              const std::string i_destname)
{
  std::ostringstream oss;
  oss << i_destname << '/' << i_sourcename;
  m_backs.insert(oss.str());
}

bool MoveMediator::IsBackTracking(const std::string& i_sourcename,
                                  const std::string i_destname) const
{
  std::ostringstream oss;
  oss << i_sourcename << '/' << i_destname;
  return m_backs.find(oss.str()) != m_backs.end();
}


void MoveMediator::StartMove()
{ 
  Player& cp = m_players.GetCurPlayer();
  m_canmulligan = cp.GetAdvances().HasMulliganGear();
  m_pnumber = 0;
  m_first = true;
  m_dicevisible = false;
  m_backs.clear();


  const Token& ship = cp.GetShip();
  size_t numdice = ship.GetSpeed();
  m_dicerolled.clear();
  m_pnumbers.clear();
  m_mp = 0;
  
  if (cp.GetAdvances().HasAutoPilot())
  {
    --numdice;
    m_dicerolled.push_back(4);
  }

  for (size_t i = 0 ; i < numdice ; ++i)
  {
    m_dicerolled.push_back(DieRoll());
  }
  
  for (size_t i = 0 ; i < m_dicerolled.size() ; ++i)
  {
    m_mp += m_dicerolled[i];
    m_pnumbers.insert(m_dicerolled[i]);
  }
}

std::string MoveMediator::GetCurrentDests() const
{    
  Player& cp = m_players.GetCurPlayer();
  const std::pair<MapSpace,Secret> curpair = 
    m_mapoverlay.GetMapSpace(cp.GetLocation());

  std::vector<Dest>::const_iterator dit;
  bool first = true;
  std::ostringstream oss;
  for (dit = m_dests.begin() ; dit != m_dests.end() ; ++dit)
  {
    if (first) first = false;
    else oss << '/';

    oss << dit->sname;
    oss << '!';
    oss << (dit->isjump ? "jump" : "walk");
    oss << '!';
    oss << dit->mpused;
    if (dit->pnumbers.size() > 0)
    {
      oss << '!';
      std::set<size_t>::const_iterator pnit;
      for (pnit = dit->pnumbers.begin() ; 
           pnit != dit->pnumbers.end() ; ++pnit)
      {
        oss << *pnit;
      }
    }
  }
  return oss.str();
}

    
void MoveMediator::ClearFirst()
{
  m_first = false;
}

const std::vector<int>& MoveMediator::GetDice() const
{
  return m_dicerolled;
}

std::string MoveMediator::GetDiceString() const
{
  std::ostringstream oss;
  std::vector<int>::const_iterator dit;
  for (dit = GetDice().begin() ; dit != GetDice().end() ; ++dit)
  {
    if (m_dicevisible)
    {
      oss << *dit;
    }
    else
    {
      oss << '?';
    }
  }
  return oss.str();
}

int MoveMediator::GetMP() const
{
  return m_mp;
}

bool MoveMediator::CanMulligan() const
{
  return m_canmulligan;
}

void MoveMediator::ClearMulligan()
{
  m_canmulligan = false;
}

void MoveMediator::MakeDiceVisible()
{
  m_dicevisible = true;
}

void MoveMediator::SetPilotNumber(int i_pnum)
{
  m_pnumber = i_pnum;
}

int MoveMediator::GetPilotNumber() const
{
  return m_pnumber;
}

bool MoveMediator::IsPilotNumberPermanent() const
{
  Player& cp = m_players.GetCurPlayer();
  if (m_pnumber == 0) return false;
  return !cp.GetAdvances().HasSwitchSwitch();
}


void MoveMediator::SpendMP(int i_decrement)
{
  m_mp -= i_decrement;
}

bool MoveMediator::IsFirst() const
{
  return m_first;
}

bool MoveMediator::AreDiceVisible() const
{
  return m_dicevisible;
}


bool MoveMediator::AutoStop() const
{
  return m_dests.size() == 0 ||
    (!IsFirst() && GetPenaltyCost() > m_players.GetCurPlayer().GetMoney());
}

const std::vector<Dest>& MoveMediator::GetDests() const
{
  return m_dests;
}

bool MoveMediator::ManualStop() const
{
  if (IsFirst()) 
  {
    if (GetPenaltyCost() > m_players.GetCurPlayer().GetMoney()) return false;
    return true;
  }

  // if one of the legal dests with pnums was removed due to backtrack, we can stil take it
  // and therefore stop
  if (m_backtrackingpnum) return true;

  const std::pair<MapSpace,Secret> spacepair = 
    m_mapoverlay.GetMapSpace(m_players.GetCurPlayer().GetLocation());
  if (spacepair.first.m_type == DOT) return false;
  if (spacepair.first.m_type == TELEGATE) return false;
  return true;
}

void MoveMediator::SetDestination(const std::string& i_dest,bool i_isjump)
{
  m_destination = i_dest;
  m_destisjump = i_isjump;
}

bool MoveMediator::ValidateDestination() const
{
  std::vector<Dest>::const_iterator dit;
  for (dit = m_dests.begin() ; dit != m_dests.end() ; ++dit)
  {
    if (dit->sname == m_destination &&
        dit->isjump == m_destisjump) return true;
  }
  return false;
}

const Dest& MoveMediator::GetChosenDest() const
{
  std::vector<Dest>::const_iterator dit;
  for (dit = m_dests.begin() ; dit != m_dests.end() ; ++dit)
  {
    if (dit->sname == m_destination &&
        dit->isjump == m_destisjump) return *dit;
  }
  THROW(std::runtime_error,"GetChosenDest() called on unvalidated destination!");
}




bool MoveMediator::DoMulligan(int i_rolltoreplace)
{
  std::vector<int>::iterator dit;
  for (dit = m_dicerolled.begin() ; dit != m_dicerolled.end() ; ++dit)
  {
    if (*dit == i_rolltoreplace) break;
  }
  if (dit == m_dicerolled.end()) return false;

  *dit = DieRoll();

  m_pnumbers.clear();
  m_mp = 0;

  for (size_t i = 0 ; i < m_dicerolled.size() ; ++i)
  {
    m_mp += m_dicerolled[i];
    m_pnumbers.insert(m_dicerolled[i]);
  }

  return true;
}


int MoveMediator::GetPenaltyCost() const
{
  int defense = m_players.GetCurPlayer().GetAdvances().GetShieldStrength();
  const std::pair<MapSpace,Secret> spacepair =
    m_mapoverlay.GetMapSpace(m_players.GetCurPlayer().GetLocation());
  int offense = spacepair.first.m_cost;
  if (spacepair.first.m_color == RED &&
      m_players.GetCurPlayer().GetAdvances().SkipsRed()) offense = 0;
  if (spacepair.first.m_color == YELLOW &&
      m_players.GetCurPlayer().GetAdvances().SkipsYellow()) offense = 0;

  int result = offense - defense;
  if (result < 0) result = 0;
  return result;
}

void MoveMediator::AddAdjacents()
{
  m_dests.clear();
  bool canfly = m_players.GetCurPlayer().GetAdvances().HasAirFoil();
  const std::pair<MapSpace,Secret> curpair = 
    m_mapoverlay.GetMapSpace(m_players.GetCurPlayer().GetLocation());

  std::vector<std::pair<std::string,bool> >::const_iterator adjit;
  int idx = 0;
  for (adjit = curpair.first.m_adjacentnames.begin() ; 
       adjit != curpair.first.m_adjacentnames.end() ; ++adjit,++idx)
  {
    if (!canfly && adjit->second) 
    {
      continue;
    }
    Dest d;
    d.sname = adjit->first;
    d.isjump = false;
    d.mpused = 0;
    for (size_t i = 1 ; i <= 6 ; ++i)
    {
      if (curpair.first.m_pilotdirs[i] == idx) 
        d.pnumbers.insert(i);
    }
    m_dests.push_back(d);
  }
}

void MoveMediator::AddTelegates()
{
  const std::pair<MapSpace,Secret> curpair = 
    m_mapoverlay.GetMapSpace(m_players.GetCurPlayer().GetLocation());
  const MapSpace& curspace = curpair.first;
  bool js = m_players.GetCurPlayer().GetAdvances().HasJumpStart();
  bool gl = m_players.GetCurPlayer().GetAdvances().HasGateLock();

  // a) determine if we have any changes to make
  if (!((curspace.m_type == TELEGATE && !gl) || (js && m_first)))
  {
    return;
  }

  // b) make the set of jump dests, and determine
  //    the complement set of pilot #'s
  std::vector<Dest> jumpdests;
  std::set<size_t> compset;
  compset.insert(1);compset.insert(2);compset.insert(3);
  compset.insert(4);compset.insert(5);compset.insert(6);
  for (size_t pnum = 1 ; pnum <= 6 ; ++pnum)
  {
    const MapSpace *pSpace = m_mapoverlay.GetTeleGate(pnum);
    if (!pSpace)
    {
      continue;
    }
    if (pSpace->m_name == curspace.m_name)
    {
      continue;
    }
    
    Dest d;
    d.sname = pSpace->m_name;
    d.isjump = true;
    d.mpused = 0;
    d.pnumbers.insert(pnum);
    jumpdests.push_back(d);
    compset.erase(pnum);
  }
  // b') if we generated no jump dests, we're done
  if (jumpdests.size() == 0) return;

  // c) take the set of existing Dests, and either
  // 1) if it has pnums, intersect their pnums with the compset
  // 2) if it has no pnums, set their pnums to compset
  // either way, if a Dest ends up with no pnums, drop it.
  std::vector<Dest> twalk = m_dests;
  m_dests.clear();
  std::vector<Dest>::iterator dit;
  for (dit = twalk.begin() ; dit != twalk.end() ; ++dit)
  {
    if (compset.size() == 0) continue;
    if (dit->pnumbers.size() == 0)
    {
      dit->pnumbers = compset;
    }
    else
    {
      // in-place intersect of pnumbers and compset
      InPlaceIntersect(dit->pnumbers,compset);
    }
    // only if it it still has pilot numbers, put it back
    if (dit->pnumbers.size() != 0) m_dests.push_back(*dit);
  }
  m_dests.insert(m_dests.end(),jumpdests.begin(),jumpdests.end());
}


// remove all destinations that require backtracking
// however, a player can legally choose a pilot number that
// backtracks, which allows the player to stop there.  This is 
// the 'm_backtrackingpnum' boolean.
// however, if there is more than one instance of a given pnum,
// only one of which is removed,
// (a non-jump exit from a telegate)
// the player is not allowed to use that as an excuse to stop.  
void MoveMediator::CullBacktracks()
{
  std::vector<Dest> tdests = m_dests;
  m_dests.clear();
  m_backtrackingpnum = false;
  std::map<size_t,int> pnumcounts;
  
  std::vector<Dest>::iterator dit;
  for (dit = tdests.begin() ; dit != tdests.end() ; ++dit)
  {
    std::set<size_t>::const_iterator cit;
    for (cit = dit->pnumbers.begin() ; cit != dit->pnumbers.end(); ++cit)
    {
      pnumcounts[*cit]++;
    }

    if (!dit->isjump && IsBackTracking(m_players.GetCurPlayer().GetLocation(),dit->sname)) 
    {
      if (dit->pnumbers.size() >0 )
      {
        for (cit = dit->pnumbers.begin() ; cit != dit->pnumbers.end(); ++cit)
        {
          pnumcounts[*cit]--;
        }
      }
      continue;
    }
    m_dests.push_back(*dit);
  }

  // each record of pnumcounts has been added to if there is a dest for that pilot number
  // that count has been subtracted if a dest for that pilot number is removed.
  // if a count is 0, then that pilot number could be used for a stall and backtrackingpnum must be set.
  std::map<size_t,int>::iterator mit;
  for (mit = pnumcounts.begin() ; mit != pnumcounts.end() ; ++mit)
  {
    if (mit->second == 0) m_backtrackingpnum = true;
  }
}

void MoveMediator::CullByPilotNumber()
{
  if (!m_dicevisible) 
  {
    return;
  }
  std::vector<Dest> tdests = m_dests;
  m_dests.clear();

  std::set<size_t> activepnum;
  if (m_pnumber == 0 || m_players.GetCurPlayer().GetAdvances().HasSwitchSwitch())
  {
    activepnum = m_pnumbers;
  }
  else
  {
    activepnum.insert(m_pnumber);
  }

  std::vector<Dest>::iterator dit;
  for (dit = tdests.begin() ; dit != tdests.end() ; ++dit)
  {
    if (dit->pnumbers.size() == 0)
    {
      m_dests.push_back(*dit);
      continue;
    }
    InPlaceIntersect(dit->pnumbers,activepnum);
    if (dit->pnumbers.size() > 0)
    {
      m_dests.push_back(*dit);
    }
  }
}

void MoveMediator::ApplyMovementPoints()
{
  std::vector<Dest> tdests = m_dests;
  m_dests.clear();

  const std::pair<MapSpace,Secret> opair = 
    m_mapoverlay.GetMapSpace(m_players.GetCurPlayer().GetLocation());

  bool sourceIsCity = opair.first.m_type == CITY;

  std::vector<Dest>::iterator dit;
  for (dit = tdests.begin() ; dit != tdests.end() ; ++dit)
  {
    dit->mpused = 1;
    const std::pair<MapSpace,Secret> dpair = m_mapoverlay.GetMapSpace(dit->sname);
    if (dpair.first.m_color == YELLOW &&
        m_players.GetCurPlayer().GetAdvances().SkipsYellow() &&
        dpair.second.PlayerKnowsSecret(m_players.GetCurPlayer().GetId()))
    {
      dit->mpused = 0;
      if (m_mp < 1) continue;
    }

    if (dpair.first.m_color == RED &&
        m_players.GetCurPlayer().GetAdvances().SkipsRed() &&
        dpair.second.PlayerKnowsSecret(m_players.GetCurPlayer().GetId()))
    {
      dit->mpused = 0;
      if (m_mp < 1) continue;
    }

    if ((sourceIsCity || dpair.first.m_type == CITY) && 
        !m_players.GetCurPlayer().GetAdvances().HasAirFoil())
    {
      dit->mpused = 2;
      if (m_mp < 2) continue;
    }

    if (m_mp < 1) continue;

    m_dests.push_back(*dit);
  }
}
  






void MoveMediator::PrepareForStep()
{
  AddAdjacents();
  AddTelegates();
  CullByPilotNumber();
  CullBacktracks();
  ApplyMovementPoints();
}


