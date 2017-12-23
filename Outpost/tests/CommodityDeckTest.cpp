#include "CommodityDeck.hpp"
#include "SerializeUtility.hpp"
#include <boost/test/auto_unit_test.hpp>
#include <iostream>

BOOST_AUTO_TEST_CASE( CommodityDeckConstructor )
{
  CommodityDeck cdeck(MOON_ORE_COMMODITY,25,14);

  BOOST_CHECK( cdeck.GetDeckSize() == 0);
  BOOST_CHECK( cdeck.GetDiscardSize() == 0);
  BOOST_CHECK_EQUAL (cdeck.GetMegaValue() ,  25);
  BOOST_CHECK_EQUAL (cdeck.GetAverageValue() , 14);

  Commodity av = cdeck.DrawCommodity(false);
  
  BOOST_CHECK( av.GetType() == MOON_ORE_COMMODITY );
  BOOST_CHECK( av.GetValue() == 14 );
  BOOST_CHECK( av.IsAverage() == true );
  BOOST_CHECK( av.IsMega() == false );
  BOOST_CHECK( av.IsNew() == true );

  Commodity mv = cdeck.DrawCommodity(true);
  
  BOOST_CHECK( mv.GetType() == MOON_ORE_COMMODITY );
  BOOST_CHECK( mv.GetValue() == 25 );
  BOOST_CHECK( mv.IsAverage() == false );
  BOOST_CHECK( mv.IsMega() == true );
  BOOST_CHECK( mv.IsNew() == true );

  cdeck.AddCards(3,1);
  BOOST_CHECK( cdeck.GetDeckSize() == 1);
  BOOST_CHECK( cdeck.GetDiscardSize() == 0);

  Commodity rv = cdeck.DrawCommodity(false);

  BOOST_CHECK( rv.GetType() == MOON_ORE_COMMODITY );
  BOOST_CHECK( rv.GetValue() == 3 );
  BOOST_CHECK( rv.IsAverage() == false );
  BOOST_CHECK( rv.IsMega() == false );
  BOOST_CHECK( rv.IsNew() == true );

  BOOST_CHECK( cdeck.GetDeckSize() == 0);
  
  Commodity av2 = cdeck.DrawCommodity(false);

  BOOST_CHECK( av2.GetType() == MOON_ORE_COMMODITY );
  BOOST_CHECK( av2.GetValue() == 14 );
  BOOST_CHECK( av2.IsAverage() == true );
  BOOST_CHECK( av2.IsMega() == false );
  BOOST_CHECK( av2.IsNew() == true );

  av.MakeOld();

  cdeck.DiscardCommodity(av);
  BOOST_CHECK( cdeck.GetDeckSize() == 0);
  BOOST_CHECK( cdeck.GetDiscardSize() == 0);

  mv.MakeOld();

  cdeck.DiscardCommodity(mv);
  BOOST_CHECK( cdeck.GetDeckSize() == 0);
  BOOST_CHECK( cdeck.GetDiscardSize() == 0);

  rv.MakeOld();

  cdeck.DiscardCommodity(rv);
  BOOST_CHECK( cdeck.GetDeckSize() == 0);
  BOOST_CHECK( cdeck.GetDiscardSize() == 1);

  Commodity rv2 = cdeck.DrawCommodity(false);
  BOOST_CHECK( cdeck.GetDeckSize() == 0);
  BOOST_CHECK( cdeck.GetDiscardSize() == 0);
  
  BOOST_CHECK( rv2.GetType() == MOON_ORE_COMMODITY );
  BOOST_CHECK( rv2.GetValue() == 3 );
  BOOST_CHECK( rv2.IsAverage() == false );
  BOOST_CHECK( rv2.IsMega() == false );
  
  BOOST_CHECK( rv2.IsNew() == true );


  CommodityDeck sdeck(RING_ORE_COMMODITY,36,17);
  sdeck.AddCards(12,4);
  sdeck.AddCards(13,5);
  sdeck.AddCards(14,6);
  sdeck.AddCards(15,7);

  std::vector<int> ccount(4);

  int i;
  
  for (i = 0 ; i < 22 ; ++i)
  {
    ccount[sdeck.DrawCommodity(false).GetValue()-12]++;
  }

  BOOST_CHECK(ccount[0] == 4);
  BOOST_CHECK(ccount[1] == 5);
  BOOST_CHECK(ccount[2] == 6);
  BOOST_CHECK(ccount[3] == 7);

  srand(time(NULL));

  std::vector<int> fdcounts(3);

  const int niters = 1000;

  for (i = 0 ; i < niters ; ++i)
  {
    CommodityDeck rdeck(RESEARCH_COMMODITY,181,148);
    rdeck.AddCards(0,1);
    rdeck.AddCards(1,2);
    rdeck.AddCards(2,3);

    Commodity rc = rdeck.DrawCommodity(false);
    
    fdcounts[rc.GetValue()]++;
  }

  std::cout << "CommodityDeck Stochastic: " << std::endl;
  std::cout << "       Real: " << fdcounts[0] << "," << fdcounts[1] << "," <<
    fdcounts[2] << std::endl;
  std::cout << "    Optimal: " << niters/6 << "," << niters/3 << "," << niters/2 << std::endl;

  BOOST_CHECK( cdeck.HasMega() == true);
  cdeck.ZeroMegaValue();
  BOOST_CHECK( cdeck.HasMega() == false);
  BOOST_CHECK_EQUAL (cdeck.GetMegaValue() ,  0);
  BOOST_CHECK_EQUAL (cdeck.GetAverageValue() , 14);
  

  CommodityDeck serdeck(NEW_CHEMICALS_COMMODITY,105,103);
  BOOST_CHECK_EQUAL (serdeck.GetMegaValue() ,  105);
  BOOST_CHECK_EQUAL (serdeck.GetAverageValue() , 103);
  serdeck.AddCards(5,3);
  serdeck.AddCards(6,6);
  serdeck.AddCards(7,1);

  serdeck.DiscardCommodity(serdeck.DrawCommodity(false));
  serdeck.DiscardCommodity(serdeck.DrawCommodity(false));

  CommodityDeck newdeck;

  SerialTransfer(serdeck,newdeck);
  BOOST_CHECK(serdeck.GetDeckSize() == 8);
  BOOST_CHECK(serdeck.GetDiscardSize() == 2);

  BOOST_CHECK(newdeck.GetDeckSize() == 8);
  BOOST_CHECK(newdeck.GetDiscardSize() == 2);

  BOOST_CHECK_EQUAL (newdeck.GetMegaValue() ,  105);
  BOOST_CHECK_EQUAL (newdeck.GetAverageValue() , 103);

  for (i = 0 ; i < 8 ; ++i)
  {
    Commodity scom = serdeck.DrawCommodity(false);
    Commodity ncom = newdeck.DrawCommodity(false);

    BOOST_CHECK( scom.GetType() == ncom.GetType() );
    BOOST_CHECK( scom.GetValue() == ncom.GetValue() );
    BOOST_CHECK( scom.IsAverage() == ncom.IsAverage() );
    BOOST_CHECK( scom.IsMega() == ncom.IsMega() );
  }

  // cleaning out the discard pile, so I can see if 
  // the copy makes Average cards correctly.
  for (i = 0 ; i < 2 ; ++i)
  {
    serdeck.DrawCommodity(false);
    newdeck.DrawCommodity(false);
  }

  {
    Commodity scom = serdeck.DrawCommodity(false);
    Commodity ncom = newdeck.DrawCommodity(false);

    BOOST_CHECK( scom.GetType() == ncom.GetType() );
    BOOST_CHECK( scom.GetValue() == ncom.GetValue() );
    BOOST_CHECK( scom.IsAverage() == ncom.IsAverage() );
    BOOST_CHECK( scom.IsMega() == ncom.IsMega() );
  }

  Commodity scom = serdeck.DrawCommodity(true);
  Commodity ncom = newdeck.DrawCommodity(true);

  BOOST_CHECK( scom.GetType() == ncom.GetType() );
  BOOST_CHECK( scom.GetValue() == ncom.GetValue() );
  BOOST_CHECK( scom.IsAverage() == ncom.IsAverage() );
  BOOST_CHECK( scom.IsMega() == ncom.IsMega() );
}
