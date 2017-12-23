#include "Serialize.hpp"
#include "Factories.hpp"
#include "OwnedItems.hpp"
#include "CommodityHand.hpp"



class Player
{
public:
  Player();
  Player(const std::string &i_Name,int i_Id);
  Player(const Player &i_Player);
  Player &operator=(const Player &i_Player);

  int GetId() const;
  void SetId(int i_Id);

  const std::string &GetName() const;
  bool OkStartup() const;
  void SetOkStartup(bool i_okstartup);

  int GetVP() const;

  const Factories &GetFactories() const;
  Factories &GetFactories();

  const OwnedItems &GetOwnedItems() const;
  OwnedItems &GetOwnedItems();

  const CommodityHand &GetCommodityHand() const;
  CommodityHand &GetCommodityHand();

  int GetHandLimit(bool i_IsEarlyDiscard) const;

  int GetNumMannedFactories(CommodityType i_type) const;
  int GetNumUnmannedFactories(CommodityType i_type) const;

  // alters the players' factory manning so that the farthest right
  // factories are manned, and if possible, using humans.
  void AutoMan();

  PurchaseState GetPurchaseState() const;
  BidState GetBidState() const;
  
  void SetPurchaseState(PurchaseState i_ps);
  void SetBidState(BidState i_bs);

  bool HasPurchased() const;
  void SetHasPurchased(bool i_purchasestate);

  // returns true if and only if the player has purchased
  // and humans, robots, items, or factories.
  bool HasPurchasedAnything() const;

  // returns the number of this item that
  // the player can purchase, given resources and items.
  int GetMaxPurchasable(PurchasableType i_pt,RobotMechanism i_mechanism) const;
  int GetMaxHumans() const;
  int GetMaxRobots(RobotMechanism i_mechanism) const;

  int GetNumRobots() const;
  int GetNumHumans() const;
  void AddRobots(int i_rcount);
  void AddHumans(int i_hcount);

  int GetHighBid() const;
  void SetHighBid(int i_highbid);

  int GetLastBid() const;
  void SetLastBid(int i_lastbid);


private:

  SERIALIZE_FUNC
  {
    SERIALIZE(m_Name);
    SERIALIZE(m_Id);
    SERIALIZE(m_okStartup);
    SERIALIZE(m_Factories);
    SERIALIZE(m_OwnedItems);
    SERIALIZE(m_CommodityHand);
    SERIALIZE(m_PurchaseState);
    SERIALIZE(m_BidState);
    SERIALIZE(m_HasPurchased);
    SERIALIZE(m_NumRobots);
    SERIALIZE(m_NumHumans);
    SERIALIZE(m_HighBid);
    SERIALIZE(m_LastBid);
  }

  std::string m_Name;
  int m_Id;
  bool m_okStartup;

  Factories m_Factories;
  OwnedItems m_OwnedItems;
  CommodityHand m_CommodityHand;

  PurchaseState m_PurchaseState;
  BidState m_BidState;

  bool m_HasPurchased;

  int m_NumRobots;
  int m_NumHumans;
  int m_HighBid;
  int m_LastBid;
};

