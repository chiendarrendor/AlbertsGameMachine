#ifndef OPTIONSHPP
#define OPTIONSHPP

#include "Serialize.hpp"

class Options
{
 public:
  Options();

  void SetWinMoney(int i_winmoney);
  void SetHideUnusedWeapons(bool i_hideunusedweapons);

  int GetWinMoney() const;
  bool GetHideUnusedWeapons() const;

 private:

  int m_winmoney;
  bool m_hideunusedweapons;

  SERIALIZE_FUNC
  {
    SERIALIZE(m_winmoney);
    SERIALIZE(m_hideunusedweapons);
  }
};

#endif
