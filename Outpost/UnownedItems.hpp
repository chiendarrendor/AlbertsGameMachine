#include "Serialize.hpp"
#include "Items.hpp"

class Player;


// m_phasebottom is the lowest numbered item that the phase can put into the game
// m_rollbottom is the lowest rollable number for the phase
// m_phasetop is the highest numbered item that can be put into the game (and also
//   the highest number that can be rolled)
struct PhaseLimits
{
  int m_phasebottom;
  int m_phasetop;
  int m_rollbottom;
};

struct PurchaseInfo
{
  size_t m_playerindex;
  ItemType m_type;
  int m_cost;

  SERIALIZE_FUNC
  {
    SERIALIZE(m_playerindex);
    SERIALIZE(m_type);
    SERIALIZE(m_cost);
  }
};

class UnownedItems : public Items
{
public:
  UnownedItems();
  void Initialize(int i_NumPlayers,StockMechanism i_mechanism = NORMAL_STOCK);
  bool Initialized() const;

  int GetItemCount(ItemType i_item) const;
  void DecrementItemCount(ItemType i_item);

  ItemType GetShipItem(size_t i_index) const;
  void SetShipItem(size_t i_index,ItemType i_item);
  std::set<size_t> GetNonEmptyShipIndices() const;

  void ClearPurchases();
  void SetPurchased(size_t i_index,ItemType i_type,size_t i_playerindex,int i_cost);
  const PurchaseInfo &GetPurchaseInfo(size_t i_index) const;


  bool InPhase(int i_phase,bool i_harshsupply,size_t i_itemindex) const;

  PhaseLimits GetPhaseLimits(int i_phase,bool i_harshsupply) const;
  void FillShip(int i_phase, int i_supplyharshness);

  size_t GetBidIndex() const;
  void SetBidIndex(size_t i_index);

  // returns true if the given player can afford the item in the ship
  // with the given index (returns false if the slot is empty or invalid)
  // if the index is ANY_SHIP_SLOT, will return true if the player can afford
  // any item in the ship.
  // 
  bool CanPlayerAffordItem(const Player &i_player,size_t i_index) const;

private:
  std::vector<int> m_ItemCount;
  std::vector<ItemType> m_SupplyShip;
  std::vector<PurchaseInfo> m_Purchases;

  size_t m_BidIndex;

  void FillNormalStock(int i_NumPlayers);
  void FillExtraStock(int i_NumPlayers);

  SERIALIZE_FUNC
  {
    SERIALIZE(m_ItemCount);
    SERIALIZE(m_SupplyShip);
    SERIALIZE(m_Purchases);
    SERIALIZE(m_BidIndex);
  }
};

