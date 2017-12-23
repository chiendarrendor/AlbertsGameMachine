#include "OutpostState.hpp"
#include "SerializeUtility.hpp"
#include <boost/test/auto_unit_test.hpp>

#include "OutpostSet.hpp"

BOOST_AUTO_TEST_CASE ( OutpostStateConstructor )
{
  OutpostSet oset("");

  OutpostState ostate(oset);

  ostate.GetPlayers().add("Albert");
  ostate.GetPlayers().add("Bravo");
  ostate.GetPlayers()[0].GetFactories().AddFactory(WATER_FACTORY);
  ostate.GetPlayers()[0].GetFactories().AddFactory(WATER_FACTORY);
  ostate.GetPlayers()[0].GetFactories().AddFactory(WATER_FACTORY);
  ostate.GetPlayers()[0].GetFactories().AddFactory(WATER_FACTORY);
  ostate.GetPlayers()[0].GetFactories().AlterManning("HHHH",4,0);
  ostate.GetCommodities().GetDeck(ORE_COMMODITY).DrawCommodity(false);
  ostate.GetCommodities().GetDeck(ORE_COMMODITY).DrawCommodity(false);
  ostate.GetCommodities().GetDeck(ORE_COMMODITY).DrawCommodity(false);
  ostate.GetOptions().SetEarlyDiscard(true);
  ostate.GetOptions().SetRefineries(true);
  ostate.GetOptions().SetOneUpgradePerTurn(true);
  ostate.GetOptions().SetRobotMechanism(ALWAYS_MULTIPLICATIVE);
  ostate.GetOptions().SetResearchMega(true);
  ostate.GetOptions().SetBlindDraws(true);

  ostate.GetUnownedItems().Initialize(6);
  ostate.SetCurTurn(3);

  ostate.GetPlayers().DetermineTurnOrder();
  ostate.GetProductionManager().StartProduction(ostate.GetPlayers(),
                                                ostate.GetCommodities(),
                                                false,
                                                false);

  InputInfo ii = ostate.GetProductionManager().GetMegaInfo();

  BOOST_CHECK_EQUAL( ostate.GetPlayers().GetCurTurnPlayer().GetName(), "Albert" );
  BOOST_CHECK_EQUAL( ii.m_Commodity , WATER_COMMODITY );
  BOOST_CHECK_EQUAL( ii.m_MaxMegas , 1 );

  OutpostState ostate2(oset);
  SerialTransfer(ostate,ostate2);

  BOOST_CHECK_EQUAL(ostate2.GetPlayers().size() , (size_t)2);
  BOOST_CHECK_EQUAL(ostate2.GetPlayers()[0].GetName() , "Albert");
  BOOST_CHECK_EQUAL(ostate2.GetPlayers()[1].GetName() , "Bravo");
  BOOST_CHECK_EQUAL(ostate2.GetCommodities().
                    GetDeck(ORE_COMMODITY).GetDeckSize() , 23 );
  BOOST_CHECK_EQUAL(ostate2.GetOptions().GetEarlyDiscard() , true );
  BOOST_CHECK_EQUAL(ostate2.GetOptions().GetRefineries() , true );
  BOOST_CHECK_EQUAL(ostate2.GetOptions().GetOneUpgradePerTurn() , true );
  BOOST_CHECK_EQUAL(ostate2.GetOptions().GetMustBeActiveToBid() , false );
  BOOST_CHECK_EQUAL(ostate2.GetOptions().GetSupplyHarshness() , 0 );
  BOOST_CHECK_EQUAL(ostate2.GetOptions().GetRobotMechanism() , ALWAYS_MULTIPLICATIVE );
  BOOST_CHECK_EQUAL(ostate2.GetOptions().GetResearchMega() , true );
  BOOST_CHECK_EQUAL(ostate2.GetOptions().GetResearchIsSmall() , false );
  BOOST_CHECK_EQUAL(ostate2.GetOptions().GetMicroIsSmall() , false );
  BOOST_CHECK_EQUAL(ostate2.GetOptions().GetBlindDraws() , true );

  ii = ostate2.GetProductionManager().GetMegaInfo();

  BOOST_CHECK_EQUAL( ostate.GetPlayers().GetCurTurnPlayer().GetName(), "Albert" );
  BOOST_CHECK_EQUAL( ii.m_Commodity , WATER_COMMODITY );
  BOOST_CHECK_EQUAL( ii.m_MaxMegas , 1 );


  BOOST_CHECK_EQUAL(ostate2.GetCurTurn() , 3 );

  BOOST_CHECK_EQUAL(ostate2.GetUnownedItems().
                    GetItemCount(DATA_LIBRARY) , 5);
  
}

