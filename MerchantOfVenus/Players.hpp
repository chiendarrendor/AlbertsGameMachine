#include "Serialize.hpp"
#include "Player.hpp"
#include "MerchantOfVenus.hpp"

const size_t NO_PLAYER = 100;

class Players
{
public:
  Players();
  virtual ~Players();


  size_t size() const;
  void add(const std::string &i_Name);
  void remove(const std::string &i_Name);
  bool IsPlayer(const std::string &i_Name) const;

  const Player &operator[](size_t i_Num) const;
  Player &operator[](size_t i_Num) ;

  // will throw an exception if the name is not a player's name
  Player &GetPlayerByName(const std::string &i_Name);
  const Player &GetPlayerByName(const std::string &i_Name) const;

  size_t GetPlayerId(const std::string &i_Name) const;

  bool AllOk() const;

  // assigns a random turn order.  Also,
  void RandomizeTurnOrder();
  // a string of length #numplayers
  // where the digit at the nth index is the turn order of the nth player
  // (0-based)
  std::string GetTurnOrder() const;
  size_t GetCurPlayerId() const; 
  void NextPlayer();
  Player& GetCurPlayer();
  const Player& GetCurPlayer() const;

private:

  SERIALIZE_FUNC
  {
    SERIALIZE(m_Players);
    SERIALIZE(m_TurnOrder);
    SERIALIZE(m_CurPlayerIdx);
  }
  std::vector<Player> m_Players;
  std::vector<size_t> m_TurnOrder;
  size_t m_CurPlayerIdx;
};
