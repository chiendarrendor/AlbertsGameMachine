#include "OutpostState.hpp"

OutpostState::OutpostState(const OutpostSet &i_Set) :
  m_Set(i_Set),
  m_Players(),
  m_Commodities(),
  m_Options(),
  m_UnownedItems(),
  m_ProductionManager(),
  m_CurTurn(0)
{
}

const OutpostSet &OutpostState::GetOutpostSet() const { return m_Set; }
Players &OutpostState::GetPlayers() { return m_Players; }
const Players &OutpostState::GetPlayers() const { return m_Players; }
const CommodityManager &OutpostState::GetCommodities() const { return m_Commodities; }
CommodityManager &OutpostState::GetCommodities() { return m_Commodities; }
const Options &OutpostState::GetOptions() const { return m_Options; }
Options &OutpostState::GetOptions() { return m_Options; }
const UnownedItems &OutpostState::GetUnownedItems() const { return m_UnownedItems; }
UnownedItems &OutpostState::GetUnownedItems() { return m_UnownedItems; }
const ProductionManager &OutpostState::GetProductionManager() const { return m_ProductionManager; }
ProductionManager &OutpostState::GetProductionManager() { return m_ProductionManager; }

void OutpostState::SetCurTurn(int i_CurTurn) { m_CurTurn = i_CurTurn; }
int OutpostState::GetCurTurn() const { return m_CurTurn; }

const std::vector<Spend>& OutpostState::GetSpends() const { return m_Spends; }
std::vector<Spend>& OutpostState::GetSpends() { return m_Spends; }



