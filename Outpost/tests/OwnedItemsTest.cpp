#include "OwnedItems.hpp"
#include "SerializeUtility.hpp"
#include <boost/test/auto_unit_test.hpp>
#include <iostream>



 
void OwnedItemsDataLibrary()
{
  OwnedItems oi;
  FactoryType fac;
  std::vector<ItemType> it;

  fac = oi.AddItem(DATA_LIBRARY);
  
  BOOST_CHECK ( fac == NO_FACTORY );
  BOOST_CHECK ( oi.GetItemVPs() == 1 );
  BOOST_CHECK ( oi.GetCostSum() == 15 );

  BOOST_CHECK ( oi.GetDiscount(DATA_LIBRARY) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(WAREHOUSE) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(HEAVY_EQUIPMENT) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(NODULE) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(SCIENTISTS) == 10 ); 
  BOOST_CHECK ( oi.GetDiscount(ORBITAL_LAB) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ROBOTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(LABORATORY) == 10 ); 
  BOOST_CHECK ( oi.GetDiscount(ECOPLANTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(OUTPOST) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(SPACE_STATION) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(PLANETARY_CRUISER) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(MOON_BASE) == 0 ); 

  BOOST_CHECK ( oi.GetHandIncrease() == 0 );
  BOOST_CHECK ( oi.GetPeopleIncrease() == 0 );
  BOOST_CHECK ( oi.GetPeopleCost() == 10 );
  BOOST_CHECK ( oi.GetResearchProduction() == 0 );
  BOOST_CHECK ( oi.GetMicrobioticProduction() == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( FIRST_UNLIMITED ) == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( SECOND_UNLIMITED ) == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( ALWAYS_MULTIPLICATIVE ) == 0 );
  BOOST_CHECK ( oi.CanBuildResearch() == false );
  BOOST_CHECK ( oi.CanBuildTitanium() == false );

  it.push_back(DATA_LIBRARY);
  BOOST_CHECK ( oi.GetItemList() == it);

  fac = oi.AddItem(DATA_LIBRARY);
  
  BOOST_CHECK ( fac == NO_FACTORY );
  BOOST_CHECK ( oi.GetItemVPs() == 2 );
  BOOST_CHECK ( oi.GetCostSum() == 30 );

  BOOST_CHECK ( oi.GetDiscount(DATA_LIBRARY) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(WAREHOUSE) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(HEAVY_EQUIPMENT) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(NODULE) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(SCIENTISTS) == 20 ); 
  BOOST_CHECK ( oi.GetDiscount(ORBITAL_LAB) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ROBOTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(LABORATORY) == 20 ); 
  BOOST_CHECK ( oi.GetDiscount(ECOPLANTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(OUTPOST) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(SPACE_STATION) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(PLANETARY_CRUISER) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(MOON_BASE) == 0 ); 

  BOOST_CHECK ( oi.GetHandIncrease() == 0 );
  BOOST_CHECK ( oi.GetPeopleIncrease() == 0 );
  BOOST_CHECK ( oi.GetPeopleCost() == 10 );
  BOOST_CHECK ( oi.GetResearchProduction() == 0 );
  BOOST_CHECK ( oi.GetMicrobioticProduction() == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( FIRST_UNLIMITED ) == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( SECOND_UNLIMITED ) == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( ALWAYS_MULTIPLICATIVE ) == 0 );
  BOOST_CHECK ( oi.CanBuildResearch() == false );
  BOOST_CHECK ( oi.CanBuildTitanium() == false );

  it.push_back(DATA_LIBRARY);
  BOOST_CHECK ( oi.GetItemList() == it);
}


void OwnedItemsWarehouse()
{
  OwnedItems oi;
  std::vector<ItemType> it;
  FactoryType fac;

  fac = oi.AddItem(WAREHOUSE);
  
  BOOST_CHECK ( fac == NO_FACTORY );
  BOOST_CHECK ( oi.GetItemVPs() == 1 );
  BOOST_CHECK ( oi.GetCostSum() == 25 );

  BOOST_CHECK ( oi.GetDiscount(DATA_LIBRARY) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(WAREHOUSE) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(HEAVY_EQUIPMENT) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(NODULE) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(SCIENTISTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ORBITAL_LAB) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ROBOTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(LABORATORY) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ECOPLANTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(OUTPOST) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(SPACE_STATION) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(PLANETARY_CRUISER) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(MOON_BASE) == 0 ); 

  BOOST_CHECK ( oi.GetHandIncrease() == 3 );
  BOOST_CHECK ( oi.GetPeopleIncrease() == 0 );
  BOOST_CHECK ( oi.GetPeopleCost() == 10 );
  BOOST_CHECK ( oi.GetResearchProduction() == 0 );
  BOOST_CHECK ( oi.GetMicrobioticProduction() == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( FIRST_UNLIMITED ) == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( SECOND_UNLIMITED ) == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( ALWAYS_MULTIPLICATIVE ) == 0 );
  BOOST_CHECK ( oi.CanBuildResearch() == false );
  BOOST_CHECK ( oi.CanBuildTitanium() == false );

  it.push_back(WAREHOUSE);
  BOOST_CHECK ( oi.GetItemList() == it);

  fac = oi.AddItem(WAREHOUSE);
  
  BOOST_CHECK ( fac == NO_FACTORY );
  BOOST_CHECK ( oi.GetItemVPs() == 2 );
  BOOST_CHECK ( oi.GetCostSum() == 50 );

  BOOST_CHECK ( oi.GetDiscount(DATA_LIBRARY) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(WAREHOUSE) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(HEAVY_EQUIPMENT) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(NODULE) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(SCIENTISTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ORBITAL_LAB) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ROBOTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(LABORATORY) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ECOPLANTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(OUTPOST) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(SPACE_STATION) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(PLANETARY_CRUISER) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(MOON_BASE) == 0 ); 

  BOOST_CHECK ( oi.GetHandIncrease() == 6 );
  BOOST_CHECK ( oi.GetPeopleIncrease() == 0 );
  BOOST_CHECK ( oi.GetPeopleCost() == 10 );
  BOOST_CHECK ( oi.GetResearchProduction() == 0 );
  BOOST_CHECK ( oi.GetMicrobioticProduction() == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( FIRST_UNLIMITED ) == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( SECOND_UNLIMITED ) == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( ALWAYS_MULTIPLICATIVE ) == 0 );
  BOOST_CHECK ( oi.CanBuildResearch() == false );
  BOOST_CHECK ( oi.CanBuildTitanium() == false );

  it.push_back(WAREHOUSE);
  BOOST_CHECK ( oi.GetItemList() == it);
}

void OwnedItemsHeavyEquipment()
{
  OwnedItems oi;
  FactoryType fac;
  std::vector<ItemType> it;

  fac = oi.AddItem(HEAVY_EQUIPMENT);
  
  BOOST_CHECK ( fac == NO_FACTORY );
  BOOST_CHECK ( oi.GetItemVPs() == 1 );
  BOOST_CHECK ( oi.GetCostSum() == 30 );

  BOOST_CHECK ( oi.GetDiscount(DATA_LIBRARY) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(WAREHOUSE) == 5 ); 
  BOOST_CHECK ( oi.GetDiscount(HEAVY_EQUIPMENT) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(NODULE) == 5 ); 
  BOOST_CHECK ( oi.GetDiscount(SCIENTISTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ORBITAL_LAB) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ROBOTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(LABORATORY) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ECOPLANTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(OUTPOST) == 10 ); 
  BOOST_CHECK ( oi.GetDiscount(SPACE_STATION) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(PLANETARY_CRUISER) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(MOON_BASE) == 0 ); 

  BOOST_CHECK ( oi.GetHandIncrease() == 0 );
  BOOST_CHECK ( oi.GetPeopleIncrease() == 0 );
  BOOST_CHECK ( oi.GetPeopleCost() == 10 );
  BOOST_CHECK ( oi.GetResearchProduction() == 0 );
  BOOST_CHECK ( oi.GetMicrobioticProduction() == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( FIRST_UNLIMITED ) == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( SECOND_UNLIMITED ) == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( ALWAYS_MULTIPLICATIVE ) == 0 );
  BOOST_CHECK ( oi.CanBuildResearch() == false );
  BOOST_CHECK ( oi.CanBuildTitanium() == true );

  it.push_back(HEAVY_EQUIPMENT);
  BOOST_CHECK ( oi.GetItemList() == it);

  fac = oi.AddItem(HEAVY_EQUIPMENT);
  
  BOOST_CHECK ( fac == NO_FACTORY );
  BOOST_CHECK ( oi.GetItemVPs() == 2 );
  BOOST_CHECK ( oi.GetCostSum() == 60 );

  BOOST_CHECK ( oi.GetDiscount(DATA_LIBRARY) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(WAREHOUSE) == 10 ); 
  BOOST_CHECK ( oi.GetDiscount(HEAVY_EQUIPMENT) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(NODULE) == 10 ); 
  BOOST_CHECK ( oi.GetDiscount(SCIENTISTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ORBITAL_LAB) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ROBOTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(LABORATORY) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ECOPLANTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(OUTPOST) == 20 ); 
  BOOST_CHECK ( oi.GetDiscount(SPACE_STATION) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(PLANETARY_CRUISER) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(MOON_BASE) == 0 ); 

  BOOST_CHECK ( oi.GetHandIncrease() == 0 );
  BOOST_CHECK ( oi.GetPeopleIncrease() == 0 );
  BOOST_CHECK ( oi.GetPeopleCost() == 10 );
  BOOST_CHECK ( oi.GetResearchProduction() == 0 );
  BOOST_CHECK ( oi.GetMicrobioticProduction() == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( FIRST_UNLIMITED ) == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( SECOND_UNLIMITED ) == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( ALWAYS_MULTIPLICATIVE ) == 0 );
  BOOST_CHECK ( oi.CanBuildResearch() == false );
  BOOST_CHECK ( oi.CanBuildTitanium() == true );

  it.push_back(HEAVY_EQUIPMENT);
  BOOST_CHECK ( oi.GetItemList() == it);
}

void OwnedItemsNodule()
{
  OwnedItems oi;
  FactoryType fac;
  std::vector<ItemType> it;

  fac = oi.AddItem(NODULE);
  
  BOOST_CHECK ( fac == NO_FACTORY );
  BOOST_CHECK ( oi.GetItemVPs() == 2 );
  BOOST_CHECK ( oi.GetCostSum() == 25 );

  BOOST_CHECK ( oi.GetDiscount(DATA_LIBRARY) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(WAREHOUSE) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(HEAVY_EQUIPMENT) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(NODULE) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(SCIENTISTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ORBITAL_LAB) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ROBOTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(LABORATORY) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ECOPLANTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(OUTPOST) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(SPACE_STATION) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(PLANETARY_CRUISER) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(MOON_BASE) == 0 ); 

  BOOST_CHECK ( oi.GetHandIncrease() == 0 );
  BOOST_CHECK ( oi.GetPeopleIncrease() == 3 );
  BOOST_CHECK ( oi.GetPeopleCost() == 10 );
  BOOST_CHECK ( oi.GetResearchProduction() == 0 );
  BOOST_CHECK ( oi.GetMicrobioticProduction() == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( FIRST_UNLIMITED ) == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( SECOND_UNLIMITED ) == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( ALWAYS_MULTIPLICATIVE ) == 0 );
  BOOST_CHECK ( oi.CanBuildResearch() == false );
  BOOST_CHECK ( oi.CanBuildTitanium() == false );

  it.push_back(NODULE);
  BOOST_CHECK ( oi.GetItemList() == it);

  fac = oi.AddItem(NODULE);
  
  BOOST_CHECK ( fac == NO_FACTORY );
  BOOST_CHECK ( oi.GetItemVPs() == 4 );
  BOOST_CHECK ( oi.GetCostSum() == 50 );

  BOOST_CHECK ( oi.GetDiscount(DATA_LIBRARY) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(WAREHOUSE) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(HEAVY_EQUIPMENT) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(NODULE) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(SCIENTISTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ORBITAL_LAB) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ROBOTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(LABORATORY) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ECOPLANTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(OUTPOST) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(SPACE_STATION) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(PLANETARY_CRUISER) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(MOON_BASE) == 0 ); 

  BOOST_CHECK ( oi.GetHandIncrease() == 0 );
  BOOST_CHECK ( oi.GetPeopleIncrease() == 6 );
  BOOST_CHECK ( oi.GetPeopleCost() == 10 );
  BOOST_CHECK ( oi.GetResearchProduction() == 0 );
  BOOST_CHECK ( oi.GetMicrobioticProduction() == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( FIRST_UNLIMITED ) == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( SECOND_UNLIMITED ) == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( ALWAYS_MULTIPLICATIVE ) == 0 );
  BOOST_CHECK ( oi.CanBuildResearch() == false );
  BOOST_CHECK ( oi.CanBuildTitanium() == false );

  it.push_back(NODULE);
  BOOST_CHECK ( oi.GetItemList() == it);
}

void OwnedItemsScientists()
{
  OwnedItems oi;
  FactoryType fac;
  std::vector<ItemType> it;

  fac = oi.AddItem(SCIENTISTS);
  
  BOOST_CHECK ( fac == NO_FACTORY );
  BOOST_CHECK ( oi.GetItemVPs() == 2 );
  BOOST_CHECK ( oi.GetCostSum() == 40 );

  BOOST_CHECK ( oi.GetDiscount(DATA_LIBRARY) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(WAREHOUSE) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(HEAVY_EQUIPMENT) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(NODULE) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(SCIENTISTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ORBITAL_LAB) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ROBOTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(LABORATORY) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ECOPLANTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(OUTPOST) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(SPACE_STATION) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(PLANETARY_CRUISER) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(MOON_BASE) == 0 ); 

  BOOST_CHECK ( oi.GetHandIncrease() == 0 );
  BOOST_CHECK ( oi.GetPeopleIncrease() == 0 );
  BOOST_CHECK ( oi.GetPeopleCost() == 10 );
  BOOST_CHECK ( oi.GetResearchProduction() == 1 );
  BOOST_CHECK ( oi.GetMicrobioticProduction() == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( FIRST_UNLIMITED ) == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( SECOND_UNLIMITED ) == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( ALWAYS_MULTIPLICATIVE ) == 0 );
  BOOST_CHECK ( oi.CanBuildResearch() == false );
  BOOST_CHECK ( oi.CanBuildTitanium() == false );

  it.push_back(SCIENTISTS);
  BOOST_CHECK ( oi.GetItemList() == it);

  fac = oi.AddItem(SCIENTISTS);
  
  BOOST_CHECK ( fac == NO_FACTORY );
  BOOST_CHECK ( oi.GetItemVPs() == 4 );
  BOOST_CHECK ( oi.GetCostSum() == 80 );

  BOOST_CHECK ( oi.GetDiscount(DATA_LIBRARY) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(WAREHOUSE) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(HEAVY_EQUIPMENT) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(NODULE) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(SCIENTISTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ORBITAL_LAB) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ROBOTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(LABORATORY) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ECOPLANTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(OUTPOST) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(SPACE_STATION) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(PLANETARY_CRUISER) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(MOON_BASE) == 0 ); 

  BOOST_CHECK ( oi.GetHandIncrease() == 0 );
  BOOST_CHECK ( oi.GetPeopleIncrease() == 0 );
  BOOST_CHECK ( oi.GetPeopleCost() == 10 );
  BOOST_CHECK ( oi.GetResearchProduction() == 2 );
  BOOST_CHECK ( oi.GetMicrobioticProduction() == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( FIRST_UNLIMITED ) == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( SECOND_UNLIMITED ) == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( ALWAYS_MULTIPLICATIVE ) == 0 );
  BOOST_CHECK ( oi.CanBuildResearch() == false );
  BOOST_CHECK ( oi.CanBuildTitanium() == false );

  it.push_back(SCIENTISTS);
  BOOST_CHECK ( oi.GetItemList() == it);
}

void OwnedItemsOrbitalLab()
{
  OwnedItems oi;
  FactoryType fac;
  std::vector<ItemType> it;

  fac = oi.AddItem(ORBITAL_LAB);
  
  BOOST_CHECK ( fac == NO_FACTORY );
  BOOST_CHECK ( oi.GetItemVPs() == 3 );
  BOOST_CHECK ( oi.GetCostSum() == 50 );

  BOOST_CHECK ( oi.GetDiscount(DATA_LIBRARY) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(WAREHOUSE) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(HEAVY_EQUIPMENT) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(NODULE) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(SCIENTISTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ORBITAL_LAB) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ROBOTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(LABORATORY) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ECOPLANTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(OUTPOST) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(SPACE_STATION) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(PLANETARY_CRUISER) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(MOON_BASE) == 0 ); 

  BOOST_CHECK ( oi.GetHandIncrease() == 0 );
  BOOST_CHECK ( oi.GetPeopleIncrease() == 0 );
  BOOST_CHECK ( oi.GetPeopleCost() == 10 );
  BOOST_CHECK ( oi.GetResearchProduction() == 0 );
  BOOST_CHECK ( oi.GetMicrobioticProduction() == 1 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( FIRST_UNLIMITED ) == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( SECOND_UNLIMITED ) == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( ALWAYS_MULTIPLICATIVE ) == 0 );
  BOOST_CHECK ( oi.CanBuildResearch() == false );
  BOOST_CHECK ( oi.CanBuildTitanium() == false );

  it.push_back(ORBITAL_LAB);
  BOOST_CHECK ( oi.GetItemList() == it);

  fac = oi.AddItem(ORBITAL_LAB);
  
  BOOST_CHECK ( fac == NO_FACTORY );
  BOOST_CHECK ( oi.GetItemVPs() == 6 );
  BOOST_CHECK ( oi.GetCostSum() == 100 );

  BOOST_CHECK ( oi.GetDiscount(DATA_LIBRARY) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(WAREHOUSE) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(HEAVY_EQUIPMENT) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(NODULE) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(SCIENTISTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ORBITAL_LAB) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ROBOTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(LABORATORY) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ECOPLANTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(OUTPOST) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(SPACE_STATION) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(PLANETARY_CRUISER) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(MOON_BASE) == 0 ); 

  BOOST_CHECK ( oi.GetHandIncrease() == 0 );
  BOOST_CHECK ( oi.GetPeopleIncrease() == 0 );
  BOOST_CHECK ( oi.GetPeopleCost() == 10 );
  BOOST_CHECK ( oi.GetResearchProduction() == 0 );
  BOOST_CHECK ( oi.GetMicrobioticProduction() == 2 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( FIRST_UNLIMITED ) == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( SECOND_UNLIMITED ) == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( ALWAYS_MULTIPLICATIVE ) == 0 );
  BOOST_CHECK ( oi.CanBuildResearch() == false );
  BOOST_CHECK ( oi.CanBuildTitanium() == false );

  it.push_back(ORBITAL_LAB);
  BOOST_CHECK ( oi.GetItemList() == it);
}

void OwnedItemsRobots()
{
  OwnedItems oi;
  FactoryType fac;
  std::vector<ItemType> it;

  fac = oi.AddItem(ROBOTS);
  
  BOOST_CHECK ( fac == NO_FACTORY );
  BOOST_CHECK ( oi.GetItemVPs() == 3 );
  BOOST_CHECK ( oi.GetCostSum() == 50 );

  BOOST_CHECK ( oi.GetDiscount(DATA_LIBRARY) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(WAREHOUSE) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(HEAVY_EQUIPMENT) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(NODULE) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(SCIENTISTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ORBITAL_LAB) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ROBOTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(LABORATORY) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ECOPLANTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(OUTPOST) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(SPACE_STATION) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(PLANETARY_CRUISER) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(MOON_BASE) == 0 ); 

  BOOST_CHECK ( oi.GetHandIncrease() == 0 );
  BOOST_CHECK ( oi.GetPeopleIncrease() == 0 );
  BOOST_CHECK ( oi.GetPeopleCost() == 10 );
  BOOST_CHECK ( oi.GetResearchProduction() == 0 );
  BOOST_CHECK ( oi.GetMicrobioticProduction() == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( FIRST_UNLIMITED ) == UNLIMITED_ROBOTS );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( SECOND_UNLIMITED ) == 1 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( ALWAYS_MULTIPLICATIVE ) == 1 );
  BOOST_CHECK ( oi.CanBuildResearch() == false );
  BOOST_CHECK ( oi.CanBuildTitanium() == false );

  it.push_back(ROBOTS);
  BOOST_CHECK ( oi.GetItemList() == it);

  fac = oi.AddItem(ROBOTS);
  
  BOOST_CHECK ( fac == NO_FACTORY );
  BOOST_CHECK ( oi.GetItemVPs() == 6 );
  BOOST_CHECK ( oi.GetCostSum() == 100 );

  BOOST_CHECK ( oi.GetDiscount(DATA_LIBRARY) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(WAREHOUSE) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(HEAVY_EQUIPMENT) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(NODULE) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(SCIENTISTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ORBITAL_LAB) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ROBOTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(LABORATORY) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ECOPLANTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(OUTPOST) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(SPACE_STATION) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(PLANETARY_CRUISER) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(MOON_BASE) == 0 ); 

  BOOST_CHECK ( oi.GetHandIncrease() == 0 );
  BOOST_CHECK ( oi.GetPeopleIncrease() == 0 );
  BOOST_CHECK ( oi.GetPeopleCost() == 10 );
  BOOST_CHECK ( oi.GetResearchProduction() == 0 );
  BOOST_CHECK ( oi.GetMicrobioticProduction() == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( FIRST_UNLIMITED ) == UNLIMITED_ROBOTS );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( SECOND_UNLIMITED ) == UNLIMITED_ROBOTS );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( ALWAYS_MULTIPLICATIVE ) == 2 );
  BOOST_CHECK ( oi.CanBuildResearch() == false );
  BOOST_CHECK ( oi.CanBuildTitanium() == false );

  it.push_back(ROBOTS);
  BOOST_CHECK ( oi.GetItemList() == it);
}


void OwnedItemsLaboratory()
{
  OwnedItems oi;
  FactoryType fac;
  std::vector<ItemType> it;

  fac = oi.AddItem(LABORATORY);
  
  BOOST_CHECK ( fac == RESEARCH_FACTORY );
  BOOST_CHECK ( oi.GetItemVPs() == 5 );
  BOOST_CHECK ( oi.GetCostSum() == 100 );

  BOOST_CHECK ( oi.GetDiscount(DATA_LIBRARY) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(WAREHOUSE) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(HEAVY_EQUIPMENT) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(NODULE) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(SCIENTISTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ORBITAL_LAB) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ROBOTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(LABORATORY) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ECOPLANTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(OUTPOST) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(SPACE_STATION) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(PLANETARY_CRUISER) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(MOON_BASE) == 0 ); 

  BOOST_CHECK ( oi.GetHandIncrease() == 0 );
  BOOST_CHECK ( oi.GetPeopleIncrease() == 0 );
  BOOST_CHECK ( oi.GetPeopleCost() == 10 );
  BOOST_CHECK ( oi.GetResearchProduction() == 0 );
  BOOST_CHECK ( oi.GetMicrobioticProduction() == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( FIRST_UNLIMITED ) == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( SECOND_UNLIMITED ) == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( ALWAYS_MULTIPLICATIVE ) == 0 );
  BOOST_CHECK ( oi.CanBuildResearch() == true );
  BOOST_CHECK ( oi.CanBuildTitanium() == false );

  it.push_back(LABORATORY);
  BOOST_CHECK ( oi.GetItemList() == it);

  fac = oi.AddItem(LABORATORY);
  
  BOOST_CHECK ( fac == RESEARCH_FACTORY );
  BOOST_CHECK ( oi.GetItemVPs() == 10 );
  BOOST_CHECK ( oi.GetCostSum() == 200 );

  BOOST_CHECK ( oi.GetDiscount(DATA_LIBRARY) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(WAREHOUSE) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(HEAVY_EQUIPMENT) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(NODULE) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(SCIENTISTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ORBITAL_LAB) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ROBOTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(LABORATORY) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ECOPLANTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(OUTPOST) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(SPACE_STATION) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(PLANETARY_CRUISER) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(MOON_BASE) == 0 ); 

  BOOST_CHECK ( oi.GetHandIncrease() == 0 );
  BOOST_CHECK ( oi.GetPeopleIncrease() == 0 );
  BOOST_CHECK ( oi.GetPeopleCost() == 10 );
  BOOST_CHECK ( oi.GetResearchProduction() == 0 );
  BOOST_CHECK ( oi.GetMicrobioticProduction() == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( FIRST_UNLIMITED ) == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( SECOND_UNLIMITED ) == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( ALWAYS_MULTIPLICATIVE ) == 0 );
  BOOST_CHECK ( oi.CanBuildResearch() == true );
  BOOST_CHECK ( oi.CanBuildTitanium() == false );

  it.push_back(LABORATORY);
  BOOST_CHECK ( oi.GetItemList() == it);
}

void OwnedItemsEcoplants()
{
  OwnedItems oi;
  FactoryType fac;
  std::vector<ItemType> it;

  fac = oi.AddItem(ECOPLANTS);
  
  BOOST_CHECK ( fac == NO_FACTORY );
  BOOST_CHECK ( oi.GetItemVPs() == 5 );
  BOOST_CHECK ( oi.GetCostSum() == 50 );

  BOOST_CHECK ( oi.GetDiscount(DATA_LIBRARY) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(WAREHOUSE) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(HEAVY_EQUIPMENT) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(NODULE) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(SCIENTISTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ORBITAL_LAB) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ROBOTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(LABORATORY) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ECOPLANTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(OUTPOST) == 10 ); 
  BOOST_CHECK ( oi.GetDiscount(SPACE_STATION) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(PLANETARY_CRUISER) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(MOON_BASE) == 0 ); 

  BOOST_CHECK ( oi.GetHandIncrease() == 0 );
  BOOST_CHECK ( oi.GetPeopleIncrease() == 0 );
  BOOST_CHECK ( oi.GetPeopleCost() == 5 );
  BOOST_CHECK ( oi.GetResearchProduction() == 0 );
  BOOST_CHECK ( oi.GetMicrobioticProduction() == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( FIRST_UNLIMITED ) == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( SECOND_UNLIMITED ) == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( ALWAYS_MULTIPLICATIVE ) == 0 );
  BOOST_CHECK ( oi.CanBuildResearch() == false );
  BOOST_CHECK ( oi.CanBuildTitanium() == false );

  it.push_back(ECOPLANTS);
  BOOST_CHECK ( oi.GetItemList() == it);

  fac = oi.AddItem(ECOPLANTS);
  
  BOOST_CHECK ( fac == NO_FACTORY );
  BOOST_CHECK ( oi.GetItemVPs() == 10 );
  BOOST_CHECK ( oi.GetCostSum() == 100 );

  BOOST_CHECK ( oi.GetDiscount(DATA_LIBRARY) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(WAREHOUSE) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(HEAVY_EQUIPMENT) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(NODULE) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(SCIENTISTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ORBITAL_LAB) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ROBOTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(LABORATORY) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ECOPLANTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(OUTPOST) == 20 ); 
  BOOST_CHECK ( oi.GetDiscount(SPACE_STATION) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(PLANETARY_CRUISER) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(MOON_BASE) == 0 ); 

  BOOST_CHECK ( oi.GetHandIncrease() == 0 );
  BOOST_CHECK ( oi.GetPeopleIncrease() == 0 );
  BOOST_CHECK ( oi.GetPeopleCost() == 5 );
  BOOST_CHECK ( oi.GetResearchProduction() == 0 );
  BOOST_CHECK ( oi.GetMicrobioticProduction() == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( FIRST_UNLIMITED ) == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( SECOND_UNLIMITED ) == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( ALWAYS_MULTIPLICATIVE ) == 0 );
  BOOST_CHECK ( oi.CanBuildResearch() == false );
  BOOST_CHECK ( oi.CanBuildTitanium() == false );

  it.push_back(ECOPLANTS);
  BOOST_CHECK ( oi.GetItemList() == it);
}

void OwnedItemsOutpost()
{
  OwnedItems oi;
  FactoryType fac;
  std::vector<ItemType> it;

  fac = oi.AddItem(OUTPOST);
  
  BOOST_CHECK ( fac == TITANIUM_FACTORY );
  BOOST_CHECK ( oi.GetItemVPs() == 5 );
  BOOST_CHECK ( oi.GetCostSum() == 100 );

  BOOST_CHECK ( oi.GetDiscount(DATA_LIBRARY) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(WAREHOUSE) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(HEAVY_EQUIPMENT) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(NODULE) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(SCIENTISTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ORBITAL_LAB) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ROBOTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(LABORATORY) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ECOPLANTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(OUTPOST) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(SPACE_STATION) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(PLANETARY_CRUISER) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(MOON_BASE) == 0 ); 

  BOOST_CHECK ( oi.GetHandIncrease() == 3 );
  BOOST_CHECK ( oi.GetPeopleIncrease() == 5 );
  BOOST_CHECK ( oi.GetPeopleCost() == 10 );
  BOOST_CHECK ( oi.GetResearchProduction() == 0 );
  BOOST_CHECK ( oi.GetMicrobioticProduction() == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( FIRST_UNLIMITED ) == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( SECOND_UNLIMITED ) == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( ALWAYS_MULTIPLICATIVE ) == 0 );
  BOOST_CHECK ( oi.CanBuildResearch() == false );
  BOOST_CHECK ( oi.CanBuildTitanium() == false );

  it.push_back(OUTPOST);
  BOOST_CHECK ( oi.GetItemList() == it);

  fac = oi.AddItem(OUTPOST);
  
  BOOST_CHECK ( fac == TITANIUM_FACTORY );
  BOOST_CHECK ( oi.GetItemVPs() == 10 );
  BOOST_CHECK ( oi.GetCostSum() == 200 );

  BOOST_CHECK ( oi.GetDiscount(DATA_LIBRARY) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(WAREHOUSE) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(HEAVY_EQUIPMENT) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(NODULE) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(SCIENTISTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ORBITAL_LAB) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ROBOTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(LABORATORY) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ECOPLANTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(OUTPOST) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(SPACE_STATION) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(PLANETARY_CRUISER) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(MOON_BASE) == 0 ); 

  BOOST_CHECK ( oi.GetHandIncrease() == 6 );
  BOOST_CHECK ( oi.GetPeopleIncrease() == 10 );
  BOOST_CHECK ( oi.GetPeopleCost() == 10 );
  BOOST_CHECK ( oi.GetResearchProduction() == 0 );
  BOOST_CHECK ( oi.GetMicrobioticProduction() == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( FIRST_UNLIMITED ) == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( SECOND_UNLIMITED ) == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( ALWAYS_MULTIPLICATIVE ) == 0 );
  BOOST_CHECK ( oi.CanBuildResearch() == false );
  BOOST_CHECK ( oi.CanBuildTitanium() == false );

  it.push_back(OUTPOST);
  BOOST_CHECK ( oi.GetItemList() == it);
}

void OwnedItemsSpaceStation()
{
  OwnedItems oi;
  FactoryType fac;
  std::vector<ItemType> it;

  fac = oi.AddItem(SPACE_STATION);
  
  BOOST_CHECK ( fac == SPACE_STATION_FACTORY );
  BOOST_CHECK ( oi.GetItemVPs() == 0 );
  BOOST_CHECK ( oi.GetCostSum() == 120 );

  BOOST_CHECK ( oi.GetDiscount(DATA_LIBRARY) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(WAREHOUSE) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(HEAVY_EQUIPMENT) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(NODULE) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(SCIENTISTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ORBITAL_LAB) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ROBOTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(LABORATORY) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ECOPLANTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(OUTPOST) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(SPACE_STATION) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(PLANETARY_CRUISER) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(MOON_BASE) == 0 ); 

  BOOST_CHECK ( oi.GetHandIncrease() == 0 );
  BOOST_CHECK ( oi.GetPeopleIncrease() == 0 );
  BOOST_CHECK ( oi.GetPeopleCost() == 10 );
  BOOST_CHECK ( oi.GetResearchProduction() == 0 );
  BOOST_CHECK ( oi.GetMicrobioticProduction() == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( FIRST_UNLIMITED ) == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( SECOND_UNLIMITED ) == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( ALWAYS_MULTIPLICATIVE ) == 0 );
  BOOST_CHECK ( oi.CanBuildResearch() == false );
  BOOST_CHECK ( oi.CanBuildTitanium() == false );

  it.push_back(SPACE_STATION);
  BOOST_CHECK ( oi.GetItemList() == it);

  fac = oi.AddItem(SPACE_STATION);
  
  BOOST_CHECK ( fac == SPACE_STATION_FACTORY );
  BOOST_CHECK ( oi.GetItemVPs() == 0 );
  BOOST_CHECK ( oi.GetCostSum() == 240 );

  BOOST_CHECK ( oi.GetDiscount(DATA_LIBRARY) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(WAREHOUSE) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(HEAVY_EQUIPMENT) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(NODULE) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(SCIENTISTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ORBITAL_LAB) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ROBOTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(LABORATORY) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ECOPLANTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(OUTPOST) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(SPACE_STATION) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(PLANETARY_CRUISER) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(MOON_BASE) == 0 ); 

  BOOST_CHECK ( oi.GetHandIncrease() == 0 );
  BOOST_CHECK ( oi.GetPeopleIncrease() == 0 );
  BOOST_CHECK ( oi.GetPeopleCost() == 10 );
  BOOST_CHECK ( oi.GetResearchProduction() == 0 );
  BOOST_CHECK ( oi.GetMicrobioticProduction() == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( FIRST_UNLIMITED ) == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( SECOND_UNLIMITED ) == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( ALWAYS_MULTIPLICATIVE ) == 0 );
  BOOST_CHECK ( oi.CanBuildResearch() == false );
  BOOST_CHECK ( oi.CanBuildTitanium() == false );

  it.push_back(SPACE_STATION);
  BOOST_CHECK ( oi.GetItemList() == it);
}


void OwnedItemsPlanetaryCruiser()
{
  OwnedItems oi;
  FactoryType fac;
  std::vector<ItemType> it;

  fac = oi.AddItem(PLANETARY_CRUISER);
  
  BOOST_CHECK ( fac == PLANETARY_CRUISER_FACTORY );
  BOOST_CHECK ( oi.GetItemVPs() == 0 );
  BOOST_CHECK ( oi.GetCostSum() == 160 );

  BOOST_CHECK ( oi.GetDiscount(DATA_LIBRARY) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(WAREHOUSE) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(HEAVY_EQUIPMENT) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(NODULE) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(SCIENTISTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ORBITAL_LAB) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ROBOTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(LABORATORY) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ECOPLANTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(OUTPOST) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(SPACE_STATION) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(PLANETARY_CRUISER) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(MOON_BASE) == 0 ); 

  BOOST_CHECK ( oi.GetHandIncrease() == 0 );
  BOOST_CHECK ( oi.GetPeopleIncrease() == 0 );
  BOOST_CHECK ( oi.GetPeopleCost() == 10 );
  BOOST_CHECK ( oi.GetResearchProduction() == 0 );
  BOOST_CHECK ( oi.GetMicrobioticProduction() == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( FIRST_UNLIMITED ) == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( SECOND_UNLIMITED ) == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( ALWAYS_MULTIPLICATIVE ) == 0 );
  BOOST_CHECK ( oi.CanBuildResearch() == false );
  BOOST_CHECK ( oi.CanBuildTitanium() == false );

  it.push_back(PLANETARY_CRUISER);
  BOOST_CHECK ( oi.GetItemList() == it);

  fac = oi.AddItem(PLANETARY_CRUISER);
  
  BOOST_CHECK ( fac == PLANETARY_CRUISER_FACTORY );
  BOOST_CHECK ( oi.GetItemVPs() == 0 );
  BOOST_CHECK ( oi.GetCostSum() == 320 );

  BOOST_CHECK ( oi.GetDiscount(DATA_LIBRARY) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(WAREHOUSE) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(HEAVY_EQUIPMENT) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(NODULE) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(SCIENTISTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ORBITAL_LAB) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ROBOTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(LABORATORY) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ECOPLANTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(OUTPOST) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(SPACE_STATION) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(PLANETARY_CRUISER) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(MOON_BASE) == 0 ); 

  BOOST_CHECK ( oi.GetHandIncrease() == 0 );
  BOOST_CHECK ( oi.GetPeopleIncrease() == 0 );
  BOOST_CHECK ( oi.GetPeopleCost() == 10 );
  BOOST_CHECK ( oi.GetResearchProduction() == 0 );
  BOOST_CHECK ( oi.GetMicrobioticProduction() == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( FIRST_UNLIMITED ) == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( SECOND_UNLIMITED ) == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( ALWAYS_MULTIPLICATIVE ) == 0 );
  BOOST_CHECK ( oi.CanBuildResearch() == false );
  BOOST_CHECK ( oi.CanBuildTitanium() == false );

  it.push_back(PLANETARY_CRUISER);
  BOOST_CHECK ( oi.GetItemList() == it);
}


void OwnedItemsMoonBase()
{
  OwnedItems oi;
  FactoryType fac;
  std::vector<ItemType> it;

  fac = oi.AddItem(MOON_BASE);
  
  BOOST_CHECK ( fac == MOON_BASE_FACTORY );
  BOOST_CHECK ( oi.GetItemVPs() == 0 );
  BOOST_CHECK ( oi.GetCostSum() == 200 );

  BOOST_CHECK ( oi.GetDiscount(DATA_LIBRARY) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(WAREHOUSE) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(HEAVY_EQUIPMENT) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(NODULE) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(SCIENTISTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ORBITAL_LAB) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ROBOTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(LABORATORY) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ECOPLANTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(OUTPOST) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(SPACE_STATION) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(PLANETARY_CRUISER) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(MOON_BASE) == 0 ); 

  BOOST_CHECK ( oi.GetHandIncrease() == 0 );
  BOOST_CHECK ( oi.GetPeopleIncrease() == 0 );
  BOOST_CHECK ( oi.GetPeopleCost() == 10 );
  BOOST_CHECK ( oi.GetResearchProduction() == 0 );
  BOOST_CHECK ( oi.GetMicrobioticProduction() == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( FIRST_UNLIMITED ) == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( SECOND_UNLIMITED ) == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( ALWAYS_MULTIPLICATIVE ) == 0 );
  BOOST_CHECK ( oi.CanBuildResearch() == false );
  BOOST_CHECK ( oi.CanBuildTitanium() == false );

  it.push_back(MOON_BASE);
  BOOST_CHECK ( oi.GetItemList() == it);

  fac = oi.AddItem(MOON_BASE);
  
  BOOST_CHECK ( fac == MOON_BASE_FACTORY );
  BOOST_CHECK ( oi.GetItemVPs() == 0 );
  BOOST_CHECK ( oi.GetCostSum() == 400 );

  BOOST_CHECK ( oi.GetDiscount(DATA_LIBRARY) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(WAREHOUSE) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(HEAVY_EQUIPMENT) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(NODULE) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(SCIENTISTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ORBITAL_LAB) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ROBOTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(LABORATORY) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(ECOPLANTS) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(OUTPOST) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(SPACE_STATION) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(PLANETARY_CRUISER) == 0 ); 
  BOOST_CHECK ( oi.GetDiscount(MOON_BASE) == 0 ); 

  BOOST_CHECK ( oi.GetHandIncrease() == 0 );
  BOOST_CHECK ( oi.GetPeopleIncrease() == 0 );
  BOOST_CHECK ( oi.GetPeopleCost() == 10 );
  BOOST_CHECK ( oi.GetResearchProduction() == 0 );
  BOOST_CHECK ( oi.GetMicrobioticProduction() == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( FIRST_UNLIMITED ) == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( SECOND_UNLIMITED ) == 0 );
  BOOST_CHECK ( oi.GetNumRobotsPerPerson( ALWAYS_MULTIPLICATIVE ) == 0 );
  BOOST_CHECK ( oi.CanBuildResearch() == false );
  BOOST_CHECK ( oi.CanBuildTitanium() == false );

  it.push_back(MOON_BASE);
  BOOST_CHECK ( oi.GetItemList() == it);
}

BOOST_AUTO_TEST_CASE( OwnedItemsConstructor )
{
  OwnedItemsDataLibrary();
  OwnedItemsWarehouse();
  OwnedItemsHeavyEquipment();
  OwnedItemsNodule();
  OwnedItemsScientists();
  OwnedItemsOrbitalLab();
  OwnedItemsRobots();
  OwnedItemsLaboratory();
  OwnedItemsEcoplants();
  OwnedItemsOutpost();
  OwnedItemsSpaceStation();
  OwnedItemsPlanetaryCruiser();
  OwnedItemsMoonBase();

  OwnedItems oi;
  std::vector<ItemType> it;
  oi.AddItem(LABORATORY);      it.push_back(LABORATORY);
  oi.AddItem(HEAVY_EQUIPMENT); it.push_back(HEAVY_EQUIPMENT);
  oi.AddItem(ROBOTS);          it.push_back(ROBOTS);
  oi.AddItem(ROBOTS);          it.push_back(ROBOTS);

  BOOST_CHECK( oi.GetItemList() == it);

  OwnedItems noi;
  SerialTransfer(oi,noi);

  BOOST_CHECK( noi.GetItemList() == it);
}
