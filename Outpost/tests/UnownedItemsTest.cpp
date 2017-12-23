#include "UnownedItems.hpp"
#include "SerializeUtility.hpp"
#include <boost/test/auto_unit_test.hpp>
#include <iostream>
#include <stdexcept>
#include "Player.hpp"


BOOST_AUTO_TEST_CASE( UnownedItemsConstructor )
{

  UnownedItems uoi;

  BOOST_CHECK_EQUAL( uoi.Initialized() , false);
  BOOST_CHECK_EQUAL( uoi.GetBidIndex() , NO_BID_INDEX);

  uoi.Initialize(3);

  BOOST_CHECK_EQUAL( uoi.Initialized() , true);
  BOOST_CHECK_EQUAL( uoi.GetBidIndex() , NO_BID_INDEX);

  BOOST_CHECK_THROW(uoi.GetItemCount(NO_ITEM),std::out_of_range);
  BOOST_CHECK_THROW(uoi.GetItemCount((ItemType)-1),std::out_of_range);
  BOOST_CHECK_THROW(uoi.GetItemCount((ItemType)14),std::out_of_range);

  size_t i;
  for (i = 1 ; i <= 13 ; ++i)
  {
    BOOST_CHECK_EQUAL(uoi.GetItemCount((ItemType)i),2);
  }

  BOOST_CHECK_THROW(uoi.DecrementItemCount((ItemType)14),std::out_of_range);
  BOOST_CHECK_THROW(uoi.DecrementItemCount(NO_ITEM),std::out_of_range);

  uoi.DecrementItemCount(WAREHOUSE);
  uoi.DecrementItemCount(OUTPOST);
  uoi.DecrementItemCount(MOON_BASE);
  uoi.DecrementItemCount(OUTPOST);

  for (i = 1 ; i <= 13 ; ++i)
  {
    switch((ItemType)i)
    {
    case WAREHOUSE:
    case MOON_BASE:
      BOOST_CHECK_EQUAL(uoi.GetItemCount((ItemType)i),1);
      break;
    case OUTPOST:
      BOOST_CHECK_EQUAL(uoi.GetItemCount((ItemType)i),0);
      break;
    default:
      BOOST_CHECK_EQUAL(uoi.GetItemCount((ItemType)i),2);
      break;
    }
  }

  BOOST_CHECK_THROW(uoi.GetShipItem(3),std::out_of_range);
  BOOST_CHECK_THROW(uoi.SetShipItem(3,NODULE),std::out_of_range);


  for (i = 0 ; i < 3 ; ++i)
  {
    BOOST_CHECK_EQUAL(uoi.GetShipItem(i),NO_ITEM);
  }

  uoi.SetShipItem(2,WAREHOUSE);
  uoi.SetShipItem(1,SCIENTISTS);
  uoi.SetBidIndex(2);

  BOOST_CHECK_EQUAL(uoi.GetShipItem(0),NO_ITEM);
  BOOST_CHECK_EQUAL(uoi.GetShipItem(1),SCIENTISTS);
  BOOST_CHECK_EQUAL(uoi.GetShipItem(2),WAREHOUSE);
  BOOST_CHECK_EQUAL(uoi.GetBidIndex() , (size_t)2);

  std::set<size_t> items = uoi.GetNonEmptyShipIndices();
  BOOST_CHECK_EQUAL(items.size() , (size_t)2);
  BOOST_CHECK( items.find(1) != items.end());
  BOOST_CHECK( items.find(2) != items.end());

  BOOST_CHECK_THROW( uoi.GetPurchaseInfo(3) , std::out_of_range );

  for (i = 0 ; i < 3 ; ++i)
  {
    BOOST_CHECK_EQUAL( NO_PLAYER_INDEX , uoi.GetPurchaseInfo(i).m_playerindex );
    BOOST_CHECK_EQUAL( NO_ITEM , uoi.GetPurchaseInfo(i).m_type );
    BOOST_CHECK_EQUAL( -1 , uoi.GetPurchaseInfo(i).m_cost );
  }

  BOOST_CHECK_THROW( uoi.SetPurchased(3,NODULE,1,26) , std::out_of_range);

  for (i = 0 ; i < 3 ; ++i)
  {
    BOOST_CHECK_EQUAL( NO_PLAYER_INDEX , uoi.GetPurchaseInfo(i).m_playerindex );
    BOOST_CHECK_EQUAL( NO_ITEM , uoi.GetPurchaseInfo(i).m_type );
    BOOST_CHECK_EQUAL( -1 , uoi.GetPurchaseInfo(i).m_cost );
  }

  BOOST_CHECK_THROW( uoi.SetPurchased(2,NODULE,3,25) , std::out_of_range);

  for (i = 0 ; i < 3 ; ++i)
  {
    BOOST_CHECK_EQUAL( NO_PLAYER_INDEX , uoi.GetPurchaseInfo(i).m_playerindex );
    BOOST_CHECK_EQUAL( NO_ITEM , uoi.GetPurchaseInfo(i).m_type );
    BOOST_CHECK_EQUAL( -1 , uoi.GetPurchaseInfo(i).m_cost );
  }

  BOOST_CHECK_NO_THROW( uoi.SetPurchased(2,NODULE,1,25) );
  BOOST_CHECK_NO_THROW( uoi.SetPurchased(0,ORBITAL_LAB, 0, 150) );

  BOOST_CHECK_EQUAL( (size_t)0 , uoi.GetPurchaseInfo(0).m_playerindex );
  BOOST_CHECK_EQUAL( ORBITAL_LAB , uoi.GetPurchaseInfo(0).m_type );
  BOOST_CHECK_EQUAL( 150 , uoi.GetPurchaseInfo(0).m_cost );

  BOOST_CHECK_EQUAL( NO_PLAYER_INDEX , uoi.GetPurchaseInfo(1).m_playerindex );
  BOOST_CHECK_EQUAL( NO_ITEM , uoi.GetPurchaseInfo(1).m_type );
  BOOST_CHECK_EQUAL( -1 , uoi.GetPurchaseInfo(1).m_cost );

  BOOST_CHECK_EQUAL( (size_t)1 , uoi.GetPurchaseInfo(2).m_playerindex );
  BOOST_CHECK_EQUAL( NODULE , uoi.GetPurchaseInfo(2).m_type );
  BOOST_CHECK_EQUAL( 25 , uoi.GetPurchaseInfo(2).m_cost );

  UnownedItems uoi2;

  BOOST_CHECK_EQUAL( uoi2.Initialized() , false);

  SerialTransfer(uoi,uoi2);

  BOOST_CHECK_EQUAL( uoi2.Initialized() , true);

  for (i = 1 ; i <= 13 ; ++i)
  {
    switch((ItemType)i)
    {
    case WAREHOUSE:
    case MOON_BASE:
      BOOST_CHECK_EQUAL(uoi2.GetItemCount((ItemType)i),1);
      break;
    case OUTPOST:
      BOOST_CHECK_EQUAL(uoi2.GetItemCount((ItemType)i),0);
      break;
    default:
      BOOST_CHECK_EQUAL(uoi2.GetItemCount((ItemType)i),2);
      break;
    }
  }

  items = uoi2.GetNonEmptyShipIndices();
  BOOST_CHECK_EQUAL(items.size() , (size_t)2);
  BOOST_CHECK( items.find(1) != items.end());
  BOOST_CHECK( items.find(2) != items.end());


  BOOST_CHECK_EQUAL(uoi2.GetShipItem(0),NO_ITEM);
  BOOST_CHECK_EQUAL(uoi2.GetShipItem(1),SCIENTISTS);
  BOOST_CHECK_EQUAL(uoi2.GetShipItem(2),WAREHOUSE);
  BOOST_CHECK_EQUAL(uoi2.GetBidIndex() , (size_t)2);

  BOOST_CHECK_EQUAL( (size_t)0 , uoi2.GetPurchaseInfo(0).m_playerindex );
  BOOST_CHECK_EQUAL( ORBITAL_LAB , uoi2.GetPurchaseInfo(0).m_type );
  BOOST_CHECK_EQUAL( 150 , uoi2.GetPurchaseInfo(0).m_cost );

  BOOST_CHECK_EQUAL( NO_PLAYER_INDEX , uoi2.GetPurchaseInfo(1).m_playerindex );
  BOOST_CHECK_EQUAL( NO_ITEM , uoi2.GetPurchaseInfo(1).m_type );
  BOOST_CHECK_EQUAL( -1 , uoi2.GetPurchaseInfo(1).m_cost );

  BOOST_CHECK_EQUAL( (size_t)1 , uoi2.GetPurchaseInfo(2).m_playerindex );
  BOOST_CHECK_EQUAL( NODULE , uoi2.GetPurchaseInfo(2).m_type );
  BOOST_CHECK_EQUAL( 25 , uoi2.GetPurchaseInfo(2).m_cost );

  uoi2.ClearPurchases();

  for (i = 0 ; i < 3 ; ++i)
  {
    BOOST_CHECK_EQUAL( NO_PLAYER_INDEX , uoi2.GetPurchaseInfo(i).m_playerindex );
    BOOST_CHECK_EQUAL( NO_ITEM , uoi2.GetPurchaseInfo(i).m_type );
    BOOST_CHECK_EQUAL( -1 , uoi2.GetPurchaseInfo(i).m_cost );
  }



}

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

BOOST_AUTO_TEST_CASE( GetPhaseLimits )
{
  UnownedItems uoi;
  uoi.Initialize(6);

  BOOST_CHECK_THROW(uoi.GetPhaseLimits(0,false),std::out_of_range);
  BOOST_CHECK_THROW(uoi.GetPhaseLimits(0,true),std::out_of_range);
  BOOST_CHECK_THROW(uoi.GetPhaseLimits(4,false),std::out_of_range);
  BOOST_CHECK_THROW(uoi.GetPhaseLimits(4,true),std::out_of_range);

  BOOST_CHECK_EQUAL(uoi.GetPhaseLimits(1,false).m_phasebottom , 1);
  BOOST_CHECK_EQUAL(uoi.GetPhaseLimits(1,false).m_phasetop    , 4);
  BOOST_CHECK_EQUAL(uoi.GetPhaseLimits(1,false).m_rollbottom  , 1);
  BOOST_CHECK_EQUAL(uoi.InPhase(1,false,1) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(1,false,2) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(1,false,3) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(1,false,4) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(1,false,5) , false );
  BOOST_CHECK_EQUAL(uoi.InPhase(1,false,6) , false );
  BOOST_CHECK_EQUAL(uoi.InPhase(1,false,7) , false );
  BOOST_CHECK_EQUAL(uoi.InPhase(1,false,8) , false );
  BOOST_CHECK_EQUAL(uoi.InPhase(1,false,9) , false );
  BOOST_CHECK_EQUAL(uoi.InPhase(1,false,10) , false );
  BOOST_CHECK_EQUAL(uoi.InPhase(1,false,11) , false );
  BOOST_CHECK_EQUAL(uoi.InPhase(1,false,12) , false );
  BOOST_CHECK_EQUAL(uoi.InPhase(1,false,13) , false );

  BOOST_CHECK_EQUAL(uoi.GetPhaseLimits(1,true ).m_phasebottom , 1);
  BOOST_CHECK_EQUAL(uoi.GetPhaseLimits(1,true ).m_phasetop    , 4);
  BOOST_CHECK_EQUAL(uoi.GetPhaseLimits(1,true ).m_rollbottom  , 1);
  BOOST_CHECK_EQUAL(uoi.InPhase(1,true,1) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(1,true,2) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(1,true,3) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(1,true,4) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(1,true,5) , false );
  BOOST_CHECK_EQUAL(uoi.InPhase(1,true,6) , false );
  BOOST_CHECK_EQUAL(uoi.InPhase(1,true,7) , false );
  BOOST_CHECK_EQUAL(uoi.InPhase(1,true,8) , false );
  BOOST_CHECK_EQUAL(uoi.InPhase(1,true,9) , false );
  BOOST_CHECK_EQUAL(uoi.InPhase(1,true,10) , false );
  BOOST_CHECK_EQUAL(uoi.InPhase(1,true,11) , false );
  BOOST_CHECK_EQUAL(uoi.InPhase(1,true,12) , false );
  BOOST_CHECK_EQUAL(uoi.InPhase(1,true,13) , false );

  BOOST_CHECK_EQUAL(uoi.GetPhaseLimits(2,false).m_phasebottom , 1);
  BOOST_CHECK_EQUAL(uoi.GetPhaseLimits(2,false).m_phasetop    , 10);
  BOOST_CHECK_EQUAL(uoi.GetPhaseLimits(2,false).m_rollbottom  , 1);
  BOOST_CHECK_EQUAL(uoi.InPhase(2,false,1) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(2,false,2) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(2,false,3) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(2,false,4) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(2,false,5) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(2,false,6) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(2,false,7) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(2,false,8) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(2,false,9) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(2,false,10) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(2,false,11) , false );
  BOOST_CHECK_EQUAL(uoi.InPhase(2,false,12) , false );
  BOOST_CHECK_EQUAL(uoi.InPhase(2,false,13) , false );

  BOOST_CHECK_EQUAL(uoi.GetPhaseLimits(2,true ).m_phasebottom , 1);
  BOOST_CHECK_EQUAL(uoi.GetPhaseLimits(2,true ).m_phasetop    , 10);
  BOOST_CHECK_EQUAL(uoi.GetPhaseLimits(2,true ).m_rollbottom  , 1);
  BOOST_CHECK_EQUAL(uoi.InPhase(2,true,1) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(2,true,2) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(2,true,3) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(2,true,4) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(2,true,5) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(2,true,6) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(2,true,7) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(2,true,8) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(2,true,9) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(2,true,10) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(2,true,11) , false );
  BOOST_CHECK_EQUAL(uoi.InPhase(2,true,12) , false );
  BOOST_CHECK_EQUAL(uoi.InPhase(2,true,13) , false );

  BOOST_CHECK_EQUAL(uoi.GetPhaseLimits(3,false).m_phasebottom , 1);
  BOOST_CHECK_EQUAL(uoi.GetPhaseLimits(3,false).m_phasetop    , 13);
  BOOST_CHECK_EQUAL(uoi.GetPhaseLimits(3,false).m_rollbottom  , 2);
  BOOST_CHECK_EQUAL(uoi.InPhase(3,false,1) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(3,false,2) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(3,false,3) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(3,false,4) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(3,false,5) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(3,false,6) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(3,false,7) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(3,false,8) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(3,false,9) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(3,false,10) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(3,false,11) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(3,false,12) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(3,false,13) , true );

  BOOST_CHECK_EQUAL(uoi.GetPhaseLimits(3,true ).m_phasebottom , 2);
  BOOST_CHECK_EQUAL(uoi.GetPhaseLimits(3,true ).m_phasetop    , 13);
  BOOST_CHECK_EQUAL(uoi.GetPhaseLimits(3,true ).m_rollbottom  , 2);
  BOOST_CHECK_EQUAL(uoi.InPhase(3,true,1) , false );
  BOOST_CHECK_EQUAL(uoi.InPhase(3,true,2) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(3,true,3) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(3,true,4) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(3,true,5) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(3,true,6) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(3,true,7) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(3,true,8) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(3,true,9) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(3,true,10) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(3,true,11) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(3,true,12) , true );
  BOOST_CHECK_EQUAL(uoi.InPhase(3,true,13) , true );
}





BOOST_AUTO_TEST_CASE( UnownedItemsFillShip1 )
{
  size_t i;
  // check that a full ship gets no updates.
  UnownedItems uoi;
  UnownedItemsStart(uoi,WAREHOUSE,NODULE,SCIENTISTS,WAREHOUSE,
                    ORBITAL_LAB,MOON_BASE,OUTPOST);

  for (i = 1 ; i <= 13 ; ++i)
  {
    BOOST_CHECK_EQUAL(uoi.GetItemCount((ItemType)i),6);
  }

  BOOST_CHECK_EQUAL(uoi.GetShipItem(0),WAREHOUSE);
  BOOST_CHECK_EQUAL(uoi.GetShipItem(1),NODULE);
  BOOST_CHECK_EQUAL(uoi.GetShipItem(2),SCIENTISTS);
  BOOST_CHECK_EQUAL(uoi.GetShipItem(3),WAREHOUSE);
  BOOST_CHECK_EQUAL(uoi.GetShipItem(4),ORBITAL_LAB);
  BOOST_CHECK_EQUAL(uoi.GetShipItem(5),MOON_BASE);
  BOOST_CHECK_EQUAL(uoi.GetShipItem(6),OUTPOST);


  BOOST_CHECK_THROW(uoi.FillShip(0,false),std::out_of_range);
  BOOST_CHECK_THROW(uoi.FillShip(0,true),std::out_of_range);
  BOOST_CHECK_THROW(uoi.FillShip(4,false),std::out_of_range);
  BOOST_CHECK_THROW(uoi.FillShip(4,true),std::out_of_range);



  for (i = 1 ; i <= 13 ; ++i)
  {
    BOOST_CHECK_EQUAL(uoi.GetItemCount((ItemType)i),6);
  }

  BOOST_CHECK_EQUAL(uoi.GetShipItem(0),WAREHOUSE);
  BOOST_CHECK_EQUAL(uoi.GetShipItem(1),NODULE);
  BOOST_CHECK_EQUAL(uoi.GetShipItem(2),SCIENTISTS);
  BOOST_CHECK_EQUAL(uoi.GetShipItem(3),WAREHOUSE);
  BOOST_CHECK_EQUAL(uoi.GetShipItem(4),ORBITAL_LAB);
  BOOST_CHECK_EQUAL(uoi.GetShipItem(5),MOON_BASE);
  BOOST_CHECK_EQUAL(uoi.GetShipItem(6),OUTPOST);

  uoi.FillShip(1,false);
  
  for (i = 1 ; i <= 13 ; ++i)
  {
    BOOST_CHECK_EQUAL(uoi.GetItemCount((ItemType)i),6);
  }

  BOOST_CHECK_EQUAL(uoi.GetShipItem(0),WAREHOUSE);
  BOOST_CHECK_EQUAL(uoi.GetShipItem(1),NODULE);
  BOOST_CHECK_EQUAL(uoi.GetShipItem(2),SCIENTISTS);
  BOOST_CHECK_EQUAL(uoi.GetShipItem(3),WAREHOUSE);
  BOOST_CHECK_EQUAL(uoi.GetShipItem(4),ORBITAL_LAB);
  BOOST_CHECK_EQUAL(uoi.GetShipItem(5),MOON_BASE);
  BOOST_CHECK_EQUAL(uoi.GetShipItem(6),OUTPOST);
}



BOOST_AUTO_TEST_CASE( UnownedItemsFillShip12 )
{
  UnownedItems uoi;

  // make sure that we can get multiple items.
  UnownedItemsStart(uoi,WAREHOUSE,WAREHOUSE,SCIENTISTS,WAREHOUSE,
                    NO_ITEM,NO_ITEM,NO_ITEM);

  uoi.FillShip(1,false);

  BOOST_CHECK( uoi.GetShipItem(4) != NO_ITEM );
  BOOST_CHECK( uoi.GetShipItem(5) != NO_ITEM );
  BOOST_CHECK( uoi.GetShipItem(6) != NO_ITEM );

  BOOST_CHECK( uoi.GetShipItem(4) != WAREHOUSE );
  BOOST_CHECK( uoi.GetShipItem(5) != WAREHOUSE );
  BOOST_CHECK( uoi.GetShipItem(6) != WAREHOUSE );
}

BOOST_AUTO_TEST_CASE( UnownedItemsFillShip13 )
{
  UnownedItems uoi;

  // make sure that we can give up when there are no items to get.
  UnownedItemsStart(uoi,WAREHOUSE,WAREHOUSE,SCIENTISTS,WAREHOUSE,
                    NO_ITEM,NO_ITEM,NO_ITEM);

  int j;
  for (j = 0 ; j < 6 ; ++j) uoi.DecrementItemCount(NODULE);
  for (j = 0 ; j < 6 ; ++j) uoi.DecrementItemCount(DATA_LIBRARY);
  for (j = 0 ; j < 6 ; ++j) uoi.DecrementItemCount(HEAVY_EQUIPMENT);
  


  uoi.FillShip(1,false);

  BOOST_CHECK( uoi.GetShipItem(4) == NO_ITEM );
  BOOST_CHECK( uoi.GetShipItem(5) == NO_ITEM );
  BOOST_CHECK( uoi.GetShipItem(6) == NO_ITEM );

}

BOOST_AUTO_TEST_CASE( UnownedItemsFillShip14 )
{
  UnownedItems uoi;

  // in phase 3 not harsh we can count Data libraries even though we can't roll them.
  UnownedItemsStart(uoi,WAREHOUSE,WAREHOUSE,SCIENTISTS,WAREHOUSE,
                    NO_ITEM,NO_ITEM,NO_ITEM);

  int j;
  for (j = 0 ; j < 4 ; ++j) uoi.DecrementItemCount(DATA_LIBRARY); // leaving 2 left.
  for (j = 0 ; j < 6 ; ++j) uoi.DecrementItemCount(WAREHOUSE);
  for (j = 0 ; j < 6 ; ++j) uoi.DecrementItemCount(HEAVY_EQUIPMENT);
  for (j = 0 ; j < 6 ; ++j) uoi.DecrementItemCount(NODULE);
  for (j = 0 ; j < 6 ; ++j) uoi.DecrementItemCount(SCIENTISTS);
  for (j = 0 ; j < 6 ; ++j) uoi.DecrementItemCount(ORBITAL_LAB);
  for (j = 0 ; j < 6 ; ++j) uoi.DecrementItemCount(ROBOTS);
  for (j = 0 ; j < 6 ; ++j) uoi.DecrementItemCount(LABORATORY);
  for (j = 0 ; j < 6 ; ++j) uoi.DecrementItemCount(ECOPLANTS);
  for (j = 0 ; j < 6 ; ++j) uoi.DecrementItemCount(OUTPOST);
  for (j = 0 ; j < 6 ; ++j) uoi.DecrementItemCount(SPACE_STATION);
  for (j = 0 ; j < 6 ; ++j) uoi.DecrementItemCount(PLANETARY_CRUISER);
  for (j = 0 ; j < 6 ; ++j) uoi.DecrementItemCount(MOON_BASE);
  


  uoi.FillShip(3,false);

  BOOST_CHECK_EQUAL( uoi.GetShipItem(4) , DATA_LIBRARY );
  BOOST_CHECK_EQUAL( uoi.GetShipItem(5) , DATA_LIBRARY );
  BOOST_CHECK_EQUAL( uoi.GetShipItem(6) , NO_ITEM );

}
  

BOOST_AUTO_TEST_CASE( UnownedItemsFillShip15 )
{
  UnownedItems uoi;

  // in phase 3 not harsh we can count Data libraries even though we can't roll them.
  // very rare case...in order to count Data library, we have to be able to get there.
  // this case should see that because there are warehouses to be had, but we can't
  // put them on the ship, that we can't count the data libraries
  UnownedItemsStart(uoi,WAREHOUSE,WAREHOUSE,SCIENTISTS,WAREHOUSE,
                    NO_ITEM,NO_ITEM,NO_ITEM);

  int j;
  for (j = 0 ; j < 4 ; ++j) uoi.DecrementItemCount(DATA_LIBRARY); // leaving 2 left.
  for (j = 0 ; j < 6 ; ++j) uoi.DecrementItemCount(HEAVY_EQUIPMENT);
  for (j = 0 ; j < 6 ; ++j) uoi.DecrementItemCount(NODULE);
  for (j = 0 ; j < 6 ; ++j) uoi.DecrementItemCount(SCIENTISTS);
  for (j = 0 ; j < 6 ; ++j) uoi.DecrementItemCount(ORBITAL_LAB);
  for (j = 0 ; j < 6 ; ++j) uoi.DecrementItemCount(ROBOTS);
  for (j = 0 ; j < 6 ; ++j) uoi.DecrementItemCount(LABORATORY);
  for (j = 0 ; j < 6 ; ++j) uoi.DecrementItemCount(ECOPLANTS);
  for (j = 0 ; j < 6 ; ++j) uoi.DecrementItemCount(OUTPOST);
  for (j = 0 ; j < 6 ; ++j) uoi.DecrementItemCount(SPACE_STATION);
  for (j = 0 ; j < 6 ; ++j) uoi.DecrementItemCount(PLANETARY_CRUISER);
  for (j = 0 ; j < 6 ; ++j) uoi.DecrementItemCount(MOON_BASE);
  


  uoi.FillShip(3,false);

  BOOST_CHECK_EQUAL( uoi.GetShipItem(4) , NO_ITEM );
  BOOST_CHECK_EQUAL( uoi.GetShipItem(5) , NO_ITEM );
  BOOST_CHECK_EQUAL( uoi.GetShipItem(6) , NO_ITEM );

}
  
BOOST_AUTO_TEST_CASE ( UnownedItemsCanPlayerAffordItem )
{
  UnownedItems uoi;
  bool result;

  uoi.Initialize(7);

  Player pl("Albert",301);
  BOOST_CHECK_THROW(uoi.CanPlayerAffordItem(pl,7),std::out_of_range);
  
  BOOST_CHECK_NO_THROW(result = uoi.CanPlayerAffordItem(pl,6));
  BOOST_CHECK_EQUAL( result , false );

  BOOST_CHECK_NO_THROW(result = uoi.CanPlayerAffordItem(pl,ANY_SHIP_SLOT));
  BOOST_CHECK_EQUAL( result , false );

  pl.GetOwnedItems().AddItem(DATA_LIBRARY);
  pl.GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,90,false,false));

  uoi.SetShipItem(2,OUTPOST);

  BOOST_CHECK_EQUAL( uoi.CanPlayerAffordItem(pl,1) , false);
  BOOST_CHECK_EQUAL( uoi.CanPlayerAffordItem(pl,2) , false);
  BOOST_CHECK_EQUAL( uoi.CanPlayerAffordItem(pl,3) , false );
  BOOST_CHECK_EQUAL( uoi.CanPlayerAffordItem(pl,ANY_SHIP_SLOT) , false );

  uoi.SetShipItem(3,LABORATORY);

  BOOST_CHECK_EQUAL( uoi.CanPlayerAffordItem(pl,1) , false);
  BOOST_CHECK_EQUAL( uoi.CanPlayerAffordItem(pl,2) , false);
  BOOST_CHECK_EQUAL( uoi.CanPlayerAffordItem(pl,3) , true );
  BOOST_CHECK_EQUAL( uoi.CanPlayerAffordItem(pl,ANY_SHIP_SLOT) , true );

  pl.GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,10,false,false));

  BOOST_CHECK_EQUAL( uoi.CanPlayerAffordItem(pl,1) , false);
  BOOST_CHECK_EQUAL( uoi.CanPlayerAffordItem(pl,2) , true );
  BOOST_CHECK_EQUAL( uoi.CanPlayerAffordItem(pl,3) , true );
  BOOST_CHECK_EQUAL( uoi.CanPlayerAffordItem(pl,ANY_SHIP_SLOT) , true );
}

