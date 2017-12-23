#include "UnownedItems.hpp"
#include "SerializeUtility.hpp"
#include <boost/test/auto_unit_test.hpp>
#include <iostream>
#include <stdexcept>

void UnownedItemsStart(UnownedItems &i_uoi,ItemType i1,ItemType i2,ItemType i3,
                       ItemType i4,ItemType i5,ItemType i6,ItemType i7)
{
  i_uoi.Initialize(7);
  i_uoi.SetShipItem(0,i1);
  i_uoi.SetShipItem(1,i2);
  i_uoi.SetShipItem(2,i3);
  i_uoi.SetShipItem(3,i4);
  i_uoi.SetShipItem(4,i5);
  i_uoi.SetShipItem(5,i6);
  i_uoi.SetShipItem(6,i7);
}


BOOST_AUTO_TEST_CASE( UnownedItemsFillShip2 )
{
  size_t i;
  // check that a ship with one item empty in phase 1, not harsh, gets
  // one of the first four items
  UnownedItems uoi;

  size_t numiter = 1000;
  std::vector<int> ncount(5);

  for (i = 0 ; i < numiter ; ++i)
  {
    UnownedItemsStart(uoi,WAREHOUSE,NODULE,SCIENTISTS,WAREHOUSE,
                      NO_ITEM,MOON_BASE,OUTPOST);

    uoi.FillShip(1,false);

    if (uoi.GetShipItem(4) != DATA_LIBRARY &&
        uoi.GetShipItem(4) != WAREHOUSE &&
        uoi.GetShipItem(4) != NODULE &&
        uoi.GetShipItem(4) != HEAVY_EQUIPMENT)
    {
      BOOST_FAIL("illegal value for new item");
    }

    int j;
    for (j = 1; j <= 13 ; ++j)
    {
      if (j == uoi.GetShipItem(4))
      {
        BOOST_CHECK_EQUAL(uoi.GetItemCount((ItemType)j),5);
      }
      else
      {
        BOOST_CHECK_EQUAL(uoi.GetItemCount((ItemType)j),6);
      }
    }
    ncount[(size_t)uoi.GetShipItem(4)]++;
  }
  std::cout << "Single empty, phase 1, not harsh, stochastic: " << std::endl;
  std::cout << "       Real: ";
  for (i = 1 ; i <= 4 ; ++i)
  {
    std::cout << ncount[i] << "  ";
  }
  std::cout << std::endl;

  std::cout << "    Optimal: ";
  for (i = 1 ; i <= 4 ; ++i)
  {
    std::cout << numiter/4 << "  ";
  }
  std::cout << std::endl;

} 

BOOST_AUTO_TEST_CASE( UnownedItemsFillShip3 )
{
  size_t i;
  // check that a ship with one item empty in phase 2, not harsh, gets
  // one of the first ten items
  UnownedItems uoi;

  size_t numiter = 2000;
  std::vector<int> ncount(11);

  for (i = 0 ; i < numiter ; ++i)
  {
    UnownedItemsStart(uoi,WAREHOUSE,NODULE,SCIENTISTS,WAREHOUSE,
                      NO_ITEM,MOON_BASE,OUTPOST);

    uoi.FillShip(2,false);

    if (uoi.GetShipItem(4) == MOON_BASE ||
        uoi.GetShipItem(4) == SPACE_STATION ||
        uoi.GetShipItem(4) == PLANETARY_CRUISER ||
        uoi.GetShipItem(4) == NO_ITEM)
    {
      BOOST_FAIL("illegal value for new item");
    }

    int j;
    for (j = 1; j <= 13 ; ++j)
    {
      if (j == uoi.GetShipItem(4))
      {
        BOOST_CHECK_EQUAL(uoi.GetItemCount((ItemType)j),5);
      }
      else
      {
        BOOST_CHECK_EQUAL(uoi.GetItemCount((ItemType)j),6);
      }
    }
    ncount[(size_t)uoi.GetShipItem(4)]++;
  }
  std::cout << "Single empty, phase 2, not harsh, stochastic: " << std::endl;
  std::cout << "       Real: ";
  for (i = 1 ; i <= 10 ; ++i)
  {
    std::cout << ncount[i] << "  ";
  }
  std::cout << std::endl;
  std::cout << "    Optimal: ";
  for (i = 1 ; i <= 10 ; ++i)
  {
    std::cout << numiter/10 << "  ";
  }
  std::cout << std::endl;
} 


BOOST_AUTO_TEST_CASE( UnownedItemsFillShip4 )
{
  size_t i;
  // check that a ship with one item empty in phase 3, not harsh, gets
  // one of the items 2-13
  UnownedItems uoi;

  size_t numiter = 2000;
  std::vector<int> ncount(14);

  for (i = 0 ; i < numiter ; ++i)
  {
    UnownedItemsStart(uoi,WAREHOUSE,NODULE,SCIENTISTS,WAREHOUSE,
                      NO_ITEM,MOON_BASE,OUTPOST);

    uoi.FillShip(3,false);

    if (uoi.GetShipItem(4) == DATA_LIBRARY ||
        uoi.GetShipItem(4) == NO_ITEM)
    {
      BOOST_FAIL("illegal value for new item");
    }

    int j;
    for (j = 1; j <= 13 ; ++j)
    {
      if (j == uoi.GetShipItem(4))
      {
        BOOST_CHECK_EQUAL(uoi.GetItemCount((ItemType)j),5);
      }
      else
      {
        BOOST_CHECK_EQUAL(uoi.GetItemCount((ItemType)j),6);
      }
    }
    ncount[(size_t)uoi.GetShipItem(4)]++;
  }
  std::cout << "Single empty, phase 3, not harsh, stochastic: " << std::endl;
  std::cout << "       Real: ";
  for (i = 1 ; i <= 13 ; ++i)
  {
    std::cout << std::setw(3) << ncount[i] << "  ";
  }
  std::cout << std::endl;
  std::cout << "    Optimal: ";
  for (i = 1 ; i <= 13 ; ++i)
  {
    int num = numiter/12;
    if (i == 1) num = 0;
    std::cout << std::setw(3) << num << "  ";
  }
  std::cout << std::endl;
} 

// not harsh ship, missing items

BOOST_AUTO_TEST_CASE( UnownedItemsFillShip5 )
{
  size_t i;
  // check that a ship with one item empty in phase 1, not harsh, gets
  // one of the first four items
  UnownedItems uoi;

  size_t numiter = 1000;
  std::vector<int> ncount(5);

  for (i = 0 ; i < numiter ; ++i)
  {
    int j;
    UnownedItemsStart(uoi,WAREHOUSE,NODULE,SCIENTISTS,WAREHOUSE,
                      NO_ITEM,MOON_BASE,OUTPOST);

    for (j = 0 ; j < 6 ; ++j) uoi.DecrementItemCount(NODULE);

    BOOST_CHECK_EQUAL(uoi.GetItemCount(NODULE),0);


    uoi.FillShip(1,false);

    BOOST_CHECK(uoi.GetShipItem(4) != NODULE);

    if (uoi.GetShipItem(4) != DATA_LIBRARY &&
        uoi.GetShipItem(4) != WAREHOUSE &&
        uoi.GetShipItem(4) != HEAVY_EQUIPMENT)
    {
      BOOST_FAIL("illegal value for new item: " << uoi.GetShipItem(4) );
    }

    for (j = 1; j <= 13 ; ++j)
    {
      if ((ItemType)j == NODULE)
      {
        BOOST_CHECK_EQUAL(uoi.GetItemCount(NODULE),0);
      }
      else if (j == uoi.GetShipItem(4))
      {
        BOOST_CHECK_EQUAL(uoi.GetItemCount((ItemType)j),5);
      }
      else
      {
        BOOST_CHECK_EQUAL(uoi.GetItemCount((ItemType)j),6);
      }
    }
    ncount[(size_t)uoi.GetShipItem(4)]++;
  }
  std::cout << "Single empty/missings, phase 1, not harsh, stochastic: " << std::endl;
  std::cout << "       Real: ";
  for (i = 1 ; i <= 4 ; ++i)
  {
    std::cout << ncount[i] << "  ";
  }
  std::cout << std::endl;

  std::cout << "    Optimal: ";
  for (i = 1 ; i <= 4 ; ++i)
  {
    int num = numiter/4;
    if (i == 3) num = numiter/4 * 2;
    if (i == 4) num = 0;

    std::cout << num << "  ";
  }
  std::cout << std::endl;

} 

BOOST_AUTO_TEST_CASE( UnownedItemsFillShip5a )
{
  size_t i;
  // check that a ship with one item empty in phase 1, not harsh, gets
  // one of the first four items, using lowest item.
  UnownedItems uoi;

  size_t numiter = 1000;
  std::vector<int> ncount(5);

  for (i = 0 ; i < numiter ; ++i)
  {
    int j;
    UnownedItemsStart(uoi,WAREHOUSE,NODULE,SCIENTISTS,WAREHOUSE,
                      NO_ITEM,MOON_BASE,OUTPOST);

    for (j = 0 ; j < 6 ; ++j) uoi.DecrementItemCount(DATA_LIBRARY);

    BOOST_CHECK_EQUAL(uoi.GetItemCount(DATA_LIBRARY),0);

    uoi.FillShip(1,false);

    BOOST_CHECK(uoi.GetShipItem(4) != DATA_LIBRARY);

    if (uoi.GetShipItem(4) != DATA_LIBRARY &&
        uoi.GetShipItem(4) != WAREHOUSE &&
        uoi.GetShipItem(4) != NODULE &&
        uoi.GetShipItem(4) != HEAVY_EQUIPMENT)
    {
      BOOST_FAIL("illegal value for new item");
    }

    for (j = 1; j <= 13 ; ++j)
    {
      if ((ItemType)j == DATA_LIBRARY)
      {
        BOOST_CHECK_EQUAL(uoi.GetItemCount(DATA_LIBRARY),0);
      }
      else if (j == uoi.GetShipItem(4))
      {
        BOOST_CHECK_EQUAL(uoi.GetItemCount((ItemType)j),5);
      }
      else
      {
        BOOST_CHECK_EQUAL(uoi.GetItemCount((ItemType)j),6);
      }
    }
    ncount[(size_t)uoi.GetShipItem(4)]++;
  }
  std::cout << "Single empty/missings at bottom, phase 1, not harsh, stochastic: " << std::endl;
  std::cout << "       Real: ";
  for (i = 1 ; i <= 4 ; ++i)
  {
    std::cout << ncount[i] << "  ";
  }
  std::cout << std::endl;

  std::cout << "    Optimal: ";
  for (i = 1 ; i <= 4 ; ++i)
  {
    int num = numiter/3;
    if (i == 1) num = 0;

    std::cout << num << "  ";
  }
  std::cout << std::endl;

} 



BOOST_AUTO_TEST_CASE( UnownedItemsFillShip6 )
{
  size_t i;
  // check that a ship with one item empty in phase 2, not harsh, gets
  // one of the first ten items
  UnownedItems uoi;

  size_t numiter = 2000;
  std::vector<int> ncount(11);

  for (i = 0 ; i < numiter ; ++i)
  {
    UnownedItemsStart(uoi,WAREHOUSE,NODULE,SCIENTISTS,WAREHOUSE,
                      NO_ITEM,MOON_BASE,OUTPOST);

    int j;
    for (j = 0 ; j < 6 ; ++j) uoi.DecrementItemCount(ECOPLANTS);
    for (j = 0 ; j < 6 ; ++j) uoi.DecrementItemCount(LABORATORY);


    uoi.FillShip(2,false);

    if (uoi.GetShipItem(4) == MOON_BASE ||
        uoi.GetShipItem(4) == SPACE_STATION ||
        uoi.GetShipItem(4) == PLANETARY_CRUISER ||
        uoi.GetShipItem(4) == NO_ITEM)
    {
      BOOST_FAIL("illegal value for new item");
    }

    for (j = 1; j <= 13 ; ++j)
    {
      if ((ItemType)j == ECOPLANTS)
      {
        BOOST_CHECK_EQUAL(uoi.GetItemCount((ItemType)j),0);
      }      
      else if ((ItemType)j == LABORATORY)
      {
        BOOST_CHECK_EQUAL(uoi.GetItemCount((ItemType)j),0);
      }      
      else if (j == uoi.GetShipItem(4))
      {
        BOOST_CHECK_EQUAL(uoi.GetItemCount((ItemType)j),5);
      }
      else
      {
        BOOST_CHECK_EQUAL(uoi.GetItemCount((ItemType)j),6);
      }
    }
    ncount[(size_t)uoi.GetShipItem(4)]++;
  }
  std::cout << "Single empty/missings, phase 2, not harsh, stochastic: " << std::endl;
  std::cout << "       Real: ";
  for (i = 1 ; i <= 10 ; ++i)
  {
    std::cout << std::setw(3) << ncount[i] << "  ";
  }
  std::cout << std::endl;
  std::cout << "    Optimal: ";
  for (i = 1 ; i <= 10 ; ++i)
  {
    int num = numiter / 10;
    if (i == 9 || i == 8) num = 0;
    if (i == 7) num = numiter / 10 * 3;
    std::cout << std::setw(3) << num << "  ";
  }
  std::cout << std::endl;
} 


BOOST_AUTO_TEST_CASE( UnownedItemsFillShip7 )
{
  size_t i;
  // check that a ship with one item empty in phase 3, not harsh, gets
  // one of the items 2-13
  UnownedItems uoi;

  size_t numiter = 2000;
  std::vector<int> ncount(14);

  for (i = 0 ; i < numiter ; ++i)
  {
    UnownedItemsStart(uoi,WAREHOUSE,NODULE,SCIENTISTS,WAREHOUSE,
                      NO_ITEM,MOON_BASE,OUTPOST);

    int j;
    for (j = 0 ; j < 6 ; ++j) uoi.DecrementItemCount(WAREHOUSE);
    for (j = 0 ; j < 6 ; ++j) uoi.DecrementItemCount(HEAVY_EQUIPMENT);

    uoi.FillShip(3,false);

    if (uoi.GetShipItem(4) == NO_ITEM)
    {
      BOOST_FAIL("illegal value for new item");
    }

    for (j = 1; j <= 13 ; ++j)
    {
      if ((ItemType)j == WAREHOUSE)
      {
        BOOST_CHECK_EQUAL(uoi.GetItemCount((ItemType)j),0);
      }      
      else if ((ItemType)j == HEAVY_EQUIPMENT)
      {
        BOOST_CHECK_EQUAL(uoi.GetItemCount((ItemType)j),0);
      }      
      else if (j == uoi.GetShipItem(4))
      {
        BOOST_CHECK_EQUAL(uoi.GetItemCount((ItemType)j),5);
      }
      else
      {
        BOOST_CHECK_EQUAL(uoi.GetItemCount((ItemType)j),6);
      }
    }
    ncount[(size_t)uoi.GetShipItem(4)]++;
  }
  std::cout << "Single empty/missings, phase 3, not harsh, stochastic: " << std::endl;
  std::cout << "       Real: ";
  for (i = 1 ; i <= 13 ; ++i)
  {
    std::cout << std::setw(3) << ncount[i] << "  ";
  }
  std::cout << std::endl;
  std::cout << "    Optimal: ";
  for (i = 1 ; i <= 13 ; ++i)
  {
    int num = numiter/12;
    if (i == 1) num *= 2;
    if (i == 2 || i == 3) num = 0;

    std::cout << std::setw(3) << num << "  ";
  }
  std::cout << std::endl;
} 

// harsh ship

BOOST_AUTO_TEST_CASE( UnownedItemsFillShip8 )
{
  size_t i;
  // check that a ship with one item empty in phase 1, harsh, gets
  // one of the first four items
  UnownedItems uoi;

  size_t numiter = 1000;
  std::vector<int> ncount(5);

  for (i = 0 ; i < numiter ; ++i)
  {
    int j;
    UnownedItemsStart(uoi,WAREHOUSE,NODULE,SCIENTISTS,WAREHOUSE,
                      NO_ITEM,MOON_BASE,OUTPOST);

    for (j = 0 ; j < 6 ; ++j) uoi.DecrementItemCount(NODULE);

    BOOST_CHECK_EQUAL(uoi.GetItemCount(NODULE),0);


    uoi.FillShip(1,true);

    BOOST_CHECK(uoi.GetShipItem(4) != NODULE);

    if (uoi.GetShipItem(4) != DATA_LIBRARY &&
        uoi.GetShipItem(4) != WAREHOUSE &&
        uoi.GetShipItem(4) != NO_ITEM &&
        uoi.GetShipItem(4) != HEAVY_EQUIPMENT)
    {
      BOOST_FAIL("illegal value for new item: " << uoi.GetShipItem(4));
    }

    for (j = 1; j <= 13 ; ++j)
    {
      if ((ItemType)j == NODULE)
      {
        BOOST_CHECK_EQUAL(uoi.GetItemCount(NODULE),0);
      }
      else if (j == uoi.GetShipItem(4))
      {
        BOOST_CHECK_EQUAL(uoi.GetItemCount((ItemType)j),5);
      }
      else
      {
        BOOST_CHECK_EQUAL(uoi.GetItemCount((ItemType)j),6);
      }
    }
    ncount[(size_t)uoi.GetShipItem(4)]++;
  }
  std::cout << "Single empty/missings, phase 1, harsh, stochastic: " << std::endl;
  std::cout << "       Real: ";
  for (i = 0 ; i <= 4 ; ++i)
  {
    std::cout << ncount[i] << "  ";
  }
  std::cout << std::endl;

  std::cout << "    Optimal: ";
  for (i = 0 ; i <= 4 ; ++i)
  {
    int num = numiter/4;
    if (i == 4) num = 0;

    std::cout << num << "  ";
  }
  std::cout << std::endl;

} 

  

  
BOOST_AUTO_TEST_CASE( UnownedItemsFillShip9 )
{
  size_t i;
  // check that a ship with one item empty in phase 2, harsh, gets
  // one of the first ten items
  UnownedItems uoi;

  size_t numiter = 2000;
  std::vector<int> ncount(11);

  for (i = 0 ; i < numiter ; ++i)
  {
    UnownedItemsStart(uoi,WAREHOUSE,NODULE,SCIENTISTS,WAREHOUSE,
                      NO_ITEM,MOON_BASE,OUTPOST);

    int j;
    for (j = 0 ; j < 6 ; ++j) uoi.DecrementItemCount(ECOPLANTS);
    for (j = 0 ; j < 6 ; ++j) uoi.DecrementItemCount(LABORATORY);


    uoi.FillShip(2,true);

    if (uoi.GetShipItem(4) == MOON_BASE ||
        uoi.GetShipItem(4) == SPACE_STATION ||
        uoi.GetShipItem(4) == PLANETARY_CRUISER ||
        uoi.GetShipItem(4) == ECOPLANTS ||
        uoi.GetShipItem(4) == LABORATORY)
    {
      BOOST_FAIL("illegal value for new item: " << uoi.GetShipItem(4));
    }

    for (j = 1; j <= 13 ; ++j)
    {
      if ((ItemType)j == ECOPLANTS)
      {
        BOOST_CHECK_EQUAL(uoi.GetItemCount((ItemType)j),0);
      }      
      else if ((ItemType)j == LABORATORY)
      {
        BOOST_CHECK_EQUAL(uoi.GetItemCount((ItemType)j),0);
      }      
      else if (j == uoi.GetShipItem(4))
      {
        BOOST_CHECK_EQUAL(uoi.GetItemCount((ItemType)j),5);
      }
      else
      {
        BOOST_CHECK_EQUAL(uoi.GetItemCount((ItemType)j),6);
      }
    }
    ncount[(size_t)uoi.GetShipItem(4)]++;
  }
  std::cout << "Single empty/missings, phase 2, harsh, stochastic: " << std::endl;
  std::cout << "       Real: ";
  for (i = 0 ; i <= 10 ; ++i)
  {
    std::cout << std::setw(3) << ncount[i] << "  ";
  }
  std::cout << std::endl;
  std::cout << "    Optimal: ";
  for (i = 0 ; i <= 10 ; ++i)
  {
    int num = numiter / 10;
    if (i == 9 || i == 8) num = 0;
    if (i == 0) num = numiter / 10 * 2;
    std::cout << std::setw(3) << num << "  ";
  }
  std::cout << std::endl;
} 


BOOST_AUTO_TEST_CASE( UnownedItemsFillShip10 )
{
  size_t i;
  // check that a ship with one item empty in phase 3, harsh, gets
  // one of the items 2-13
  UnownedItems uoi;

  size_t numiter = 2000;
  std::vector<int> ncount(14);

  for (i = 0 ; i < numiter ; ++i)
  {
    UnownedItemsStart(uoi,WAREHOUSE,NODULE,SCIENTISTS,WAREHOUSE,
                      NO_ITEM,MOON_BASE,OUTPOST);

    int j;
    for (j = 0 ; j < 6 ; ++j) uoi.DecrementItemCount(WAREHOUSE);
    for (j = 0 ; j < 6 ; ++j) uoi.DecrementItemCount(HEAVY_EQUIPMENT);
    for (j = 0 ; j < 6 ; ++j) uoi.DecrementItemCount(OUTPOST);

    uoi.FillShip(3,true);

    if (uoi.GetShipItem(4) == WAREHOUSE ||
        uoi.GetShipItem(4) == HEAVY_EQUIPMENT ||
        uoi.GetShipItem(4) == DATA_LIBRARY ||
        uoi.GetShipItem(4) == OUTPOST)
    {
      BOOST_FAIL("illegal value for new item");
    }

    for (j = 1; j <= 13 ; ++j)
    {
      if ((ItemType)j == WAREHOUSE)
      {
        BOOST_CHECK_EQUAL(uoi.GetItemCount((ItemType)j),0);
      }      
      else if ((ItemType)j == HEAVY_EQUIPMENT)
      {
        BOOST_CHECK_EQUAL(uoi.GetItemCount((ItemType)j),0);
      }      
      else if ((ItemType)j == OUTPOST)
      {
        BOOST_CHECK_EQUAL(uoi.GetItemCount((ItemType)j),0);
      }      
      else if (j == uoi.GetShipItem(4))
      {
        BOOST_CHECK_EQUAL(uoi.GetItemCount((ItemType)j),5);
      }
      else
      {
        BOOST_CHECK_EQUAL(uoi.GetItemCount((ItemType)j),6);
      }
    }
    ncount[(size_t)uoi.GetShipItem(4)]++;
  }
  std::cout << "Single empty/missings, phase 3, harsh, stochastic: " << std::endl;
  std::cout << "       Real: ";
  for (i = 0 ; i <= 13 ; ++i)
  {
    std::cout << std::setw(3) << ncount[i] << "  ";
  }
  std::cout << std::endl;
  std::cout << "    Optimal: ";
  for (i = 0 ; i <= 13 ; ++i)
  {
    int num = numiter/12;
    if (i == 0) num *= 3;
    if (i == 1 || i == 2 || i == 3 || i == 10) num = 0;

    std::cout << std::setw(3) << num << "  ";
  }
  std::cout << std::endl;
} 


  
BOOST_AUTO_TEST_CASE( UnownedItemsFillShip11 )
{
  size_t i;
  // phase 2, not harsh, making sure that the rule that no more
  // than half the ship may contain a single item.
  // also validate, stochastically, that when this case occurs, 
  // that the dice are rerolled instead of descending one.
  UnownedItems uoi;

  size_t numiter = 2000;
  std::vector<int> ncount(11);

  for (i = 0 ; i < numiter ; ++i)
  {
    UnownedItemsStart(uoi,WAREHOUSE,NODULE,SCIENTISTS,WAREHOUSE,
                      NO_ITEM,MOON_BASE,WAREHOUSE);

    int j;

    uoi.FillShip(2,false);

    if (uoi.GetShipItem(4) == MOON_BASE ||
        uoi.GetShipItem(4) == SPACE_STATION ||
        uoi.GetShipItem(4) == PLANETARY_CRUISER ||
        uoi.GetShipItem(4) == WAREHOUSE || 
        uoi.GetShipItem(4) == NO_ITEM)
    {
      BOOST_FAIL("illegal value for new item: " << uoi.GetShipItem(4));
    }

    for (j = 1; j <= 13 ; ++j)
    {
      if (j == uoi.GetShipItem(4))
      {
        BOOST_CHECK_EQUAL(uoi.GetItemCount((ItemType)j),5);
      }
      else
      {
        BOOST_CHECK_EQUAL(uoi.GetItemCount((ItemType)j),6);
      }
    }
    ncount[(size_t)uoi.GetShipItem(4)]++;
  }
  std::cout << "Phase 2, not harsh, max ship stochastic: " << std::endl;
  std::cout << "       Real: ";
  for (i = 0 ; i <= 10 ; ++i)
  {
    std::cout << std::setw(3) << ncount[i] << "  ";
  }
  std::cout << std::endl;
  std::cout << "    Optimal: ";
  for (i = 0 ; i <= 10 ; ++i)
  {
    int num = numiter / 9;
    if (i == 0 || i == 2) num = 0;
    std::cout << std::setw(3) << num << "  ";
  }
  std::cout << std::endl;
} 

BOOST_AUTO_TEST_CASE( TestSomeExtraStocking )
{

  UnownedItems throwuoi;
  BOOST_CHECK_THROW( throwuoi.Initialize(3,(StockMechanism)4) , std::out_of_range );

  std::vector<int> twocountv(14);
  std::vector<int> threecountv(14);
  std::vector<int> numextracountv(14);

  std::cout << "UnownedItems Extra Stocking Stochastic:" << std::endl;

  int i;
  int j;
  int loopsize = 100;

  for (i = 0 ; i < loopsize ; ++i)
  {
    int twocount = 0;
    int threecount = 0;
    UnownedItems uoi;
    uoi.Initialize(3,SOME_ONE_EXTRA);

    for (j = 1 ; j <= 13 ; ++j)
    {
      int ic = uoi.GetItemCount((ItemType)j);
      BOOST_CHECK( ic == 2 || ic == 3);
      if (ic == 2) { twocount++; twocountv[j]++; }
      if (ic == 3) { threecount++; threecountv[j]++; }
    }

    BOOST_CHECK_EQUAL( 13 , twocount + threecount );
    BOOST_CHECK( threecount >= 4 );
    BOOST_CHECK( threecount <= 10 );
    numextracountv[threecount]++;
  }    

  std::cout << "   Item: ";
  for (j = 1 ; j <= 13 ; ++j)
  {
    std::cout << std::setw(3) << j << " ";
  }
  std::cout << std::endl;

  std::cout << " Real 2: ";
  for (j = 1 ; j <= 13 ; ++j)
  {
    std::cout << std::setw(3) << twocountv[j] << " ";
  }
  std::cout << std::endl;

  std::cout << " Real 3: ";
  for (j = 1 ; j <= 13 ; ++j)
  {
    std::cout << std::setw(3) << threecountv[j] << " ";
  }
  std::cout << std::endl;


  std::cout << "Optimal: ";
  for (j = 1 ; j <= 13 ; ++j)
  {
    std::cout << std::setw(3) << loopsize/2 << " ";
  }
  std::cout << std::endl;

  std::cout << "UnownedItems Extra Stocking Stochastic Extra Count:" << std::endl;

  std::cout << "  Count: ";
  for (j = 0 ; j <= 13 ; ++j)
  {
    std::cout << std::setw(3) << j << " ";
  }
  std::cout << std::endl;


  std::cout << "   Real: ";
  for (j = 0 ; j <= 13 ; ++j)
  {
    std::cout << std::setw(3) << numextracountv[j] << " ";
  }
  std::cout << std::endl;
}

int g_iterations = 1000;

void HarshnessTest(int i_harshness)
{
  std::vector<int> io_list(11);
  int i;
  for (i = 0 ; i < g_iterations ; ++i)
  {
    UnownedItems uoi;
    uoi.Initialize(5);
    int i;
    for (i = 0 ; i < 4 ; ++i)
    {
      uoi.DecrementItemCount((ItemType)1);
      uoi.DecrementItemCount((ItemType)3);
      uoi.DecrementItemCount((ItemType)5);
      uoi.DecrementItemCount((ItemType)7);
      uoi.DecrementItemCount((ItemType)9);
    }

    uoi.SetShipItem(0,NO_ITEM);
    uoi.SetShipItem(1,MOON_BASE);
    uoi.SetShipItem(2,MOON_BASE);
    uoi.SetShipItem(3,SPACE_STATION);
    uoi.SetShipItem(4,SPACE_STATION);

    uoi.FillShip(2,i_harshness);
    
    io_list[uoi.GetShipItem(0)]++;
  }

  std::cout << "Level " << i_harshness << ": " << std::endl;
  int emptydenom = 1;
  
  for (i = 0 ; i < i_harshness; ++i)
  {
    emptydenom *= 2;
  }

  // 1x 2 3x 4 5x 6 7x 8 9x 10
  int emptytotal = g_iterations / emptydenom;
  int fulltotal = g_iterations - emptytotal;
  int emptycount = emptytotal / 5;
  int fullcount = fulltotal / 5;

  std::cout << "  Opml: " ;
  for (i = 1 ; i <= 10 ; ++i)
  {
    if (i % 2 == 1)
    {
      std::cout << std::setw(3) << emptycount << ' ';
    }
    else
    {
      std::cout << std::setw(3) << fullcount << ' ';
    }
  }

  std::cout << std::endl;
  std::cout << "  Real: " ;
  for (i = 1 ; i <= 10 ; ++i)
  {
    std::cout << std::setw(3) << io_list[i] << ' ';
  }
  std::cout << std::endl;
}
    

BOOST_AUTO_TEST_CASE( TestLessHarshness )
{
  std::cout << "Variable Harshness: " << std::endl;
  
  HarshnessTest(1);
  HarshnessTest(2);
  HarshnessTest(3);
  HarshnessTest(4);
  HarshnessTest(5);
}
  


  
