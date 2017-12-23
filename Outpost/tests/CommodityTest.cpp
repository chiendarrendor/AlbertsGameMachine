#include "Commodity.hpp"
#include "SerializeUtility.hpp"
#include <boost/test/auto_unit_test.hpp>

BOOST_AUTO_TEST_CASE( CommodityConstructor )
{
  Commodity c;

  Commodity v(MOON_ORE_COMMODITY,3,true,true);

  BOOST_CHECK( v.GetType() == MOON_ORE_COMMODITY );
  BOOST_CHECK( v.GetValue() == 3 );
  BOOST_CHECK( v.IsAverage() == true );
  BOOST_CHECK( v.IsMega() == true );
  BOOST_CHECK( v.IsNew() == true );

  v.MakeOld();

  BOOST_CHECK( v.IsNew() == false );

  c = v;

  BOOST_CHECK( c.GetType() == MOON_ORE_COMMODITY );
  BOOST_CHECK( c.GetValue() == 3 );
  BOOST_CHECK( c.IsAverage() == true );
  BOOST_CHECK( c.IsMega() == true );

  BOOST_CHECK( c.IsNew() == false );

  Commodity ld;
  SerialTransfer(v,ld);

  BOOST_CHECK( ld.GetType() == MOON_ORE_COMMODITY );
  BOOST_CHECK( ld.GetValue() == 3 );
  BOOST_CHECK( ld.IsAverage() == true );
  BOOST_CHECK( ld.IsMega() == true );
  BOOST_CHECK( ld.IsNew() == false );

  ld.MakeNew();

  BOOST_CHECK( ld.IsNew() == true );

}

