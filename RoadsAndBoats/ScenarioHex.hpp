#ifndef SCENARIOHEXHPP
#define SCENARIOHEXHPP

#include "RoadsAndBoats.hpp"
#include <set>


class ScenarioHex
{
public:
  ScenarioHex(TileType i_tiletype);
  ScenarioHex(const ScenarioHex &i_right);

  void SetRiver(int i_edge);
  void SetStartingSpace(int i_edge);
  
  TileType GetTileType() const;
  const std::set<int> &GetRivers() const;
  const std::set<int> &GetStartingSpaces() const;
  // if GetRivers() is size 0 or 1:
  //   empty if not starting tile
  //   non-empty if is starting tile
  // if GetRivers() is larger than 1:
  //   GetStartingSpaces() contains river
  //   edge identifiers such that the starting
  //   space is the space immediately clockwise from the river
private:
  TileType m_TileType;
  std::set<int> m_Rivers;
  std::set<int> m_StartingSpaces;
};

#endif
