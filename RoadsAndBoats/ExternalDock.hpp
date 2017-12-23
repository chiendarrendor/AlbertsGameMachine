
class ExternalDock
{
public:
  ExternalDock();
  ExternalDock(const ExternalDock &i_right);
  ExternalDock(int i_oid,int i_landspaceoid,const Exit &i_landedge,
               int i_seaoid,const Exit &i_seaedge);

  
  int GetOID() const;
  int GetLandSpaceOID() const;
  int GetSeaSpaceOID() const;
  const Exit &GetLandExit() const;
  const Exit &GetSeaExit() const;
public:
  int m_oid;
  int m_landoid;
  int m_seaoid;
  Exit m_landedge;
  Exit m_seaedge;
};
