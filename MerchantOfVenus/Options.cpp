#include "Options.hpp"

Options::Options() :
  m_winmoney(2000),
  m_hideunusedweapons(true)
{
}

void Options::SetWinMoney(int i_winmoney) { m_winmoney = i_winmoney; }
void Options::SetHideUnusedWeapons(bool i_hideunusedweapons) { m_hideunusedweapons = i_hideunusedweapons; }

int Options::GetWinMoney() const { return m_winmoney; }
bool Options::GetHideUnusedWeapons() const { return m_hideunusedweapons; }
