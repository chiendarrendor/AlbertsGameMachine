#include "OwnedItems.hpp"

OwnedItems::OwnedItems() : m_Items()
{
}

FactoryType OwnedItems::AddItem(ItemType i_Item)
{
  m_Items.push_back(i_Item);

  switch(i_Item)
  {
  case  DATA_LIBRARY:
  case  WAREHOUSE:
  case  NODULE:
  case  HEAVY_EQUIPMENT:
  case  SCIENTISTS:
  case  ORBITAL_LAB:
  case  ROBOTS:
  case  ECOPLANTS:
  case NO_ITEM:
    return NO_FACTORY;
  case  LABORATORY:
    return RESEARCH_FACTORY;
  case  OUTPOST:
    return TITANIUM_FACTORY;
  case  SPACE_STATION:
    return SPACE_STATION_FACTORY;
  case  PLANETARY_CRUISER:
    return PLANETARY_CRUISER_FACTORY;
  case  MOON_BASE:
    return MOON_BASE_FACTORY;
  }
  return NO_FACTORY;
}


int OwnedItems::GetItemVPs() const
{
  int result = 0;

  size_t i;
  for (i = 0 ; i < m_Items.size() ; ++i)
  {
    ItemType item = m_Items[i];
    switch(item)
    {
    case  DATA_LIBRARY:
    case  HEAVY_EQUIPMENT:
    case  WAREHOUSE:
      result += 1;
      break;
    case  NODULE:
    case  SCIENTISTS:
      result += 2;
      break;
    case  ORBITAL_LAB:
    case  ROBOTS:
      result += 3;
      break;
    case  LABORATORY:
    case  ECOPLANTS:
    case  OUTPOST:
      result += 5;
      break;
    case  SPACE_STATION:
    case  PLANETARY_CRUISER:
    case  MOON_BASE:
    case NO_ITEM:
      // these VP's are determined by the bonus factory for this card.
      result += 0;
      break;
    }
  }
  return result;
}

int OwnedItems::GetCostSum() const
{
  int result = 0;

  size_t i;
  for (i = 0 ; i < m_Items.size() ; ++i)
  {
    ItemType item = m_Items[i];
    result += GetCost(item);
  }
  return result;
}

int OwnedItems::GetDiscount(ItemType i_Item) const
{
  int result = 0;
  
  size_t i;
  for (i = 0 ; i < m_Items.size() ; ++i)
  {
    ItemType item = m_Items[i];

    if (i_Item == SCIENTISTS && item == DATA_LIBRARY) result += 10;
    if (i_Item == LABORATORY && item == DATA_LIBRARY) result += 10;
    if (i_Item == NODULE && item == HEAVY_EQUIPMENT) result += 5;
    if (i_Item == WAREHOUSE && item == HEAVY_EQUIPMENT) result += 5;
    if (i_Item == OUTPOST && item == HEAVY_EQUIPMENT) result += 10;
    if (i_Item == OUTPOST && item == ECOPLANTS) result += 10;
  }

  return result;
}

int OwnedItems::GetHandIncrease() const
{
  int result = 0;
  
  size_t i;
  for (i = 0 ; i < m_Items.size() ; ++i)
  {
    ItemType item = m_Items[i];

    if (item == WAREHOUSE) result += 3;
    if (item == OUTPOST) result += 3;
  }

  return result;
}

int OwnedItems::GetPeopleIncrease() const
{
  int result = 0;
  
  size_t i;
  for (i = 0 ; i < m_Items.size() ; ++i)
  {
    ItemType item = m_Items[i];

    if (item == NODULE) result += 3;
    if (item == OUTPOST) result += 5;
  }

  return result;
}

int OwnedItems::GetResearchProduction() const
{
  int result = 0;
  
  size_t i;
  for (i = 0 ; i < m_Items.size() ; ++i)
  {
    ItemType item = m_Items[i];

    if (item == SCIENTISTS) result ++;
  }

  return result;
}

int OwnedItems::GetMicrobioticProduction() const
{
  int result = 0;
  
  size_t i;
  for (i = 0 ; i < m_Items.size() ; ++i)
  {
    ItemType item = m_Items[i];

    if (item == ORBITAL_LAB) result ++;
  }

  return result;
}

int OwnedItems::GetNumRobotsPerPerson(RobotMechanism i_mechanism) const
{
  int result = 0;
  
  size_t i;
  for (i = 0 ; i < m_Items.size() ; ++i)
  {
    ItemType item = m_Items[i];

    if (item == ROBOTS) result ++;
  }

  switch(i_mechanism)
  {
  case FIRST_UNLIMITED: 
    if (result == 0) break;
    result = UNLIMITED_ROBOTS;
    break;
  case SECOND_UNLIMITED:
    if (result == 0 || result == 1) break;
    result = UNLIMITED_ROBOTS;
    break;
  case ALWAYS_MULTIPLICATIVE:
    break;
  }
  return result;
}

bool OwnedItems::CanBuildResearch() const
{
  size_t i;
  for (i = 0 ; i < m_Items.size() ; ++i)
  {
    ItemType item = m_Items[i];

    if (item == LABORATORY) return true;
  }

  return false;
}


bool OwnedItems::CanBuildTitanium() const
{
  size_t i;
  for (i = 0 ; i < m_Items.size() ; ++i)
  {
    ItemType item = m_Items[i];

    if (item == HEAVY_EQUIPMENT) return true;
  }

  return false;
}

int OwnedItems::GetPeopleCost() const
{
  size_t i;
  for (i = 0 ; i < m_Items.size() ; ++i)
  {
    ItemType item = m_Items[i];

    if (item == ECOPLANTS) return 5;
  }

  return 10;
}

const std::vector<ItemType> &OwnedItems::GetItemList() const
{
  return m_Items;
}
