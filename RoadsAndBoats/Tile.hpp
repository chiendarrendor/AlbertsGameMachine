
typedef int TileOID;
#define TILEOIDVALUE(T_X,T_Y) { T_X << 16 + T_Y; }
#define TILEOIDX(T_TOID) { (T_OID >> 16) & 0xffff; }
#define TILEOIDY(T_TOID) { T_OID & 0xffff; }


class Tile
{
public:
  Tile();
  Tile(const Tile &i_right);
  Tile(int i_x,int i_y,const std::set<int> &i_riveredges,const std::set<int> &i_startingspaces);

  void AddSpaceOID(int i_oid);

  int GetX() const;
  int GetY() const;
  TileOID GetOID() const; // return TILEOIDVALUE(m_x,m_y)
  const std::set<int> &GetRiverEdges() const;
  const std::set<int> &GetStartingSpaces() const;
  const std::set<int> &GetSpaceOIDs() const;

private:
  int m_x;
  int m_y;
  std::set<int> m_rivers;
  std::set<int> m_starting;
  std::set<int> m_spaceoids;
};
