#include "Serialize.hpp"

class TurnLogEntry
{
public:
  TurnLogEntry();
  TurnLogEntry(int i_Turn,int i_Total);
  TurnLogEntry(const TurnLogEntry &i_right);
  TurnLogEntry &operator=(const TurnLogEntry &i_right);
  
  void GetObservedInfo(bool i_AmIObserver,int &o_Total,int &o_Tokens,int &o_Cash) const;

  int GetTurn() const;
  int GetTotal() const;
  int GetTokens() const;
  int GetCash() const;
  bool IsUpdated() const;
  bool IsPrivate() const;

  void SetTokens(int i_NumTokens);
  void MakePublic();
private:
  SERIALIZE_FUNC
  {
    SERIALIZE(m_Turn);
    SERIALIZE(m_Total);
    SERIALIZE(m_Tokens);
    SERIALIZE(m_Cash);
    SERIALIZE(m_TokensUpdated);
    SERIALIZE(m_IsPrivate);
  }
  int m_Turn;
  int m_Total;
  int m_Tokens;
  int m_Cash;
  bool m_TokensUpdated;
  bool m_IsPrivate;
};
  
