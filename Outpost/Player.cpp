#include "Player.hpp"
#include <stdexcept>

Player::Player() :
  m_Name(""),
  m_Id(0),
  m_okStartup(false),
  m_Factories(),
  m_OwnedItems(),
  m_CommodityHand(),
  m_PurchaseState(DONE),
  m_BidState(PASS_OUT),
  m_HasPurchased(false),
  m_NumRobots(0),
  m_NumHumans(0),
  m_HighBid(NOT_HIGH_BID),
  m_LastBid(NOT_HIGH_BID)
{
}

Player::Player(const std::string &i_Name,int i_Id) : 
  m_Name(i_Name),
  m_Id(i_Id),
  m_okStartup(false),
  m_Factories(),
  m_OwnedItems(),
  m_CommodityHand(),
  m_PurchaseState(DONE),
  m_BidState(PASS_OUT),
  m_HasPurchased(false),
  m_NumRobots(0),
  m_NumHumans(0),
  m_HighBid(NOT_HIGH_BID),
  m_LastBid(NOT_HIGH_BID)
{
}

Player::Player(const Player &i_Player) 
{
  *this = i_Player;
}

Player &Player::operator=(const Player &i_Player)
{
  if (this != &i_Player)
  {
    m_Name = i_Player.m_Name;
    m_Id = i_Player.m_Id;
    m_okStartup = i_Player.m_okStartup;
    m_Factories = i_Player.m_Factories;
    m_OwnedItems = i_Player.m_OwnedItems;
    m_CommodityHand = i_Player.m_CommodityHand;
    m_PurchaseState = i_Player.m_PurchaseState;
    m_BidState = i_Player.m_BidState;
    m_HasPurchased = i_Player.m_HasPurchased;
    m_NumRobots = i_Player.m_NumRobots;
    m_NumHumans = i_Player.m_NumHumans;
    m_HighBid = i_Player.m_HighBid;
    m_LastBid = i_Player.m_LastBid;
  }
  return *this;
}

int Player::GetId() const
{
  return m_Id;
}

void Player::SetId(int i_Id)
{
  m_Id = i_Id;
}

const std::string &Player::GetName() const
{
  return m_Name;
}

bool Player::OkStartup() const
{
  return m_okStartup;
}

void Player::SetOkStartup(bool i_okstartup)
{
  m_okStartup = i_okstartup;
}

int Player::GetVP() const
{
  return m_OwnedItems.GetItemVPs() + m_Factories.GetVPs();
}


const Factories &Player::GetFactories() const
{
  return m_Factories;
}

Factories &Player::GetFactories()
{
  return m_Factories;
}

const OwnedItems &Player::GetOwnedItems() const
{
  return m_OwnedItems;
}

OwnedItems &Player::GetOwnedItems()
{
  return m_OwnedItems;
}

const CommodityHand &Player::GetCommodityHand() const
{
  return m_CommodityHand;
}

CommodityHand &Player::GetCommodityHand()
{
  return m_CommodityHand;
}


int Player::GetNumMannedFactories(CommodityType i_type) const
{
  switch(i_type)
  {
  case NO_COMMODITY:
    throw std::runtime_error("NO_COMMODITY in Player::GetNumMannedFactories()");
  case ORE_COMMODITY: 
    return m_Factories.GetMannedFactoryCount(ORE_FACTORY);
  case WATER_COMMODITY:
    return m_Factories.GetMannedFactoryCount(WATER_FACTORY);
  case TITANIUM_COMMODITY:
    return m_Factories.GetMannedFactoryCount(TITANIUM_FACTORY);
  case RESEARCH_COMMODITY:
    return 
      m_Factories.GetMannedFactoryCount(RESEARCH_FACTORY) +
      m_OwnedItems.GetResearchProduction();
  case MICROBIOTICS_COMMODITY:
    return m_OwnedItems.GetMicrobioticProduction();
  case NEW_CHEMICALS_COMMODITY:
    return m_Factories.GetMannedFactoryCount(NEW_CHEMICALS_FACTORY);
  case ORBITAL_MEDICINE_COMMODITY:
    return m_Factories.GetMannedFactoryCount(SPACE_STATION_FACTORY);
  case RING_ORE_COMMODITY:
    return m_Factories.GetMannedFactoryCount(PLANETARY_CRUISER_FACTORY);
  case MOON_ORE_COMMODITY:
    return m_Factories.GetMannedFactoryCount(MOON_BASE_FACTORY);
  }
  throw std::runtime_error("Unknown Commodity Type in Player::GetNumMannedFactories()");
}

int Player::GetNumUnmannedFactories(CommodityType i_type) const
{
  switch(i_type)
  {
  case NO_COMMODITY:
    throw std::runtime_error("NO_COMMODITY in Player::GetNumUnmannedFactories()");
  case ORE_COMMODITY: 
    return m_Factories.GetUnmannedFactoryCount(ORE_FACTORY);
  case WATER_COMMODITY:
    return m_Factories.GetUnmannedFactoryCount(WATER_FACTORY);
  case TITANIUM_COMMODITY:
    return m_Factories.GetUnmannedFactoryCount(TITANIUM_FACTORY);
  case RESEARCH_COMMODITY:
    return m_Factories.GetUnmannedFactoryCount(RESEARCH_FACTORY);
  case MICROBIOTICS_COMMODITY:
    return 0;
  case NEW_CHEMICALS_COMMODITY:
    return m_Factories.GetUnmannedFactoryCount(NEW_CHEMICALS_FACTORY);
  case ORBITAL_MEDICINE_COMMODITY:
    return m_Factories.GetUnmannedFactoryCount(SPACE_STATION_FACTORY);
  case RING_ORE_COMMODITY:
    return m_Factories.GetUnmannedFactoryCount(PLANETARY_CRUISER_FACTORY);
  case MOON_ORE_COMMODITY:
    return m_Factories.GetUnmannedFactoryCount(MOON_BASE_FACTORY);
  }
  throw std::runtime_error("Unknown Commodity Type in Player::GetNumUnmannedFactories()");
}

int Player::GetHandLimit(bool i_IsEarlyDiscard) const
{
  int base = i_IsEarlyDiscard ? 10 : 5;
  return base + GetOwnedItems().GetHandIncrease();
}
  
PurchaseState Player::GetPurchaseState() const
{
  return m_PurchaseState;
}

BidState Player::GetBidState() const
{
  return m_BidState;
}

void Player::SetPurchaseState(PurchaseState i_ps)
{
  m_PurchaseState = i_ps;
}

void Player::SetBidState(BidState i_bs)
{
  m_BidState = i_bs;
}

bool Player::HasPurchased() const
{
  return m_HasPurchased;
}

void Player::SetHasPurchased(bool i_purchasestate)
{
  m_HasPurchased = i_purchasestate;
}

int Player::GetNumRobots() const
{
  return m_NumRobots;
}

int Player::GetNumHumans() const
{
  return m_NumHumans;
}

void Player::AddHumans(int i_hcount)
{
  m_NumHumans += i_hcount;
}

void Player::AddRobots(int i_rcount)
{
  m_NumRobots += i_rcount;
}

int Player::GetMaxHumans() const
{
  return 5 + 
    GetOwnedItems().GetPeopleIncrease() +
    GetFactories().GetPeopleIncrease();
}

int Player::GetMaxRobots(RobotMechanism i_mechanism) const
{
  int nr = m_OwnedItems.GetNumRobotsPerPerson(i_mechanism);
  if (nr == 0) return 0;
  if (nr == UNLIMITED_ROBOTS) return UNLIMITED_ROBOTS;
  if (GetNumHumans() > GetMaxHumans())
  {
    return GetMaxHumans() * nr;
  }
  return GetNumHumans() * nr;
}

int Player::GetMaxPurchasable(PurchasableType i_pt,RobotMechanism i_mechanism) const
{
  int funds = m_CommodityHand.GetHandValue();

  if (i_pt == ROBOT_PURCHASABLE)
  {
    int maxr = GetMaxRobots(i_mechanism);
    if (maxr == 0) return 0;
    int rcount = funds / 10;

    if (maxr != UNLIMITED_ROBOTS)
    {
      int rmax = maxr - GetNumRobots();
      if (rmax < rcount) return rmax;
    }
    return rcount;
  }

  if (i_pt == MAN_PURCHASABLE)
  {
    int nc = m_OwnedItems.GetPeopleCost();
    int max = GetMaxHumans() - GetNumHumans();
    int hcount = funds / nc;
    if (max < hcount) return max;
    return hcount;
  }

  if (i_pt == ORE_FACTORY_PURCHASABLE)
  {
    return funds / 10;
  }

  if (i_pt == WATER_FACTORY_PURCHASABLE)
  {
    return funds / 20;
  }

  if (i_pt == TITANIUM_FACTORY_PURCHASABLE)
  {
    if (!m_OwnedItems.CanBuildTitanium()) return 0;
    return funds / 30;
  }

  if (i_pt == RESEARCH_FACTORY_PURCHASABLE)
  {
    if (!m_OwnedItems.CanBuildResearch()) return 0;
    return funds / 40;
  }

  if (i_pt == NEW_CHEMICALS_FACTORY_PURCHASABLE)
  {
    int fmax = funds / 60;
    int rmax = m_CommodityHand.GetNumResearch();
    if (fmax < rmax) return fmax;
    return rmax;
  }

  return 0;
}

bool Player::HasPurchasedAnything() const
{
  if (GetNumRobots() > 0) return true;
  if (GetNumHumans() > STARTHUMANS) return true;
  if (GetOwnedItems().GetItemList().size() > 0) return true;
  if (GetFactories().size() > 3) return true;

  return false;
}


int Player::GetHighBid() const
{
  return m_HighBid;
}

void Player::SetHighBid(int i_HighBid)
{
  m_HighBid = i_HighBid;
}

int Player::GetLastBid() const
{
  return m_LastBid;
}

void Player::SetLastBid(int i_LastBid)
{
  m_LastBid = i_LastBid;
}

void Player::AutoMan()
{
  std::string manstring;

  manstring.insert(manstring.begin(),
                   (((int)GetFactories().size() < GetNumHumans()) ? GetFactories().size() : GetNumHumans()) , 
                   'H' );

  if ((int)GetFactories().size() > GetNumHumans())
  {
    int leftover = (int)GetFactories().size() - GetNumHumans();
    manstring.insert(manstring.begin(),
                     ((leftover < GetNumRobots()) ? leftover : GetNumRobots()) , 
                     'B' );
  }

  if ((int)GetFactories().size() > GetNumHumans()+GetNumRobots())
  {
    manstring.insert(manstring.begin(),
                     (int)GetFactories().size() - GetNumHumans() - GetNumRobots(),
                     'U' );
  }

  if (GetFactories().AlterManning(manstring,GetNumHumans(),GetNumRobots()) != 0)
  {
    throw std::runtime_error("AutoMan invalid manning string");
  }
}
