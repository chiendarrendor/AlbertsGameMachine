#include <set>

class InternalDock
{
public:
  InternalDock();
  InternalDock(const InternalDock &i_right);
  InternalDock(int i_oid,int i_tileoid);

  void AddAdjacentSpace(int i_spaceoid);
private:
  int m_oid;
  int m_tileoid;
  std::set<int> m_adjacentspaces;
};

