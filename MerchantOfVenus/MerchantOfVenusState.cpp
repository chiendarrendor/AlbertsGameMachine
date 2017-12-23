#include "MerchantOfVenusState.hpp"
#include "MerchantOfVenusSet.hpp"

MerchantOfVenusState::MerchantOfVenusState(const MerchantOfVenusSet &i_Set) :
  m_winmoney(2000),
  m_Set(i_Set),
  m_Players(),
  m_cup(),
  m_mapoverlay(m_Set.GetMapData()),
  m_tradebases(m_Set.GetMapData()),
  m_movemediator(m_Players,m_mapoverlay)
{
}

const MerchantOfVenusSet &MerchantOfVenusState::GetMerchantOfVenusSet() const 
{ return m_Set; }
Players &MerchantOfVenusState::GetPlayers() { return m_Players; }
const Players &MerchantOfVenusState::GetPlayers() const { return m_Players; }
Cup& MerchantOfVenusState::GetCup() { return m_cup; }
MapOverlay& MerchantOfVenusState::GetMapOverlay() { return m_mapoverlay; }
const MapOverlay& MerchantOfVenusState::GetMapOverlay() const { return m_mapoverlay; }
TradeBases& MerchantOfVenusState::GetTradeBases() { return m_tradebases; }
const TradeBases& MerchantOfVenusState::GetTradeBases() const { return m_tradebases; } 
MoveMediator& MerchantOfVenusState::GetMoveMediator() { return m_movemediator; }
const MoveMediator& MerchantOfVenusState::GetMoveMediator() const { return m_movemediator; }


int MerchantOfVenusState::GetIndexOfAppropriateIOU() const
{
  const Player& curp = GetPlayers().GetCurPlayer();
  // a) solar system of player.
  const std::pair<MapSpace,Secret> curpair = 
    GetMapOverlay().GetMapSpace(curp.GetLocation());
  const std::string& ss = curpair.first.m_regionname;

  // b) get race id of trade base.
  std::map<std::string,TradeBase>::const_iterator cit;
  cit = GetTradeBases().GetBaseMap().find(ss);
  const std::string& race = cit->second.GetId();
 
  // see if we can find an IOU with that race id.
  for (size_t i = 0 ; i < curp.GetTokens().size() ; ++i)
  {
    if (curp.GetTokens()[i].GetSizeClass() == IOU_SIZE &&
        curp.GetTokens()[i].GetBuyers()[0] == race)
    {
      return i;
    }
  }
  return -1;
}

bool MerchantOfVenusState::HasRemainingBuy() const
{
  if (GetMoveMediator().IsFirst()) return true;
  if (GetMapOverlay().GetMapSpace(GetPlayers().GetCurPlayer().GetLocation()).first.m_type == STATION) return true;
  return GetPlayers().GetCurPlayer().GetBuyCount() == 0;
}

bool MerchantOfVenusState::HasRemainingSell() const
{
  if (GetMoveMediator().IsFirst()) return true;
  if (GetMapOverlay().GetMapSpace(GetPlayers().GetCurPlayer().GetLocation()).first.m_type == STATION) return true;
  return GetPlayers().GetCurPlayer().GetSellCount() == 0;
}
