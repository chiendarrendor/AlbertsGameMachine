#ifndef OUTPOSTHPP
#define OUTPOSTHPP

#include <sys/types.h>

const int UNLIMITED_ROBOTS = -1;
const size_t MAXNUMPLAYERS = 10;
const int STARTHUMANS = 3;
const int NOT_HIGH_BID = -1;
const size_t NO_BID_INDEX = 1000;
const size_t ANY_SHIP_SLOT = 2000;
const size_t NO_PLAYER_INDEX = 3000;
const int WINVP = 75;

// a list of the types of things a player can directly purchase
// without bidding.
enum PurchasableType
{
  ROBOT_PURCHASABLE,
  MAN_PURCHASABLE,
  ORE_FACTORY_PURCHASABLE,
  WATER_FACTORY_PURCHASABLE,
  TITANIUM_FACTORY_PURCHASABLE,
  RESEARCH_FACTORY_PURCHASABLE,
  NEW_CHEMICALS_FACTORY_PURCHASABLE
};

enum FactoryType
{
  NO_FACTORY,
  ORE_FACTORY,
  WATER_FACTORY,
  TITANIUM_FACTORY,
  RESEARCH_FACTORY,
  NEW_CHEMICALS_FACTORY,
  SPACE_STATION_FACTORY,
  PLANETARY_CRUISER_FACTORY,
  MOON_BASE_FACTORY
};

enum CommodityType
{
  NO_COMMODITY,
  ORE_COMMODITY,
  WATER_COMMODITY,
  TITANIUM_COMMODITY,
  RESEARCH_COMMODITY,
  MICROBIOTICS_COMMODITY,
  NEW_CHEMICALS_COMMODITY,
  ORBITAL_MEDICINE_COMMODITY,
  RING_ORE_COMMODITY,
  MOON_ORE_COMMODITY
};




enum ItemType
{
  NO_ITEM,
  DATA_LIBRARY,
  WAREHOUSE,
  HEAVY_EQUIPMENT,
  NODULE,
  SCIENTISTS,
  ORBITAL_LAB,
  ROBOTS,
  LABORATORY,
  ECOPLANTS,
  OUTPOST,
  SPACE_STATION,
  PLANETARY_CRUISER,
  MOON_BASE
};

enum ManningType
{
  UNMANNED,
  ROBOT,
  HUMAN
};

enum PurchaseState
{
  UNPLAYED = 0,
  ACTIVE = 1,
  INTERNALIZING = 2, // this state can't bid
  DONE = 3, // can only bid if non-actives can.
  INTERNAL_DONE = 4, // this state can't bid, even if non-actives can.
  FINAL_DONE = 5, // like 3, but is going to skip manning phase
  INTERNAL_FINAL_DONE = 6 // like 4, but is going to skip manning phase
};

enum BidState
{
  UNBID = 0,
  CURBID = 1,
  HASBID = 4,
  PASS = 2,
  PASS_OUT = 3
};

enum RobotMechanism
{
  FIRST_UNLIMITED = 1,
  SECOND_UNLIMITED = 2,
  ALWAYS_MULTIPLICATIVE = 3,
};

enum StockMechanism
{
  NORMAL_STOCK = 1,
  SOME_ONE_EXTRA = 2
};

    




#endif

