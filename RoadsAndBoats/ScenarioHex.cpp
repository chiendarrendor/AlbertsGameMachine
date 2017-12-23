#include "ScenarioHex.hpp"

ScenarioHex::ScenarioHex(TileType i_TileType) :
  m_TileType(i_TileType),
  m_Rivers(),
  m_StartingSpaces()
{
}

ScenarioHex::ScenarioHex(const ScenarioHex &i_right) :
  m_TileType(i_right.m_TileType),
  m_Rivers(i_right.m_Rivers),
  m_StartingSpaces(i_right.m_StartingSpaces)
{
}

void ScenarioHex::SetRiver(int i_edge)
{
  m_Rivers.insert(i_edge);
}

void ScenarioHex::SetStartingSpace(int i_edge)
{
  m_StartingSpaces.insert(i_edge);
}

TileType ScenarioHex::GetTileType() const
{
  return m_TileType;
}

const std::set<int> &ScenarioHex::GetRivers() const
{
  return m_Rivers;
}

const std::set<int> &ScenarioHex::GetStartingSpaces() const
{
  return m_StartingSpaces;
}
