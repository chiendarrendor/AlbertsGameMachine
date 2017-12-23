#include "SpaceState.hpp"
#include <stdexcept>

const std::string Piece::AGE_OLD_STRING("Old");
const std::string Piece::AGE_NEW_STRING("New");

Piece::Piece() :
  m_Capital(""),
  m_IsOld(false)
{
}

Piece::Piece(const std::string &i_Capital,bool i_IsOld) :
  m_Capital(i_Capital),
  m_IsOld(i_IsOld)
{
}

Piece::Piece(const Piece &i_right) :
  m_Capital(i_right.m_Capital),
  m_IsOld(i_right.m_IsOld)
{
}

Piece &Piece::operator=(const Piece &i_right)
{
  m_Capital = i_right.m_Capital;
  m_IsOld = i_right.m_IsOld;
  return *this;
}

bool Piece::IsOld() const
{
  return m_IsOld;
}

const std::string &Piece::GetAgeString() const
{
  if (m_IsOld) return AGE_OLD_STRING;
  return AGE_NEW_STRING;
}

const std::string &Piece::GetCapital() const
{
  return m_Capital;
}

SpaceState::SpaceState() :
  m_pSpaceInfo(NULL),
  m_Dominated(false)
{  
}

SpaceState::SpaceState(const std::string &i_Name,const SpaceInfo *i_pSpaceInfo) :
  m_Name(i_Name),
  m_pSpaceInfo(i_pSpaceInfo),
  m_Dominated(false)
{
}

SpaceState::SpaceState(const SpaceState &i_right) :
  m_Name(i_right.m_Name),
  m_pSpaceInfo(i_right.m_pSpaceInfo),
  m_Pieces(i_right.m_Pieces),
  m_Dominated(i_right.m_Dominated)
{
}

SpaceState &SpaceState::operator=(const SpaceState &i_right)
{
  m_Name = i_right.m_Name;
  m_pSpaceInfo = i_right.m_pSpaceInfo;
  m_Pieces = i_right.m_Pieces;
  m_Dominated = i_right.m_Dominated;
  return *this;
}

const std::string &SpaceState::GetName() const
{
  return m_Name;
}

void SpaceState::SetSpaceInfo(const SpaceInfo *i_pSpaceInfo)
{
  m_pSpaceInfo = i_pSpaceInfo;
}

size_t SpaceState::GetNumTokens() const
{
  if (IsDominated()) return 0;

  return m_Pieces.size();
}

bool SpaceState::IsDominated() const
{
  return m_Dominated;
}

void SpaceState::Dominate(const std::string &i_Capital,bool i_IsOld)
{
  if (GetNumTokens() > 0 || IsDominated()) throw std::out_of_range("Dominating a non-empty space!");
  m_Pieces.push_back(Piece(i_Capital,i_IsOld));
  m_Dominated = true;
}

const Piece &SpaceState::GetDomination() const
{
  if (!IsDominated()) throw std::out_of_range("Space Not Dominated!");
  return m_Pieces[0];
}

const Piece &SpaceState::operator[](size_t i_Index) const
{
  if (i_Index >= GetNumTokens()) throw std::out_of_range("Illegal # of tokens!");

  return m_Pieces[i_Index];
}

const SpaceInfo &SpaceState::GetSpaceInfo() const
{
  if (!m_pSpaceInfo) throw std::out_of_range("No SpaceInfo Pointer!");
  return *m_pSpaceInfo;
}


