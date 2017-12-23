#ifndef SPACESTATEHPP
#define SPACESTATEHPP
#include "Serialize.hpp"


class SpaceInfo;

class Piece
{
public:
  static const std::string AGE_OLD_STRING;
  static const std::string AGE_NEW_STRING;

  Piece();
  Piece(const std::string &i_Capital,bool i_IsOld);
  Piece(const Piece&);
  Piece &operator=(const Piece&);

  void SetIsOld(bool i_IsOld);
  const std::string &GetAgeString() const;
  bool IsOld() const;
  const std::string &GetCapital() const;

private:

  SERIALIZE_FUNC
  {
    SERIALIZE(m_Capital);
    SERIALIZE(m_IsOld);
  }

  std::string m_Capital;
  bool m_IsOld;
};




class SpaceState
{
public:
  SpaceState();
  SpaceState(const std::string &i_Name,const SpaceInfo *i_pSpaceInfo);
  SpaceState(const SpaceState &);
  SpaceState &operator=(const SpaceState &);

  const std::string &GetName() const;
  size_t GetNumTokens() const;
  bool IsDominated() const;

  const Piece &GetDomination() const;
  const Piece &operator[](size_t index) const;
  const SpaceInfo &GetSpaceInfo() const;

  void Dominate(const std::string &i_Capital,bool i_IsOld);
  void SetSpaceInfo(const SpaceInfo *i_pSpaceInfo);

private:

  SERIALIZE_FUNC
  {
    SERIALIZE(m_Name);
    SERIALIZE(m_Pieces);
    SERIALIZE(m_Dominated);
  }
  std::string m_Name;
  const SpaceInfo *m_pSpaceInfo;
  std::vector<Piece> m_Pieces;
  bool m_Dominated;
};

#endif
