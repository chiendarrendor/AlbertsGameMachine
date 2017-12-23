#ifndef SECRETHPP
#define SECRETHPP

#include "Serialize.hpp"

class Secret
{
public:
  Secret();
  Secret(const Secret& i_right);
  Secret& operator=(const Secret& i_right);

  bool PlayerKnowsSecret(size_t i_playerid) const;
  void PlayerLearnsSecret(size_t i_playerid);

  bool IsPublic() const;
  void MakePublic();

  // secrecy is a string of no more than numplayers digits, 
  // either 1 or 0, where a 1 indicates that the corresponding 
  // player knows this location
  // if secrecy is shorter than the number of players, all missing
  // players will be at the end and not know the secret (i.e. getting
  // 01 in a four player game means that players 2 and 3 (and 0) don't
  // know the secret
  // secrecy can also be the string 'public', with an obvious meaning -->

  std::string GetSecretString() const;
private:
  bool m_ispublic;
  std::string m_secretkeepers;

  SERIALIZE_FUNC
  {
    SERIALIZE(m_ispublic);
    SERIALIZE(m_secretkeepers);
  }
};

#endif

