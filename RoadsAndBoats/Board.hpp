#include "Scenario.hpp"
#include <map>
#include "Tile.hpp"
#include "Space.hpp"
#include "Bridge.hpp"
#include "Wall.hpp"
#include "Road.hpp"
#include "InternalDock.hpp"
#include "ExternalDock.hpp"


class Board
{
public:
  Board(const Scenario &i_scenario);
 
private:
  std::map<TileOID,Tile> m_tiles;
  std::map<int,Space> m_spaces;
  std::map<int,Bridge> m_bridges;
  std::map<int,Wall> m_walls;
  std::map<int,Road> m_roads;
  std::map<int,InternalDock> m_internaldocks;
  std::map<int,ExternalDock> m_externaldocks;

  int GetNextUniqueIdentifier();
  int m_nextUniqueIdentifier;

  void MakeTilesAndSpaces(const Scenario &i_scenario);
  void MakeAndConnectBridges();
};


