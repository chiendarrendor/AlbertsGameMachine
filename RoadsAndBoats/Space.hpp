#include "Exit.hpp"


class Space
{
public:
  Space();
  Space(const Space &i_right);
  // river infos of 0 mean no river edges (i.e. whole tile is one space)
  Space(int i_oid,int i_tileoid,int i_wriver,int i_sriver);

  // keyed by river it crosses (river 0 is used for tile with 2 spaces, where there is only one bridge)
  void AddBridgeOID(int i_river,int i_bridgeoid);
  void AddWallOID(const Exit &i_exit,int i_walloid);
  void AddRoadOID(const Exit &i_exit,int i_roadoid);
  void AddInternalDock(int i_dockoid);
  void AddExternalDock(const Exit &i_exit,int i_dockoid);

  void SetBuilding(BuildingType i_building);


  BuildingType GetBuilding() const;
  int GetOID() const;
  int GetTileOID() const;
  int GetWiddershinsRiver() const;
  int GetSunwiseRiver() const;

  const std::map<int,int> &GetBridgeOIDS() const;

private:
  int m_oid;
  int m_tileoid;
  int m_wriver;
  int m_sriver;
  BuildingType m_building; // instanciate with NOBUILDING

  std::map<int,int> m_bridges; // keyed by river it crosses
  std::map<Exit,int> m_walls; // keyed by exit
  std::map<Exit,int> m_roads; // keyed by exit

  std::vector<int> m_internaldocks; 
  std::map<Exit,int> m_externaldocks;
};
