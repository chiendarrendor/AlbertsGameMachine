#include "Players.hpp"
#include "SerializeUtility.hpp"
#include <boost/test/auto_unit_test.hpp>
#include <iostream>
#include <stdexcept>

void PrepPlayer(Player &i_pl,int i_numorefac,int i_nummbs)
{
  int i;
  std::string manning;

  // no item cost, generate 1 VP each
  for (i = 0 ; i < i_numorefac ; ++i)
  {
    i_pl.GetFactories().AddFactory(ORE_FACTORY);
    i_pl.GetFactories().AddFactory(ORE_FACTORY);
    manning += "HU";
  }
  i_pl.GetFactories().AlterManning(manning,i_numorefac,0);

  // cost 200 each, no VP
  for (i = 0 ; i < i_nummbs ; ++i)
  {
    i_pl.GetOwnedItems().AddItem(MOON_BASE);
  }
}



BOOST_AUTO_TEST_CASE( PlayersConstructor )
{
  Players pl;
  pl.add("Player0");
  pl.add("Player1");
  pl.add("Player2");
  pl.add("Player3");
  pl.add("Player4");
  pl.add("Player5");
  pl.add("Player6");
  pl.add("Player7");

  pl.remove("Player7");
  pl.add("Player7");
  pl.remove("Player7");
  pl.add("Player7");
  pl.remove("Player7");
  pl.add("Player7");
  pl.remove("Player7");
  pl.add("Player7");






  BOOST_CHECK_EQUAL( pl[0].GetName() , "Player0" );
  BOOST_CHECK_EQUAL( pl[0].GetId() , 0 );
  BOOST_CHECK_EQUAL( pl[1].GetName() , "Player1" );
  BOOST_CHECK_EQUAL( pl[1].GetId() , 1 );
  BOOST_CHECK_EQUAL( pl[2].GetName() , "Player2" );
  BOOST_CHECK_EQUAL( pl[2].GetId() , 2 );
  BOOST_CHECK_EQUAL( pl[3].GetName() , "Player3" );
  BOOST_CHECK_EQUAL( pl[3].GetId() , 3 );
  BOOST_CHECK_EQUAL( pl[4].GetName() , "Player4" );
  BOOST_CHECK_EQUAL( pl[4].GetId() , 4 );
  BOOST_CHECK_EQUAL( pl[5].GetName() , "Player5" );
  BOOST_CHECK_EQUAL( pl[5].GetId() , 5 );
  BOOST_CHECK_EQUAL( pl[6].GetName() , "Player6" );
  BOOST_CHECK_EQUAL( pl[6].GetId() , 6 );
  BOOST_CHECK_EQUAL( pl[7].GetName() , "Player7" );
  BOOST_CHECK_EQUAL( pl[7].GetId() , 7 );

  BOOST_CHECK_EQUAL( pl.IsPlayerInternalizing("Player0") , false);
  BOOST_CHECK_EQUAL( pl.IsPlayerInternalizing("Player8") , false);
  pl[0].SetPurchaseState(INTERNALIZING);
  BOOST_CHECK_EQUAL( pl.IsPlayerInternalizing("Player0") , true);


  BOOST_CHECK_EQUAL( pl.size() , (size_t)8);

  BOOST_CHECK_EQUAL( pl.IsPlayer("Player3") , true );
  BOOST_CHECK_EQUAL( pl.IsPlayer("Player9") , false );

  BOOST_CHECK_EQUAL( pl.GetPlayerByName("Player3").GetId() , 3);

  BOOST_CHECK_THROW( pl[9].GetName() , std::out_of_range);
  BOOST_CHECK_THROW(pl.GetPlayerByName("Player9"),std::out_of_range);

  BOOST_CHECK_EQUAL( pl[3].GetName() , "Player3" );

  BOOST_CHECK_EQUAL( pl.GetPlayerId("Player5") , (size_t)5 );
  BOOST_CHECK_EQUAL( pl.GetPlayerId("Player9") , NO_PLAYER);

  BOOST_CHECK_EQUAL( pl.AllOk() , false );
  pl[0].SetOkStartup(true);
  pl[1].SetOkStartup(true);
  pl[2].SetOkStartup(true);
  pl[3].SetOkStartup(true);
  BOOST_CHECK_EQUAL( pl.AllOk() , false );
  pl[4].SetOkStartup(true);
  pl[5].SetOkStartup(true);
  pl[6].SetOkStartup(true);
  pl[7].SetOkStartup(true);
  BOOST_CHECK_EQUAL( pl.AllOk() , true );
  
  pl.DetermineTurnOrder();

  Players pl2;
  SerialTransfer(pl,pl2);

  BOOST_CHECK_EQUAL( pl2[0].GetName() , "Player0" );
  BOOST_CHECK_EQUAL( pl2[1].GetName() , "Player1" );
  BOOST_CHECK_EQUAL( pl2[2].GetName() , "Player2" );
  BOOST_CHECK_EQUAL( pl2[3].GetName() , "Player3" );
  BOOST_CHECK_EQUAL( pl2[4].GetName() , "Player4" );
  BOOST_CHECK_EQUAL( pl2[5].GetName() , "Player5" );
  BOOST_CHECK_EQUAL( pl2[6].GetName() , "Player6" );
  BOOST_CHECK_EQUAL( pl2[7].GetName() , "Player7" );

  BOOST_CHECK( pl.GetTurnOrder() == pl2.GetTurnOrder() );
}

BOOST_AUTO_TEST_CASE( PlayersAllUnequalVPsTurnOrder )
{
  Players pl;
  pl.add("Player0");
  pl.add("Player1");
  pl.add("Player2");
  pl.add("Player3");
  pl.add("Player4");
  pl.add("Player5");
  pl.add("Player6");
  pl.add("Player7");

  pl.remove("Player7");
  pl.add("Player7");
  pl.remove("Player7");
  pl.add("Player7");
  pl.remove("Player7");
  pl.add("Player7");
  pl.remove("Player7");
  pl.add("Player7");

  PrepPlayer(pl[0],13,0);
  PrepPlayer(pl[1],26,0);
  PrepPlayer(pl[2],18,0);
  PrepPlayer(pl[3],3,0);
  PrepPlayer(pl[4],7,0);
  PrepPlayer(pl[5],21,0);
  PrepPlayer(pl[6],30,0);
  PrepPlayer(pl[7],45,0);

  pl.DetermineTurnOrder();

  const std::vector<int> &turnorder = pl.GetTurnOrder();
  BOOST_REQUIRE( pl.GetTurnOrder().size() == 8);
  BOOST_CHECK_EQUAL(turnorder[0],7);
  BOOST_CHECK_EQUAL(turnorder[1],6);
  BOOST_CHECK_EQUAL(turnorder[2],1);
  BOOST_CHECK_EQUAL(turnorder[3],5);
  BOOST_CHECK_EQUAL(turnorder[4],2);
  BOOST_CHECK_EQUAL(turnorder[5],0);
  BOOST_CHECK_EQUAL(turnorder[6],4);
  BOOST_CHECK_EQUAL(turnorder[7],3);

  // checking to see if the turnorder stuff works.
  pl.StartTurnOrder();
  BOOST_CHECK_EQUAL( pl.GetCurTurnPlayer().GetName() , "Player7");
  BOOST_CHECK_EQUAL( pl.AllPlayersDone() , false );
  pl.IncrementTurnOrder();
  BOOST_CHECK_EQUAL( pl.GetCurTurnPlayer().GetName() , "Player6");
  BOOST_CHECK_EQUAL( pl.AllPlayersDone() , false );
  pl.IncrementTurnOrder();
  BOOST_CHECK_EQUAL( pl.GetCurTurnPlayer().GetName() , "Player1");
  BOOST_CHECK_EQUAL( pl.AllPlayersDone() , false );
  pl.IncrementTurnOrder();
  BOOST_CHECK_EQUAL( pl.GetCurTurnPlayer().GetName() , "Player5");
  BOOST_CHECK_EQUAL( pl.AllPlayersDone() , false );
  pl.IncrementTurnOrder();
  BOOST_CHECK_EQUAL( pl.GetCurTurnPlayer().GetName() , "Player2");
  BOOST_CHECK_EQUAL( pl.AllPlayersDone() , false );
  pl.IncrementTurnOrder();

  Players pl2;
  SerialTransfer(pl,pl2);

  BOOST_CHECK_EQUAL( pl2.GetCurTurnPlayer().GetName() , "Player0");
  BOOST_CHECK_EQUAL( pl2.AllPlayersDone() , false );
  pl2.IncrementTurnOrder();
  BOOST_CHECK_EQUAL( pl2.GetCurTurnPlayer().GetName() , "Player4");
  BOOST_CHECK_EQUAL( pl2.AllPlayersDone() , false );
  pl2.IncrementTurnOrder();
  BOOST_CHECK_EQUAL( pl2.GetCurTurnPlayer().GetName() , "Player3");
  BOOST_CHECK_EQUAL( pl2.AllPlayersDone() , false );
  pl2.IncrementTurnOrder();
  BOOST_CHECK_EQUAL( pl2.AllPlayersDone() , true );


  // checking the Purchase TurnOrder stuff
  // 7,6,1,5,2,0,4,3
  pl[6].SetHasPurchased(true);
  pl[5].SetHasPurchased(true);
  pl[3].SetHasPurchased(true);
  pl[7].SetPurchaseState(INTERNALIZING);
  pl[0].SetPurchaseState(INTERNALIZING);
  pl[3].SetPurchaseState(INTERNALIZING);
  pl[1].SetPurchaseState(INTERNALIZING);
  pl.StartTurnOrder();
  pl.IncrementTurnOrder();
  pl.IncrementTurnOrder();
  BOOST_CHECK_EQUAL(pl.GetCurTurnPlayer().GetId() , 1 );
  BOOST_CHECK_EQUAL(pl.PurchaseAllPlayersDone() , false );
  pl.StartPurchasePhase();
  
  size_t i;
  for (i = 0 ; i < 8 ; ++i)
  {
    BOOST_CHECK_EQUAL( pl[i].HasPurchased() , false ) ;
    if (i == 7) BOOST_CHECK_EQUAL( pl[i].GetPurchaseState() , ACTIVE);
    else BOOST_CHECK_EQUAL( pl[i].GetPurchaseState() , UNPLAYED);
  }
  BOOST_CHECK_EQUAL( pl.GetCurTurnPlayer().GetId() , 7 );
  BOOST_CHECK_EQUAL(pl.PurchaseAllPlayersDone() , false );

  pl[6].SetPurchaseState(INTERNALIZING);
  pl[1].SetPurchaseState(DONE);
  pl[2].SetPurchaseState(INTERNALIZING);
  pl[0].SetPurchaseState(DONE);
  // 7,6,1,5,2,0,4,3
  // A I D U I D U U
  BOOST_CHECK_EQUAL(pl.PurchaseAllPlayersDone() , false );

  pl.PurchaseIncrementTurnOrder();
  // 7,6,1,5,2,0,4,3
  // D I D A I D U U

  BOOST_CHECK_EQUAL( pl[7].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( pl[6].GetPurchaseState() , INTERNALIZING );
  BOOST_CHECK_EQUAL( pl[1].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( pl[5].GetPurchaseState() , ACTIVE );
  BOOST_CHECK_EQUAL( pl.GetCurTurnPlayer().GetId() , 5 );
  BOOST_CHECK_EQUAL(pl.PurchaseAllPlayersDone() , false );

  SerialTransfer(pl,pl2);

  pl2.PurchaseIncrementTurnOrder();
  // 7,6,1,5,2,0,4,3
  // D I D D I D A U
  BOOST_CHECK_EQUAL( pl2[5].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( pl2[2].GetPurchaseState() , INTERNALIZING );
  BOOST_CHECK_EQUAL( pl2[0].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( pl2[4].GetPurchaseState() , ACTIVE );
  BOOST_CHECK_EQUAL( pl2.GetCurTurnPlayer().GetId() , 4 );
  BOOST_CHECK_EQUAL( pl2.PurchaseAllPlayersDone() , false );

  pl2.PurchaseIncrementTurnOrder();
  // 7,6,1,5,2,0,4,3
  // D I D D I D D A
  BOOST_CHECK_EQUAL( pl2[4].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( pl2[3].GetPurchaseState() , ACTIVE );
  BOOST_CHECK_EQUAL( pl2.GetCurTurnPlayer().GetId() , 3 );
  BOOST_CHECK_EQUAL( pl2.PurchaseAllPlayersDone() , false );

  pl2.PurchaseIncrementTurnOrder();
  // 7,6,1,5,2,0,4,3
  // D I D D I D D D
  BOOST_CHECK_EQUAL( pl2[3].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( pl2.AllPlayersDone() , true );
  BOOST_CHECK_EQUAL( pl2.PurchaseAllPlayersDone() , false );

  BOOST_CHECK_NO_THROW ( pl2.PurchaseIncrementTurnOrder() );
  BOOST_CHECK_EQUAL( pl2.AllPlayersDone() , true );
  BOOST_CHECK_EQUAL( pl2.PurchaseAllPlayersDone() , false );

  pl2[6].SetPurchaseState(FINAL_DONE);
  pl2[2].SetPurchaseState(INTERNAL_FINAL_DONE);
  BOOST_CHECK_EQUAL( pl2.PurchaseAllPlayersDone() , true );


}

BOOST_AUTO_TEST_CASE( PlayersSomeUnequalVPsNoTiesTurnOrder )
{
  Players pl;
  pl.add("Player0");
  pl.add("Player1");
  pl.add("Player2");
  pl.add("Player3");
  pl.add("Player4");
  pl.add("Player5");
  pl.add("Player6");
  pl.add("Player7");

  PrepPlayer(pl[0],13,0);
  PrepPlayer(pl[1],30,2);
  PrepPlayer(pl[2],18,0);
  PrepPlayer(pl[3],7,2);
  PrepPlayer(pl[4],7,4);
  PrepPlayer(pl[5],21,0);
  PrepPlayer(pl[6],30,1);
  PrepPlayer(pl[7],45,0);

  pl.DetermineTurnOrder();

  const std::vector<int> &turnorder = pl.GetTurnOrder();
  BOOST_REQUIRE( pl.GetTurnOrder().size() == 8);
  BOOST_CHECK_EQUAL(turnorder[0],7);
  BOOST_CHECK_EQUAL(turnorder[1],1);
  BOOST_CHECK_EQUAL(turnorder[2],6);
  BOOST_CHECK_EQUAL(turnorder[3],5);
  BOOST_CHECK_EQUAL(turnorder[4],2);
  BOOST_CHECK_EQUAL(turnorder[5],0);
  BOOST_CHECK_EQUAL(turnorder[6],4);
  BOOST_CHECK_EQUAL(turnorder[7],3);
}

BOOST_AUTO_TEST_CASE( PlayersAllZeroTurnOrder )
{
  Players pl;
  pl.add("Player0");
  pl.add("Player1");
  pl.add("Player2");
  pl.add("Player3");
  pl.add("Player4");
  pl.add("Player5");
  pl.add("Player6");
  pl.add("Player7");

  pl.DetermineTurnOrder();

  std::vector<int> markers(8);
  size_t j;
  BOOST_REQUIRE( pl.GetTurnOrder().size() == 8);
  for (j = 0 ; j < pl.GetTurnOrder().size() ; ++j)
  {
    markers[pl.GetTurnOrder()[j]]++;
  }
  for (j = 0 ; j < pl.GetTurnOrder().size() ; ++j)
  {
    BOOST_CHECK_EQUAL(markers[j],1);
  }
}

BOOST_AUTO_TEST_CASE( PlayersSomeTiesTurnOrder )
{
  Players pl;
  pl.add("Player0");
  pl.add("Player1");
  pl.add("Player2");
  pl.add("Player3");
  pl.add("Player4");
  pl.add("Player5");
  pl.add("Player6");
  pl.add("Player7");

  PrepPlayer(pl[0],7,4);
  PrepPlayer(pl[1],30,2);
  PrepPlayer(pl[2],18,0);
  PrepPlayer(pl[3],7,4);
  PrepPlayer(pl[4],7,4);
  PrepPlayer(pl[5],21,0);
  PrepPlayer(pl[6],30,2);
  PrepPlayer(pl[7],45,0);

  pl.DetermineTurnOrder();
  const std::vector<int> &turnorder = pl.GetTurnOrder();
  
  BOOST_REQUIRE( pl.GetTurnOrder().size() == 8);
  BOOST_CHECK_EQUAL(turnorder[0],7);

  BOOST_CHECK(turnorder[1] == 1 || turnorder[1] == 6);
  BOOST_CHECK(turnorder[2] == 6 || turnorder[2] == 1);
  BOOST_CHECK(turnorder[1] != turnorder[2]);

  BOOST_CHECK_EQUAL(turnorder[3],5);
  BOOST_CHECK_EQUAL(turnorder[4],2);

  BOOST_CHECK(turnorder[5] == 0 || turnorder[5] == 4 || turnorder[5] == 3);
  BOOST_CHECK(turnorder[6] == 0 || turnorder[6] == 4 || turnorder[6] == 3);
  BOOST_CHECK(turnorder[7] == 0 || turnorder[7] == 4 || turnorder[7] == 3);
  BOOST_CHECK(turnorder[5] != turnorder[6]);
  BOOST_CHECK(turnorder[6] != turnorder[7]);
  BOOST_CHECK(turnorder[5] != turnorder[7]);
}


BOOST_AUTO_TEST_CASE ( PlayersTestPhase )
{
  Players pl;
  pl.add("Player0");
  pl.add("Player1");
  pl.add("Player2");
  pl.add("Player3");
  pl.add("Player4");
  pl.add("Player5");
  pl.add("Player6");
  pl.add("Player7");

  BOOST_CHECK_EQUAL( pl.GetPhase() , 1 );
  PrepPlayer(pl[7],5,0);
  BOOST_CHECK_EQUAL( pl.GetPhase() , 1 );
  PrepPlayer(pl[6],9,0);
  BOOST_CHECK_EQUAL( pl.GetPhase() , 1 );
  PrepPlayer(pl[0],10,0);
  BOOST_CHECK_EQUAL( pl.GetPhase() , 2 );
  PrepPlayer(pl[5],20,0);
  BOOST_CHECK_EQUAL( pl.GetPhase() , 2 );
  PrepPlayer(pl[1],29,0);
  BOOST_CHECK_EQUAL( pl.GetPhase() , 2 );
  PrepPlayer(pl[2],30,0);
  BOOST_CHECK_EQUAL( pl.GetPhase() , 3 );
  PrepPlayer(pl[3],40,0);
  BOOST_CHECK_EQUAL( pl.GetPhase() , 3 );
  PrepPlayer(pl[4],60,0);
  BOOST_CHECK_EQUAL( pl.GetPhase() , 3 );
}

void ClearToBase(Players &pl)
{
  pl[0].SetHighBid(3); pl[0].SetBidState(PASS);
  pl[1].SetHighBid(3); pl[1].SetBidState(PASS);
  pl[2].SetHighBid(3); pl[2].SetBidState(PASS);
  pl[3].SetHighBid(3); pl[3].SetBidState(PASS);
  pl[4].SetHighBid(3); pl[4].SetBidState(PASS);
  pl[5].SetHighBid(3); pl[5].SetBidState(PASS);
  pl[6].SetHighBid(3); pl[6].SetBidState(PASS);
  pl[7].SetHighBid(3); pl[7].SetBidState(PASS);
}

BOOST_AUTO_TEST_CASE( PlayersBid )
{
  Players pl;
  pl.add("Player0");  PrepPlayer(pl[0],40,0); // #0
  pl.add("Player1");  PrepPlayer(pl[1],10,0); // #6
  pl.add("Player2");  PrepPlayer(pl[2],20,0); // #4
  pl.add("Player3");  PrepPlayer(pl[3],35,0); // #1
  pl.add("Player4");  PrepPlayer(pl[4],5,0); // #7
  pl.add("Player5");  PrepPlayer(pl[5],30,0); // #3
  pl.add("Player6");  PrepPlayer(pl[6],30,1); // #2
  pl.add("Player7");  PrepPlayer(pl[7],15,0); // #5

  pl.DetermineTurnOrder();
  
  BOOST_CHECK_EQUAL( pl.GetTurnOrder()[0] , 0);
  BOOST_CHECK_EQUAL( pl.GetTurnOrder()[1] , 3);
  BOOST_CHECK_EQUAL( pl.GetTurnOrder()[2] , 6);
  BOOST_CHECK_EQUAL( pl.GetTurnOrder()[3] , 5);
  BOOST_CHECK_EQUAL( pl.GetTurnOrder()[4] , 2);
  BOOST_CHECK_EQUAL( pl.GetTurnOrder()[5] , 7);
  BOOST_CHECK_EQUAL( pl.GetTurnOrder()[6] , 1);
  BOOST_CHECK_EQUAL( pl.GetTurnOrder()[7] , 4);

  pl.StartPurchasePhase();
  pl[3].SetPurchaseState(INTERNALIZING);
  pl[4].SetPurchaseState(INTERNAL_DONE);
  pl[5].SetPurchaseState(INTERNAL_FINAL_DONE);
  pl.PurchaseIncrementTurnOrder(); // player 6 is now active, player 0 is done.

  ClearToBase(pl);
  
  BOOST_CHECK_EQUAL( pl[0].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( pl[3].GetPurchaseState() , INTERNALIZING );
  BOOST_CHECK_EQUAL( pl[6].GetPurchaseState() , ACTIVE );
  BOOST_CHECK_EQUAL( pl[5].GetPurchaseState() , INTERNAL_FINAL_DONE );
  BOOST_CHECK_EQUAL( pl[2].GetPurchaseState() , UNPLAYED );
  BOOST_CHECK_EQUAL( pl[7].GetPurchaseState() , UNPLAYED );
  BOOST_CHECK_EQUAL( pl[1].GetPurchaseState() , UNPLAYED );
  BOOST_CHECK_EQUAL( pl[4].GetPurchaseState() , INTERNAL_DONE );


  pl.StartBidPhase(false,false,13);

  BOOST_CHECK_EQUAL( pl[0].GetBidState() , UNBID);     BOOST_CHECK_EQUAL( pl[0].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[3].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[3].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[6].GetBidState() , HASBID);    BOOST_CHECK_EQUAL( pl[6].GetHighBid() , 13);
  BOOST_CHECK_EQUAL( pl[5].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[5].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[2].GetBidState() , CURBID);    BOOST_CHECK_EQUAL( pl[2].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[7].GetBidState() , UNBID);     BOOST_CHECK_EQUAL( pl[7].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[1].GetBidState() , UNBID);     BOOST_CHECK_EQUAL( pl[1].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[4].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[4].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl.GetCurBidPlayer().GetId() , 2);
  BOOST_CHECK_EQUAL( pl.GetHighBidPlayer().GetId(), 6);

  ClearToBase(pl);
  pl.StartBidPhase(true,false,13);

  BOOST_CHECK_EQUAL( pl[0].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[0].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[3].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[3].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[6].GetBidState() , HASBID);    BOOST_CHECK_EQUAL( pl[6].GetHighBid() , 13);
  BOOST_CHECK_EQUAL( pl[5].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[5].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[2].GetBidState() , CURBID);    BOOST_CHECK_EQUAL( pl[2].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[7].GetBidState() , UNBID);     BOOST_CHECK_EQUAL( pl[7].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[1].GetBidState() , UNBID);     BOOST_CHECK_EQUAL( pl[1].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[4].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[4].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl.GetCurBidPlayer().GetId() , 2);
  BOOST_CHECK_EQUAL( pl.GetHighBidPlayer().GetId(), 6);

  ClearToBase(pl);
  pl.StartBidPhase(false,true,13);

  BOOST_CHECK_EQUAL( pl[0].GetBidState() , UNBID);     BOOST_CHECK_EQUAL( pl[0].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[3].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[3].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[6].GetBidState() , HASBID);    BOOST_CHECK_EQUAL( pl[6].GetHighBid() , 13);
  BOOST_CHECK_EQUAL( pl[5].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[5].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[2].GetBidState() , CURBID);    BOOST_CHECK_EQUAL( pl[2].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[7].GetBidState() , UNBID);     BOOST_CHECK_EQUAL( pl[7].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[1].GetBidState() , UNBID);     BOOST_CHECK_EQUAL( pl[1].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[4].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[4].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl.GetCurBidPlayer().GetId() , 2);
  BOOST_CHECK_EQUAL( pl.GetHighBidPlayer().GetId(), 6);

  ClearToBase(pl);
  pl[0].SetHasPurchased(true);
  pl.StartBidPhase(false,true,13);

  BOOST_CHECK_EQUAL( pl[0].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[0].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[3].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[3].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[6].GetBidState() , HASBID);    BOOST_CHECK_EQUAL( pl[6].GetHighBid() , 13);
  BOOST_CHECK_EQUAL( pl[5].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[5].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[2].GetBidState() , CURBID);    BOOST_CHECK_EQUAL( pl[2].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[7].GetBidState() , UNBID);     BOOST_CHECK_EQUAL( pl[7].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[1].GetBidState() , UNBID);     BOOST_CHECK_EQUAL( pl[1].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[4].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[4].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl.GetCurBidPlayer().GetId() , 2);
  BOOST_CHECK_EQUAL( pl.GetHighBidPlayer().GetId(), 6);

  ClearToBase(pl);
  pl[5].SetPurchaseState(UNPLAYED);
  pl.StartBidPhase(false,true,13);

  BOOST_CHECK_EQUAL( pl[0].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[0].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[3].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[3].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[6].GetBidState() , HASBID);    BOOST_CHECK_EQUAL( pl[6].GetHighBid() , 13);
  BOOST_CHECK_EQUAL( pl[5].GetBidState() , CURBID);    BOOST_CHECK_EQUAL( pl[5].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[2].GetBidState() , UNBID);     BOOST_CHECK_EQUAL( pl[2].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[7].GetBidState() , UNBID);     BOOST_CHECK_EQUAL( pl[7].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[1].GetBidState() , UNBID);     BOOST_CHECK_EQUAL( pl[1].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[4].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[4].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl.GetCurBidPlayer().GetId() , 5);
  BOOST_CHECK_EQUAL( pl.GetHighBidPlayer().GetId(), 6);

  pl[5].SetPurchaseState(INTERNALIZING);
  pl.PurchaseIncrementTurnOrder(); 
  pl.PurchaseIncrementTurnOrder(); 
  pl.PurchaseIncrementTurnOrder(); 

  BOOST_CHECK_EQUAL( pl[0].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( pl[3].GetPurchaseState() , INTERNALIZING );
  BOOST_CHECK_EQUAL( pl[6].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( pl[5].GetPurchaseState() , INTERNALIZING );
  BOOST_CHECK_EQUAL( pl[2].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( pl[7].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( pl[1].GetPurchaseState() , ACTIVE );
  BOOST_CHECK_EQUAL( pl[4].GetPurchaseState() , INTERNAL_DONE );

  ClearToBase(pl);
  pl.StartBidPhase(false,true,13);
  
  BOOST_CHECK_EQUAL( pl[0].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[0].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[3].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[3].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[6].GetBidState() , CURBID);    BOOST_CHECK_EQUAL( pl[6].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[5].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[5].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[2].GetBidState() , UNBID);     BOOST_CHECK_EQUAL( pl[2].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[7].GetBidState() , UNBID);     BOOST_CHECK_EQUAL( pl[7].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[1].GetBidState() , HASBID);    BOOST_CHECK_EQUAL( pl[1].GetHighBid() , 13 );
  BOOST_CHECK_EQUAL( pl[4].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[4].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl.GetCurBidPlayer().GetId() , 6);
  BOOST_CHECK_EQUAL( pl.GetCurrentHighBid() , 13 );
  BOOST_CHECK_EQUAL( pl.GetHighBidPlayer().GetId(), 1);


  pl.IncrementBid(PASS_OUT);

  BOOST_CHECK_EQUAL( pl[0].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[0].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[3].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[3].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[6].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[6].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[5].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[5].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[2].GetBidState() , CURBID);    BOOST_CHECK_EQUAL( pl[2].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[7].GetBidState() , UNBID);     BOOST_CHECK_EQUAL( pl[7].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[1].GetBidState() , HASBID);    BOOST_CHECK_EQUAL( pl[1].GetHighBid() , 13 );
  BOOST_CHECK_EQUAL( pl[4].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[4].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl.GetCurBidPlayer().GetId() , 2);
  BOOST_CHECK_EQUAL( pl.GetCurrentHighBid() , 13 );
  BOOST_CHECK_EQUAL( pl.GetHighBidPlayer().GetId(), 1);

  pl.IncrementBid(HASBID,16);

  BOOST_CHECK_EQUAL( pl[0].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[0].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[3].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[3].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[6].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[6].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[5].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[5].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[2].GetBidState() , HASBID);    BOOST_CHECK_EQUAL( pl[2].GetHighBid() , 16);
  BOOST_CHECK_EQUAL( pl[7].GetBidState() , CURBID);    BOOST_CHECK_EQUAL( pl[7].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[1].GetBidState() , HASBID);    BOOST_CHECK_EQUAL( pl[1].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( pl[4].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[4].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl.GetCurBidPlayer().GetId() , 7);
  BOOST_CHECK_EQUAL( pl.GetCurrentHighBid() , 16 );
  BOOST_CHECK_EQUAL( pl.GetHighBidPlayer().GetId(), 2);

  pl.IncrementBid(PASS);

  BOOST_CHECK_EQUAL( pl[0].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[0].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[3].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[3].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[6].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[6].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[5].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[5].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[2].GetBidState() , HASBID);    BOOST_CHECK_EQUAL( pl[2].GetHighBid() , 16);
  BOOST_CHECK_EQUAL( pl[7].GetBidState() , PASS);      BOOST_CHECK_EQUAL( pl[7].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[1].GetBidState() , CURBID);    BOOST_CHECK_EQUAL( pl[1].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( pl[4].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[4].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl.GetCurBidPlayer().GetId() , 1);
  BOOST_CHECK_EQUAL( pl.GetCurrentHighBid() , 16 );
  BOOST_CHECK_EQUAL( pl.GetHighBidPlayer().GetId(), 2);

  pl.IncrementBid(HASBID,17);

  BOOST_CHECK_EQUAL( pl[0].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[0].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[3].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[3].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[6].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[6].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[5].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[5].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[2].GetBidState() , CURBID);    BOOST_CHECK_EQUAL( pl[2].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[7].GetBidState() , PASS);      BOOST_CHECK_EQUAL( pl[7].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[1].GetBidState() , HASBID);    BOOST_CHECK_EQUAL( pl[1].GetHighBid() , 17 );
  BOOST_CHECK_EQUAL( pl[4].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[4].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl.GetCurBidPlayer().GetId() , 2);
  BOOST_CHECK_EQUAL( pl.GetCurrentHighBid() , 17 );
  BOOST_CHECK_EQUAL( pl.GetHighBidPlayer().GetId(), 1);

  pl.IncrementBid(PASS);

  BOOST_CHECK_EQUAL( pl[0].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[0].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[3].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[3].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[6].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[6].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[5].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[5].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[2].GetBidState() , PASS);      BOOST_CHECK_EQUAL( pl[2].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[7].GetBidState() , CURBID);    BOOST_CHECK_EQUAL( pl[7].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[1].GetBidState() , HASBID);    BOOST_CHECK_EQUAL( pl[1].GetHighBid() , 17 );
  BOOST_CHECK_EQUAL( pl[4].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[4].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl.GetCurBidPlayer().GetId() , 7);
  BOOST_CHECK_EQUAL( pl.GetCurrentHighBid() , 17 );
  BOOST_CHECK_EQUAL( pl.GetHighBidPlayer().GetId(), 1);


  ClearToBase(pl);
  pl.StartBidPhase(true,true,13);
  
  BOOST_CHECK_EQUAL( pl[0].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[0].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[3].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[3].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[6].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[6].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[5].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[5].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[2].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[2].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[7].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[7].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl[1].GetBidState() , CURBID);    BOOST_CHECK_EQUAL( pl[1].GetHighBid() , 13 );
  BOOST_CHECK_EQUAL( pl[4].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl[4].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl.GetCurBidPlayer().GetId() , 1);
  BOOST_CHECK_EQUAL( pl.GetCurrentHighBid() , 13 );
  BOOST_CHECK_EQUAL( pl.GetHighBidPlayer().GetId(), 1);

  Players pl2;
  SerialTransfer(pl,pl2);

  BOOST_CHECK_EQUAL( pl2[0].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl2[0].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl2[3].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl2[3].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl2[6].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl2[6].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl2[5].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl2[5].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl2[2].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl2[2].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl2[7].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl2[7].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl2[1].GetBidState() , CURBID);    BOOST_CHECK_EQUAL( pl2[1].GetHighBid() , 13 );
  BOOST_CHECK_EQUAL( pl2[4].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( pl2[4].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( pl2.GetCurBidPlayer().GetId() , 1);
  BOOST_CHECK_EQUAL( pl.GetCurrentHighBid() , 13 );
  BOOST_CHECK_EQUAL( pl.GetHighBidPlayer().GetId(), 1);

}

BOOST_AUTO_TEST_CASE( PlayersWinningPlayer )
{
  Players pl;
  pl.add("Player0");  PrepPlayer(pl[0],40,0); // #0
  pl.add("Player1");  PrepPlayer(pl[1],10,0); // #6
  pl.add("Player2");  PrepPlayer(pl[2],20,0); // #4
  pl.add("Player3");  PrepPlayer(pl[3],35,0); // #1
  pl.add("Player4");  PrepPlayer(pl[4],5,0); // #7
  pl.add("Player5");  PrepPlayer(pl[5],30,0); // #3
  pl.add("Player6");  PrepPlayer(pl[6],30,1); // #2
  pl.add("Player7");  PrepPlayer(pl[7],15,0); // #5
  
  BOOST_CHECK_EQUAL( pl.GetWinningPlayerIndices().size() , (size_t)0 );

  pl[0].GetOwnedItems().AddItem(LABORATORY); // 0 now has 45
  pl[0].GetOwnedItems().AddItem(LABORATORY); // 0 now has 50
  pl[0].GetOwnedItems().AddItem(LABORATORY); // 0 now has 55
  pl[0].GetOwnedItems().AddItem(LABORATORY); // 0 now has 60
  pl[0].GetOwnedItems().AddItem(LABORATORY); // 0 now has 65
  pl[0].GetOwnedItems().AddItem(LABORATORY); // 0 now has 70

  BOOST_CHECK_EQUAL( pl.GetWinningPlayerIndices().size() , (size_t)0 );
    
  pl[0].GetOwnedItems().AddItem(LABORATORY); // 0 now has 75.. winner!

  std::set<size_t> chk;
  chk.insert(size_t(0));

  BOOST_CHECK( pl.GetWinningPlayerIndices() == chk );
  
  pl[6].GetOwnedItems().AddItem(OUTPOST); // 35
  pl[6].GetOwnedItems().AddItem(OUTPOST); // 40
  pl[6].GetOwnedItems().AddItem(OUTPOST); // 45
  pl[6].GetOwnedItems().AddItem(OUTPOST); // 50
  pl[6].GetOwnedItems().AddItem(OUTPOST); // 55
  pl[6].GetOwnedItems().AddItem(OUTPOST); // 60
  pl[6].GetOwnedItems().AddItem(OUTPOST); // 65
  pl[6].GetOwnedItems().AddItem(OUTPOST); // 70
  pl[6].GetOwnedItems().AddItem(OUTPOST); // 75, tying.

  chk.insert(size_t(6));
  BOOST_CHECK( pl.GetWinningPlayerIndices() == chk );

  pl[0].GetOwnedItems().AddItem(LABORATORY); // 0 now has 80
  pl[6].GetOwnedItems().AddItem(OUTPOST); // 80, still tied.

  BOOST_CHECK( pl.GetWinningPlayerIndices() == chk );

  pl[6].GetOwnedItems().AddItem(OUTPOST); // 85, tie broken
  
  std::set<size_t> chk2;
  chk2.insert(size_t(6));
  BOOST_CHECK( pl.GetWinningPlayerIndices() == chk2 );
}



BOOST_AUTO_TEST_CASE( PlayersManningTurnOrder )
{
  Players pl;
  pl.add("Player0");
  pl.add("Player1");
  pl.add("Player2");
  pl.add("Player3");
  pl.add("Player4");
  pl.add("Player5");
  pl.add("Player6");
  pl.add("Player7");

  PrepPlayer(pl[0],13,0);
  PrepPlayer(pl[1],26,0);
  PrepPlayer(pl[2],18,0);
  PrepPlayer(pl[3],3,0);
  PrepPlayer(pl[4],7,0);
  PrepPlayer(pl[5],21,0);
  PrepPlayer(pl[6],30,0);
  PrepPlayer(pl[7],45,0);

  pl.DetermineTurnOrder();

  // turn order is 7,6,1,5,2,0,4,3

  pl[7].SetPurchaseState(INTERNAL_FINAL_DONE);
  pl[6].SetPurchaseState(FINAL_DONE);
  pl[1].SetPurchaseState(INTERNAL_DONE);
  pl[5].SetPurchaseState(DONE);
  pl[0].SetPurchaseState(INTERNAL_FINAL_DONE);
  pl[4].SetPurchaseState(INTERNAL_FINAL_DONE);
  pl[3].SetPurchaseState(INTERNAL_FINAL_DONE);

  pl.StartManningTurnOrder();

  BOOST_CHECK_EQUAL(pl[7].GetPurchaseState() , INTERNAL_FINAL_DONE);
  BOOST_CHECK_EQUAL(pl[6].GetPurchaseState() , FINAL_DONE);
  BOOST_CHECK_EQUAL(pl[1].GetPurchaseState() , ACTIVE);
  BOOST_CHECK_EQUAL(pl[5].GetPurchaseState() , UNPLAYED);
  BOOST_CHECK_EQUAL(pl[2].GetPurchaseState() , UNPLAYED);
  BOOST_CHECK_EQUAL(pl[0].GetPurchaseState() , INTERNAL_FINAL_DONE);
  BOOST_CHECK_EQUAL(pl[4].GetPurchaseState() , INTERNAL_FINAL_DONE);
  BOOST_CHECK_EQUAL(pl[3].GetPurchaseState() , INTERNAL_FINAL_DONE);
  
  BOOST_CHECK_EQUAL(pl.GetCurTurnPlayer().GetId() , 1 );
  BOOST_CHECK_EQUAL(pl.AllPlayersDone() , false);

  pl.IncrementManningTurnOrder();

  BOOST_CHECK_EQUAL(pl[7].GetPurchaseState() , INTERNAL_FINAL_DONE);
  BOOST_CHECK_EQUAL(pl[6].GetPurchaseState() , FINAL_DONE);
  BOOST_CHECK_EQUAL(pl[1].GetPurchaseState() , FINAL_DONE);
  BOOST_CHECK_EQUAL(pl[5].GetPurchaseState() , ACTIVE);
  BOOST_CHECK_EQUAL(pl[2].GetPurchaseState() , UNPLAYED);
  BOOST_CHECK_EQUAL(pl[0].GetPurchaseState() , INTERNAL_FINAL_DONE);
  BOOST_CHECK_EQUAL(pl[4].GetPurchaseState() , INTERNAL_FINAL_DONE);
  BOOST_CHECK_EQUAL(pl[3].GetPurchaseState() , INTERNAL_FINAL_DONE);
  
  BOOST_CHECK_EQUAL(pl.GetCurTurnPlayer().GetId() , 5 );
  BOOST_CHECK_EQUAL(pl.AllPlayersDone() , false);

  pl.IncrementManningTurnOrder();

  BOOST_CHECK_EQUAL(pl[7].GetPurchaseState() , INTERNAL_FINAL_DONE);
  BOOST_CHECK_EQUAL(pl[6].GetPurchaseState() , FINAL_DONE);
  BOOST_CHECK_EQUAL(pl[1].GetPurchaseState() , FINAL_DONE);
  BOOST_CHECK_EQUAL(pl[5].GetPurchaseState() , FINAL_DONE);
  BOOST_CHECK_EQUAL(pl[2].GetPurchaseState() , ACTIVE);
  BOOST_CHECK_EQUAL(pl[0].GetPurchaseState() , INTERNAL_FINAL_DONE);
  BOOST_CHECK_EQUAL(pl[4].GetPurchaseState() , INTERNAL_FINAL_DONE);
  BOOST_CHECK_EQUAL(pl[3].GetPurchaseState() , INTERNAL_FINAL_DONE);
  
  BOOST_CHECK_EQUAL(pl.GetCurTurnPlayer().GetId() , 2 );
  BOOST_CHECK_EQUAL(pl.AllPlayersDone() , false);

  pl.IncrementManningTurnOrder();

  BOOST_CHECK_EQUAL(pl[7].GetPurchaseState() , INTERNAL_FINAL_DONE);
  BOOST_CHECK_EQUAL(pl[6].GetPurchaseState() , FINAL_DONE);
  BOOST_CHECK_EQUAL(pl[1].GetPurchaseState() , FINAL_DONE);
  BOOST_CHECK_EQUAL(pl[5].GetPurchaseState() , FINAL_DONE);
  BOOST_CHECK_EQUAL(pl[2].GetPurchaseState() , FINAL_DONE);
  BOOST_CHECK_EQUAL(pl[0].GetPurchaseState() , INTERNAL_FINAL_DONE);
  BOOST_CHECK_EQUAL(pl[4].GetPurchaseState() , INTERNAL_FINAL_DONE);
  BOOST_CHECK_EQUAL(pl[3].GetPurchaseState() , INTERNAL_FINAL_DONE);

  BOOST_CHECK_EQUAL(pl.AllPlayersDone() , true);


  pl[7].SetPurchaseState(INTERNAL_FINAL_DONE);
  pl[6].SetPurchaseState(FINAL_DONE);
  pl[1].SetPurchaseState(INTERNAL_FINAL_DONE);
  pl[5].SetPurchaseState(FINAL_DONE);
  pl[2].SetPurchaseState(INTERNAL_FINAL_DONE);
  pl[0].SetPurchaseState(INTERNAL_FINAL_DONE);
  pl[4].SetPurchaseState(INTERNAL_FINAL_DONE);
  pl[3].SetPurchaseState(INTERNAL_FINAL_DONE);

  pl.StartManningTurnOrder();

  BOOST_CHECK_EQUAL(pl[7].GetPurchaseState() , INTERNAL_FINAL_DONE);
  BOOST_CHECK_EQUAL(pl[6].GetPurchaseState() , FINAL_DONE);
  BOOST_CHECK_EQUAL(pl[1].GetPurchaseState() , INTERNAL_FINAL_DONE);
  BOOST_CHECK_EQUAL(pl[5].GetPurchaseState() , FINAL_DONE);
  BOOST_CHECK_EQUAL(pl[2].GetPurchaseState() , INTERNAL_FINAL_DONE);
  BOOST_CHECK_EQUAL(pl[0].GetPurchaseState() , INTERNAL_FINAL_DONE);
  BOOST_CHECK_EQUAL(pl[4].GetPurchaseState() , INTERNAL_FINAL_DONE);
  BOOST_CHECK_EQUAL(pl[3].GetPurchaseState() , INTERNAL_FINAL_DONE);
  
  BOOST_CHECK_EQUAL(pl.AllPlayersDone() , true);


}
