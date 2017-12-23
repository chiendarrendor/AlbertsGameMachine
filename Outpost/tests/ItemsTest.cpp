#include "Items.hpp"
#include "SerializeUtility.hpp"
#include <boost/test/auto_unit_test.hpp>
#include <iostream>

BOOST_AUTO_TEST_CASE ( ItemsGetCost )
{
  BOOST_CHECK_EQUAL ( Items::GetCost(DATA_LIBRARY) , 15 );
  BOOST_CHECK_EQUAL ( Items::GetCost(WAREHOUSE) , 25 );
  BOOST_CHECK_EQUAL ( Items::GetCost(HEAVY_EQUIPMENT) , 30 );
  BOOST_CHECK_EQUAL ( Items::GetCost(NODULE) , 25 );
  BOOST_CHECK_EQUAL ( Items::GetCost(SCIENTISTS) , 40 );
  BOOST_CHECK_EQUAL ( Items::GetCost(ORBITAL_LAB) , 50 );
  BOOST_CHECK_EQUAL ( Items::GetCost(ROBOTS) , 50 );
  BOOST_CHECK_EQUAL ( Items::GetCost(LABORATORY) , 100 );
  BOOST_CHECK_EQUAL ( Items::GetCost(ECOPLANTS) , 50 );
  BOOST_CHECK_EQUAL ( Items::GetCost(OUTPOST) , 100 );
  BOOST_CHECK_EQUAL ( Items::GetCost(SPACE_STATION) , 120 );
  BOOST_CHECK_EQUAL ( Items::GetCost(PLANETARY_CRUISER) , 160 );
  BOOST_CHECK_EQUAL ( Items::GetCost(MOON_BASE) , 200 );
}
