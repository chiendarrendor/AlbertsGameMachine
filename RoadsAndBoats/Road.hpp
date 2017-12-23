
#include "Exit.hpp"

class Road
{
public:
  Road();
  Road(const Road &i_road);
  Road(int i_oid,int i_space1oid,const Exit &i_space1edge,
       int i_space2oid,const Exit &i_space2edge);

  void BuildRoad();

  int GetOID() const;
  int GetSpace1OID() const;
  int GetSpace2OID() const;
  const Exit &GetSpace1Exit() const;
  const Exit &GetSpace2Exit() const;
  bool IsRoadBuilt() const;
private:
  int m_oid;
  int m_space1oid;
  int m_space2oid;
  Exit m_space1edge;
  Exit m_space2edge;
  bool m_roadbuilt; // initally false;
};

  
  
