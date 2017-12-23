
class Bridge
{
public:
  Bridge();
  Bridge(const Bridge &i_right);
  Bridge(int i_oid,int i_space1oid,int i_space2oid);

  void BuildBridge();
  
  bool HasBridge() const;
  int GetOID() const;
  int GetSpace1OID() const;
  int GetSpace2OID() const;

private:
  int m_oid;
  int m_space1oid;
  int m_space2oid;
  bool m_hasbridge; // instanciate false
};
