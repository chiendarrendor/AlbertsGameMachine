
class Wall
{
public:
  Wall();
  Wall(const Wall &i_right);
  Wall(int i_oid,int i_tile1oid,int i_tile2oid);

  void Build(int i_Player);
  void Raze();

  int GetOID() const;
  int GetTile1OID() const;
  int GetTile2OID() const;
  int GetHeight() const;
  int GetPlayer() const;

private:
  int m_oid;
  int m_tile1oid;
  int m_tile2oid;
  int m_height; // starts at zero
  int m_player; // starts at -1;
};
