#include "Secret.hpp"
#include <sstream>

Secret::Secret() :
  m_ispublic(false),
  m_secretkeepers()
{
}

Secret::Secret(const Secret& i_right)
{
  *this = i_right;
}

Secret& Secret::operator=(const Secret& i_right)
{
  if (this == &i_right) return *this;
  m_ispublic = i_right.m_ispublic;
  m_secretkeepers = i_right.m_secretkeepers;
  return *this;
}

void Secret::PlayerLearnsSecret(size_t i_playerid)
{
  if (m_ispublic) return;

  while(m_secretkeepers.size() < (i_playerid + 1)) m_secretkeepers.append("0");
  m_secretkeepers[i_playerid] = '1';
}

bool Secret::PlayerKnowsSecret(size_t i_playerid) const
{
  if (m_ispublic) return true;
  if (m_secretkeepers.size() < (i_playerid+1)) return false;

  return m_secretkeepers[i_playerid] == '1';
}

void Secret::MakePublic()
{
  m_ispublic = true;
}

std::string Secret::GetSecretString() const
{
  if (m_ispublic) return "public";
  return m_secretkeepers;
}


bool Secret::IsPublic() const
{
  return m_ispublic;
}
