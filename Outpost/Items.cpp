#include "Items.hpp"

int Items::GetCost(ItemType i_Item)
{
  switch(i_Item)
  {
  case  NO_ITEM:
    return 0;
  case  DATA_LIBRARY:
    return 15;
  case  WAREHOUSE:
  case  NODULE:
    return 25;
  case  HEAVY_EQUIPMENT:
    return 30;
  case  SCIENTISTS:
    return 40;
  case  ORBITAL_LAB:
  case  ROBOTS:
  case  ECOPLANTS:
    return 50;
  case  LABORATORY:
  case  OUTPOST:
    return 100;
  case  SPACE_STATION:
    return 120;
  case  PLANETARY_CRUISER:
    return 160;
  case  MOON_BASE:
    return 200;
  }
  return 0;
}
