#include "ProductionManager.hpp"
#include "SerializeUtility.hpp"
#include <boost/test/auto_unit_test.hpp>
#include "Players.hpp"
#include "CommodityManager.hpp"
#include <stdexcept>

  
//test case 1: no first turn, no refineries, no megas
// this will test that we can iterate through multiple players, and multiple resources
// and that players recieve their goods in the right order.
BOOST_AUTO_TEST_CASE( ProductionManagerNoTurn1NoRefineriesNoMegas )
{
  size_t i;
  Players pl;
  pl.add("Player1");
  pl[0].GetFactories().AddFactory(ORE_FACTORY);
  pl[0].GetFactories().AddFactory(ORE_FACTORY);
  pl[0].GetFactories().AlterManning("HH",2,0); // 2 vp
  pl[0].GetOwnedItems().AddItem(SCIENTISTS); // 2 vp
  pl.add("Player2");
  pl[1].GetFactories().AddFactory(ORE_FACTORY);
  pl[1].GetFactories().AddFactory(ORE_FACTORY);
  pl[1].GetFactories().AddFactory(ORE_FACTORY);
  pl[1].GetFactories().AlterManning("HHH",3,0); // 3 vp
  pl[1].GetOwnedItems().AddItem(ORBITAL_LAB); // 3 vp
  pl.add("Player3");
  pl[2].GetFactories().AddFactory(ORE_FACTORY); // 1 vp
  pl[2].GetFactories().AlterManning("H",1,0);

  pl.DetermineTurnOrder(); // expect that 1 is first, then 0, then 2

  CommodityManager cm;
  CommodityDeck &cd = cm.GetDeck(ORE_COMMODITY);
  for (i = 0 ; i < 22 ; ++i) cd.DrawCommodity(false); // this draws out all but four cards.
  ProductionManager pm;
  pm.StartProduction(pl,cm,false,false);

  BOOST_CHECK_EQUAL( pm.IsProductionDone() , true );
  BOOST_CHECK_EQUAL( pl[0].GetCommodityHand().GetHandDescription(false,true) , "|RE*-OR*-OR3A|");
  BOOST_CHECK_EQUAL( pl[1].GetCommodityHand().GetHandDescription(false,true) , "|MI*-OR*-OR*-OR*|");
  BOOST_CHECK_EQUAL( pl[2].GetCommodityHand().GetHandDescription(false,true) , "|OR3A|");

  BOOST_CHECK_EQUAL( cm.GetDeck(RESEARCH_COMMODITY).GetDiscardSize() , 0 );
  BOOST_CHECK_EQUAL( cm.GetDeck(MICROBIOTICS_COMMODITY).GetDiscardSize() , 0 );
  BOOST_CHECK_EQUAL( cm.GetDeck(ORE_COMMODITY).GetDiscardSize() , 0 );

}

// tests that the doubling of resources occurs correctly, and that 
// doubling of a mega resource past a multiple of four doesn't cause the Megas to kick in.
BOOST_AUTO_TEST_CASE ( ProductionManagerTurn1 )
{
  Players pl;
  pl.add("Player1");
  pl[0].GetFactories().AddFactory(ORE_FACTORY);
  pl[0].GetFactories().AddFactory(WATER_FACTORY);
  pl[0].GetFactories().AddFactory(WATER_FACTORY);
  pl[0].GetFactories().AddFactory(WATER_FACTORY);
  pl[0].GetFactories().AlterManning("HHHH",4,0);
  pl.DetermineTurnOrder();

  CommodityManager cm;
  ProductionManager pm;
  pm.StartProduction(pl,cm,true,false);
  BOOST_CHECK_EQUAL(pm.IsProductionDone(), true);
  BOOST_CHECK_EQUAL( pl[0].GetCommodityHand().GetHandDescription(false,true) , "|WA*-WA*-WA*-WA*-WA*-WA*-OR*-OR*|");

  BOOST_CHECK_EQUAL( cm.GetDeck(WATER_COMMODITY).GetDiscardSize() , 0 );
  BOOST_CHECK_EQUAL( cm.GetDeck(ORE_COMMODITY).GetDiscardSize() , 0 );

}

// tests that refineries are handled correctly
BOOST_AUTO_TEST_CASE( ProductionManagerRefineries )
{
  Players pl;
  pl.add("Player1");
  pl[0].GetFactories().AddFactory(ORE_FACTORY);
  pl[0].GetFactories().AddFactory(ORE_FACTORY);
  pl[0].GetFactories().AddFactory(ORE_FACTORY);
  pl[0].GetFactories().AddFactory(ORE_FACTORY);
  pl[0].GetFactories().AddFactory(WATER_FACTORY);
  pl[0].GetFactories().AddFactory(WATER_FACTORY);
  pl[0].GetFactories().AddFactory(WATER_FACTORY);
  pl[0].GetFactories().AddFactory(RESEARCH_FACTORY);
  pl[0].GetFactories().AddFactory(RESEARCH_FACTORY);
  pl[0].GetFactories().AddFactory(RESEARCH_FACTORY);
  // should be RRRWWWOOOO, gives us one manned research(out of 3), two manned water(out of 3), and two manned ore(out of 4)
  pl[0].GetFactories().AlterManning("HUUHHUHHUU",5,0);
  
  CommodityManager cm;
  ProductionManager pm;
  pm.StartProduction(pl,cm,false,true);
  BOOST_CHECK_EQUAL(pm.IsProductionDone(), true);
  BOOST_CHECK_EQUAL( pl[0].GetCommodityHand().GetHandDescription(false,true) , "|RE*-WA*-WA*-OR*-OR*|");
  BOOST_CHECK_EQUAL( cm.GetDeck(RESEARCH_COMMODITY).GetDiscardSize() , 1 );
  BOOST_CHECK_EQUAL( cm.GetDeck(WATER_COMMODITY).GetDiscardSize() , 1 );
  BOOST_CHECK_EQUAL( cm.GetDeck(ORE_COMMODITY).GetDiscardSize() , 2 );

}

// tests that the right thing happens with Megas
// will also test correct behavior for Serialization
BOOST_AUTO_TEST_CASE( ProductionManagerMegas )
{
  Players pl;
  pl.add("Player1");
  pl[0].GetFactories().AddFactory(ORE_FACTORY);
  pl[0].GetFactories().AddFactory(ORE_FACTORY);
  pl[0].GetFactories().AddFactory(WATER_FACTORY);
  pl[0].GetFactories().AddFactory(WATER_FACTORY);
  pl[0].GetFactories().AddFactory(WATER_FACTORY);
  pl[0].GetFactories().AddFactory(WATER_FACTORY);
  pl[0].GetFactories().AddFactory(WATER_FACTORY);
  pl[0].GetFactories().AddFactory(WATER_FACTORY);
  pl[0].GetFactories().AddFactory(WATER_FACTORY);
  pl[0].GetFactories().AddFactory(WATER_FACTORY);
  pl[0].GetFactories().AddFactory(WATER_FACTORY);
  pl[0].GetFactories().AlterManning("HHHHHHHHHHH",11,0); // 11 VP
  pl.add("Player2");
  pl[1].GetFactories().AddFactory(ORE_FACTORY);
  pl[1].GetFactories().AddFactory(ORE_FACTORY);
  pl[1].GetFactories().AddFactory(ORE_FACTORY);
  pl[1].GetFactories().AddFactory(ORE_FACTORY);
  pl[1].GetFactories().AddFactory(TITANIUM_FACTORY);
  pl[1].GetFactories().AddFactory(TITANIUM_FACTORY);
  pl[1].GetFactories().AddFactory(TITANIUM_FACTORY);
  pl[1].GetFactories().AddFactory(TITANIUM_FACTORY);
  pl[1].GetFactories().AlterManning("HHHHHHHH",8,0); // 12 VP
  pl.add("Player3");
  pl[2].GetFactories().AddFactory(ORE_FACTORY); 
  pl[2].GetFactories().AddFactory(NEW_CHEMICALS_FACTORY);
  pl[2].GetFactories().AddFactory(NEW_CHEMICALS_FACTORY);
  pl[2].GetFactories().AddFactory(NEW_CHEMICALS_FACTORY);
  pl[2].GetFactories().AddFactory(NEW_CHEMICALS_FACTORY);
  pl[2].GetFactories().AddFactory(NEW_CHEMICALS_FACTORY);
  pl[2].GetFactories().AlterManning("HHHHHH",6,0); // 16 VP
  pl.add("Player4");
  pl[3].GetFactories().AddFactory(WATER_FACTORY);
  pl[3].GetFactories().AddFactory(WATER_FACTORY);
  pl[3].GetFactories().AddFactory(WATER_FACTORY);
  pl[3].GetFactories().AddFactory(WATER_FACTORY);
  pl[3].GetFactories().AddFactory(WATER_FACTORY);
  pl[3].GetFactories().AddFactory(WATER_FACTORY);
  pl[3].GetFactories().AddFactory(WATER_FACTORY);
  pl[3].GetFactories().AddFactory(WATER_FACTORY);
  pl[3].GetFactories().AddFactory(WATER_FACTORY);
  pl[3].GetFactories().AlterManning("HHHHHHHHH",9,0); // 9 VP

  pl.DetermineTurnOrder(); // this should cause GetTurnOrder to contain 2,1,0,3

  CommodityManager cm;
  ProductionManager pm;
  pm.StartProduction(pl,cm,false,false);

  BOOST_REQUIRE_EQUAL( pm.IsProductionDone() , false );
  InputInfo ii;
  ii = pm.GetMegaInfo();
  
  BOOST_REQUIRE_EQUAL( pl.GetCurTurnPlayer().GetName(), "Player3" );
  BOOST_REQUIRE_EQUAL( ii.m_Commodity , NEW_CHEMICALS_COMMODITY ); 
  BOOST_REQUIRE_EQUAL( ii.m_MaxMegas , 1 );

  BOOST_REQUIRE_THROW( pm.ContinueProduction(2,pl,cm,false,false) , std::out_of_range );
  BOOST_REQUIRE_NO_THROW ( pm.ContinueProduction(1,pl,cm,false,false) ); // player 2 now has a mega newchem

  BOOST_REQUIRE_EQUAL( pm.IsProductionDone() , false );
  ii = pm.GetMegaInfo();
  
  BOOST_REQUIRE_EQUAL( pl.GetCurTurnPlayer().GetName() , "Player2" );
  BOOST_REQUIRE_EQUAL( ii.m_Commodity , TITANIUM_COMMODITY ); 
  BOOST_REQUIRE_EQUAL( ii.m_MaxMegas , 1 );
  BOOST_REQUIRE_NO_THROW ( pm.ContinueProduction(0,pl,cm,false,false) ); // player 1 has no mega-titanium

  BOOST_REQUIRE_EQUAL( pm.IsProductionDone() , false );

  ProductionManager pm2;
  SerialTransfer(pm,pm2);

  ii = pm2.GetMegaInfo();
  
  BOOST_REQUIRE_EQUAL( pl.GetCurTurnPlayer().GetName() , "Player1" );
  BOOST_REQUIRE_EQUAL( ii.m_Commodity , WATER_COMMODITY ); 
  BOOST_REQUIRE_EQUAL( ii.m_MaxMegas , 2 );
  BOOST_REQUIRE_NO_THROW ( pm2.ContinueProduction(2,pl,cm,false,false) ); // player 0 has 2 mega-water

  BOOST_REQUIRE_EQUAL( pm2.IsProductionDone() , false );
  ii = pm2.GetMegaInfo();
  
  BOOST_REQUIRE_EQUAL( pl.GetCurTurnPlayer().GetName() , "Player4" );
  BOOST_REQUIRE_EQUAL( ii.m_Commodity , WATER_COMMODITY ); 
  BOOST_REQUIRE_EQUAL( ii.m_MaxMegas , 2 );
  BOOST_REQUIRE_NO_THROW ( pm2.ContinueProduction(1,pl,cm,false,false) ); // player 3 has 1 mega-water

  BOOST_REQUIRE_EQUAL( pm2.IsProductionDone() , true );

  BOOST_REQUIRE_EQUAL( pl[0].GetCommodityHand().GetHandDescription(false,true) , "|WA30M-WA30M-WA*-OR*-OR*|");
  BOOST_REQUIRE_EQUAL( pl[1].GetCommodityHand().GetHandDescription(false,true) , "|TI*-TI*-TI*-TI*-OR*-OR*-OR*-OR*|");
  BOOST_REQUIRE_EQUAL( pl[2].GetCommodityHand().GetHandDescription(false,true) , "|NE88M-NE*-OR*|");
  BOOST_REQUIRE_EQUAL( pl[3].GetCommodityHand().GetHandDescription(false,true) , "|WA30M-WA*-WA*-WA*-WA*-WA*|");


  BOOST_CHECK_EQUAL( cm.GetDeck(TITANIUM_COMMODITY).GetDiscardSize() , 0 );
  BOOST_CHECK_EQUAL( cm.GetDeck(NEW_CHEMICALS_COMMODITY).GetDiscardSize() , 0 );
  BOOST_CHECK_EQUAL( cm.GetDeck(WATER_COMMODITY).GetDiscardSize() , 0 );
  BOOST_CHECK_EQUAL( cm.GetDeck(ORE_COMMODITY).GetDiscardSize() , 0 );


}
