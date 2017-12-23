#include "OutpostGameInfo.hpp"
#include <boost/test/auto_unit_test.hpp>
#include <iostream>

#include "ActionParser.hpp"
#include "ServerGameInfo.hpp"
#include <sstream>
#include <stdexcept>
#include "TestOutputPort.hpp"
#include "TransitionTestUtilities.hpp"

const int numBidTransitions = 16;
const int numPurchaseTransitions = 14;

BOOST_AUTO_TEST_CASE( TransitionTestSTARTPURCHASE )
{
  OutpostSet oset("");
  ServerGameInfo sgi("Outpost","testOutpost","XMLLOC","XMLFILE");
  ActionParser ap("");

  std::vector<std::string> vcs;
  vcs.push_back("Player1");
  vcs.push_back("Player2");
  vcs.push_back("Player3");

  TestOutputPort top(vcs);

  OutpostStateMachine osm;
  OutpostGameInfo ogi(oset,sgi,top);

  ogi.GetPlayers().add("Player0");
  ogi.GetPlayers().add("Player1");
  ogi.GetPlayers().add("Player2");
  ogi.GetPlayers().add("Player3");
  ogi.GetPlayers().add("Player4");
  ogi.GetPlayers().add("Player5");

  ogi.GetPlayers()[5].GetOwnedItems().AddItem(OUTPOST);
  ogi.GetPlayers()[5].GetOwnedItems().AddItem(OUTPOST); // 10 vp
  ogi.GetPlayers()[4].GetOwnedItems().AddItem(OUTPOST);
  ogi.GetPlayers()[4].GetOwnedItems().AddItem(ROBOTS);  // 8 vp
  ogi.GetPlayers()[3].GetOwnedItems().AddItem(ORBITAL_LAB);
  ogi.GetPlayers()[3].GetOwnedItems().AddItem(ECOPLANTS);  // 8 vp, but lower cost
  ogi.GetPlayers()[2].GetOwnedItems().AddItem(SCIENTISTS);
  ogi.GetPlayers()[2].GetOwnedItems().AddItem(ROBOTS);  // 5 vp
  ogi.GetPlayers()[1].GetOwnedItems().AddItem(SCIENTISTS);
  ogi.GetPlayers()[1].GetOwnedItems().AddItem(NODULE);  // 4 vp
  ogi.GetPlayers()[0].GetOwnedItems().AddItem(SCIENTISTS);
  ogi.GetPlayers()[0].GetOwnedItems().AddItem(WAREHOUSE);  // 3 vp

  ogi.GetPlayers().DetermineTurnOrder();

  ogi.GetPlayers().StartTurnOrder();
  ogi.GetPlayers().IncrementTurnOrder();
  ogi.GetPlayers()[3].SetPurchaseState(INTERNALIZING);
  ogi.GetPlayers()[4].SetHasPurchased(true);

  BOOST_CHECK_EQUAL( ogi.GetPlayers().GetCurTurnPlayer().GetId() , 4);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetPurchaseState() , INTERNALIZING);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].HasPurchased() , true);

  const TransitionType *pMD = GetTransition(osm,"StartPurchase","STARTPURCHASE","Purchase",1);
  BOOST_CHECK_EQUAL(pMD->IsAuto("StartPurchase",ogi) , true );
  BOOST_CHECK_EQUAL(pMD->IsLegal("Player1","StartPurchase",ogi) , false);
  BOOST_CHECK_EQUAL(pMD->ExecuteAction("StartPurchase",ogi,"",ap) , true);

  BOOST_CHECK_EQUAL( ogi.GetPlayers().GetCurTurnPlayer().GetId() , 5);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetPurchaseState() , UNPLAYED);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetPurchaseState() , ACTIVE);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].HasPurchased() , false);

  // PLAYERSTATE: plid,vp,costsum,handlimt,poplimit,mancost,#hum,#rob,purstate,bidstate,haspurchased
  BOOST_CHECK_EQUAL( top.GetOutput() , 
                     "broadcast: PLAYERSTATE,0,3,65,8,5,0,10,0,0,0,3,0,-1,-1\n"
                     "broadcast: PLAYERSTATE,1,4,65,5,8,0,10,0,0,0,3,0,-1,-1\n"
                     "broadcast: PLAYERSTATE,2,5,90,5,5,-1,10,0,0,0,3,0,-1,-1\n"
                     "broadcast: PLAYERSTATE,3,8,100,5,5,0,5,0,0,0,3,0,-1,-1\n"
                     "broadcast: PLAYERSTATE,4,8,150,8,10,-1,10,0,0,0,3,0,-1,-1\n"
                     "broadcast: PLAYERSTATE,5,10,200,11,15,0,10,0,0,1,3,0,-1,-1\n"
                     "unicast: Player0 -- PLAYERPURCHASEINFO,0,0,0,0,0,0,0\n"
                     "unicast: Player1 -- PLAYERPURCHASEINFO,0,0,0,0,0,0,0\n"
                     "unicast: Player2 -- PLAYERPURCHASEINFO,0,0,0,0,0,0,0\n"
                     "unicast: Player3 -- PLAYERPURCHASEINFO,0,0,0,0,0,0,0\n"
                     "unicast: Player4 -- PLAYERPURCHASEINFO,0,0,0,0,0,0,0\n"
                     "unicast: Player5 -- PLAYERPURCHASEINFO,0,0,0,0,0,0,0\n"
                     );

}

                     
BOOST_AUTO_TEST_CASE( TransitionTestBUYMEN )
{
  OutpostSet oset("");
  ServerGameInfo sgi("Outpost","testOutpost","XMLLOC","XMLFILE");
  ActionParser ap("");

  std::vector<std::string> vcs;
  vcs.push_back("Player1");
  vcs.push_back("Player2");
  vcs.push_back("Player3");

  TestOutputPort top(vcs);

  OutpostStateMachine osm;
  OutpostGameInfo ogi(oset,sgi,top);

  ogi.GetPlayers().add("Player0");
  ogi.GetPlayers().add("Player1");
  ogi.GetPlayers().add("Player2");
  ogi.GetPlayers().add("Player3");
  ogi.GetPlayers().add("Player4");
  ogi.GetPlayers().add("Player5");

  ogi.GetPlayers()[5].GetOwnedItems().AddItem(OUTPOST);
  ogi.GetPlayers()[5].GetOwnedItems().AddItem(OUTPOST); // 10 vp
  ogi.GetPlayers()[4].GetOwnedItems().AddItem(OUTPOST);
  ogi.GetPlayers()[4].GetOwnedItems().AddItem(ROBOTS);  // 8 vp
  ogi.GetPlayers()[3].GetOwnedItems().AddItem(ORBITAL_LAB);
  ogi.GetPlayers()[3].GetOwnedItems().AddItem(ECOPLANTS);  // 8 vp, but lower cost
  ogi.GetPlayers()[2].GetOwnedItems().AddItem(SCIENTISTS);
  ogi.GetPlayers()[2].GetOwnedItems().AddItem(ROBOTS);  // 5 vp
  ogi.GetPlayers()[1].GetOwnedItems().AddItem(SCIENTISTS);
  ogi.GetPlayers()[1].GetOwnedItems().AddItem(NODULE);  // 4 vp
  ogi.GetPlayers()[0].GetOwnedItems().AddItem(SCIENTISTS);
  ogi.GetPlayers()[0].GetOwnedItems().AddItem(WAREHOUSE);  // 3 vp

  ogi.GetPlayers().DetermineTurnOrder();

  ogi.GetPlayers().StartPurchasePhase();
  ogi.GetPlayers().PurchaseIncrementTurnOrder(); // active player is now pl #4
  ogi.GetPlayers()[3].SetPurchaseState(INTERNALIZING);

  const TransitionType *pBM = GetTransition(osm,"Purchase","BUYMEN","Purchase",numPurchaseTransitions);
  // always manual
  BOOST_CHECK_EQUAL(pBM->IsAuto("Purchase",ogi) , false );

  // in order to be legal, the player in question must be either
  // the active player or internalizing, be able to afford at least one
  // man at its current cost, and have the space to hold them.
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , false); // not active or internalizing.
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Purchase",ogi) , false); // not even a player
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , false); // active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , false); // internalizing

  // give each of the players 5 credits...only the player with ecoplants should be able to afford it.
  ogi.GetPlayers()[1].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,5,false,false));
  ogi.GetPlayers()[4].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,5,false,false));
  ogi.GetPlayers()[3].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,5,false,false));
  
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , false); // not active or internalizing.
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , false); // active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , true); // internalizing

  // give each of the players 5 more credits...
  // now the active and internalizing players can both afford it.
  ogi.GetPlayers()[1].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,5,false,false));
  ogi.GetPlayers()[4].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,5,false,false));
  ogi.GetPlayers()[3].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,5,false,false));
  
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , false); // not active or internalizing.
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , true); // active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , true); // internalizing

  // make them high bidders, and they can't buy.
  ogi.GetPlayers()[4].SetHighBid(3);
  ogi.GetPlayers()[3].SetHighBid(5);

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , false); // active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , false); // internalizing

  // put it back.
  ogi.GetPlayers()[4].SetHighBid(NOT_HIGH_BID);
  ogi.GetPlayers()[3].SetHighBid(NOT_HIGH_BID);


  // give each of them a full complement of men, and they should go false again.
  ogi.GetPlayers()[1].AddHumans(8);
  ogi.GetPlayers()[3].AddHumans(5);
  ogi.GetPlayers()[4].AddHumans(10);
                                
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , false); // not active or internalizing.
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , false); // active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , false); // internalizing

  ogi.GetPlayers().PurchaseIncrementTurnOrder(); // active player is now pl #2

  ogi.GetPlayers()[2].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,3,false,false)); // 3
  ogi.GetPlayers()[2].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,4,false,false)); // 7
  ogi.GetPlayers()[2].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,5,false,false)); // 12
  ogi.GetPlayers()[2].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,6,false,false)); // 18
  ogi.GetPlayers()[2].GetCommodityHand().AddCommodity(Commodity(WATER_COMMODITY,10,false,false)); // 28
  ogi.GetPlayers()[2].GetCommodityHand().AddCommodity(Commodity(WATER_COMMODITY,12,false,false)); // 40

  ogi.GetPlayers()[2].AddHumans(2); // should have 3 slots for humans now.

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); // active
  ActionParser ap1("BUYMEN,-1,111111");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player2",ap1) , false); // illegal value for # men.
  BOOST_CHECK_EQUAL(top.GetOutput() , "unicast: Player2 -- ERROR,Action BUYMEN variable i_nummen is below minimum\n");
  top.ResetOutput();

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); // active
  ActionParser ap2("BUYMEN,4,111111");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player2",ap2) , false); // illegal value for # men.
  BOOST_CHECK_EQUAL(top.GetOutput() , "unicast: Player2 -- ERROR,Action BUYMEN variable i_nummen is above maximum\n");
  top.ResetOutput();

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); // active
  ActionParser ap3("BUYMEN,3,1111111");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player2",ap3) , false); // bad delete string
  BOOST_CHECK_EQUAL(top.GetOutput() , "unicast: Player2 -- ERROR,Mismatched delete string length\n" );
  top.ResetOutput();

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); // active
  ActionParser ap4("BUYMEN,3,11111");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player2",ap4) , false); // bad delete string
  BOOST_CHECK_EQUAL(top.GetOutput() , "unicast: Player2 -- ERROR,Mismatched delete string length\n" );
  top.ResetOutput();

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); // active
  ActionParser ap5("BUYMEN,3,111V11");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player2",ap5) , false); // bad delete string
  BOOST_CHECK_EQUAL(top.GetOutput() ,  "unicast: Player2 -- ERROR,Illegal characters in delete string\n" );
  top.ResetOutput();

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); // active
  ActionParser ap6("BUYMEN,3,000011");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player2",ap6) , false); // didn't spend enough.
  BOOST_CHECK_EQUAL(top.GetOutput() ,  "unicast: Player2 -- ERROR,You need to spend more to buy that.\n" );
  top.ResetOutput();

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); // active
  ActionParser ap7("BUYMEN,3,101111");

  // check to see that factories are automanned
  ogi.GetPlayers()[2].GetFactories().AddFactory(WATER_FACTORY);
  ogi.GetPlayers()[2].GetFactories().AddFactory(WATER_FACTORY);
  ogi.GetPlayers()[2].GetFactories().AddFactory(WATER_FACTORY);
  ogi.GetPlayers()[2].GetFactories().AddFactory(WATER_FACTORY);
  ogi.GetPlayers()[2].GetFactories().AddFactory(WATER_FACTORY);
  ogi.GetPlayers()[2].GetFactories().AddFactory(MOON_BASE_FACTORY);


  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player2",ap7) , true); // should be ok...
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetNumHumans() , 5 );
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetCommodityHand().GetHandDescription(true,true) , "|WA10|");

  BOOST_CHECK_EQUAL(top.GetOutput() ,  
                    "broadcast: SPENDS,0,2,0,1,30,|WA12-OR6-OR5-OR4-OR3|\n"
                    "broadcast: PLAYERSTATE,2,29,90,5,6,-1,10,5,0,1,3,0,-1,-1\n"
                    "broadcast: PLAYERFACTORIES,2,|WU-WH-WH-WH-WH-MH|\n"
                    "broadcast: PLAYERCARDBACKS,2,|WA*|\n"
                    "unicast: Player2 -- PLAYERCARDS,|WA10|\n"
                    "unicast: Player2 -- PLAYERPURCHASEINFO,1,1,1,0,0,0,0\n"
                    "broadcast: COMMODITYSTOCK,1,26,4,0,3\n"
                    "broadcast: COMMODITYSTOCK,2,39,1,30,7\n"
                    "broadcast: COMMODITYSTOCK,3,53,0,44,10\n"
                    "broadcast: COMMODITYSTOCK,4,34,0,56,13\n"
                    "broadcast: COMMODITYSTOCK,5,16,0,0,17\n"
                    "broadcast: COMMODITYSTOCK,6,23,0,88,20\n"
                    "broadcast: COMMODITYSTOCK,7,14,0,0,30\n"
                    "broadcast: COMMODITYSTOCK,8,12,0,0,40\n"
                    "broadcast: COMMODITYSTOCK,9,12,0,0,50\n"
                    );
                    
  ogi.GetPlayers().PurchaseIncrementTurnOrder(); // active player is now pl #1
  ogi.GetPlayers().PurchaseIncrementTurnOrder(); // active player is now pl #0
  BOOST_CHECK_EQUAL( ogi.GetPlayers().AllPlayersDone() , false);
  BOOST_CHECK_EQUAL( ogi.GetPlayers().PurchaseAllPlayersDone() , false );
  ogi.GetPlayers().PurchaseIncrementTurnOrder(); // should be out of active players.
  BOOST_CHECK_EQUAL( ogi.GetPlayers().AllPlayersDone() , true);
  BOOST_CHECK_EQUAL( ogi.GetPlayers().PurchaseAllPlayersDone() , false );

  // this should be enough for player 3 to buy at least one more man.
  ogi.GetPlayers()[3].GetOwnedItems().AddItem(NODULE);  
  ogi.GetPlayers()[3].GetCommodityHand().AddCommodity(Commodity(WATER_COMMODITY,10,false,false)); 

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , true); // this might fail due to poor query construction
}

BOOST_AUTO_TEST_CASE( TransitionTestBUYROBOTS )
{
  OutpostSet oset("");
  ServerGameInfo sgi("Outpost","testOutpost","XMLLOC","XMLFILE");
  ActionParser ap("");

  std::vector<std::string> vcs;
  vcs.push_back("Player1");
  vcs.push_back("Player2");
  vcs.push_back("Player3");

  TestOutputPort top(vcs);

  OutpostStateMachine osm;
  OutpostGameInfo ogi(oset,sgi,top);

  ogi.GetPlayers().add("Player0");
  ogi.GetPlayers().add("Player1");
  ogi.GetPlayers().add("Player2");
  ogi.GetPlayers().add("Player3");
  ogi.GetPlayers().add("Player4");
  ogi.GetPlayers().add("Player5");

  ogi.GetPlayers()[5].GetOwnedItems().AddItem(OUTPOST);
  ogi.GetPlayers()[5].GetOwnedItems().AddItem(OUTPOST); // 10 vp
  ogi.GetPlayers()[4].GetOwnedItems().AddItem(OUTPOST);
  ogi.GetPlayers()[4].GetOwnedItems().AddItem(ROBOTS);  // 8 vp
  ogi.GetPlayers()[3].GetOwnedItems().AddItem(ORBITAL_LAB);
  ogi.GetPlayers()[3].GetOwnedItems().AddItem(ECOPLANTS);  // 8 vp, but lower cost
  ogi.GetPlayers()[2].GetOwnedItems().AddItem(SCIENTISTS);
  ogi.GetPlayers()[2].GetOwnedItems().AddItem(ROBOTS);  // 5 vp
  ogi.GetPlayers()[1].GetOwnedItems().AddItem(SCIENTISTS);
  ogi.GetPlayers()[1].GetOwnedItems().AddItem(NODULE);  // 4 vp
  ogi.GetPlayers()[0].GetOwnedItems().AddItem(SCIENTISTS);
  ogi.GetPlayers()[0].GetOwnedItems().AddItem(WAREHOUSE);  // 3 vp

  ogi.GetPlayers().DetermineTurnOrder();

  ogi.GetPlayers().StartPurchasePhase();
  ogi.GetPlayers()[2].SetPurchaseState(INTERNALIZING);
  ogi.GetPlayers()[1].SetPurchaseState(INTERNALIZING);

  const TransitionType *pBM = GetTransition(osm,"Purchase","BUYROBOTS","Purchase",numPurchaseTransitions);
  // always manual
  BOOST_CHECK_EQUAL(pBM->IsAuto("Purchase",ogi) , false );

  ogi.GetOptions().SetRobotMechanism(SECOND_UNLIMITED);

  // in order to be legal, the player in question must be either
  // active or internalizing, own at least one robots card, have at least ten credits,
  // and have the space to hold them.
  
  // current player is 5, 2 and 1 are internalizing, and 4 and 2 have robots.
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Purchase",ogi) , false); // not even a player
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Purchase",ogi) , false); // doesn't have robots
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , false); // not active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , false); // not active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , false); // internalizing, robots, but no money.
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , false); // internalizing, no robots
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Purchase",ogi) , false); // neither active nor robots

  // give each of the players 10 credits...(but no one has any people, so no space for robots)
  ogi.GetPlayers()[5].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,10,false,false));
  ogi.GetPlayers()[4].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,10,false,false));
  ogi.GetPlayers()[3].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,10,false,false));
  ogi.GetPlayers()[2].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,10,false,false));
  ogi.GetPlayers()[1].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,10,false,false));
  ogi.GetPlayers()[0].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,10,false,false));

  // current player is 5, 2 and 1 are internalizing, and 4 and 2 have robots.
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Purchase",ogi) , false); // not even a player
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Purchase",ogi) , false); // doesn't have robots
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , false); // not active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , false); // not active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , false); // internalizing, robots, but no money.
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , false); // internalizing, no robots
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Purchase",ogi) , false); // neither active nor robots

  ogi.GetPlayers().PurchaseIncrementTurnOrder(); 
  // current player is 4, 2 and 1 are internalizing, and 4 and 2 have robots.
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Purchase",ogi) , false); // not even a player
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Purchase",ogi) , false); // doesn't have robots
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , false); // not active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , false); // not active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , false); // internalizing, robots, but no money.
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , false); // internalizing, no robots
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Purchase",ogi) , false); // neither active nor robots

  ogi.GetPlayers().PurchaseIncrementTurnOrder(); 
  // current player is 3, 2 and 1 are internalizing, and 4 and 2 have robots.
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Purchase",ogi) , false); // not even a player
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Purchase",ogi) , false); // doesn't have robots
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , false); // not active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , false); // not active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , false); // internalizing, robots, but no money.
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , false); // internalizing, no robots
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Purchase",ogi) , false); // neither active nor robots

  ogi.GetPlayers().PurchaseIncrementTurnOrder(); 
  // current player is 0, 2 and 1 are internalizing, and 4 and 2 have robots.
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Purchase",ogi) , false); // not even a player
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Purchase",ogi) , false); // doesn't have robots
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , false); // not active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , false); // not active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , false); // internalizing, robots, but no money.
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , false); // internalizing, no robots
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Purchase",ogi) , false); // neither active nor robots

  // add a person, giving everyone the space for one robot
  ogi.GetPlayers()[5].AddHumans(1); 
  ogi.GetPlayers()[4].AddHumans(1); 
  ogi.GetPlayers()[3].AddHumans(1); 
  ogi.GetPlayers()[2].AddHumans(1); 
  ogi.GetPlayers()[1].AddHumans(1); 
  ogi.GetPlayers()[0].AddHumans(1); 

  ogi.GetPlayers().StartPurchasePhase();
  ogi.GetPlayers()[2].SetPurchaseState(INTERNALIZING);
  ogi.GetPlayers()[1].SetPurchaseState(INTERNALIZING);

  // current player is 5, 2 and 1 are internalizing, and 4 and 2 have robots.
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Purchase",ogi) , false); // not even a player
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Purchase",ogi) , false); // doesn't have robots
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , false); // not active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , false); // not active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); // internalizing, robots, and money
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , false); // internalizing, no robots
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Purchase",ogi) , false); // neither active nor robots

  ogi.GetPlayers().PurchaseIncrementTurnOrder(); 
  // current player is 4, 2 and 1 are internalizing, and 4 and 2 have robots.
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Purchase",ogi) , false); // not even a player
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Purchase",ogi) , false); // doesn't have robots
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , true); // active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , false); // not active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); // internalizing, robots, money.
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , false); // internalizing, no robots
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Purchase",ogi) , false); // neither active nor robots

  // making them high bidders make them not purchasable.
  ogi.GetPlayers()[4].SetHighBid(3);
  ogi.GetPlayers()[2].SetHighBid(4);
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , false); // active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , false); // internalizing, robots, money.
  ogi.GetPlayers()[4].SetHighBid(NOT_HIGH_BID);
  ogi.GetPlayers()[2].SetHighBid(NOT_HIGH_BID);


  ogi.GetPlayers().PurchaseIncrementTurnOrder(); 
  // current player is 3, 2 and 1 are internalizing, and 4 and 2 have robots.
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Purchase",ogi) , false); // not even a player
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Purchase",ogi) , false); // doesn't have robots
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , false); // not active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , false); // not active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); // internalizing, robots, money.
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , false); // internalizing, no robots
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Purchase",ogi) , false); // neither active nor robots

  ogi.GetPlayers().PurchaseIncrementTurnOrder(); 
  // current player is 0, 2 and 1 are internalizing, and 4 and 2 have robots.
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Purchase",ogi) , false); // not even a player
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Purchase",ogi) , false); // doesn't have robots
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , false); // not active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , false); // not active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); // internalizing, robots, money.
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , false); // internalizing, no robots
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Purchase",ogi) , false); // neither active nor robots

  ogi.GetPlayers().PurchaseIncrementTurnOrder(); 
  BOOST_CHECK_EQUAL( ogi.GetPlayers().AllPlayersDone() , true);
  BOOST_CHECK_EQUAL( ogi.GetPlayers().PurchaseAllPlayersDone() , false );


  // if we give player 2 a robot, he shouldn't have space for any more.
  ogi.GetPlayers()[2].AddRobots(1); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , false);
  
  // unless we have first unlimited robots
  ogi.GetOptions().SetRobotMechanism(FIRST_UNLIMITED);
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true);

  // if we give player 2 another robot card, with multiplicative robots on, he should have space for one more.
  ogi.GetOptions().SetRobotMechanism(ALWAYS_MULTIPLICATIVE);
  ogi.GetPlayers()[2].GetOwnedItems().AddItem(ROBOTS); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true);

  // if we add another one, he should be full again.
  ogi.GetPlayers()[2].AddRobots(1); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , false);

  // if we set the option to second unlimited again, he should be able to take more...
  ogi.GetOptions().SetRobotMechanism(SECOND_UNLIMITED);
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true);

  // a whole bunch more, potentially.
  ogi.GetPlayers()[2].AddRobots(6); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true);

  // giving him more money so he can buy more than one robot.
  ogi.GetPlayers()[2].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,7,false,false));
  ogi.GetPlayers()[2].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,3,false,false));
  ogi.GetPlayers()[2].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,12,false,false));
  // he should be now limited to 3 robots, since he has 32 resources.

  ActionParser ap1("BUYROBOTS,-1,1111");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player2",ap1) , false); // illegal value for # men.
  BOOST_CHECK_EQUAL(top.GetOutput() , "unicast: Player2 -- ERROR,Action BUYROBOTS variable i_numrobots is below minimum\n");
  top.ResetOutput();

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); // active
  ActionParser ap2("BUYROBOTS,4,111111");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player2",ap2) , false); // illegal value for # men.
  BOOST_CHECK_EQUAL(top.GetOutput() , "unicast: Player2 -- ERROR,Action BUYROBOTS variable i_numrobots is above maximum\n");
  top.ResetOutput();

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); // active
  ActionParser ap3("BUYROBOTS,2,11111");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player2",ap3) , false); // bad delete string
  BOOST_CHECK_EQUAL(top.GetOutput() , "unicast: Player2 -- ERROR,Mismatched delete string length\n" );
  top.ResetOutput();

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); // active
  ActionParser ap4("BUYROBOTS,2,111");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player2",ap4) , false); // bad delete string
  BOOST_CHECK_EQUAL(top.GetOutput() , "unicast: Player2 -- ERROR,Mismatched delete string length\n" );
  top.ResetOutput();

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); // active
  ActionParser ap5("BUYROBOTS,2,1V11");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player2",ap5) , false); // bad delete string
  BOOST_CHECK_EQUAL(top.GetOutput() ,  "unicast: Player2 -- ERROR,Illegal characters in delete string\n" );
  top.ResetOutput();

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); // active
  ActionParser ap6("BUYROBOTS,2,0011");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player2",ap6) , false); // didn't spend enough.
  BOOST_CHECK_EQUAL(top.GetOutput() ,  "unicast: Player2 -- ERROR,You need to spend more to buy that.\n" );
  top.ResetOutput();

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); // active
  ActionParser ap7("BUYROBOTS,2,1100");

  ogi.GetPlayers()[2].GetFactories().AddFactory(RESEARCH_FACTORY);
  ogi.GetPlayers()[2].GetFactories().AddFactory(NEW_CHEMICALS_FACTORY);

  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player2",ap7) , true); // should be ok...
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetNumRobots() , 10 );
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetCommodityHand().GetHandDescription(true,true) , "|OR7-OR3|");

  BOOST_CHECK_EQUAL(top.GetOutput() ,  
                    "broadcast: SPENDS,0,2,0,0,20,|OR12-OR10|\n"
                    "broadcast: PLAYERSTATE,2,13,140,5,5,-1,10,1,10,2,3,0,-1,-1\n"
                    "broadcast: PLAYERFACTORIES,2,|RB-NH|\n"
                    "broadcast: PLAYERCARDBACKS,2,|OR*-OR*|\n"
                    "unicast: Player2 -- PLAYERCARDS,|OR7-OR3|\n"
                    "unicast: Player2 -- PLAYERPURCHASEINFO,1,1,1,0,0,0,0\n"
                    "broadcast: COMMODITYSTOCK,1,26,2,0,3\n"
                    "broadcast: COMMODITYSTOCK,2,39,0,30,7\n"
                    "broadcast: COMMODITYSTOCK,3,53,0,44,10\n"
                    "broadcast: COMMODITYSTOCK,4,34,0,56,13\n"
                    "broadcast: COMMODITYSTOCK,5,16,0,0,17\n"
                    "broadcast: COMMODITYSTOCK,6,23,0,88,20\n"
                    "broadcast: COMMODITYSTOCK,7,14,0,0,30\n"
                    "broadcast: COMMODITYSTOCK,8,12,0,0,40\n"
                    "broadcast: COMMODITYSTOCK,9,12,0,0,50\n"
                    );
                    
}


BOOST_AUTO_TEST_CASE( TransitionTestBUYOREFACTORIES )
{
  OutpostSet oset("");
  ServerGameInfo sgi("Outpost","testOutpost","XMLLOC","XMLFILE");
  ActionParser ap("");

  std::vector<std::string> vcs;
  vcs.push_back("Player1");
  vcs.push_back("Player2");
  vcs.push_back("Player3");

  TestOutputPort top(vcs);

  OutpostStateMachine osm;
  OutpostGameInfo ogi(oset,sgi,top);

  ogi.GetPlayers().add("Player0");
  ogi.GetPlayers().add("Player1");
  ogi.GetPlayers().add("Player2");
  ogi.GetPlayers().add("Player3");
  ogi.GetPlayers().add("Player4");
  ogi.GetPlayers().add("Player5");

  ogi.GetPlayers()[5].GetOwnedItems().AddItem(OUTPOST);
  ogi.GetPlayers()[5].GetOwnedItems().AddItem(OUTPOST); // 10 vp
  ogi.GetPlayers()[4].GetOwnedItems().AddItem(OUTPOST);
  ogi.GetPlayers()[4].GetOwnedItems().AddItem(ROBOTS);  // 8 vp
  ogi.GetPlayers()[3].GetOwnedItems().AddItem(ORBITAL_LAB);
  ogi.GetPlayers()[3].GetOwnedItems().AddItem(ECOPLANTS);  // 8 vp, but lower cost
  ogi.GetPlayers()[2].GetOwnedItems().AddItem(SCIENTISTS);
  ogi.GetPlayers()[2].GetOwnedItems().AddItem(ROBOTS);  // 5 vp
  ogi.GetPlayers()[1].GetOwnedItems().AddItem(SCIENTISTS);
  ogi.GetPlayers()[1].GetOwnedItems().AddItem(NODULE);  // 4 vp
  ogi.GetPlayers()[0].GetOwnedItems().AddItem(SCIENTISTS);
  ogi.GetPlayers()[0].GetOwnedItems().AddItem(WAREHOUSE);  // 3 vp

  ogi.GetPlayers().DetermineTurnOrder();

  ogi.GetPlayers().StartPurchasePhase();
  ogi.GetPlayers()[2].SetPurchaseState(INTERNALIZING);
  ogi.GetPlayers()[1].SetPurchaseState(INTERNALIZING);

  const TransitionType *pBM = GetTransition(osm,"Purchase","BUYOREFACTORIES","Purchase",numPurchaseTransitions);
  // always manual
  BOOST_CHECK_EQUAL(pBM->IsAuto("Purchase",ogi) , false );

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Purchase",ogi) , false); // active, but no money.
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , false); // internalizing, but no money
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , false); // internalizing, but no money.
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Purchase",ogi) , false); 

  ogi.GetPlayers()[5].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,10,false,false));
  ogi.GetPlayers()[2].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,10,false,false));

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Purchase",ogi) , true); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , false); // internalizing, but no money.
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Purchase",ogi) , false); 
  
  ogi.GetPlayers()[5].SetHighBid(3);
  ogi.GetPlayers()[2].SetHighBid(4);
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , false); 

  ogi.GetPlayers()[5].SetHighBid(NOT_HIGH_BID);
  ogi.GetPlayers()[2].SetHighBid(NOT_HIGH_BID);

  ogi.GetPlayers().PurchaseIncrementTurnOrder(); 
  ogi.GetPlayers().PurchaseIncrementTurnOrder(); 
  ogi.GetPlayers().PurchaseIncrementTurnOrder(); 
  ogi.GetPlayers().PurchaseIncrementTurnOrder(); 

  BOOST_CHECK_EQUAL( ogi.GetPlayers().AllPlayersDone() , true);
  BOOST_CHECK_EQUAL( ogi.GetPlayers().PurchaseAllPlayersDone() , false );

 
  // four at ten each
  ogi.GetPlayers()[2].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,10,false,false));
  ogi.GetPlayers()[2].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,10,false,false));
  ogi.GetPlayers()[2].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,10,false,false));

  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetFactories().GetFactoryDescription() , "||");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetVP() , 5 );
  
  ActionParser ap1("BUYOREFACTORIES,-1,1111");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player2",ap1) , false); // illegal value for # men.
  BOOST_CHECK_EQUAL(top.GetOutput() , "unicast: Player2 -- ERROR,Action BUYOREFACTORIES variable i_numfactories is below minimum\n");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetFactories().GetFactoryDescription() , "||");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetVP() , 5 );
  top.ResetOutput();

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); // active
  ActionParser ap2("BUYOREFACTORIES,5,111111");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player2",ap2) , false); // illegal value for # men.
  BOOST_CHECK_EQUAL(top.GetOutput() , "unicast: Player2 -- ERROR,Action BUYOREFACTORIES variable i_numfactories is above maximum\n");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetFactories().GetFactoryDescription() , "||");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetVP() , 5 );
  top.ResetOutput();

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); // active
  ActionParser ap3("BUYOREFACTORIES,2,11111");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player2",ap3) , false); // bad delete string
  BOOST_CHECK_EQUAL(top.GetOutput() , "unicast: Player2 -- ERROR,Mismatched delete string length\n" );
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetFactories().GetFactoryDescription() , "||");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetVP() , 5 );
  top.ResetOutput();

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); // active
  ActionParser ap4("BUYOREFACTORIES,2,111");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player2",ap4) , false); // bad delete string
  BOOST_CHECK_EQUAL(top.GetOutput() , "unicast: Player2 -- ERROR,Mismatched delete string length\n" );
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetFactories().GetFactoryDescription() , "||");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetVP() , 5 );
  top.ResetOutput();

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); // active
  ActionParser ap5("BUYOREFACTORIES,2,1V11");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player2",ap5) , false); // bad delete string
  BOOST_CHECK_EQUAL(top.GetOutput() ,  "unicast: Player2 -- ERROR,Illegal characters in delete string\n" );
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetFactories().GetFactoryDescription() , "||");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetVP() , 5 );
  top.ResetOutput();

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); // active
  ActionParser ap6("BUYOREFACTORIES,3,0011");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player2",ap6) , false); // didn't spend enough.
  BOOST_CHECK_EQUAL(top.GetOutput() ,  "unicast: Player2 -- ERROR,You need to spend more to buy that.\n" );
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetFactories().GetFactoryDescription() , "||");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetVP() , 5 );
  top.ResetOutput();

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); // active
  ActionParser ap7("BUYOREFACTORIES,2,1100");

  ogi.GetPlayers()[2].AddHumans(1);

  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player2",ap7) , true); // should be ok...
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetCommodityHand().GetHandDescription(true,true) , "|OR10-OR10|");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetFactories().GetFactoryDescription() , "|OU-OH|");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetVP() , 6 );

  BOOST_CHECK_EQUAL(top.GetOutput() ,  
                    "broadcast: SPENDS,0,2,0,2,20,|OR10-OR10|\n"
                    "broadcast: PLAYERSTATE,2,6,90,5,5,-1,10,1,0,2,3,0,-1,-1\n"
                    "broadcast: PLAYERFACTORIES,2,|OU-OH|\n"
                    "broadcast: PLAYERCARDBACKS,2,|OR*-OR*|\n"
                    "unicast: Player2 -- PLAYERCARDS,|OR10-OR10|\n"
                    "unicast: Player2 -- PLAYERPURCHASEINFO,2,2,2,1,0,0,0\n"
                    "broadcast: COMMODITYSTOCK,1,26,2,0,3\n"
                    "broadcast: COMMODITYSTOCK,2,39,0,30,7\n"
                    "broadcast: COMMODITYSTOCK,3,53,0,44,10\n"
                    "broadcast: COMMODITYSTOCK,4,34,0,56,13\n"
                    "broadcast: COMMODITYSTOCK,5,16,0,0,17\n"
                    "broadcast: COMMODITYSTOCK,6,23,0,88,20\n"
                    "broadcast: COMMODITYSTOCK,7,14,0,0,30\n"
                    "broadcast: COMMODITYSTOCK,8,12,0,0,40\n"
                    "broadcast: COMMODITYSTOCK,9,12,0,0,50\n"
                    );
}                    


BOOST_AUTO_TEST_CASE( TransitionTestBUYWATERFACTORIES )
{
  OutpostSet oset("");
  ServerGameInfo sgi("Outpost","testOutpost","XMLLOC","XMLFILE");
  ActionParser ap("");

  std::vector<std::string> vcs;
  vcs.push_back("Player1");
  vcs.push_back("Player2");
  vcs.push_back("Player3");

  TestOutputPort top(vcs);

  OutpostStateMachine osm;
  OutpostGameInfo ogi(oset,sgi,top);

  ogi.GetPlayers().add("Player0");
  ogi.GetPlayers().add("Player1");
  ogi.GetPlayers().add("Player2");
  ogi.GetPlayers().add("Player3");
  ogi.GetPlayers().add("Player4");
  ogi.GetPlayers().add("Player5");

  ogi.GetPlayers()[5].GetOwnedItems().AddItem(OUTPOST);
  ogi.GetPlayers()[5].GetOwnedItems().AddItem(OUTPOST); // 10 vp
  ogi.GetPlayers()[4].GetOwnedItems().AddItem(OUTPOST);
  ogi.GetPlayers()[4].GetOwnedItems().AddItem(ROBOTS);  // 8 vp
  ogi.GetPlayers()[3].GetOwnedItems().AddItem(ORBITAL_LAB);
  ogi.GetPlayers()[3].GetOwnedItems().AddItem(ECOPLANTS);  // 8 vp, but lower cost
  ogi.GetPlayers()[2].GetOwnedItems().AddItem(SCIENTISTS);
  ogi.GetPlayers()[2].GetOwnedItems().AddItem(ROBOTS);  // 5 vp
  ogi.GetPlayers()[1].GetOwnedItems().AddItem(SCIENTISTS);
  ogi.GetPlayers()[1].GetOwnedItems().AddItem(NODULE);  // 4 vp
  ogi.GetPlayers()[0].GetOwnedItems().AddItem(SCIENTISTS);
  ogi.GetPlayers()[0].GetOwnedItems().AddItem(WAREHOUSE);  // 3 vp

  ogi.GetPlayers().DetermineTurnOrder();

  ogi.GetPlayers().StartPurchasePhase();
  ogi.GetPlayers()[2].SetPurchaseState(INTERNALIZING);
  ogi.GetPlayers()[1].SetPurchaseState(INTERNALIZING);

  const TransitionType *pBM = GetTransition(osm,"Purchase","BUYWATERFACTORIES","Purchase",numPurchaseTransitions);
  // always manual
  BOOST_CHECK_EQUAL(pBM->IsAuto("Purchase",ogi) , false );

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Purchase",ogi) , false); // active, but no money.
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , false); // internalizing, but no money
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , false); // internalizing, but no money.
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Purchase",ogi) , false); 

  ogi.GetPlayers()[5].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,20,false,false));
  ogi.GetPlayers()[2].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,20,false,false));

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Purchase",ogi) , true); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , false); // internalizing, but no money.
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Purchase",ogi) , false); 

  ogi.GetPlayers()[5].SetHighBid(3);
  ogi.GetPlayers()[2].SetHighBid(4);
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , false); 

  ogi.GetPlayers()[5].SetHighBid(NOT_HIGH_BID);
  ogi.GetPlayers()[2].SetHighBid(NOT_HIGH_BID);


 
  // four at twenty each
  ogi.GetPlayers()[2].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,20,false,false));
  ogi.GetPlayers()[2].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,20,false,false));
  ogi.GetPlayers()[2].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,20,false,false));

  ogi.GetPlayers().PurchaseIncrementTurnOrder(); 
  ogi.GetPlayers().PurchaseIncrementTurnOrder(); 
  ogi.GetPlayers().PurchaseIncrementTurnOrder(); 
  ogi.GetPlayers().PurchaseIncrementTurnOrder(); 

  BOOST_CHECK_EQUAL( ogi.GetPlayers().AllPlayersDone() , true);
  BOOST_CHECK_EQUAL( ogi.GetPlayers().PurchaseAllPlayersDone() , false );

  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetFactories().GetFactoryDescription() , "||");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetVP() , 5 );
  
  ActionParser ap1("BUYWATERFACTORIES,-1,1111");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player2",ap1) , false); // illegal value for # men.
  BOOST_CHECK_EQUAL(top.GetOutput() , "unicast: Player2 -- ERROR,Action BUYWATERFACTORIES variable i_numfactories is below minimum\n");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetFactories().GetFactoryDescription() , "||");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetVP() , 5 );
  top.ResetOutput();

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); // active
  ActionParser ap2("BUYWATERFACTORIES,5,111111");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player2",ap2) , false); // illegal value for # men.
  BOOST_CHECK_EQUAL(top.GetOutput() , "unicast: Player2 -- ERROR,Action BUYWATERFACTORIES variable i_numfactories is above maximum\n");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetFactories().GetFactoryDescription() , "||");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetVP() , 5 );
  top.ResetOutput();

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); // active
  ActionParser ap3("BUYWATERFACTORIES,2,11111");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player2",ap3) , false); // bad delete string
  BOOST_CHECK_EQUAL(top.GetOutput() , "unicast: Player2 -- ERROR,Mismatched delete string length\n" );
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetFactories().GetFactoryDescription() , "||");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetVP() , 5 );
  top.ResetOutput();

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); // active
  ActionParser ap4("BUYWATERFACTORIES,2,111");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player2",ap4) , false); // bad delete string
  BOOST_CHECK_EQUAL(top.GetOutput() , "unicast: Player2 -- ERROR,Mismatched delete string length\n" );
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetFactories().GetFactoryDescription() , "||");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetVP() , 5 );
  top.ResetOutput();

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); // active
  ActionParser ap5("BUYWATERFACTORIES,2,1V11");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player2",ap5) , false); // bad delete string
  BOOST_CHECK_EQUAL(top.GetOutput() ,  "unicast: Player2 -- ERROR,Illegal characters in delete string\n" );
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetFactories().GetFactoryDescription() , "||");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetVP() , 5 );
  top.ResetOutput();

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); // active
  ActionParser ap6("BUYWATERFACTORIES,3,0011");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player2",ap6) , false); // didn't spend enough.
  BOOST_CHECK_EQUAL(top.GetOutput() ,  "unicast: Player2 -- ERROR,You need to spend more to buy that.\n" );
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetFactories().GetFactoryDescription() , "||");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetVP() , 5 );
  top.ResetOutput();

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); // active
  ActionParser ap7("BUYWATERFACTORIES,2,1100");

  ogi.GetPlayers()[2].AddRobots(1);

  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player2",ap7) , true); // should be ok...
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetCommodityHand().GetHandDescription(true,true) , "|OR20-OR20|");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetFactories().GetFactoryDescription() , "|WU-WB|");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetVP() , 6 );

  BOOST_CHECK_EQUAL(top.GetOutput() ,  
                    "broadcast: SPENDS,0,2,0,3,40,|OR20-OR20|\n"
                    "broadcast: PLAYERSTATE,2,6,90,5,5,-1,10,0,1,2,3,0,-1,-1\n"
                    "broadcast: PLAYERFACTORIES,2,|WU-WB|\n"
                    "broadcast: PLAYERCARDBACKS,2,|OR*-OR*|\n"
                    "unicast: Player2 -- PLAYERCARDS,|OR20-OR20|\n"
                    "unicast: Player2 -- PLAYERPURCHASEINFO,4,4,4,2,0,0,0\n"
                    "broadcast: COMMODITYSTOCK,1,26,2,0,3\n"
                    "broadcast: COMMODITYSTOCK,2,39,0,30,7\n"
                    "broadcast: COMMODITYSTOCK,3,53,0,44,10\n"
                    "broadcast: COMMODITYSTOCK,4,34,0,56,13\n"
                    "broadcast: COMMODITYSTOCK,5,16,0,0,17\n"
                    "broadcast: COMMODITYSTOCK,6,23,0,88,20\n"
                    "broadcast: COMMODITYSTOCK,7,14,0,0,30\n"
                    "broadcast: COMMODITYSTOCK,8,12,0,0,40\n"
                    "broadcast: COMMODITYSTOCK,9,12,0,0,50\n"
                    );
}                    


BOOST_AUTO_TEST_CASE( TransitionTestBUYTITANIUMFACTORIES )
{
  OutpostSet oset("");
  ServerGameInfo sgi("Outpost","testOutpost","XMLLOC","XMLFILE");
  ActionParser ap("");

  std::vector<std::string> vcs;
  vcs.push_back("Player1");
  vcs.push_back("Player2");
  vcs.push_back("Player3");

  TestOutputPort top(vcs);

  OutpostStateMachine osm;
  OutpostGameInfo ogi(oset,sgi,top);

  ogi.GetPlayers().add("Player0");
  ogi.GetPlayers().add("Player1");
  ogi.GetPlayers().add("Player2");
  ogi.GetPlayers().add("Player3");
  ogi.GetPlayers().add("Player4");
  ogi.GetPlayers().add("Player5");

  ogi.GetPlayers()[5].GetOwnedItems().AddItem(OUTPOST);
  ogi.GetPlayers()[5].GetOwnedItems().AddItem(OUTPOST); // 10 vp
  ogi.GetPlayers()[4].GetOwnedItems().AddItem(OUTPOST);
  ogi.GetPlayers()[4].GetOwnedItems().AddItem(ROBOTS);  // 8 vp
  ogi.GetPlayers()[3].GetOwnedItems().AddItem(ORBITAL_LAB);
  ogi.GetPlayers()[3].GetOwnedItems().AddItem(ECOPLANTS);  // 8 vp, but lower cost
  ogi.GetPlayers()[2].GetOwnedItems().AddItem(SCIENTISTS);
  ogi.GetPlayers()[2].GetOwnedItems().AddItem(ROBOTS);  // 5 vp
  ogi.GetPlayers()[1].GetOwnedItems().AddItem(SCIENTISTS);
  ogi.GetPlayers()[1].GetOwnedItems().AddItem(NODULE);  // 4 vp
  ogi.GetPlayers()[0].GetOwnedItems().AddItem(SCIENTISTS);
  ogi.GetPlayers()[0].GetOwnedItems().AddItem(WAREHOUSE);  // 3 vp

  ogi.GetPlayers().DetermineTurnOrder();

  ogi.GetPlayers().StartPurchasePhase();
  ogi.GetPlayers()[2].SetPurchaseState(INTERNALIZING);
  ogi.GetPlayers()[1].SetPurchaseState(INTERNALIZING);

  const TransitionType *pBM = GetTransition(osm,"Purchase","BUYTITANIUMFACTORIES","Purchase",numPurchaseTransitions);
  // always manual
  BOOST_CHECK_EQUAL(pBM->IsAuto("Purchase",ogi) , false );

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Purchase",ogi) , false); // active, but no money.
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , false); // internalizing, but no money
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , false); // internalizing, but no money.
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Purchase",ogi) , false); 

  ogi.GetPlayers()[5].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,30,false,false));
  ogi.GetPlayers()[2].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,30,false,false));
  ogi.GetPlayers()[2].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,30,false,false));

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Purchase",ogi) , false); // no titanium Production
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , false); // no titanium Production
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Purchase",ogi) , false); 

  ogi.GetPlayers()[5].GetOwnedItems().AddItem(HEAVY_EQUIPMENT);
  ogi.GetPlayers()[2].GetOwnedItems().AddItem(HEAVY_EQUIPMENT);

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Purchase",ogi) , true); // no titanium Production
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); // no titanium Production
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Purchase",ogi) , false); 

  ogi.GetPlayers()[5].SetHighBid(3);
  ogi.GetPlayers()[2].SetHighBid(4);
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , false); 

  ogi.GetPlayers()[5].SetHighBid(NOT_HIGH_BID);
  ogi.GetPlayers()[2].SetHighBid(NOT_HIGH_BID);

 
  // four at thirty each
  ogi.GetPlayers()[2].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,30,false,false));
  ogi.GetPlayers()[2].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,30,false,false));

  ogi.GetPlayers().PurchaseIncrementTurnOrder(); 
  ogi.GetPlayers().PurchaseIncrementTurnOrder(); 
  ogi.GetPlayers().PurchaseIncrementTurnOrder(); 
  ogi.GetPlayers().PurchaseIncrementTurnOrder(); 

  BOOST_CHECK_EQUAL( ogi.GetPlayers().AllPlayersDone() , true);
  BOOST_CHECK_EQUAL( ogi.GetPlayers().PurchaseAllPlayersDone() , false );

  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetFactories().GetFactoryDescription() , "||");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetVP() , 6 );
  
  ActionParser ap1("BUYTITANIUMFACTORIES,-1,1111");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player2",ap1) , false); // illegal value for # men.
  BOOST_CHECK_EQUAL(top.GetOutput() , "unicast: Player2 -- ERROR,Action BUYTITANIUMFACTORIES variable i_numfactories is below minimum\n");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetFactories().GetFactoryDescription() , "||");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetVP() ,6 );
  top.ResetOutput();

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); // active
  ActionParser ap2("BUYTITANIUMFACTORIES,5,111111");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player2",ap2) , false); // illegal value for # men.
  BOOST_CHECK_EQUAL(top.GetOutput() , "unicast: Player2 -- ERROR,Action BUYTITANIUMFACTORIES variable i_numfactories is above maximum\n");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetFactories().GetFactoryDescription() , "||");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetVP() ,6 );
  top.ResetOutput();

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); // active
  ActionParser ap3("BUYTITANIUMFACTORIES,2,11111");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player2",ap3) , false); // bad delete string
  BOOST_CHECK_EQUAL(top.GetOutput() , "unicast: Player2 -- ERROR,Mismatched delete string length\n" );
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetFactories().GetFactoryDescription() , "||");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetVP() ,6 );
  top.ResetOutput();

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); // active
  ActionParser ap4("BUYTITANIUMFACTORIES,2,111");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player2",ap4) , false); // bad delete string
  BOOST_CHECK_EQUAL(top.GetOutput() , "unicast: Player2 -- ERROR,Mismatched delete string length\n" );
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetFactories().GetFactoryDescription() , "||");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetVP() ,6 );
  top.ResetOutput();

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); // active
  ActionParser ap5("BUYTITANIUMFACTORIES,2,1V11");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player2",ap5) , false); // bad delete string
  BOOST_CHECK_EQUAL(top.GetOutput() ,  "unicast: Player2 -- ERROR,Illegal characters in delete string\n" );
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetFactories().GetFactoryDescription() , "||");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetVP() ,6 );
  top.ResetOutput();

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); // active
  ActionParser ap6("BUYTITANIUMFACTORIES,3,0011");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player2",ap6) , false); // didn't spend enough.
  BOOST_CHECK_EQUAL(top.GetOutput() ,  "unicast: Player2 -- ERROR,You need to spend more to buy that.\n" );
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetFactories().GetFactoryDescription() , "||");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetVP() ,6 );
  top.ResetOutput();

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); // active
  ActionParser ap7("BUYTITANIUMFACTORIES,2,1100");

  ogi.GetPlayers()[2].AddHumans(1);
  ogi.GetPlayers()[2].AddRobots(1);


  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player2",ap7) , true); // should be ok...
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetCommodityHand().GetHandDescription(true,true) , "|OR30-OR30|");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetFactories().GetFactoryDescription() , "|TB-TH|");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetVP() ,10 );

  BOOST_CHECK_EQUAL(top.GetOutput() ,  
                    "broadcast: SPENDS,0,2,0,4,60,|OR30-OR30|\n"
                    "broadcast: PLAYERSTATE,2,10,120,5,5,-1,10,1,1,2,3,0,-1,-1\n"
                    "broadcast: PLAYERFACTORIES,2,|TB-TH|\n"
                    "broadcast: PLAYERCARDBACKS,2,|OR*-OR*|\n"
                    "unicast: Player2 -- PLAYERCARDS,|OR30-OR30|\n"
                    "unicast: Player2 -- PLAYERPURCHASEINFO,6,4,6,3,2,0,0\n"
                    "broadcast: COMMODITYSTOCK,1,26,2,0,3\n"
                    "broadcast: COMMODITYSTOCK,2,39,0,30,7\n"
                    "broadcast: COMMODITYSTOCK,3,53,0,44,10\n"
                    "broadcast: COMMODITYSTOCK,4,34,0,56,13\n"
                    "broadcast: COMMODITYSTOCK,5,16,0,0,17\n"
                    "broadcast: COMMODITYSTOCK,6,23,0,88,20\n"
                    "broadcast: COMMODITYSTOCK,7,14,0,0,30\n"
                    "broadcast: COMMODITYSTOCK,8,12,0,0,40\n"
                    "broadcast: COMMODITYSTOCK,9,12,0,0,50\n"
                    );
}                    

BOOST_AUTO_TEST_CASE( TransitionTestBUYRESEARCHFACTORIES )
{
  OutpostSet oset("");
  ServerGameInfo sgi("Outpost","testOutpost","XMLLOC","XMLFILE");
  ActionParser ap("");

  std::vector<std::string> vcs;
  vcs.push_back("Player1");
  vcs.push_back("Player2");
  vcs.push_back("Player3");

  TestOutputPort top(vcs);

  OutpostStateMachine osm;
  OutpostGameInfo ogi(oset,sgi,top);

  ogi.GetPlayers().add("Player0");
  ogi.GetPlayers().add("Player1");
  ogi.GetPlayers().add("Player2");
  ogi.GetPlayers().add("Player3");
  ogi.GetPlayers().add("Player4");
  ogi.GetPlayers().add("Player5");

  ogi.GetPlayers()[5].GetOwnedItems().AddItem(OUTPOST);
  ogi.GetPlayers()[5].GetOwnedItems().AddItem(OUTPOST); // 10 vp
  ogi.GetPlayers()[4].GetOwnedItems().AddItem(OUTPOST);
  ogi.GetPlayers()[4].GetOwnedItems().AddItem(ROBOTS);  // 8 vp
  ogi.GetPlayers()[3].GetOwnedItems().AddItem(ORBITAL_LAB);
  ogi.GetPlayers()[3].GetOwnedItems().AddItem(ECOPLANTS);  // 8 vp, but lower cost
  ogi.GetPlayers()[2].GetOwnedItems().AddItem(SCIENTISTS);
  ogi.GetPlayers()[2].GetOwnedItems().AddItem(ROBOTS);  // 5 vp
  ogi.GetPlayers()[1].GetOwnedItems().AddItem(SCIENTISTS);
  ogi.GetPlayers()[1].GetOwnedItems().AddItem(NODULE);  // 4 vp
  ogi.GetPlayers()[0].GetOwnedItems().AddItem(SCIENTISTS);
  ogi.GetPlayers()[0].GetOwnedItems().AddItem(WAREHOUSE);  // 3 vp

  ogi.GetPlayers().DetermineTurnOrder();

  ogi.GetPlayers().StartPurchasePhase();
  ogi.GetPlayers()[2].SetPurchaseState(INTERNALIZING);
  ogi.GetPlayers()[1].SetPurchaseState(INTERNALIZING);

  const TransitionType *pBM = GetTransition(osm,"Purchase","BUYRESEARCHFACTORIES","Purchase",numPurchaseTransitions);
  // always manual
  BOOST_CHECK_EQUAL(pBM->IsAuto("Purchase",ogi) , false );

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Purchase",ogi) , false); // active, but no money.
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , false); // internalizing, but no money
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , false); // internalizing, but no money.
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Purchase",ogi) , false); 

  ogi.GetPlayers()[5].GetCommodityHand().AddCommodity(Commodity(RESEARCH_COMMODITY,40,false,false));
  ogi.GetPlayers()[2].GetCommodityHand().AddCommodity(Commodity(RESEARCH_COMMODITY,40,false,false));
  ogi.GetPlayers()[2].GetCommodityHand().AddCommodity(Commodity(RESEARCH_COMMODITY,40,false,false));

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Purchase",ogi) , false); // no titanium Production
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , false); // no titanium Production
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Purchase",ogi) , false); 

  ogi.GetPlayers()[5].GetOwnedItems().AddItem(LABORATORY);
  ogi.GetPlayers()[2].GetOwnedItems().AddItem(LABORATORY);

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Purchase",ogi) , true); // no titanium Production
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); // no titanium Production
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Purchase",ogi) , false); 

  ogi.GetPlayers()[5].SetHighBid(3);
  ogi.GetPlayers()[2].SetHighBid(4);
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , false); 

  ogi.GetPlayers()[5].SetHighBid(NOT_HIGH_BID);
  ogi.GetPlayers()[2].SetHighBid(NOT_HIGH_BID);

 
  // four at thirty each
  ogi.GetPlayers()[2].GetCommodityHand().AddCommodity(Commodity(RESEARCH_COMMODITY,40,false,false));
  ogi.GetPlayers()[2].GetCommodityHand().AddCommodity(Commodity(RESEARCH_COMMODITY,40,false,false));

  ogi.GetPlayers().PurchaseIncrementTurnOrder(); 
  ogi.GetPlayers().PurchaseIncrementTurnOrder(); 
  ogi.GetPlayers().PurchaseIncrementTurnOrder(); 
  ogi.GetPlayers().PurchaseIncrementTurnOrder(); 

  BOOST_CHECK_EQUAL( ogi.GetPlayers().AllPlayersDone() , true);
  BOOST_CHECK_EQUAL( ogi.GetPlayers().PurchaseAllPlayersDone() , false );

  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetFactories().GetFactoryDescription() , "||");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetVP() , 10 );
  
  ActionParser ap1("BUYRESEARCHFACTORIES,-1,1111");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player2",ap1) , false); // illegal value for # men.
  BOOST_CHECK_EQUAL(top.GetOutput() , "unicast: Player2 -- ERROR,Action BUYRESEARCHFACTORIES variable i_numfactories is below minimum\n");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetFactories().GetFactoryDescription() , "||");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetVP() ,10 );
  top.ResetOutput();

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); // active
  ActionParser ap2("BUYRESEARCHFACTORIES,5,111111");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player2",ap2) , false); // illegal value for # men.
  BOOST_CHECK_EQUAL(top.GetOutput() , "unicast: Player2 -- ERROR,Action BUYRESEARCHFACTORIES variable i_numfactories is above maximum\n");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetFactories().GetFactoryDescription() , "||");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetVP() ,10 );
  top.ResetOutput();

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); // active
  ActionParser ap3("BUYRESEARCHFACTORIES,2,11111");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player2",ap3) , false); // bad delete string
  BOOST_CHECK_EQUAL(top.GetOutput() , "unicast: Player2 -- ERROR,Mismatched delete string length\n" );
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetFactories().GetFactoryDescription() , "||");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetVP() ,10 );
  top.ResetOutput();

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); // active
  ActionParser ap4("BUYRESEARCHFACTORIES,2,111");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player2",ap4) , false); // bad delete string
  BOOST_CHECK_EQUAL(top.GetOutput() , "unicast: Player2 -- ERROR,Mismatched delete string length\n" );
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetFactories().GetFactoryDescription() , "||");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetVP() ,10 );
  top.ResetOutput();

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); // active
  ActionParser ap5("BUYRESEARCHFACTORIES,2,1V11");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player2",ap5) , false); // bad delete string
  BOOST_CHECK_EQUAL(top.GetOutput() ,  "unicast: Player2 -- ERROR,Illegal characters in delete string\n" );
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetFactories().GetFactoryDescription() , "||");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetVP() ,10 );
  top.ResetOutput();

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); // active
  ActionParser ap6("BUYRESEARCHFACTORIES,3,0011");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player2",ap6) , false); // didn't spend enough.
  BOOST_CHECK_EQUAL(top.GetOutput() ,  "unicast: Player2 -- ERROR,You need to spend more to buy that.\n" );
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetFactories().GetFactoryDescription() , "||");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetVP() ,10 );
  top.ResetOutput();

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); // active
  ActionParser ap7("BUYRESEARCHFACTORIES,2,1100");

  ogi.GetPlayers()[2].AddHumans(1);

  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player2",ap7) , true); // should be ok...
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetCommodityHand().GetHandDescription(true,true) , "|RE40-RE40|");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetFactories().GetFactoryDescription() , "|RU-RH|");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetVP() ,12 );

  BOOST_CHECK_EQUAL(top.GetOutput() ,  
                    "broadcast: SPENDS,0,2,0,5,80,|RE40-RE40|\n"
                    "broadcast: PLAYERSTATE,2,12,190,5,5,-1,10,1,0,2,3,0,-1,-1\n"
                    "broadcast: PLAYERFACTORIES,2,|RU-RH|\n"
                    "broadcast: PLAYERCARDBACKS,2,|RE*-RE*|\n"
                    "unicast: Player2 -- PLAYERCARDS,|RE40-RE40|\n"
                    "unicast: Player2 -- PLAYERPURCHASEINFO,8,4,8,4,0,2,1\n"
                    "broadcast: COMMODITYSTOCK,1,26,0,0,3\n"
                    "broadcast: COMMODITYSTOCK,2,39,0,30,7\n"
                    "broadcast: COMMODITYSTOCK,3,53,0,44,10\n"
                    "broadcast: COMMODITYSTOCK,4,34,2,56,13\n"
                    "broadcast: COMMODITYSTOCK,5,16,0,0,17\n"
                    "broadcast: COMMODITYSTOCK,6,23,0,88,20\n"
                    "broadcast: COMMODITYSTOCK,7,14,0,0,30\n"
                    "broadcast: COMMODITYSTOCK,8,12,0,0,40\n"
                    "broadcast: COMMODITYSTOCK,9,12,0,0,50\n"
                    );
}                    


BOOST_AUTO_TEST_CASE( TransitionTestBUYNEWCHEMFACTORIES )
{
  OutpostSet oset("");
  ServerGameInfo sgi("Outpost","testOutpost","XMLLOC","XMLFILE");
  ActionParser ap("");

  std::vector<std::string> vcs;
  vcs.push_back("Player1");
  vcs.push_back("Player2");
  vcs.push_back("Player3");

  TestOutputPort top(vcs);

  OutpostStateMachine osm;
  OutpostGameInfo ogi(oset,sgi,top);

  ogi.GetPlayers().add("Player0");
  ogi.GetPlayers().add("Player1");
  ogi.GetPlayers().add("Player2");
  ogi.GetPlayers().add("Player3");
  ogi.GetPlayers().add("Player4");
  ogi.GetPlayers().add("Player5");

  ogi.GetPlayers()[5].GetOwnedItems().AddItem(OUTPOST);
  ogi.GetPlayers()[5].GetOwnedItems().AddItem(OUTPOST); // 10 vp
  ogi.GetPlayers()[4].GetOwnedItems().AddItem(OUTPOST);
  ogi.GetPlayers()[4].GetOwnedItems().AddItem(ROBOTS);  // 8 vp
  ogi.GetPlayers()[3].GetOwnedItems().AddItem(ORBITAL_LAB);
  ogi.GetPlayers()[3].GetOwnedItems().AddItem(ECOPLANTS);  // 8 vp, but lower cost
  ogi.GetPlayers()[2].GetOwnedItems().AddItem(SCIENTISTS);
  ogi.GetPlayers()[2].GetOwnedItems().AddItem(ROBOTS);  // 5 vp
  ogi.GetPlayers()[1].GetOwnedItems().AddItem(SCIENTISTS);
  ogi.GetPlayers()[1].GetOwnedItems().AddItem(NODULE);  // 4 vp
  ogi.GetPlayers()[0].GetOwnedItems().AddItem(SCIENTISTS);
  ogi.GetPlayers()[0].GetOwnedItems().AddItem(WAREHOUSE);  // 3 vp

  ogi.GetPlayers().DetermineTurnOrder();

  ogi.GetPlayers().StartPurchasePhase();
  ogi.GetPlayers()[2].SetPurchaseState(INTERNALIZING);
  ogi.GetPlayers()[1].SetPurchaseState(INTERNALIZING);

  const TransitionType *pBM = GetTransition(osm,"Purchase","BUYNEWCHEMFACTORIES","Purchase",numPurchaseTransitions);
  // always manual
  BOOST_CHECK_EQUAL(pBM->IsAuto("Purchase",ogi) , false );

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Purchase",ogi) , false); // active, but no money.
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , false); // internalizing, but no money
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , false); // internalizing, but no money.
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Purchase",ogi) , false); 

  ogi.GetPlayers()[5].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,60,false,false));
  ogi.GetPlayers()[2].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,30,false,false));
  ogi.GetPlayers()[2].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,30,false,false));

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , false); // internalizing, but no money.
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Purchase",ogi) , false); 
 
  // four at twenty each
  ogi.GetPlayers()[5].GetCommodityHand().AddCommodity(Commodity(RESEARCH_COMMODITY,60,false,false));
  ogi.GetPlayers()[2].GetCommodityHand().AddCommodity(Commodity(RESEARCH_COMMODITY,30,false,false));
  ogi.GetPlayers()[2].GetCommodityHand().AddCommodity(Commodity(RESEARCH_COMMODITY,30,false,false));
  ogi.GetPlayers()[2].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,60,false,false));

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Purchase",ogi) , true); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , false); // internalizing, but no money.
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Purchase",ogi) , false); 

  ogi.GetPlayers()[5].SetHighBid(3);
  ogi.GetPlayers()[2].SetHighBid(4);
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , false); 

  ogi.GetPlayers()[5].SetHighBid(NOT_HIGH_BID);
  ogi.GetPlayers()[2].SetHighBid(NOT_HIGH_BID);


  ogi.GetPlayers().PurchaseIncrementTurnOrder(); 
  ogi.GetPlayers().PurchaseIncrementTurnOrder(); 
  ogi.GetPlayers().PurchaseIncrementTurnOrder(); 
  ogi.GetPlayers().PurchaseIncrementTurnOrder(); 

  BOOST_CHECK_EQUAL( ogi.GetPlayers().AllPlayersDone() , true);
  BOOST_CHECK_EQUAL( ogi.GetPlayers().PurchaseAllPlayersDone() , false );

  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetFactories().GetFactoryDescription() , "||");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetVP() , 5 );
  
  ActionParser ap1("BUYNEWCHEMFACTORIES,-1,11111");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player2",ap1) , false); // illegal value for # men.
  BOOST_CHECK_EQUAL(top.GetOutput() , "unicast: Player2 -- ERROR,Action BUYNEWCHEMFACTORIES variable i_numfactories is below minimum\n");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetFactories().GetFactoryDescription() , "||");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetVP() , 5 );
  top.ResetOutput();

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); // active
  ActionParser ap2("BUYNEWCHEMFACTORIES,5,11111");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player2",ap2) , false); // illegal value for # men.
  BOOST_CHECK_EQUAL(top.GetOutput() , "unicast: Player2 -- ERROR,Action BUYNEWCHEMFACTORIES variable i_numfactories is above maximum\n");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetFactories().GetFactoryDescription() , "||");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetVP() , 5 );
  top.ResetOutput();

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); // active
  ActionParser ap3("BUYNEWCHEMFACTORIES,2,111111");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player2",ap3) , false); // bad delete string
  BOOST_CHECK_EQUAL(top.GetOutput() , "unicast: Player2 -- ERROR,Mismatched delete string length\n" );
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetFactories().GetFactoryDescription() , "||");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetVP() , 5 );
  top.ResetOutput();

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); // active
  ActionParser ap4("BUYNEWCHEMFACTORIES,2,1111");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player2",ap4) , false); // bad delete string
  BOOST_CHECK_EQUAL(top.GetOutput() , "unicast: Player2 -- ERROR,Mismatched delete string length\n" );
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetFactories().GetFactoryDescription() , "||");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetVP() , 5 );
  top.ResetOutput();

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); // active
  ActionParser ap5("BUYNEWCHEMFACTORIES,2,1V111");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player2",ap5) , false); // bad delete string
  BOOST_CHECK_EQUAL(top.GetOutput() ,  "unicast: Player2 -- ERROR,Illegal characters in delete string\n" );
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetFactories().GetFactoryDescription() , "||");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetVP() , 5 );
  top.ResetOutput();

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); // active
  ActionParser ap6("BUYNEWCHEMFACTORIES,2,00001");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player2",ap6) , false); // didn't spend enough.
  BOOST_CHECK_EQUAL(top.GetOutput() ,  "unicast: Player2 -- ERROR,You need to spend more to buy that.\n" );
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetFactories().GetFactoryDescription() , "||");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetVP() , 5 );
  top.ResetOutput();

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); // active
  ActionParser ap6a("BUYNEWCHEMFACTORIES,2,00111");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player2",ap6a) , false); // enough, but no research
  BOOST_CHECK_EQUAL(top.GetOutput() ,  "unicast: Player2 -- ERROR,You need to spend more Research resource cards to buy that.\n" );
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetFactories().GetFactoryDescription() , "||");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetVP() , 5 );
  top.ResetOutput();

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , true); // active
  ActionParser ap7("BUYNEWCHEMFACTORIES,2,11011");

  ogi.GetPlayers()[2].AddHumans(1);

  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player2",ap7) , true); // should be ok...
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetCommodityHand().GetHandDescription(true,true) , "|OR60|");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetFactories().GetFactoryDescription() , "|NU-NH|");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetVP() , 8 );

  BOOST_CHECK_EQUAL(top.GetOutput() ,  
                    "broadcast: SPENDS,0,2,0,6,120,|RE30-RE30-OR30-OR30|\n"
                    "broadcast: PLAYERSTATE,2,8,90,5,5,-1,10,1,0,2,3,0,-1,-1\n"
                    "broadcast: PLAYERFACTORIES,2,|NU-NH|\n"
                    "broadcast: PLAYERCARDBACKS,2,|OR*|\n"
                    "unicast: Player2 -- PLAYERCARDS,|OR60|\n"
                    "unicast: Player2 -- PLAYERPURCHASEINFO,6,4,6,3,0,0,0\n"
                    "broadcast: COMMODITYSTOCK,1,26,2,0,3\n"
                    "broadcast: COMMODITYSTOCK,2,39,0,30,7\n"
                    "broadcast: COMMODITYSTOCK,3,53,0,44,10\n"
                    "broadcast: COMMODITYSTOCK,4,34,2,56,13\n"
                    "broadcast: COMMODITYSTOCK,5,16,0,0,17\n"
                    "broadcast: COMMODITYSTOCK,6,23,0,88,20\n"
                    "broadcast: COMMODITYSTOCK,7,14,0,0,30\n"
                    "broadcast: COMMODITYSTOCK,8,12,0,0,40\n"
                    "broadcast: COMMODITYSTOCK,9,12,0,0,50\n"
                    );
}                    


BOOST_AUTO_TEST_CASE( TransitionTestWATERMULLIGAN )
{
  OutpostSet oset("");
  ServerGameInfo sgi("Outpost","testOutpost","XMLLOC","XMLFILE");
  ActionParser ap("");

  std::vector<std::string> vcs;
  vcs.push_back("Player1");
  vcs.push_back("Player2");
  vcs.push_back("Player3");

  TestOutputPort top(vcs);

  OutpostStateMachine osm;
  OutpostGameInfo ogi(oset,sgi,top);

  ogi.GetPlayers().add("Player0");
  ogi.GetPlayers().add("Player1");
  ogi.GetPlayers().add("Player2");
  ogi.GetPlayers().add("Player3");
  ogi.GetPlayers().add("Player4");
  ogi.GetPlayers().add("Player5");

  ogi.GetPlayers().DetermineTurnOrder();
  // ok...so, now we have a completely random turn order...
  
  Player &pl0 = ogi.GetPlayers().GetCurTurnPlayer(); ogi.GetPlayers().IncrementTurnOrder();
  Player &pl1 = ogi.GetPlayers().GetCurTurnPlayer(); ogi.GetPlayers().IncrementTurnOrder();
  Player &pl2 = ogi.GetPlayers().GetCurTurnPlayer(); ogi.GetPlayers().IncrementTurnOrder();
  Player &pl3 = ogi.GetPlayers().GetCurTurnPlayer(); ogi.GetPlayers().IncrementTurnOrder();
  Player &pl4 = ogi.GetPlayers().GetCurTurnPlayer(); ogi.GetPlayers().IncrementTurnOrder();
  Player &pl5 = ogi.GetPlayers().GetCurTurnPlayer(); ogi.GetPlayers().IncrementTurnOrder();

  // pl0 will be the active player
  // pl1 will be internalizing
  // pl3 will be internalizing
  // pl4 will be internalizing

  ogi.GetPlayers().StartPurchasePhase();
  pl1.SetPurchaseState(INTERNALIZING);
  pl3.SetPurchaseState(INTERNALIZING);
  pl4.SetPurchaseState(INTERNALIZING);

  const TransitionType *pBM = GetTransition(osm,"Purchase","WATERMULLIGAN","Purchase",numPurchaseTransitions);
  // always manual
  BOOST_CHECK_EQUAL(pBM->IsAuto("Purchase",ogi) , false );

  // only allowed on turn 1
  ogi.SetCurTurn(2);

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal(pl0.GetName(),"Purchase",ogi) , false); // active, but no money.
  BOOST_CHECK_EQUAL(pBM->IsLegal(pl1.GetName(),"Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal(pl2.GetName(),"Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal(pl3.GetName(),"Purchase",ogi) , false); // internalizing, but no money
  BOOST_CHECK_EQUAL(pBM->IsLegal(pl4.GetName(),"Purchase",ogi) , false); // internalizing, but no money.
  BOOST_CHECK_EQUAL(pBM->IsLegal(pl5.GetName(),"Purchase",ogi) , false); 

  ogi.SetCurTurn(1);

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal(pl0.GetName(),"Purchase",ogi) , true); // active, but no money.
  BOOST_CHECK_EQUAL(pBM->IsLegal(pl1.GetName(),"Purchase",ogi) , true); // internalizing, no money
  BOOST_CHECK_EQUAL(pBM->IsLegal(pl2.GetName(),"Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal(pl3.GetName(),"Purchase",ogi) , true); // internalizing, but no money
  BOOST_CHECK_EQUAL(pBM->IsLegal(pl4.GetName(),"Purchase",ogi) , true); // internalizing, but no money.
  BOOST_CHECK_EQUAL(pBM->IsLegal(pl5.GetName(),"Purchase",ogi) , false); 

  // at least 20 credits prevents you from using the mulligan
  pl0.GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,20,false,false));

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal(pl0.GetName(),"Purchase",ogi) , false); // active, but no money.
  BOOST_CHECK_EQUAL(pBM->IsLegal(pl1.GetName(),"Purchase",ogi) , true); 
  BOOST_CHECK_EQUAL(pBM->IsLegal(pl2.GetName(),"Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal(pl3.GetName(),"Purchase",ogi) , true); // internalizing, but no money
  BOOST_CHECK_EQUAL(pBM->IsLegal(pl4.GetName(),"Purchase",ogi) , true); // internalizing, but no money.
  BOOST_CHECK_EQUAL(pBM->IsLegal(pl5.GetName(),"Purchase",ogi) , false); 

  // having purchased anything keeps you from using the mulligan.
  pl4.GetFactories().AddFactory(ORE_FACTORY);
  pl4.GetFactories().AddFactory(ORE_FACTORY);
  pl4.GetFactories().AddFactory(ORE_FACTORY);
  pl4.GetFactories().AddFactory(ORE_FACTORY);
  
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal(pl0.GetName(),"Purchase",ogi) , false); // active, but no money.
  BOOST_CHECK_EQUAL(pBM->IsLegal(pl1.GetName(),"Purchase",ogi) , true); 
  BOOST_CHECK_EQUAL(pBM->IsLegal(pl2.GetName(),"Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal(pl3.GetName(),"Purchase",ogi) , true); // internalizing, but no money
  BOOST_CHECK_EQUAL(pBM->IsLegal(pl4.GetName(),"Purchase",ogi) , false); // internalizing, but no money.
  BOOST_CHECK_EQUAL(pBM->IsLegal(pl5.GetName(),"Purchase",ogi) , false); 

  pl1.SetHighBid(3);
  pl3.SetHighBid(4);
  BOOST_CHECK_EQUAL(pBM->IsLegal(pl1.GetName(),"Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal(pl3.GetName(),"Purchase",ogi) , false); 

  pl1.SetHighBid(NOT_HIGH_BID);
  pl3.SetHighBid(NOT_HIGH_BID);

  pl3.GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,3,false,false));
  pl3.GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,5,false,false));
  pl3.GetCommodityHand().AddCommodity(Commodity(WATER_COMMODITY,7,false,false));

  bool b;
  BOOST_CHECK_NO_THROW( b = pBM->IsLegal(pl3.GetName(),"Purchase",ogi); );

  BOOST_CHECK_EQUAL( b , true); // internalizing, but no money

  ogi.GetPlayers().PurchaseIncrementTurnOrder(); 
  ogi.GetPlayers().PurchaseIncrementTurnOrder(); 
  ogi.GetPlayers().PurchaseIncrementTurnOrder(); 
  ogi.GetPlayers().PurchaseIncrementTurnOrder(); 

  BOOST_CHECK_EQUAL( ogi.GetPlayers().AllPlayersDone() , true);
  BOOST_CHECK_EQUAL( ogi.GetPlayers().PurchaseAllPlayersDone() , false );

  BOOST_CHECK_EQUAL(pl3.GetFactories().GetFactoryDescription() , "||");
  BOOST_CHECK_EQUAL(pl3.GetVP() , 0 );
  BOOST_CHECK_EQUAL(ogi.GetCommodities().GetDeck(ORE_COMMODITY).GetDiscardSize() , 0);
  BOOST_CHECK_EQUAL(ogi.GetCommodities().GetDeck(WATER_COMMODITY).GetDiscardSize() , 0);

  BOOST_CHECK_NO_THROW( b = ogi.GetPlayers().CanPlayerMulligan(pl3.GetName()) );
  BOOST_CHECK_EQUAL( b , true );

  ActionParser ap1("WATERMULLIGAN");

  pl3.GetFactories().AddFactory(ORE_FACTORY);
  pl3.GetFactories().AddFactory(ORE_FACTORY);
  pl3.GetFactories().AddFactory(ORE_FACTORY);
  pl3.AddHumans(3);

  BOOST_CHECK_NO_THROW ( b = pBM->ExecuteAction("Purchase",ogi,pl3.GetName(),ap1) );
  BOOST_CHECK_EQUAL( b , true); 

  BOOST_CHECK_EQUAL(pl3.GetCommodityHand().GetHandDescription(true,true) , "||");
  BOOST_CHECK_EQUAL(pl3.GetFactories().GetFactoryDescription() , "|OU-OH-OH-WH|");
  BOOST_CHECK_EQUAL(pl3.GetVP() , 3 );

  BOOST_CHECK_EQUAL(ogi.GetCommodities().GetDeck(ORE_COMMODITY).GetDiscardSize() , 2);
  BOOST_CHECK_EQUAL(ogi.GetCommodities().GetDeck(WATER_COMMODITY).GetDiscardSize() , 1);

  std::ostringstream oss;
  oss << "broadcast: SPENDS,1," << pl3.GetId() << ",3,3,0,|WA7-OR5-OR3|\n";
  oss << "broadcast: PLAYERSTATE," << pl3.GetId() << ",3,0,5,5,0,10,3,0,2,3,0,-1,-1\n";
  oss << "broadcast: PLAYERFACTORIES," << pl3.GetId() << ",|OU-OH-OH-WH|\n";
  oss << "broadcast: PLAYERCARDBACKS," << pl3.GetId() << ",||\n";
  oss << "unicast: " << pl3.GetName() << " -- PLAYERCARDS,||\n";
  oss << "unicast: " << pl3.GetName() << " -- PLAYERPURCHASEINFO,0,0,0,0,0,0,0\n";
  oss << 
                    "broadcast: COMMODITYSTOCK,1,26,2,0,3\n"
                    "broadcast: COMMODITYSTOCK,2,39,1,30,7\n"
                    "broadcast: COMMODITYSTOCK,3,53,0,44,10\n"
                    "broadcast: COMMODITYSTOCK,4,34,0,56,13\n"
                    "broadcast: COMMODITYSTOCK,5,16,0,0,17\n"
                    "broadcast: COMMODITYSTOCK,6,23,0,88,20\n"
                    "broadcast: COMMODITYSTOCK,7,14,0,0,30\n"
                    "broadcast: COMMODITYSTOCK,8,12,0,0,40\n"
                    "broadcast: COMMODITYSTOCK,9,12,0,0,50\n"
    ;
  BOOST_CHECK_EQUAL(top.GetOutput() ,  oss.str() );

}                    

BOOST_AUTO_TEST_CASE( TransitionTestINTERNALIZE )
{
  OutpostSet oset("");
  ServerGameInfo sgi("Outpost","testOutpost","XMLLOC","XMLFILE");
  ActionParser ap("INTERNALIZE");

  std::vector<std::string> vcs;
  vcs.push_back("Player1");
  vcs.push_back("Player2");
  vcs.push_back("Player3");

  TestOutputPort top(vcs);

  OutpostStateMachine osm;
  OutpostGameInfo ogi(oset,sgi,top);

  ogi.GetPlayers().add("Player0");
  ogi.GetPlayers().add("Player1");
  ogi.GetPlayers().add("Player2");
  ogi.GetPlayers().add("Player3");
  ogi.GetPlayers().add("Player4");
  ogi.GetPlayers().add("Player5");

  ogi.GetPlayers()[5].GetOwnedItems().AddItem(OUTPOST);
  ogi.GetPlayers()[5].GetOwnedItems().AddItem(OUTPOST); // 10 vp
  ogi.GetPlayers()[4].GetOwnedItems().AddItem(OUTPOST);
  ogi.GetPlayers()[4].GetOwnedItems().AddItem(ROBOTS);  // 8 vp
  ogi.GetPlayers()[3].GetOwnedItems().AddItem(ORBITAL_LAB);
  ogi.GetPlayers()[3].GetOwnedItems().AddItem(ECOPLANTS);  // 8 vp, but lower cost
  ogi.GetPlayers()[2].GetOwnedItems().AddItem(SCIENTISTS);
  ogi.GetPlayers()[2].GetOwnedItems().AddItem(ROBOTS);  // 5 vp
  ogi.GetPlayers()[1].GetOwnedItems().AddItem(SCIENTISTS);
  ogi.GetPlayers()[1].GetOwnedItems().AddItem(NODULE);  // 4 vp
  ogi.GetPlayers()[0].GetOwnedItems().AddItem(SCIENTISTS);
  ogi.GetPlayers()[0].GetOwnedItems().AddItem(WAREHOUSE);  // 3 vp

  ogi.GetPlayers().DetermineTurnOrder();

  ogi.GetPlayers().StartPurchasePhase();
  ogi.GetPlayers()[2].SetPurchaseState(INTERNAL_DONE);
  ogi.GetPlayers()[1].SetPurchaseState(INTERNALIZING);
  ogi.GetPlayers().PurchaseIncrementTurnOrder();
  // at this point, 5 is done, 4 is active, 3 and 0 are unplayed, 2 is INTERNAL_DONE, and 1 is INTERNALIZING

  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetPurchaseState() , ACTIVE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetPurchaseState() , UNPLAYED );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetPurchaseState() , INTERNAL_DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetPurchaseState() , INTERNALIZING );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetPurchaseState() , UNPLAYED );

  const TransitionType *pBM = GetTransition(osm,"Purchase","INTERNALIZE","Purchase",numPurchaseTransitions);
  // always manual
  BOOST_CHECK_EQUAL(pBM->IsAuto("Purchase",ogi) , false );

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , true); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , true); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Purchase",ogi) , true); 


  ogi.GetPlayers()[4].SetBidState(CURBID);
  ogi.GetPlayers()[3].SetBidState(CURBID);
  ogi.GetPlayers()[0].SetBidState(CURBID);
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Purchase",ogi) , false); 

  ogi.GetPlayers()[4].SetBidState(PASS);
  ogi.GetPlayers()[3].SetBidState(UNBID);
  ogi.GetPlayers()[0].SetBidState(HASBID);
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , true); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , true); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Purchase",ogi) , true); 

  ogi.GetPlayers()[4].SetHighBid(3);
  ogi.GetPlayers()[3].SetHighBid(4);
  ogi.GetPlayers()[0].SetHighBid(5);

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Purchase",ogi) , false); 

  ogi.GetPlayers()[4].SetHighBid(NOT_HIGH_BID);
  ogi.GetPlayers()[3].SetHighBid(NOT_HIGH_BID);
  ogi.GetPlayers()[0].SetHighBid(NOT_HIGH_BID);

  ogi.GetPlayers().PurchaseIncrementTurnOrder();

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , true); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Purchase",ogi) , true); 

  ogi.GetPlayers().PurchaseIncrementTurnOrder();

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Purchase",ogi) , true); 

  ogi.GetPlayers().PurchaseIncrementTurnOrder();

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Purchase",ogi) , false); 

  ogi.GetPlayers().StartPurchasePhase();
  ogi.GetPlayers()[2].SetPurchaseState(INTERNAL_DONE);
  ogi.GetPlayers()[1].SetPurchaseState(INTERNALIZING);
  ogi.GetPlayers().PurchaseIncrementTurnOrder();
  // at this point, 5 is done, 4 is active, 3 and 0 are unplayed, 2 is INTERNAL_DONE, and 1 is INTERNALIZING

  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetPurchaseState() , ACTIVE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetPurchaseState() , UNPLAYED );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetPurchaseState() , INTERNAL_DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetPurchaseState() , INTERNALIZING );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetPurchaseState() , UNPLAYED );

  ogi.GetPlayers()[4].SetBidState(HASBID);

  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player4",ap) , true); // should be ok...
  BOOST_CHECK_EQUAL( top.GetOutput() , 
                     "broadcast: PLAYERSTATE,4,8,150,8,10,-1,10,0,0,2,3,0,-1,-1\n"
                     "broadcast: PLAYERSTATE,3,8,100,5,5,0,5,0,0,1,0,0,-1,-1\n"
                     );
  top.ResetOutput();
  BOOST_CHECK_EQUAL( ogi.GetPlayers().GetCurTurnPlayer().GetId() , 3 );

  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetPurchaseState() , INTERNALIZING );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetPurchaseState() , ACTIVE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetPurchaseState() , INTERNAL_DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetPurchaseState() , INTERNALIZING );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetPurchaseState() , UNPLAYED );
  
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player0",ap) , true); // should be ok...
  BOOST_CHECK_EQUAL( top.GetOutput() , 
                     "broadcast: PLAYERSTATE,0,3,65,8,5,0,10,0,0,2,3,0,-1,-1\n"
                     );
  top.ResetOutput();
  
  BOOST_CHECK_EQUAL( ogi.GetPlayers().GetCurTurnPlayer().GetId() , 3 );

  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetPurchaseState() , INTERNALIZING );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetPurchaseState() , ACTIVE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetPurchaseState() , INTERNAL_DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetPurchaseState() , INTERNALIZING );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetPurchaseState() , INTERNALIZING );
  
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player3",ap) , true); // should be ok...
  BOOST_CHECK_EQUAL( top.GetOutput() , 
                     "broadcast: PLAYERSTATE,3,8,100,5,5,0,5,0,0,2,3,0,-1,-1\n"
                     );

  BOOST_CHECK_EQUAL( ogi.GetPlayers().AllPlayersDone() , true );

  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetPurchaseState() , INTERNALIZING );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetPurchaseState() , INTERNALIZING );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetPurchaseState() , INTERNAL_DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetPurchaseState() , INTERNALIZING );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetPurchaseState() , INTERNALIZING );
}

BOOST_AUTO_TEST_CASE( TransitionTestDONE )
{
  OutpostSet oset("");
  ServerGameInfo sgi("Outpost","testOutpost","XMLLOC","XMLFILE");
  ActionParser ap("DONE");

  std::vector<std::string> vcs;
  vcs.push_back("Player1");
  vcs.push_back("Player2");
  vcs.push_back("Player3");

  TestOutputPort top(vcs);

  OutpostStateMachine osm;
  OutpostGameInfo ogi(oset,sgi,top);

  ogi.GetPlayers().add("Player0");
  ogi.GetPlayers().add("Player1");
  ogi.GetPlayers().add("Player2");
  ogi.GetPlayers().add("Player3");
  ogi.GetPlayers().add("Player4");
  ogi.GetPlayers().add("Player5");

  ogi.GetPlayers()[5].GetOwnedItems().AddItem(OUTPOST);
  ogi.GetPlayers()[5].GetOwnedItems().AddItem(OUTPOST); // 10 vp
  ogi.GetPlayers()[4].GetOwnedItems().AddItem(OUTPOST);
  ogi.GetPlayers()[4].GetOwnedItems().AddItem(ROBOTS);  // 8 vp
  ogi.GetPlayers()[3].GetOwnedItems().AddItem(ORBITAL_LAB);
  ogi.GetPlayers()[3].GetOwnedItems().AddItem(ECOPLANTS);  // 8 vp, but lower cost
  ogi.GetPlayers()[2].GetOwnedItems().AddItem(SCIENTISTS);
  ogi.GetPlayers()[2].GetOwnedItems().AddItem(ROBOTS);  // 5 vp
  ogi.GetPlayers()[1].GetOwnedItems().AddItem(SCIENTISTS);
  ogi.GetPlayers()[1].GetOwnedItems().AddItem(NODULE);  // 4 vp
  ogi.GetPlayers()[0].GetOwnedItems().AddItem(SCIENTISTS);
  ogi.GetPlayers()[0].GetOwnedItems().AddItem(WAREHOUSE);  // 3 vp

  ogi.GetPlayers().DetermineTurnOrder();

  ogi.GetPlayers().StartPurchasePhase();
  ogi.GetPlayers()[2].SetPurchaseState(INTERNAL_DONE);
  ogi.GetPlayers()[1].SetPurchaseState(INTERNALIZING);
  ogi.GetPlayers().PurchaseIncrementTurnOrder();
  // at this point, 5 is done, 4 is active, 3 and 0 are unplayed, 2 is INTERNAL_DONE, and 1 is INTERNALIZING

  const TransitionType *pBM = GetTransition(osm,"Purchase","DONE","Purchase",numPurchaseTransitions);
  // always manual
  BOOST_CHECK_EQUAL(pBM->IsAuto("Purchase",ogi) , false );

  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetPurchaseState() , ACTIVE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetPurchaseState() , UNPLAYED );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetPurchaseState() , INTERNAL_DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetPurchaseState() , INTERNALIZING );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetPurchaseState() , UNPLAYED );

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , true); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , true); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Purchase",ogi) , false); 

  ogi.GetPlayers()[4].SetBidState(CURBID);
  ogi.GetPlayers()[1].SetBidState(CURBID);
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , false); 

  ogi.GetPlayers()[4].SetBidState(PASS);
  ogi.GetPlayers()[1].SetBidState(HASBID);
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , true); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , true); 

  ogi.GetPlayers()[4].SetHighBid(3);
  ogi.GetPlayers()[1].SetHighBid(6);
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , false); 

  ogi.GetPlayers()[4].SetHighBid(NOT_HIGH_BID);
  ogi.GetPlayers()[1].SetHighBid(NOT_HIGH_BID);

  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player4",ap) , true); // should be ok...
  BOOST_CHECK_EQUAL( top.GetOutput(),
                     "broadcast: PLAYERSTATE,4,8,150,8,10,-1,10,0,0,3,3,0,-1,-1\n"
                     "broadcast: PLAYERSTATE,3,8,100,5,5,0,5,0,0,1,3,0,-1,-1\n"
                     );
  top.ResetOutput();

  
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetPurchaseState() , ACTIVE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetPurchaseState() , INTERNAL_DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetPurchaseState() , INTERNALIZING );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetPurchaseState() , UNPLAYED );

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , true); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , true); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Purchase",ogi) , false); 

  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player1",ap) , true); // should be ok...
  BOOST_CHECK_EQUAL( top.GetOutput(),
                     "broadcast: PLAYERSTATE,1,4,65,5,8,0,10,0,0,4,3,0,-1,-1\n"
                     );
  top.ResetOutput();

  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetPurchaseState() , ACTIVE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetPurchaseState() , INTERNAL_DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetPurchaseState() , INTERNAL_DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetPurchaseState() , UNPLAYED );

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , true); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Purchase",ogi) , false); 

  ogi.GetPlayers().PurchaseIncrementTurnOrder();

  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetPurchaseState() , INTERNAL_DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetPurchaseState() , INTERNAL_DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetPurchaseState() , ACTIVE );

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Purchase",ogi) , true); 

  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player0",ap) , true); // should be ok...
  BOOST_CHECK_EQUAL( top.GetOutput(),
                     "broadcast: PLAYERSTATE,0,3,65,8,5,0,10,0,0,3,3,0,-1,-1\n"
                     );
  top.ResetOutput();

  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetPurchaseState() , INTERNAL_DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetPurchaseState() , INTERNAL_DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetPurchaseState() , DONE );

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Purchase",ogi) , false); 
}


BOOST_AUTO_TEST_CASE( TransitionTestFINALDONE )
{
  OutpostSet oset("");
  ServerGameInfo sgi("Outpost","testOutpost","XMLLOC","XMLFILE");
  ActionParser ap("DONE");

  std::vector<std::string> vcs;
  vcs.push_back("Player1");
  vcs.push_back("Player2");
  vcs.push_back("Player3");

  TestOutputPort top(vcs);

  OutpostStateMachine osm;
  OutpostGameInfo ogi(oset,sgi,top);

  ogi.GetPlayers().add("Player0");
  ogi.GetPlayers().add("Player1");
  ogi.GetPlayers().add("Player2");
  ogi.GetPlayers().add("Player3");
  ogi.GetPlayers().add("Player4");
  ogi.GetPlayers().add("Player5");

  ogi.GetPlayers()[5].GetOwnedItems().AddItem(OUTPOST);
  ogi.GetPlayers()[5].GetOwnedItems().AddItem(OUTPOST); // 10 vp
  ogi.GetPlayers()[4].GetOwnedItems().AddItem(OUTPOST);
  ogi.GetPlayers()[4].GetOwnedItems().AddItem(ROBOTS);  // 8 vp
  ogi.GetPlayers()[3].GetOwnedItems().AddItem(ORBITAL_LAB);
  ogi.GetPlayers()[3].GetOwnedItems().AddItem(ECOPLANTS);  // 8 vp, but lower cost
  ogi.GetPlayers()[2].GetOwnedItems().AddItem(SCIENTISTS);
  ogi.GetPlayers()[2].GetOwnedItems().AddItem(ROBOTS);  // 5 vp
  ogi.GetPlayers()[1].GetOwnedItems().AddItem(SCIENTISTS);
  ogi.GetPlayers()[1].GetOwnedItems().AddItem(NODULE);  // 4 vp
  ogi.GetPlayers()[0].GetOwnedItems().AddItem(SCIENTISTS);
  ogi.GetPlayers()[0].GetOwnedItems().AddItem(WAREHOUSE);  // 3 vp

  ogi.GetPlayers().DetermineTurnOrder();

  ogi.GetPlayers().StartPurchasePhase();
  ogi.GetPlayers()[2].SetPurchaseState(INTERNAL_DONE);
  ogi.GetPlayers()[1].SetPurchaseState(INTERNALIZING);
  ogi.GetPlayers().PurchaseIncrementTurnOrder();
  // at this point, 5 is done, 4 is active, 3 and 0 are unplayed, 2 is INTERNAL_DONE, and 1 is INTERNALIZING

  const TransitionType *pBM = GetTransition(osm,"Purchase","FINALDONE","Purchase",numPurchaseTransitions);
  // always manual
  BOOST_CHECK_EQUAL(pBM->IsAuto("Purchase",ogi) , false );

  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetPurchaseState() , ACTIVE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetPurchaseState() , UNPLAYED );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetPurchaseState() , INTERNAL_DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetPurchaseState() , INTERNALIZING );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetPurchaseState() , UNPLAYED );

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , true); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , true); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Purchase",ogi) , false); 

  ogi.GetPlayers()[4].SetBidState(CURBID);
  ogi.GetPlayers()[1].SetBidState(CURBID);
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , false); 

  ogi.GetPlayers()[4].SetBidState(PASS);
  ogi.GetPlayers()[1].SetBidState(HASBID);
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , true); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , true); 

  ogi.GetPlayers()[4].SetHighBid(3);
  ogi.GetPlayers()[1].SetHighBid(6);
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , false); 

  ogi.GetPlayers()[4].SetHighBid(NOT_HIGH_BID);
  ogi.GetPlayers()[1].SetHighBid(NOT_HIGH_BID);

  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player4",ap) , true); // should be ok...
  BOOST_CHECK_EQUAL( top.GetOutput(),
                     "broadcast: PLAYERSTATE,4,8,150,8,10,-1,10,0,0,5,3,0,-1,-1\n"
                     "broadcast: PLAYERSTATE,3,8,100,5,5,0,5,0,0,1,3,0,-1,-1\n"
                     );
  top.ResetOutput();

  
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetPurchaseState() , FINAL_DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetPurchaseState() , ACTIVE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetPurchaseState() , INTERNAL_DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetPurchaseState() , INTERNALIZING );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetPurchaseState() , UNPLAYED );

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , true); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , true); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Purchase",ogi) , false); 

  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player1",ap) , true); // should be ok...
  BOOST_CHECK_EQUAL( top.GetOutput(),
                     "broadcast: PLAYERSTATE,1,4,65,5,8,0,10,0,0,6,3,0,-1,-1\n"
                     );
  top.ResetOutput();

  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetPurchaseState() , FINAL_DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetPurchaseState() , ACTIVE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetPurchaseState() , INTERNAL_DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetPurchaseState() , INTERNAL_FINAL_DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetPurchaseState() , UNPLAYED );

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , true); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Purchase",ogi) , false); 

  ogi.GetPlayers().PurchaseIncrementTurnOrder();

  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetPurchaseState() , FINAL_DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetPurchaseState() , INTERNAL_DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetPurchaseState() , INTERNAL_FINAL_DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetPurchaseState() , ACTIVE );

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Purchase",ogi) , true); 

  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player0",ap) , true); // should be ok...
  BOOST_CHECK_EQUAL( top.GetOutput(),
                     "broadcast: PLAYERSTATE,0,3,65,8,5,0,10,0,0,5,3,0,-1,-1\n"
                     );
  top.ResetOutput();

  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetPurchaseState() , FINAL_DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetPurchaseState() , INTERNAL_DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetPurchaseState() , INTERNAL_FINAL_DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetPurchaseState() , FINAL_DONE );

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Purchase",ogi) , false); 
}




BOOST_AUTO_TEST_CASE( TransitionTestPURCHASEDONE )
{
  OutpostSet oset("");
  ServerGameInfo sgi("Outpost","testOutpost","XMLLOC","XMLFILE");
  ActionParser ap("DONE");

  std::vector<std::string> vcs;
  vcs.push_back("Player1");
  vcs.push_back("Player2");
  vcs.push_back("Player3");

  TestOutputPort top(vcs);

  OutpostStateMachine osm;
  OutpostGameInfo ogi(oset,sgi,top);

  ogi.GetPlayers().add("Player0");
  ogi.GetPlayers().add("Player1");
  ogi.GetPlayers().add("Player2");
  ogi.GetPlayers().add("Player3");
  ogi.GetPlayers().add("Player4");
  ogi.GetPlayers().add("Player5");

  ogi.GetPlayers()[5].GetOwnedItems().AddItem(OUTPOST);
  ogi.GetPlayers()[5].GetOwnedItems().AddItem(OUTPOST); // 10 vp
  ogi.GetPlayers()[4].GetOwnedItems().AddItem(OUTPOST);
  ogi.GetPlayers()[4].GetOwnedItems().AddItem(ROBOTS);  // 8 vp
  ogi.GetPlayers()[3].GetOwnedItems().AddItem(ORBITAL_LAB);
  ogi.GetPlayers()[3].GetOwnedItems().AddItem(ECOPLANTS);  // 8 vp, but lower cost
  ogi.GetPlayers()[2].GetOwnedItems().AddItem(SCIENTISTS);
  ogi.GetPlayers()[2].GetOwnedItems().AddItem(ROBOTS);  // 5 vp
  ogi.GetPlayers()[1].GetOwnedItems().AddItem(SCIENTISTS);
  ogi.GetPlayers()[1].GetOwnedItems().AddItem(NODULE);  // 4 vp
  ogi.GetPlayers()[0].GetOwnedItems().AddItem(SCIENTISTS);
  ogi.GetPlayers()[0].GetOwnedItems().AddItem(WAREHOUSE);  // 3 vp

  ogi.GetPlayers().DetermineTurnOrder();

  ogi.GetPlayers().StartPurchasePhase();
  ogi.GetPlayers()[2].SetPurchaseState(INTERNAL_DONE);
  ogi.GetPlayers()[1].SetPurchaseState(INTERNALIZING);
  ogi.GetPlayers().PurchaseIncrementTurnOrder();
  // at this point, 5 is done, 4 is active, 3 and 0 are unplayed, 2 is INTERNAL_DONE, and 1 is INTERNALIZING

  const TransitionType *pBM = GetTransition(osm,"Purchase","PURCHASEDONE","StartFinalManning",numPurchaseTransitions);

  // not legal.
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Purchase",ogi) , false); 

  // auto only if all players are done or internal done.
  BOOST_CHECK_EQUAL(pBM->IsAuto("Purchase",ogi) , false );

  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetPurchaseState() , ACTIVE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetPurchaseState() , UNPLAYED );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetPurchaseState() , INTERNAL_DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetPurchaseState() , INTERNALIZING );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetPurchaseState() , UNPLAYED );
  
  ogi.GetPlayers().PurchaseIncrementTurnOrder();
  
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetPurchaseState() , ACTIVE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetPurchaseState() , INTERNAL_DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetPurchaseState() , INTERNALIZING );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetPurchaseState() , UNPLAYED );

  // auto only if all players are done or internal done.
  BOOST_CHECK_EQUAL(pBM->IsAuto("Purchase",ogi) , false );

  ogi.GetPlayers().PurchaseIncrementTurnOrder();
  
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetPurchaseState() , INTERNAL_DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetPurchaseState() , INTERNALIZING );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetPurchaseState() , ACTIVE );

  // auto only if all players are done or internal done.
  BOOST_CHECK_EQUAL(pBM->IsAuto("Purchase",ogi) , false );

  ogi.GetPlayers().PurchaseIncrementTurnOrder();
  
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetPurchaseState() , INTERNAL_DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetPurchaseState() , INTERNALIZING );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetPurchaseState() , DONE );

  // auto only if all players are done or internal done.
  BOOST_CHECK_EQUAL(pBM->IsAuto("Purchase",ogi) , false );

  ogi.GetPlayers()[1].SetPurchaseState(INTERNAL_DONE);

  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetPurchaseState() , INTERNAL_DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetPurchaseState() , INTERNAL_DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetPurchaseState() , DONE );

  // auto only if all players are done or internal done.
  BOOST_CHECK_EQUAL(pBM->IsAuto("Purchase",ogi) , true );

  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"",ap) , true); // should be ok...
  BOOST_CHECK_EQUAL( top.GetOutput(),"");
  top.ResetOutput();
}



BOOST_AUTO_TEST_CASE( TransitionTestALTERMANNING )
{
  OutpostSet oset("");
  ServerGameInfo sgi("Outpost","testOutpost","XMLLOC","XMLFILE");
  ActionParser ap("DONE");

  std::vector<std::string> vcs;
  vcs.push_back("Player1");
  vcs.push_back("Player2");
  vcs.push_back("Player3");

  TestOutputPort top(vcs);

  OutpostStateMachine osm;
  OutpostGameInfo ogi(oset,sgi,top);

  ogi.GetPlayers().add("Player0");
  ogi.GetPlayers().add("Player1");
  ogi.GetPlayers().add("Player2");
  ogi.GetPlayers().add("Player3");
  ogi.GetPlayers().add("Player4");
  ogi.GetPlayers().add("Player5");

  ogi.GetPlayers()[5].GetOwnedItems().AddItem(OUTPOST);
  ogi.GetPlayers()[5].GetOwnedItems().AddItem(OUTPOST); // 10 vp
  ogi.GetPlayers()[4].GetOwnedItems().AddItem(OUTPOST);
  ogi.GetPlayers()[4].GetOwnedItems().AddItem(ROBOTS);  // 8 vp
  ogi.GetPlayers()[3].GetOwnedItems().AddItem(ORBITAL_LAB);
  ogi.GetPlayers()[3].GetOwnedItems().AddItem(ECOPLANTS);  // 8 vp, but lower cost
  ogi.GetPlayers()[2].GetOwnedItems().AddItem(SCIENTISTS);
  ogi.GetPlayers()[2].GetOwnedItems().AddItem(ROBOTS);  // 5 vp
  ogi.GetPlayers()[1].GetOwnedItems().AddItem(SCIENTISTS);
  ogi.GetPlayers()[1].GetOwnedItems().AddItem(NODULE);  // 4 vp
  ogi.GetPlayers()[0].GetOwnedItems().AddItem(SCIENTISTS);
  ogi.GetPlayers()[0].GetOwnedItems().AddItem(WAREHOUSE);  // 3 vp

  ogi.GetPlayers().DetermineTurnOrder();

  ogi.GetPlayers().StartPurchasePhase();
  ogi.GetPlayers()[2].SetPurchaseState(INTERNAL_DONE);
  ogi.GetPlayers()[1].SetPurchaseState(INTERNALIZING);
  ogi.GetPlayers().PurchaseIncrementTurnOrder();
  // at this point, 5 is done, 4 is active, 3 and 0 are unplayed, 2 is INTERNAL_DONE, and 1 is INTERNALIZING

  const TransitionType *pBMP = GetTransition(osm,"Purchase","ALTERMANNING","Purchase",numPurchaseTransitions);
  const TransitionType *pBMB = GetTransition(osm,"Bid","ALTERMANNING","Bid",numBidTransitions);
  const TransitionType *pBMM = GetTransition(osm,"FinalManning","ALTERMANNING","FinalManning",4);

  // never auto
  BOOST_CHECK_EQUAL(pBMP->IsAuto("Purchase",ogi) , false );
  BOOST_CHECK_EQUAL(pBMB->IsAuto("Bid",ogi) , false );
  BOOST_CHECK_EQUAL(pBMM->IsAuto("FinalManning",ogi) , false );

  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetPurchaseState() , ACTIVE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetPurchaseState() , UNPLAYED );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetPurchaseState() , INTERNAL_DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetPurchaseState() , INTERNALIZING );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetPurchaseState() , UNPLAYED );

  // in  Purchase and Bid phases, any actual player can alter their manning.
  // in final manning, only the active player can.
  BOOST_CHECK_EQUAL(pBMP->IsLegal("Player9","Purchase",ogi) , false); 
  BOOST_CHECK_EQUAL(pBMP->IsLegal("Player5","Purchase",ogi) , true); 
  BOOST_CHECK_EQUAL(pBMP->IsLegal("Player4","Purchase",ogi) , true); 
  BOOST_CHECK_EQUAL(pBMP->IsLegal("Player3","Purchase",ogi) , true); 
  BOOST_CHECK_EQUAL(pBMP->IsLegal("Player2","Purchase",ogi) , true); 
  BOOST_CHECK_EQUAL(pBMP->IsLegal("Player1","Purchase",ogi) , true); 
  BOOST_CHECK_EQUAL(pBMP->IsLegal("Player0","Purchase",ogi) , true); 

  BOOST_CHECK_EQUAL(pBMB->IsLegal("Player9","Bid",ogi) , false); 
  BOOST_CHECK_EQUAL(pBMB->IsLegal("Player5","Bid",ogi) , true); 
  BOOST_CHECK_EQUAL(pBMB->IsLegal("Player4","Bid",ogi) , true); 
  BOOST_CHECK_EQUAL(pBMB->IsLegal("Player3","Bid",ogi) , true); 
  BOOST_CHECK_EQUAL(pBMB->IsLegal("Player2","Bid",ogi) , true); 
  BOOST_CHECK_EQUAL(pBMB->IsLegal("Player1","Bid",ogi) , true); 
  BOOST_CHECK_EQUAL(pBMB->IsLegal("Player0","Bid",ogi) , true); 

  BOOST_CHECK_EQUAL(pBMM->IsLegal("Player9","FinalManning",ogi) , false); 
  BOOST_CHECK_EQUAL(pBMM->IsLegal("Player5","FinalManning",ogi) , false); 
  BOOST_CHECK_EQUAL(pBMM->IsLegal("Player4","FinalManning",ogi) , true); 
  BOOST_CHECK_EQUAL(pBMM->IsLegal("Player3","FinalManning",ogi) , false); 
  BOOST_CHECK_EQUAL(pBMM->IsLegal("Player2","FinalManning",ogi) , false); 
  BOOST_CHECK_EQUAL(pBMM->IsLegal("Player1","FinalManning",ogi) , false); 
  BOOST_CHECK_EQUAL(pBMM->IsLegal("Player0","FinalManning",ogi) , false); 

  ogi.GetPlayers()[4].GetFactories().AddFactory(MOON_BASE_FACTORY);
  ogi.GetPlayers()[4].GetFactories().AddFactory(WATER_FACTORY);
  ogi.GetPlayers()[4].GetFactories().AddFactory(ORE_FACTORY);
  ogi.GetPlayers()[4].AddHumans(1);
  ogi.GetPlayers()[4].AddRobots(1);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetFactories().AlterManning("HUB",1,1) , 0);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetFactories().GetFactoryDescription() , "|OH-WU-MB|");

  ActionParser ap1("ALTERMANNING,BUHU");
  BOOST_CHECK_EQUAL(pBMM->ExecuteAction("FinalManning",ogi,"Player4",ap1) , false); // manning string too long.
  BOOST_CHECK_EQUAL( top.GetOutput(), "unicast: Player4 -- ERROR,Invalid Length of Manning List\n" );
  top.ResetOutput();

  ActionParser ap2("ALTERMANNING,BU");
  BOOST_CHECK_EQUAL(pBMM->ExecuteAction("FinalManning",ogi,"Player4",ap2) , false); // manning string too short
  BOOST_CHECK_EQUAL( top.GetOutput(), "unicast: Player4 -- ERROR,Invalid Length of Manning List\n" );
  top.ResetOutput();

  ActionParser ap3("ALTERMANNING,BBU");
  BOOST_CHECK_EQUAL(pBMM->ExecuteAction("FinalManning",ogi,"Player4",ap3) , false); // too many robots
  BOOST_CHECK_EQUAL( top.GetOutput(), "unicast: Player4 -- ERROR,Too Many Robots or People\n" );
  top.ResetOutput();

  ActionParser ap4("ALTERMANNING,HHU");
  BOOST_CHECK_EQUAL(pBMM->ExecuteAction("FinalManning",ogi,"Player4",ap4) , false); // too many humans
  BOOST_CHECK_EQUAL( top.GetOutput(), "unicast: Player4 -- ERROR,Too Many Robots or People\n" );
  top.ResetOutput();

  ActionParser ap5("ALTERMANNING,HVU");
  BOOST_CHECK_EQUAL(pBMM->ExecuteAction("FinalManning",ogi,"Player4",ap5) , false); // too many humans
  BOOST_CHECK_EQUAL( top.GetOutput(), "unicast: Player4 -- ERROR,Invalid Character in Manning List\n" );
  top.ResetOutput();

  ActionParser ap6("ALTERMANNING,UBH");
  BOOST_CHECK_EQUAL(pBMM->ExecuteAction("FinalManning",ogi,"Player4",ap6) , true); // should work
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetFactories().GetFactoryDescription() , "|OU-WB-MH|");
  BOOST_CHECK_EQUAL( top.GetOutput(), 
                     "broadcast: PLAYERFACTORIES,4,|OU-WB-MH|\n"
                     "broadcast: PLAYERSTATE,4,29,150,8,11,-1,10,1,1,1,3,0,-1,-1\n"
                     "unicast: Player4 -- PLAYERPURCHASEINFO,0,0,0,0,0,0,0\n"
                     );
  top.ResetOutput();

  ActionParser ap7("ALTERMANNING,BUH");

  // just futzing with the robot numbers
  ogi.GetOptions().SetRobotMechanism(SECOND_UNLIMITED);

  BOOST_CHECK_EQUAL(pBMM->ExecuteAction("FinalManning",ogi,"Player4",ap7) , true); // should work
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetFactories().GetFactoryDescription() , "|OB-WU-MH|");
  BOOST_CHECK_EQUAL( top.GetOutput(), 
                     "broadcast: PLAYERFACTORIES,4,|OB-WU-MH|\n" 
                     "broadcast: PLAYERSTATE,4,29,150,8,11,1,10,1,1,1,3,0,-1,-1\n"
                     "unicast: Player4 -- PLAYERPURCHASEINFO,0,0,0,0,0,0,0\n"
                     );
  top.ResetOutput();

  ActionParser ap8("ALTERMANNING,BHU");
  BOOST_CHECK_EQUAL(pBMM->ExecuteAction("FinalManning",ogi,"Player4",ap8) , true); // unmanning mannable item
  BOOST_CHECK_EQUAL( top.GetOutput(), 
                     "broadcast: PLAYERFACTORIES,4,|OB-WH-MU|\n" 
                     "broadcast: PLAYERSTATE,4,10,150,8,10,1,10,1,1,1,3,0,-1,-1\n"
                     "unicast: Player4 -- PLAYERPURCHASEINFO,0,0,0,0,0,0,0\n"
                     );

  top.ResetOutput();

  ogi.GetPlayers().IncrementTurnOrder();
  ogi.GetPlayers().IncrementTurnOrder();
  ogi.GetPlayers().IncrementTurnOrder();
  ogi.GetPlayers().IncrementTurnOrder();
  ogi.GetPlayers().IncrementTurnOrder();

  // now, no player should be active. these should all return false, but still execute correctly
  BOOST_CHECK_EQUAL(pBMM->IsLegal("Player9","FinalManning",ogi) , false); 
  BOOST_CHECK_EQUAL(pBMM->IsLegal("Player5","FinalManning",ogi) , false); 
  BOOST_CHECK_EQUAL(pBMM->IsLegal("Player4","FinalManning",ogi) , false); 
  BOOST_CHECK_EQUAL(pBMM->IsLegal("Player3","FinalManning",ogi) , false); 
  BOOST_CHECK_EQUAL(pBMM->IsLegal("Player2","FinalManning",ogi) , false); 
  BOOST_CHECK_EQUAL(pBMM->IsLegal("Player1","FinalManning",ogi) , false); 
  BOOST_CHECK_EQUAL(pBMM->IsLegal("Player0","FinalManning",ogi) , false); 

}

  
  
