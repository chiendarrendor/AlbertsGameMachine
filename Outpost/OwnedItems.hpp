#include "Serialize.hpp"
#include "Items.hpp"

class OwnedItems : public Items
{
public:
  OwnedItems();
  FactoryType AddItem(ItemType i_Item);

  // this does not include manned items, because VP
  // on whether it is manned...the mannable list will know.
  int GetItemVPs() const;

  int GetCostSum() const;

  int GetDiscount(ItemType i_Item) const;

  int GetHandIncrease() const;
  // this does not include manned items because 
  // the people increase will depend on whether it is manned.
  int GetPeopleIncrease() const;

  int GetPeopleCost() const;

  int GetResearchProduction() const;
  int GetMicrobioticProduction() const;

  int GetNumRobotsPerPerson(RobotMechanism i_mechanism) const;

  bool CanBuildResearch() const;
  bool CanBuildTitanium() const;

  const std::vector<ItemType> &GetItemList() const;

private:

  SERIALIZE_FUNC
  {
    SERIALIZE(m_Items);
  }

  std::vector<ItemType> m_Items;
};

