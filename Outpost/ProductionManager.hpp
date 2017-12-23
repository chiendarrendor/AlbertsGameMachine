#include "Serialize.hpp"
#include "Outpost.hpp"

class CommodityManager;
class Players;
class Player;

struct InputInfo
{
  CommodityType m_Commodity;
  int m_MaxMegas;
};


class ProductionManager
{
public:
  ProductionManager();
  ~ProductionManager();

  // will begin the process of generating production for the players
  // will pull resource cards from i_comms:
  //   as many as are specified by the given Player's manned factories.
  //   + as many as are specified by the unmanned factories (up to the # of manned) if i_refineries is true
  //   and put the highest value '# of manned factories' cards into the player's hand
  //   if i_IsFirstTurn is true, will generate twice the normal resources.
  // if a given player has at least four manned factories of a resource that has mega-resources,
  //   this method wil return such that IsProductionDone() == false, and GetMegaInfo() returns an object
  //   that describes that player, that resource, and the maximum number of megas they can have.
  // if no such player/factory/resource combination is found, StartProduction will return
  //   such that IsProductionDone == true.  GetMegaInfo() will not return anything particularly useful

  void StartProduction(Players &i_Players,CommodityManager &i_comms,bool i_IsFirstTurn,bool i_refineries);
  
  // if IsProductionDone is false, this will continue the process described above, after first 
  // applying to the current player as many megas as specified, and then continuing as above.
  // this will throw if i_NumMegas * 4 exceeds the number of manned factories for the current player.

  void ContinueProduction(int i_NumMegas,Players &i_Players,CommodityManager &i_comms,bool i_IsFirstTurn,bool i_refineries);
  
  bool IsProductionDone() const;
  InputInfo GetMegaInfo() const;

private:
  CommodityType m_CurCommodity;
  int m_FactoryCount;

  void IncrementIndex(Players &i_Players);
  bool ProcessOne(Player &i_Player,CommodityManager &i_comms,bool i_IsFirstTurn,bool i_refineries,int i_nummega);
  
  SERIALIZE_FUNC
  {
    SERIALIZE(m_CurCommodity);
    SERIALIZE(m_FactoryCount);
  }
};


