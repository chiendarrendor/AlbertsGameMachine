#include "Serialize.hpp"
#include "Players.hpp"
#include "Utilities.hpp"
#include "CommodityManager.hpp"
#include "Options.hpp"
#include "UnownedItems.hpp"
#include "ProductionManager.hpp"
#include "Spend.hpp"

class OutpostSet;

// primary game state interface.

class OutpostState
{
public:
  // required external interfaces
  OutpostState(const OutpostSet &i_Set);
  
  const OutpostSet &GetOutpostSet() const;

  Players &GetPlayers();
  const Players &GetPlayers() const;

  const CommodityManager &GetCommodities() const;
  CommodityManager &GetCommodities();

  const Options &GetOptions() const;
  Options &GetOptions();

  const UnownedItems &GetUnownedItems() const;
  UnownedItems &GetUnownedItems();

  const ProductionManager &GetProductionManager() const;
  ProductionManager &GetProductionManager();

  void SetCurTurn(int i_CurTurn);
  int GetCurTurn() const;

  const std::vector<Spend>& GetSpends() const;
  std::vector<Spend>& GetSpends();

private:
  const OutpostSet &m_Set;

  Players m_Players;
  CommodityManager m_Commodities;
  Options m_Options;
  UnownedItems m_UnownedItems;
  ProductionManager m_ProductionManager;
  int m_CurTurn;
  std::vector<Spend> m_Spends;

  SERIALIZE_FUNC
  {
    SERIALIZE(m_Players);
    SERIALIZE(m_Commodities);
    SERIALIZE(m_Options);
    SERIALIZE(m_UnownedItems);
    SERIALIZE(m_ProductionManager);
    SERIALIZE(m_CurTurn);
    SERIALIZE(m_Spends);
  }


};



