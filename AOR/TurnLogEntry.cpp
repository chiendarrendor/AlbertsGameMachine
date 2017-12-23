#include "TurnLogEntry.hpp"

TurnLogEntry::TurnLogEntry() :
  m_Turn(0),
  m_Total(0),
  m_Tokens(0),
  m_Cash(0),
  m_TokensUpdated(false),
  m_IsPrivate(true)
{
}

TurnLogEntry::TurnLogEntry(int i_Turn,int i_Total) :
  m_Turn(i_Turn),
  m_Total(i_Total),
  m_Tokens(0),
  m_Cash(0),
  m_TokensUpdated(false),
  m_IsPrivate(true)
{
}

TurnLogEntry::TurnLogEntry(const TurnLogEntry &i_right) :
  m_Turn(i_right.m_Turn),
  m_Total(i_right.m_Total),
  m_Tokens(i_right.m_Tokens),
  m_Cash(i_right.m_Cash),
  m_TokensUpdated(i_right.m_TokensUpdated),
  m_IsPrivate(i_right.m_IsPrivate)
{
}

TurnLogEntry &TurnLogEntry::operator=(const TurnLogEntry &i_right)
{
  m_Turn = i_right.m_Turn;
  m_Total = i_right.m_Total;
  m_Tokens = i_right.m_Tokens;
  m_Cash = i_right.m_Cash;
  m_TokensUpdated = i_right.m_TokensUpdated;
  m_IsPrivate = i_right.m_IsPrivate;
  return *this;
}  
  
int TurnLogEntry::GetTurn() const
{
  return m_Turn;
}

int TurnLogEntry::GetTotal() const
{
  return m_Total;
}

int TurnLogEntry::GetTokens() const
{
  return m_Tokens;
}

int TurnLogEntry::GetCash() const
{
  return m_Cash;
}

bool TurnLogEntry::IsUpdated() const
{
  return m_TokensUpdated;
}

bool TurnLogEntry::IsPrivate() const
{
  return m_IsPrivate;
}

void TurnLogEntry::SetTokens(int i_NumTokens)
{
  m_Tokens = i_NumTokens;
  if (i_NumTokens < 0)
  {
    i_NumTokens *= -1;
  }

  m_Cash = m_Total - i_NumTokens;
  m_TokensUpdated = true;
}

  
void TurnLogEntry::MakePublic()
{
  m_IsPrivate = false;
}

void TurnLogEntry::GetObservedInfo(bool i_AmIObserver,int &o_Total,int &o_Tokens,int &o_Cash) const
{
  if (i_AmIObserver || !m_IsPrivate)
  {
    o_Total = m_Total;
  }
  else
  {
    o_Total = SECRET_BID;
  }

  if (!m_TokensUpdated)
  {
    o_Tokens = NO_BID;
    o_Cash = NO_BID;
  }
  else if (m_IsPrivate && !i_AmIObserver)
  {
    o_Tokens = SECRET_BID;
    o_Cash = SECRET_BID;
  }
  else
  {
    o_Tokens = m_Tokens;
    o_Cash = m_Cash;
  }
}
    
