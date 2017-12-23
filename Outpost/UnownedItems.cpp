#include <stdexcept>
#include "UnownedItems.hpp"
#include "Utilities.hpp"
#include "Player.hpp"
#include <iostream>

UnownedItems::UnownedItems() :
  m_ItemCount(),
  m_SupplyShip(),
  m_Purchases(),
  m_BidIndex(NO_BID_INDEX)
{
}

bool UnownedItems::Initialized() const
{
  return m_SupplyShip.size() > 0;
}

void UnownedItems::Initialize(int i_NumPlayers,StockMechanism i_mechanism)
{
  m_ItemCount = std::vector<int>(14);
  m_SupplyShip = std::vector<ItemType>(i_NumPlayers);
  m_Purchases = std::vector<PurchaseInfo>(i_NumPlayers);

  int i;

  if (i_mechanism == NORMAL_STOCK) FillNormalStock(i_NumPlayers);
  else if (i_mechanism == SOME_ONE_EXTRA) FillExtraStock(i_NumPlayers);
  else throw std::out_of_range("Unknown Stock Mechanism!");

  for (i = 0 ; i < i_NumPlayers ; ++i)
  {
    m_SupplyShip[i] = NO_ITEM;
  }
  ClearPurchases();
}

void UnownedItems::FillNormalStock(int i_NumPlayers)
{
  int i;

  for(i = 1 ; i <= 13 ; ++i)
  {
    m_ItemCount[i] = i_NumPlayers-1;
  }
}

void UnownedItems::FillExtraStock(int i_NumPlayers)
{
  int extracount = 0;

  while (extracount < 4 || extracount > 10)
  {
    extracount = 0;
    int i;
    for (i = 1 ; i <= 13 ; ++i)
    {
      int ct = i_NumPlayers - myrand(2);
      m_ItemCount[i] = ct;
      if (ct == i_NumPlayers) extracount++;
    }
  }
}

int UnownedItems::GetItemCount(ItemType i_item) const
{
  if ((int)i_item < 1 || (int)i_item > 13)
  {
    throw std::out_of_range("bad item count");
  }

  return m_ItemCount[(size_t)i_item];
}

void UnownedItems::DecrementItemCount(ItemType i_item)
{
  if ((int)i_item < 1 || (int)i_item > 13)
  {
    throw std::out_of_range("bad item count");
  }

  m_ItemCount[(size_t)i_item]--;
}

ItemType UnownedItems::GetShipItem(size_t i_index) const
{
  if (i_index < 0 || i_index >= m_SupplyShip.size())
  {
    throw std::out_of_range("bad ship count");
  }
  return m_SupplyShip[i_index];
}

void UnownedItems::SetShipItem(size_t i_index,ItemType i_item)
{
  if (i_index < 0 || i_index >= m_SupplyShip.size())
  {
    throw std::out_of_range("bad ship count");
  }
  m_SupplyShip[i_index] = i_item;
}

bool UnownedItems::InPhase(int i_phase,bool i_harshsupply,size_t i_itemindex) const
{
  PhaseLimits pl = GetPhaseLimits(i_phase,i_harshsupply);
  return (i_itemindex >= (size_t)pl.m_phasebottom && i_itemindex <= (size_t)pl.m_phasetop);
}

PhaseLimits UnownedItems::GetPhaseLimits(int i_phase,bool i_harshsupply) const
{
  PhaseLimits result;

  switch(i_phase)
  {
  case 1:
    result.m_phasebottom = 1;
    result.m_rollbottom = 1;
    result.m_phasetop = 4;
    break;
  case 2:
    result.m_phasebottom = 1;
    result.m_rollbottom = 1;
    result.m_phasetop = 10;
    break;
  case 3:
    result.m_phasebottom = i_harshsupply ? 2 : 1;
    result.m_rollbottom = 2;
    result.m_phasetop = 13;
    break;
  default:
    throw std::out_of_range("Illegal phase for FillShip");
  }
  return result;
}

void UnownedItems::FillShip(int i_phase,int i_supplyharshness)
{
  int nplayers = m_SupplyShip.size();
  int maxcount = nplayers / 2;
  int phase = i_phase;
  bool harshsupply = (i_supplyharshness != 0);
  size_t i;

  PhaseLimits pl = GetPhaseLimits(i_phase,harshsupply);

  // m_ItemCount[x] contains the count of available item x.
  // maxcount is the maximum number of any one item that can be on the ship
  // curitemcount[x] contains the count of item x on board the ship.
  std::vector<int> curitemcount(14);
  for (i = 0 ; i < m_SupplyShip.size() ; ++i)
  {
    if (m_SupplyShip[i] != NO_ITEM)
    {
      curitemcount[m_SupplyShip[i]]++;
      if (curitemcount[m_SupplyShip[i]] > maxcount)
      {
        throw std::out_of_range("Already too many of this item!");
      }
    }
  }

  size_t slotidx = 0;
  while(slotidx < m_SupplyShip.size())
  {
    // if the slot already has an item, we leave it.
    if (m_SupplyShip[slotidx] != NO_ITEM)
    {
      slotidx++;
      continue;
    }

    // one (very rare) wierd case:
    // in phase 3, not harsh, in order to get to 
    // item 1 at all, item 2 must be zero. if item 2 is nonzero,
    // even if the ship already has max # of these, we can't
    // count the item 1's
    int checkbottom = pl.m_phasebottom;
    if (phase == 3 && harshsupply == false && m_ItemCount[2] > 0) checkbottom++;

    // try to find at least one item that we can get.
    int chkidx;
    for (chkidx = checkbottom ; chkidx <= pl.m_phasetop ; ++chkidx)
    {
      if (m_ItemCount[chkidx] > 0 &&
          curitemcount[chkidx] < maxcount) break;
    }
    
    // if this hits, there are no items that can be put on the ship right now.
    if (chkidx > pl.m_phasetop) break;

    // if we get here, we know that we can get an item.

    if (harshsupply)
    {
      // the harsh way: if the one we roll can't go in, we skip it 
      // (after i_supplyharshness tries)
      int scount;
      for (scount = 0 ; scount < i_supplyharshness ; ++scount)
      {
        int rollidx = RandomBetween(pl.m_rollbottom,pl.m_phasetop);
        
        if (m_ItemCount[rollidx] > 0 && curitemcount[rollidx] < maxcount)
        {
          curitemcount[rollidx]++;
          m_ItemCount[rollidx]--;
          m_SupplyShip[slotidx] = (ItemType)rollidx;
          break;
        }
      }
    }
    else
    {
      int rollidx = RandomBetween(pl.m_rollbottom,pl.m_phasetop);
      // ok...if we're here, we do it the nice way.
      while(rollidx > 0 && m_ItemCount[rollidx] == 0) rollidx--;
      if (rollidx == 0) continue;
      if (curitemcount[rollidx] == maxcount) continue;

      // ok...if we get here, we know that rollidx points to an
      // item that has spare items and that isn't overduplicated in the ship.

      curitemcount[rollidx]++;
      m_ItemCount[rollidx]--;
      m_SupplyShip[slotidx] = (ItemType)rollidx;
    }
    slotidx++;
  }
}

std::set<size_t> UnownedItems::GetNonEmptyShipIndices() const
{
  std::set<size_t> result;
  size_t i;

  for (i = 0 ; i < m_SupplyShip.size() ; ++i)
  {
    if (m_SupplyShip[i] != NO_ITEM) result.insert(i);
  }
  return result;
}

size_t UnownedItems::GetBidIndex() const
{
  return m_BidIndex;
}

void UnownedItems::SetBidIndex(size_t i_index)
{
  m_BidIndex = i_index;
}

bool UnownedItems::CanPlayerAffordItem(const Player &i_player, size_t i_index) const
{
  if (i_index == ANY_SHIP_SLOT)
  {
    size_t i;
    for (i = 0 ; i < m_SupplyShip.size() ; ++i)
    {
      if (CanPlayerAffordItem(i_player,i)) return true;
    }
    return false;
  }
  
  if (i_index >= m_SupplyShip.size()) 
  {
    throw std::out_of_range("Illegal index for CanPlayerAffordItem");
  }

  if (m_SupplyShip[i_index] == NO_ITEM) return false;

  ItemType itype = m_SupplyShip[i_index];

  return GetCost(itype) <=
    i_player.GetCommodityHand().GetHandValue() + 
    i_player.GetOwnedItems().GetDiscount(itype);
}

void UnownedItems::ClearPurchases()
{
  size_t i;
  for (i = 0 ; i < m_Purchases.size() ; ++i)
  {
    m_Purchases[i].m_playerindex = NO_PLAYER_INDEX;
    m_Purchases[i].m_type = NO_ITEM;
    m_Purchases[i].m_cost = -1;
  }
}

void UnownedItems::SetPurchased(size_t i_index,ItemType i_type,size_t i_playerindex,int i_cost)
{
  if (i_index >= m_Purchases.size()) throw std::out_of_range("Bad Index for Purchased");
  if (i_playerindex >= m_Purchases.size()) throw std::out_of_range("Bad Player Index for Purchased");

  m_Purchases[i_index].m_playerindex = i_playerindex;
  m_Purchases[i_index].m_type = i_type;
  m_Purchases[i_index].m_cost = i_cost;

}

const PurchaseInfo& UnownedItems::GetPurchaseInfo(size_t i_index) const
{
  if (i_index >= m_Purchases.size()) throw std::out_of_range("Bad Index for Purchased");

  return m_Purchases[i_index];
}


