#include "ProductionManager.hpp"
#include "CommodityManager.hpp"
#include "Players.hpp"
#include <stdexcept>

const int UNKNOWN_MEGA = -1;

ProductionManager::ProductionManager() :
  m_CurCommodity(NO_COMMODITY),
  m_FactoryCount(0)
{
}

ProductionManager::~ProductionManager()
{
}

bool ProductionManager::IsProductionDone() const
{
  return m_CurCommodity == NO_COMMODITY;
}

InputInfo ProductionManager::GetMegaInfo() const
{
  InputInfo result;
  result.m_Commodity = m_CurCommodity;
  result.m_MaxMegas = m_FactoryCount / 4;

  return result;
}

void ProductionManager::IncrementIndex(Players &i_Players)
{
  i_Players.IncrementTurnOrder();
  if (i_Players.AllPlayersDone())
  {
    m_CurCommodity = (CommodityType)((int)m_CurCommodity - 1);
    if (!IsProductionDone()) i_Players.StartTurnOrder();
  }
}

void ProductionManager::StartProduction(Players &i_Players,CommodityManager &i_comms,bool i_IsFirstTurn,bool i_refineries)
{
  i_Players.StartTurnOrder();
  m_CurCommodity = MOON_ORE_COMMODITY;


  while(!IsProductionDone())
  {
    if (!ProcessOne(i_Players.GetCurTurnPlayer(),i_comms,i_IsFirstTurn,i_refineries,UNKNOWN_MEGA)) break;
    
    IncrementIndex(i_Players);
  }
}

void ProductionManager::ContinueProduction(int i_NumMegas,
					   Players &i_Players,CommodityManager &i_comms,bool i_IsFirstTurn,bool i_refineries)
{
  ProcessOne(i_Players.GetCurTurnPlayer(),i_comms,i_IsFirstTurn,i_refineries,i_NumMegas);
  IncrementIndex(i_Players);

  while(!IsProductionDone())
  {
    if (!ProcessOne(i_Players.GetCurTurnPlayer(),i_comms,i_IsFirstTurn,i_refineries,UNKNOWN_MEGA)) break;
    
    IncrementIndex(i_Players);
  }
}

struct CommSorter
{
  bool operator()(const Commodity &i_left,const Commodity &i_right)
  {
    return i_left.GetValue() > i_right.GetValue();
  }
};



bool ProductionManager::ProcessOne(Player &i_Player,CommodityManager &i_comms,bool i_IsFirstTurn,bool i_refineries,int i_nummega)
{
  int i;
  CommodityDeck &deck = i_comms.GetDeck(m_CurCommodity);
  m_FactoryCount = i_Player.GetNumMannedFactories(m_CurCommodity);

  // if we don't know how many megas to apply to a player/commodity pair that could take them
  // (i.e. the deck has them, and the player has enough to possibly get one or more)
  // then we return false.

  if (deck.HasMega() && i_nummega == UNKNOWN_MEGA && m_FactoryCount >= 4)
  {
    return false;
  }

  // if we do know how many megas to apply in a mega situation, we draw them into the player's hand.
  // (we ignore i_nummega if the deck or the factory count is too small)
  if (deck.HasMega() && m_FactoryCount >= 4)
  {
    if (i_nummega * 4 > m_FactoryCount)
    {
      throw std::out_of_range("Too many megas!");
    }
    for (i = 0 ; i < i_nummega ; ++i)
    {
      i_Player.GetCommodityHand().AddCommodity(deck.DrawCommodity(true));
      m_FactoryCount -= 4;
    }
  }

  // ok....now we handle the normal case.
  // 1. figure out how many to draw and discard.
  int drawcount;
  int disccount;
  // special case, not that this will ever come up, but cannot mix refineries and first turn.
  if (i_IsFirstTurn)
  {
    drawcount = m_FactoryCount * 2;
    disccount = 0;
  }
  else
  {
    drawcount = m_FactoryCount;
    if (i_refineries)
    {
      int ucount = i_Player.GetNumUnmannedFactories(m_CurCommodity);
      if (drawcount > ucount) drawcount += ucount;
      else drawcount *= 2;
    }
    disccount = drawcount - m_FactoryCount;
  }

  // 2. draw them.
  std::vector<Commodity> draws;
  for (i = 0 ; i < drawcount ; ++i)
  {
    draws.push_back(deck.DrawCommodity(false));
  }

  // 3. sort them by value.
  std::sort(draws.begin(),draws.end(),CommSorter());

  // 4. put the best ones in the player's hand, and discard the rest.
  for (i = 0 ; i < drawcount ; ++i)
  {
    if (i < drawcount - disccount) i_Player.GetCommodityHand().AddCommodity(draws[i]);
    else deck.DiscardCommodity(draws[i]);
  }


  return true;
}

