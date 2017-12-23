#include "Serialize.hpp"
#include "Player.hpp"

const size_t NO_PLAYER = 100;

class Players
{
public:
  size_t size() const;
  void add(const std::string &i_Name);
  bool IsPlayer(const std::string &i_Name) const;

  const Player &operator[](size_t i_Num) const;
  Player &operator[](size_t i_Num) ;

  size_t GetPlayerId(const std::string &i_Name) const;

  bool AllBid() const;

  void ExecuteCapitalBid();

  // extends all players turn logs to include a
  // turn log entry for the specified turn (at the point
  // where this is called, that should be curturn + 1)
  void ExtendTurnLog(int i_TurnToExtend);

private:
  SERIALIZE_FUNC
  {
    SERIALIZE(m_Players);
  }
  std::vector<Player> m_Players;
};
