#include "OutpostGameInfo.hpp"
#include <boost/test/auto_unit_test.hpp>
#include <iostream>

#include "ActionParser.hpp"
#include "ServerGameInfo.hpp"
#include "TestOutputPort.hpp"
#include "TransitionTestUtilities.hpp"
#include <sstream>
#include <stdexcept>

#include <boost/algorithm/string/replace.hpp>

const int numInitialStateTransitions = 6;


BOOST_AUTO_TEST_CASE( TransitionTestJOIN )
{
  OutpostSet oset("");
  ServerGameInfo sgi("Outpost","testOutpost","XMLLOC","XMLFILE");
  TestOutputPort top;

  OutpostStateMachine osm;
  const TransitionType *pJoin = GetTransition(osm,"InitialState","JOIN","InitialState",numInitialStateTransitions);

  OutpostGameInfo ogi(oset,sgi,top);
  BOOST_CHECK_EQUAL (ogi.GetPlayers().size() , (size_t)0);
  ActionParser ap("JOIN");

  BOOST_CHECK_EQUAL(pJoin->IsAuto("InitialState",ogi) , false );
  BOOST_CHECK_EQUAL(pJoin->IsLegal("Player1","InitialState",ogi) , true);
  BOOST_CHECK_EQUAL(pJoin->ExecuteAction("InitialState",ogi,"Player1",ap) , true);
  BOOST_CHECK_EQUAL (ogi.GetPlayers().size() , (size_t)1);
  BOOST_CHECK_EQUAL (ogi.GetPlayers()[0].GetName() ,"Player1");
  BOOST_CHECK_EQUAL (top.GetOutput() , 
                     "broadcast: PLAYER,Player1,0\n" 
                     "unicast: Player1 -- IAM,0\n"
                     );

  top.ResetOutput();

  // can't join if you've already joined.
  BOOST_CHECK_EQUAL(pJoin->IsAuto("InitialState",ogi) , false );
  BOOST_CHECK_EQUAL(pJoin->IsLegal("Player1","InitialState",ogi) , false);

  ogi.GetPlayers().add("Player2");
  ogi.GetPlayers().add("Player3");
  ogi.GetPlayers().add("Player4");
  ogi.GetPlayers().add("Player5");
  ogi.GetPlayers().add("Player6");
  ogi.GetPlayers().add("Player7");
  ogi.GetPlayers().add("Player10");
  ogi.GetPlayers().add("Player11");

  // can join if there are less than 10 players.
  BOOST_CHECK_EQUAL(pJoin->IsAuto("InitialState",ogi) , false );
  BOOST_CHECK_EQUAL(pJoin->IsLegal("Player8","InitialState",ogi) , true);
  BOOST_CHECK_EQUAL(pJoin->ExecuteAction("InitialState",ogi,"Player8",ap) , true);
  BOOST_CHECK_EQUAL (top.GetOutput() , 
                     "broadcast: PLAYER,Player8,9\n" 
                     "unicast: Player8 -- IAM,9\n"
                     );

  // can't join if there are 10 or more.
  BOOST_CHECK_EQUAL (ogi.GetPlayers().size() , (size_t)10);
  BOOST_CHECK_EQUAL(pJoin->IsAuto("InitialState",ogi) , false );
  BOOST_CHECK_EQUAL(pJoin->IsLegal("Player9","InitialState",ogi) , false);
}

BOOST_AUTO_TEST_CASE( TransitionTestUNJOIN )
{
  OutpostSet oset("");
  ServerGameInfo sgi("Outpost","testOutpost","XMLLOC","XMLFILE");
  TestOutputPort top;
  ActionParser ap("UNJOIN");

  OutpostStateMachine osm;
  const TransitionType *pUnJoin = GetTransition(osm,"InitialState","UNJOIN","InitialState",numInitialStateTransitions);
  OutpostGameInfo ogi(oset,sgi,top);

  ogi.GetPlayers().add("PlayerA");
  ogi.GetPlayers().add("PlayerB");
  ogi.GetPlayers().add("PlayerC");

  BOOST_CHECK_EQUAL(ogi.GetPlayers()[0].GetName() , "PlayerA");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[1].GetName() , "PlayerB");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetName() , "PlayerC");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[0].GetId() , 0);
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[1].GetId() , 1);
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[2].GetId() , 2);


  // never auto
  BOOST_CHECK_EQUAL(pUnJoin->IsAuto("InitialState",ogi) , false );

  // only joined players can unjoin
  BOOST_CHECK_EQUAL(pUnJoin->IsLegal("Player8","InitialState",ogi) , false);
  BOOST_CHECK_EQUAL(pUnJoin->IsLegal("PlayerA","InitialState",ogi) , true);
  BOOST_CHECK_EQUAL(pUnJoin->IsLegal("PlayerB","InitialState",ogi) , true);
  BOOST_CHECK_EQUAL(pUnJoin->IsLegal("PlayerC","InitialState",ogi) , true);

  BOOST_CHECK_EQUAL(pUnJoin->ExecuteAction("InitialState",ogi,"PlayerB",ap) , true);
  BOOST_CHECK_EQUAL (top.GetOutput() , 
                     "broadcast: RESET,Outpost\n"
                     "broadcast: PLAYER,PlayerA,0\n"
                     "broadcast: PLAYER,PlayerC,1\n"
                     "unicast: PlayerA -- IAM,0\n"
                     "unicast: PlayerC -- IAM,1\n"
                     );

  top.ResetOutput();

  BOOST_CHECK_EQUAL(ogi.GetPlayers().size() , (size_t)2);

  BOOST_CHECK_EQUAL(ogi.GetPlayers()[0].GetName() , "PlayerA");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[1].GetName() , "PlayerC");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[0].GetId() , 0);
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[1].GetId() , 1);

  BOOST_CHECK_EQUAL(pUnJoin->ExecuteAction("InitialState",ogi,"PlayerC",ap) , true);
  
  BOOST_CHECK_EQUAL (top.GetOutput() , 
                     "broadcast: RESET,Outpost\n"
                     "broadcast: PLAYER,PlayerA,0\n"
                     "unicast: PlayerA -- IAM,0\n"
                     );

  top.ResetOutput();

  BOOST_CHECK_EQUAL(ogi.GetPlayers().size() , (size_t)1);

  BOOST_CHECK_EQUAL(ogi.GetPlayers()[0].GetName() , "PlayerA");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[0].GetId() , 0);


  ogi.GetPlayers().add("PlayerD");
  ogi.GetPlayers().add("PlayerE");

  BOOST_CHECK_EQUAL(pUnJoin->ExecuteAction("InitialState",ogi,"PlayerA",ap) , true);

  BOOST_CHECK_EQUAL (top.GetOutput() , 
                     "broadcast: RESET,Outpost\n"
                     "broadcast: PLAYER,PlayerD,0\n"
                     "broadcast: PLAYER,PlayerE,1\n"
                     "unicast: PlayerD -- IAM,0\n"
                     "unicast: PlayerE -- IAM,1\n"
                     );

  top.ResetOutput();

  BOOST_CHECK_EQUAL(ogi.GetPlayers().size() , (size_t)2);

  BOOST_CHECK_EQUAL(ogi.GetPlayers()[0].GetName() , "PlayerD");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[1].GetName() , "PlayerE");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[0].GetId() , 0);
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[1].GetId() , 1);


}



// tests OPTIONS,STANDARDBASIC, and STANDARDEXPERT
BOOST_AUTO_TEST_CASE( TransitionTestOPTIONS )
{
  OutpostSet oset("");
  ServerGameInfo sgi("Outpost","testOutpost","XMLLOC","XMLFILE");
  TestOutputPort top;

  OutpostStateMachine osm;
  const TransitionType *pOptions = GetTransition(osm,"InitialState","OPTIONS","InitialState",numInitialStateTransitions);
  OutpostGameInfo ogi(oset,sgi,top);
  ActionParser ap("OPTIONS,0,0,0,1,3,2,0,1,0,1,2");

  ogi.GetPlayers().add("Player1");

  // only players in the game can change options.
  BOOST_CHECK_EQUAL(pOptions->IsAuto("InitialState",ogi) , false );
  BOOST_CHECK_EQUAL(pOptions->IsLegal("Player2","InitialState",ogi) , false);

  // this should work.
  BOOST_CHECK_EQUAL(pOptions->IsAuto("InitialState",ogi) , false );
  BOOST_CHECK_EQUAL(pOptions->IsLegal("Player1","InitialState",ogi) , true);
  BOOST_CHECK_EQUAL(pOptions->ExecuteAction("InitialState",ogi,"Player1",ap) , true);

  BOOST_CHECK_EQUAL(top.GetOutput() , "broadcast: OPTIONS,0,0,0,1,4,2,0,1,0,1,2\n");
  top.ResetOutput();

  BOOST_CHECK_EQUAL(ogi.GetOptions().GetEarlyDiscard() , false);
  BOOST_CHECK_EQUAL(ogi.GetOptions().GetRefineries() , false);
  BOOST_CHECK_EQUAL(ogi.GetOptions().GetOneUpgradePerTurn() , false);
  BOOST_CHECK_EQUAL(ogi.GetOptions().GetMustBeActiveToBid() , true);
  BOOST_CHECK_EQUAL(ogi.GetOptions().GetSupplyHarshness() , 3);
  BOOST_CHECK_EQUAL(ogi.GetOptions().GetRobotMechanism() , SECOND_UNLIMITED);
  BOOST_CHECK_EQUAL(ogi.GetOptions().GetResearchMega() , false);
  BOOST_CHECK_EQUAL(ogi.GetOptions().GetResearchIsSmall() , true);
  BOOST_CHECK_EQUAL(ogi.GetOptions().GetMicroIsSmall() , false);
  BOOST_CHECK_EQUAL(ogi.GetOptions().GetBlindDraws() , true);
  BOOST_CHECK_EQUAL(ogi.GetOptions().GetStockMechanism() , SOME_ONE_EXTRA);

  ActionParser ap2("STANDARDBASIC");
  const TransitionType *pSB = GetTransition(osm,"InitialState","STANDARDBASIC","InitialState",numInitialStateTransitions);

  // only players in the game can change options.
  BOOST_CHECK_EQUAL(pSB->IsAuto("InitialState",ogi) , false );
  BOOST_CHECK_EQUAL(pSB->IsLegal("Player2","InitialState",ogi) , false);

  // this should work.
  BOOST_CHECK_EQUAL(pSB->IsAuto("InitialState",ogi) , false );
  BOOST_CHECK_EQUAL(pSB->IsLegal("Player1","InitialState",ogi) , true);
  BOOST_CHECK_EQUAL(pSB->ExecuteAction("InitialState",ogi,"Player1",ap2) , true);

  BOOST_CHECK_EQUAL(top.GetOutput() , "broadcast: OPTIONS,0,0,0,0,1,1,0,0,0,0,1\n");
  top.ResetOutput();

  BOOST_CHECK_EQUAL(ogi.GetOptions().GetEarlyDiscard() , false);
  BOOST_CHECK_EQUAL(ogi.GetOptions().GetRefineries() , false);
  BOOST_CHECK_EQUAL(ogi.GetOptions().GetOneUpgradePerTurn() , false);
  BOOST_CHECK_EQUAL(ogi.GetOptions().GetMustBeActiveToBid() , false);
  BOOST_CHECK_EQUAL(ogi.GetOptions().GetSupplyHarshness() , 0);
  BOOST_CHECK_EQUAL(ogi.GetOptions().GetRobotMechanism() , FIRST_UNLIMITED);
  BOOST_CHECK_EQUAL(ogi.GetOptions().GetResearchMega() , false);
  BOOST_CHECK_EQUAL(ogi.GetOptions().GetResearchIsSmall() , false);
  BOOST_CHECK_EQUAL(ogi.GetOptions().GetMicroIsSmall() , false);

  ActionParser ap3("STANDARDEXPERT");
  const TransitionType *pSE = GetTransition(osm,"InitialState","STANDARDEXPERT","InitialState",numInitialStateTransitions);

  // only players in the game can change options.
  BOOST_CHECK_EQUAL(pSE->IsAuto("InitialState",ogi) , false );
  BOOST_CHECK_EQUAL(pSE->IsLegal("Player2","InitialState",ogi) , false);

  // this should work.
  BOOST_CHECK_EQUAL(pSE->IsAuto("InitialState",ogi) , false );
  BOOST_CHECK_EQUAL(pSE->IsLegal("Player1","InitialState",ogi) , true);
  BOOST_CHECK_EQUAL(pSE->ExecuteAction("InitialState",ogi,"Player1",ap3) , true);

  BOOST_CHECK_EQUAL(top.GetOutput() , "broadcast: OPTIONS,1,0,0,0,1,1,0,1,1,0,1\n");
  top.ResetOutput();

  BOOST_CHECK_EQUAL(ogi.GetOptions().GetEarlyDiscard() , true);
  BOOST_CHECK_EQUAL(ogi.GetOptions().GetRefineries() , false);
  BOOST_CHECK_EQUAL(ogi.GetOptions().GetOneUpgradePerTurn() , false);
  BOOST_CHECK_EQUAL(ogi.GetOptions().GetMustBeActiveToBid() , false);
  BOOST_CHECK_EQUAL(ogi.GetOptions().GetSupplyHarshness() , false);
  BOOST_CHECK_EQUAL(ogi.GetOptions().GetRobotMechanism() , FIRST_UNLIMITED);
  BOOST_CHECK_EQUAL(ogi.GetOptions().GetResearchMega() , false);
  BOOST_CHECK_EQUAL(ogi.GetOptions().GetResearchIsSmall() , true);
  BOOST_CHECK_EQUAL(ogi.GetOptions().GetMicroIsSmall() , true);


}

// this non-auto transition can only be done by players, when
// there are at least two players, and sets all
// the players okstartup to false.


BOOST_AUTO_TEST_CASE( TransitionTestVALIDATESETUP )
{
  OutpostSet oset("");
  ServerGameInfo sgi("Outpost","testOutpost","XMLLOC","XMLFILE");
  TestOutputPort top;

  OutpostStateMachine osm;
  const TransitionType *pVS = GetTransition(osm,"InitialState","VALIDATESETUP","ValidateSetup",numInitialStateTransitions);
  OutpostGameInfo ogi(oset,sgi,top);
  ActionParser ap("VALIDATESETUP");

  ogi.GetPlayers().add("Player1");

  // only players in the game can do it
  BOOST_CHECK_EQUAL(pVS->IsAuto("InitialState",ogi) , false );
  BOOST_CHECK_EQUAL(pVS->IsLegal("Player2","InitialState",ogi) , false);

  // there have to be at least two players.
  BOOST_CHECK_EQUAL(pVS->IsLegal("Player1","InitialState",ogi) , false);

  ogi.GetPlayers().add("Player2");
  // only players in the game can do it
  BOOST_CHECK_EQUAL(pVS->IsAuto("InitialState",ogi) , false );
  BOOST_CHECK_EQUAL(pVS->IsLegal("Player3","InitialState",ogi) , false);

  ogi.GetPlayers()[0].SetOkStartup(true);
  ogi.GetPlayers()[1].SetOkStartup(true);
  
  // this should work.
  BOOST_CHECK_EQUAL(pVS->IsAuto("InitialState",ogi) , false );
  BOOST_CHECK_EQUAL(pVS->IsLegal("Player1","InitialState",ogi) , true);
  BOOST_CHECK_EQUAL(pVS->ExecuteAction("InitialState",ogi,"Player1",ap) , true);
  BOOST_CHECK_EQUAL(top.GetOutput() , 
                    "broadcast: PLAYEROKSTARTUP,0,0\n"
                    "broadcast: PLAYEROKSTARTUP,1,0\n");
  
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[0].OkStartup() , false);
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[1].OkStartup() , false);
}

// handles tests for two transitions:
// DONTLIKEOPTIONS -- next state is 
// LIKEOPTIONS -- sets player's ok bit
// both can only be done by players who have not already set their ok bit.


BOOST_AUTO_TEST_CASE( TransitionTestLIKEORNOT )
{
  OutpostSet oset("");
  ServerGameInfo sgi("Outpost","testOutpost","XMLLOC","XMLFILE");
  TestOutputPort top;

  OutpostStateMachine osm;
  const TransitionType *pLO = GetTransition(osm,"ValidateSetup","LIKEOPTIONS","ValidateSetup",3);
  const TransitionType *pDL = GetTransition(osm,"ValidateSetup","DONTLIKEOPTIONS","InitialState",3);
  OutpostGameInfo ogi(oset,sgi,top);
  ActionParser ap("LIKEOPTIONS");
  ActionParser ap2("DONTLIKEOPTIONS");

  ogi.GetPlayers().add("Player1");
  ogi.GetPlayers().add("Player3");

  ogi.GetPlayers()[1].SetOkStartup(false);

  // only players in the game can do it
  BOOST_CHECK_EQUAL(pLO->IsAuto("ValidateSetup",ogi) , false );
  BOOST_CHECK_EQUAL(pLO->IsLegal("Player2","ValidateSetup",ogi) , false);
  BOOST_CHECK_EQUAL(pDL->IsAuto("ValidateSetup",ogi) , false );
  BOOST_CHECK_EQUAL(pDL->IsLegal("Player2","ValidateSetup",ogi) , false);

  ogi.GetPlayers()[0].SetOkStartup(true);

  // only players with okstartup not set can do it.
  BOOST_CHECK_EQUAL(pLO->IsAuto("ValidateSetup",ogi) , false );
  BOOST_CHECK_EQUAL(pLO->IsLegal("Player1","ValidateSetup",ogi) , false);
  BOOST_CHECK_EQUAL(pDL->IsAuto("ValidateSetup",ogi) , false );
  BOOST_CHECK_EQUAL(pDL->IsLegal("Player1","ValidateSetup",ogi) , false);

  ogi.GetPlayers()[0].SetOkStartup(false);

  // this should work.
  BOOST_CHECK_EQUAL(pLO->IsAuto("ValidateSetup",ogi) , false );
  BOOST_CHECK_EQUAL(pLO->IsLegal("Player1","ValidateSetup",ogi) , true);
  BOOST_CHECK_EQUAL(pLO->ExecuteAction("ValidateSetup",ogi,"Player1",ap) , true);
  BOOST_CHECK_EQUAL(top.GetOutput() , "broadcast: PLAYEROKSTARTUP,0,1\n");
  top.ResetOutput();
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[0].OkStartup() , true);
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[1].OkStartup() , false);
  
  ogi.GetPlayers()[0].SetOkStartup(false);
  BOOST_CHECK_EQUAL(pDL->IsAuto("ValidateSetup",ogi) , false );
  BOOST_CHECK_EQUAL(pDL->IsLegal("Player1","ValidateSetup",ogi) , true);
  BOOST_CHECK_EQUAL(pDL->ExecuteAction("ValidateSetup",ogi,"Player1",ap) , true);
  BOOST_CHECK_EQUAL(top.GetOutput() , "");
}

BOOST_AUTO_TEST_CASE( TransitionTestSTARTGAME )
{
  OutpostSet oset("");
  ServerGameInfo sgi("Outpost","testOutpost","XMLLOC","XMLFILE");
  TestOutputPort top;

  OutpostStateMachine osm;
  const TransitionType *pSG = GetTransition(osm,"ValidateSetup","STARTGAME","TurnOrder",3);
  OutpostGameInfo ogi(oset,sgi,top);
  ActionParser ap("");

  // STARTGAME should never be a legal action.
  BOOST_CHECK_EQUAL(pSG->IsLegal("Player1","ValidateSetup",ogi) , false);
  
  ogi.GetPlayers().add("Player1");
  ogi.GetPlayers().add("Player3");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[0].OkStartup() , false);
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[1].OkStartup() , false);
  // it can, however, be an auto, but only if all players are OkStartup
  BOOST_CHECK_EQUAL(pSG->IsLegal("Player1","ValidateSetup",ogi) , false);
  BOOST_CHECK_EQUAL(pSG->IsAuto("ValidateSetup",ogi) , false );

  ogi.GetPlayers()[0].SetOkStartup(true);
  BOOST_CHECK_EQUAL(pSG->IsLegal("Player1","ValidateSetup",ogi) , false);
  BOOST_CHECK_EQUAL(pSG->IsAuto("ValidateSetup",ogi) , false );

  ogi.GetPlayers()[1].SetOkStartup(true);
  BOOST_CHECK_EQUAL(pSG->IsLegal("Player1","ValidateSetup",ogi) , false);
  BOOST_CHECK_EQUAL(pSG->IsAuto("ValidateSetup",ogi) , true );

  BOOST_CHECK_EQUAL(ogi.GetCurTurn() , 0);
  BOOST_CHECK_EQUAL(ogi.GetCommodities().GetDeck(RESEARCH_COMMODITY).DrawCommodity(true).GetValue() , 56);
  BOOST_CHECK_EQUAL(ogi.GetUnownedItems().Initialized() , false);
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[0].GetFactories().GetFactoryDescription(),"||");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[0].GetNumHumans() , 0);
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[0].GetNumRobots() , 0);
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[1].GetFactories().GetFactoryDescription(),"||");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[1].GetNumHumans() , 0);
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[1].GetNumRobots() , 0);

  BOOST_CHECK_EQUAL(pSG->ExecuteAction("ValidateSetup",ogi,"",ap) , true);

  BOOST_CHECK_EQUAL(top.GetOutput() , 
                    "broadcast: CURTURN,1\n"
                    "broadcast: ITEMSTOCK,1,1\n"
                    "broadcast: ITEMSTOCK,2,1\n"
                    "broadcast: ITEMSTOCK,3,1\n"
                    "broadcast: ITEMSTOCK,4,1\n"
                    "broadcast: ITEMSTOCK,5,1\n"
                    "broadcast: ITEMSTOCK,6,1\n"
                    "broadcast: ITEMSTOCK,7,1\n"
                    "broadcast: ITEMSTOCK,8,1\n"
                    "broadcast: ITEMSTOCK,9,1\n"
                    "broadcast: ITEMSTOCK,10,1\n"
                    "broadcast: ITEMSTOCK,11,1\n"
                    "broadcast: ITEMSTOCK,12,1\n"
                    "broadcast: ITEMSTOCK,13,1\n"
                    "broadcast: PLAYERFACTORIES,0,|OH-OH-WH|\n"
                    "broadcast: PLAYERFACTORIES,1,|OH-OH-WH|\n"
                    "broadcast: PLAYERSTATE,0,3,0,5,5,0,10,3,0,3,3,0,-1,-1\n"
                    "broadcast: PLAYERSTATE,1,3,0,5,5,0,10,3,0,3,3,0,-1,-1\n"
                    "broadcast: COMMODITYSTOCK,1,26,0,0,3\n"
                    "broadcast: COMMODITYSTOCK,2,39,0,30,7\n"
                    "broadcast: COMMODITYSTOCK,3,53,0,44,10\n"
                    "broadcast: COMMODITYSTOCK,4,34,0,0,13\n"
                    "broadcast: COMMODITYSTOCK,5,16,0,0,17\n"
                    "broadcast: COMMODITYSTOCK,6,23,0,88,20\n"
                    "broadcast: COMMODITYSTOCK,7,14,0,0,30\n"
                    "broadcast: COMMODITYSTOCK,8,12,0,0,40\n"
                    "broadcast: COMMODITYSTOCK,9,12,0,0,50\n"
                    );

  top.ResetOutput();

  BOOST_CHECK_EQUAL(ogi.GetCurTurn() , 1);
  BOOST_CHECK_EQUAL(ogi.GetCommodities().GetDeck(RESEARCH_COMMODITY).DrawCommodity(true).GetValue() , 0);
  BOOST_CHECK_EQUAL(ogi.GetUnownedItems().Initialized() , true);
  BOOST_CHECK_EQUAL(ogi.GetUnownedItems().GetItemCount(DATA_LIBRARY) , 1);

  BOOST_CHECK_EQUAL(ogi.GetPlayers()[0].GetFactories().GetFactoryDescription(),"|OH-OH-WH|");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[0].GetNumHumans() , 3);
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[0].GetNumRobots() , 0);
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[1].GetFactories().GetFactoryDescription(),"|OH-OH-WH|");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[1].GetNumHumans() , 3);
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[1].GetNumRobots() , 0);

  OutpostGameInfo ogi2(oset,sgi,top);
  ogi2.GetOptions().SetResearchMega(true);

  ogi2.GetPlayers().add("Player1");
  ogi2.GetPlayers().add("Player2");
  ogi2.GetPlayers().add("Player3");
  ogi2.GetPlayers()[0].SetOkStartup(true);
  ogi2.GetPlayers()[1].SetOkStartup(true);
  ogi2.GetPlayers()[2].SetOkStartup(true);

  BOOST_CHECK_EQUAL(ogi2.GetCurTurn() , 0);
  BOOST_CHECK_EQUAL(ogi2.GetCommodities().GetDeck(RESEARCH_COMMODITY).DrawCommodity(true).GetValue() , 56);
  BOOST_CHECK_EQUAL(ogi2.GetUnownedItems().Initialized() , false);

  BOOST_CHECK_EQUAL(pSG->ExecuteAction("ValidateSetup",ogi2,"",ap) , true);

  BOOST_CHECK_EQUAL(top.GetOutput() , 
                    "broadcast: CURTURN,1\n"
                    "broadcast: ITEMSTOCK,1,2\n"
                    "broadcast: ITEMSTOCK,2,2\n"
                    "broadcast: ITEMSTOCK,3,2\n"
                    "broadcast: ITEMSTOCK,4,2\n"
                    "broadcast: ITEMSTOCK,5,2\n"
                    "broadcast: ITEMSTOCK,6,2\n"
                    "broadcast: ITEMSTOCK,7,2\n"
                    "broadcast: ITEMSTOCK,8,2\n"
                    "broadcast: ITEMSTOCK,9,2\n"
                    "broadcast: ITEMSTOCK,10,2\n"
                    "broadcast: ITEMSTOCK,11,2\n"
                    "broadcast: ITEMSTOCK,12,2\n"
                    "broadcast: ITEMSTOCK,13,2\n"
                    "broadcast: PLAYERFACTORIES,0,|OH-OH-WH|\n"
                    "broadcast: PLAYERFACTORIES,1,|OH-OH-WH|\n"
                    "broadcast: PLAYERFACTORIES,2,|OH-OH-WH|\n"
                    "broadcast: PLAYERSTATE,0,3,0,5,5,0,10,3,0,3,3,0,-1,-1\n"
                    "broadcast: PLAYERSTATE,1,3,0,5,5,0,10,3,0,3,3,0,-1,-1\n"
                    "broadcast: PLAYERSTATE,2,3,0,5,5,0,10,3,0,3,3,0,-1,-1\n"
                    "broadcast: COMMODITYSTOCK,1,26,0,0,3\n"
                    "broadcast: COMMODITYSTOCK,2,39,0,30,7\n"
                    "broadcast: COMMODITYSTOCK,3,53,0,44,10\n"
                    "broadcast: COMMODITYSTOCK,4,34,0,56,13\n"
                    "broadcast: COMMODITYSTOCK,5,16,0,0,17\n"
                    "broadcast: COMMODITYSTOCK,6,23,0,88,20\n"
                    "broadcast: COMMODITYSTOCK,7,14,0,0,30\n"
                    "broadcast: COMMODITYSTOCK,8,12,0,0,40\n"
                    "broadcast: COMMODITYSTOCK,9,12,0,0,50\n"
                    );


  BOOST_CHECK_EQUAL(ogi2.GetCurTurn() , 1);
  BOOST_CHECK_EQUAL(ogi2.GetCommodities().GetDeck(RESEARCH_COMMODITY).DrawCommodity(true).GetValue() , 56);
  BOOST_CHECK_EQUAL(ogi2.GetUnownedItems().Initialized() , true);
  BOOST_CHECK_EQUAL(ogi2.GetUnownedItems().GetItemCount(DATA_LIBRARY) , 2);

  OutpostGameInfo ogi3(oset,sgi,top);
  ogi3.GetOptions().SetStockMechanism(SOME_ONE_EXTRA);

  ogi3.GetPlayers().add("Player1");
  ogi3.GetPlayers().add("Player2");
  ogi3.GetPlayers()[0].SetOkStartup(true);
  ogi3.GetPlayers()[1].SetOkStartup(true);

  BOOST_CHECK_EQUAL(pSG->ExecuteAction("ValidateSetup",ogi3,"",ap) , true);
  
  int extracount = 0;

  int i;
  for (i = 1 ; i <= 13 ; ++i)
  {
    if (ogi3.GetUnownedItems().GetItemCount((ItemType)i) == 2) extracount++;
  }

  BOOST_CHECK( extracount >= 4 && extracount <= 10);
}

BOOST_AUTO_TEST_CASE( TransitionTestTURNORDER )
{
  OutpostSet oset("");
  ServerGameInfo sgi("Outpost","testOutpost","XMLLOC","XMLFILE");
  TestOutputPort top;

  OutpostStateMachine osm;
  const TransitionType *pTO = GetTransition(osm,"TurnOrder","DETERMINETURNORDER","SupplyFill",1);
  OutpostGameInfo ogi(oset,sgi,top);
  ActionParser ap("");

  // should never be a legal action.
  BOOST_CHECK_EQUAL(pTO->IsLegal("Player1","TurnOrder",ogi) , false);
  // should always be an auto action
  BOOST_CHECK_EQUAL(pTO->IsAuto("TurnOrder",ogi) , true );

  ogi.GetPlayers().add("Player1");
  ogi.GetPlayers().add("Player2");
  ogi.GetPlayers().add("Player3");

  ogi.GetPlayers()[1].GetOwnedItems().AddItem(ORBITAL_LAB); // 3 VP, so, should be in first place.
  ogi.GetPlayers()[0].GetOwnedItems().AddItem(DATA_LIBRARY); // 1 VP, for cost of 15, third.
  ogi.GetPlayers()[2].GetOwnedItems().AddItem(WAREHOUSE); // 1 VP, 25 cost, second

  BOOST_CHECK_EQUAL(ogi.GetPlayers().GetTurnOrder().size() , (size_t)3);
  BOOST_CHECK_EQUAL(ogi.GetPlayers().GetTurnOrder()[0] , 0);
  BOOST_CHECK_EQUAL(ogi.GetPlayers().GetTurnOrder()[1] , 0);
  BOOST_CHECK_EQUAL(ogi.GetPlayers().GetTurnOrder()[2] , 0);

  BOOST_CHECK_EQUAL(pTO->ExecuteAction("TurnOrder",ogi,"",ap) , true);

  BOOST_CHECK_EQUAL(ogi.GetPlayers().GetTurnOrder().size() , (size_t)3);
  BOOST_CHECK_EQUAL(ogi.GetPlayers().GetTurnOrder()[0] , 1);
  BOOST_CHECK_EQUAL(ogi.GetPlayers().GetTurnOrder()[1] , 2);
  BOOST_CHECK_EQUAL(ogi.GetPlayers().GetTurnOrder()[2] , 0);

  BOOST_CHECK_EQUAL(top.GetOutput() , 
                    "broadcast: TURNORDER,1,1\n"
                    "broadcast: TURNORDER,2,2\n"
                    "broadcast: TURNORDER,0,3\n");
}


BOOST_AUTO_TEST_CASE( TransitionTestFILLSHIP )
{
  OutpostSet oset("");
  ServerGameInfo sgi("Outpost","testOutpost","XMLLOC","XMLFILE");
  TestOutputPort top;

  OutpostStateMachine osm;
  const TransitionType *pFS = GetTransition(osm,"SupplyFill","FILLSHIP","StartResources",1);
  OutpostGameInfo ogi(oset,sgi,top);
  ActionParser ap("");

  // should never be a legal action.
  BOOST_CHECK_EQUAL(pFS->IsLegal("Player1","SupplyFill",ogi) , false);
  // should always be an auto action
  BOOST_CHECK_EQUAL(pFS->IsAuto("SupplyFill",ogi) , true );

  ogi.GetPlayers().add("Player1");
  ogi.GetPlayers().add("Player2");
  ogi.GetPlayers().add("Player3");
  ogi.GetUnownedItems().Initialize(3);

  BOOST_CHECK_EQUAL( ogi.GetUnownedItems().GetShipItem(0) , NO_ITEM);
  BOOST_CHECK_EQUAL( ogi.GetUnownedItems().GetShipItem(1) , NO_ITEM);
  BOOST_CHECK_EQUAL( ogi.GetUnownedItems().GetShipItem(2) , NO_ITEM);

  ogi.GetUnownedItems().SetPurchased(1,NODULE,0,30);

  BOOST_CHECK_EQUAL(pFS->ExecuteAction("SupplyFill",ogi,"",ap) , true);

  BOOST_CHECK( ogi.GetUnownedItems().GetShipItem(0) != NO_ITEM);
  BOOST_CHECK( ogi.GetUnownedItems().GetShipItem(1) != NO_ITEM);
  BOOST_CHECK( ogi.GetUnownedItems().GetShipItem(2) != NO_ITEM);

  BOOST_CHECK_EQUAL( ogi.GetUnownedItems().GetPurchaseInfo(1).m_type , NO_ITEM);
  BOOST_CHECK_EQUAL( ogi.GetUnownedItems().GetPurchaseInfo(1).m_playerindex , NO_PLAYER_INDEX);

  std::ostringstream oss;

  oss << "broadcast: SUPPLYSHIP,0," << ogi.GetUnownedItems().GetShipItem(0) << "\n";
  oss << "broadcast: SUPPLYSHIP,1," << ogi.GetUnownedItems().GetShipItem(1) << "\n";
  oss << "broadcast: SUPPLYSHIP,2," << ogi.GetUnownedItems().GetShipItem(2) << "\n";

  std::map<int,int> icount;
  icount[ogi.GetUnownedItems().GetShipItem(0)]++;
  icount[ogi.GetUnownedItems().GetShipItem(1)]++;
  icount[ogi.GetUnownedItems().GetShipItem(2)]++;

  int i;
  for (i = 1 ; i <= 13 ; ++i)
  {
    oss << "broadcast: ITEMSTOCK," << i << "," << 2 - icount[i];
    oss << "\n";
  }

  oss << 
    "broadcast: PURCHASEDATA,0,3000,0,-1\n"
    "broadcast: PURCHASEDATA,1,3000,0,-1\n"
    "broadcast: PURCHASEDATA,2,3000,0,-1\n"
    ;


  BOOST_CHECK_EQUAL( top.GetOutput() , oss.str() );
}

BOOST_AUTO_TEST_CASE( TransitionTestINITCOMMODITY )
{
  OutpostSet oset("");
  ServerGameInfo sgi("Outpost","testOutpost","XMLLOC","XMLFILE");

  std::vector<std::string> vcs;
  vcs.push_back("Player1");
  vcs.push_back("Player2");
  vcs.push_back("Player3");

  TestOutputPort top(vcs);

  OutpostStateMachine osm;
  const TransitionType *pFS = GetTransition(osm,"StartResources","INITCOMMODITY","PromptMegaResources",1);
  OutpostGameInfo ogi(oset,sgi,top);
  ActionParser ap("");

  // should never be a legal action.
  BOOST_CHECK_EQUAL(pFS->IsLegal("Player1","StartResources",ogi) , false);
  // should always be an auto action
  BOOST_CHECK_EQUAL(pFS->IsAuto("StartResources",ogi) , true );

  ogi.GetPlayers().add("Player1");
  ogi.GetPlayers()[0].GetFactories().AddFactory(ORE_FACTORY);
  ogi.GetPlayers()[0].GetFactories().AddFactory(ORE_FACTORY);
  ogi.GetPlayers()[0].GetFactories().AlterManning("HU",2,0);
  ogi.GetPlayers().add("Player2");
  ogi.GetPlayers().DetermineTurnOrder();
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetCommodityHand().GetHandDescription(false,true) , "||");
  BOOST_CHECK_EQUAL( ogi.GetCommodities().GetDeck(ORE_COMMODITY).GetDeckSize() , 26);
  BOOST_CHECK_EQUAL( ogi.GetCommodities().GetDeck(ORE_COMMODITY).GetDiscardSize() , 0);
  BOOST_CHECK_EQUAL(pFS->ExecuteAction("StartResources",ogi,"",ap) , true);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetCommodityHand().GetHandDescription(false,true) , "|OR*|");
  BOOST_CHECK_EQUAL( ogi.GetCommodities().GetDeck(ORE_COMMODITY).GetDeckSize() , 25);
  BOOST_CHECK_EQUAL( ogi.GetCommodities().GetDeck(ORE_COMMODITY).GetDiscardSize() , 0);
  BOOST_CHECK_EQUAL( ogi.GetProductionManager().IsProductionDone() , true );
  
  std::ostringstream oss;
  oss << "broadcast: PLAYERCARDBACKS,0," << 
    ogi.GetPlayers()[0].GetCommodityHand().GetHandDescription(false,true) << std::endl;
  oss << "broadcast: PLAYERCARDBACKS,1," << 
    ogi.GetPlayers()[1].GetCommodityHand().GetHandDescription(false,true) << std::endl;
  oss << "unicast: Player1 -- PLAYERCARDS," << 
    ogi.GetPlayers()[0].GetCommodityHand().GetHandDescription(true,true) << std::endl;
  oss << "unicast: Player2 -- PLAYERCARDS," << 
    ogi.GetPlayers()[1].GetCommodityHand().GetHandDescription(true,true) << std::endl;

  oss << "broadcast: PLAYERSTATE,0,1,0,5,5,0,10,0,0,3,3,0,-1,-1\n";
  oss << "broadcast: PLAYERSTATE,1,0,0,5,5,0,10,0,0,3,3,0,-1,-1\n";

  oss << 
                    "broadcast: COMMODITYSTOCK,1,25,0,0,3\n"
                    "broadcast: COMMODITYSTOCK,2,39,0,30,7\n"
                    "broadcast: COMMODITYSTOCK,3,53,0,44,10\n"
                    "broadcast: COMMODITYSTOCK,4,34,0,56,13\n"
                    "broadcast: COMMODITYSTOCK,5,16,0,0,17\n"
                    "broadcast: COMMODITYSTOCK,6,23,0,88,20\n"
                    "broadcast: COMMODITYSTOCK,7,14,0,0,30\n"
                    "broadcast: COMMODITYSTOCK,8,12,0,0,40\n"
                    "broadcast: COMMODITYSTOCK,9,12,0,0,50\n"
    ;


  BOOST_CHECK_EQUAL( top.GetOutput() , oss.str() );
  top.ResetOutput();

  // also going to check the two completion transitions for Production, since
  // we're here and they don't do anything, the false cases will be tested when we have
  // a non-complete Production Manager

  const TransitionType *pDI = GetTransition(osm,"PromptMegaResources","COMMODITYDONETODISCARD","StartDiscard",3);
  const TransitionType *pPU = GetTransition(osm,"PromptMegaResources","COMMODITYDONETOPURCHASE","StartPurchase",3);
  // should never be a legal action.
  BOOST_CHECK_EQUAL(pDI->IsLegal("Player1","PromptMegaResources",ogi) , false);
  BOOST_CHECK_EQUAL(pPU->IsLegal("Player1","PromptMegaResources",ogi) , false);
  // should be an auto action when we're complete and earlydiscard is true.
  BOOST_CHECK_EQUAL(pDI->IsAuto("PromptMegaResources",ogi) , false );
  BOOST_CHECK_EQUAL(pPU->IsAuto("PromptMegaResources",ogi) , true );

  ogi.GetOptions().SetEarlyDiscard(true);
  BOOST_CHECK_EQUAL(pDI->IsAuto("PromptMegaResources",ogi) , true );
  BOOST_CHECK_EQUAL(pPU->IsAuto("PromptMegaResources",ogi) , false );

  // these aren't supposed to do anything but return true.
  BOOST_CHECK_EQUAL(pDI->ExecuteAction("PromptMegaResources",ogi,"",ap) , true);
  BOOST_CHECK_EQUAL(top.GetOutput() , "");
  BOOST_CHECK_EQUAL(pPU->ExecuteAction("PromptMegaResources",ogi,"",ap) , true);
  BOOST_CHECK_EQUAL(top.GetOutput() , "");
  
}

BOOST_AUTO_TEST_CASE( TransitionTestINITCOMMODITY_Turn1 )
{
  OutpostSet oset("");
  ServerGameInfo sgi("Outpost","testOutpost","XMLLOC","XMLFILE");

  std::vector<std::string> vcs;
  vcs.push_back("Player1");
  vcs.push_back("Player2");
  vcs.push_back("Player3");

  TestOutputPort top(vcs);

  OutpostStateMachine osm;
  const TransitionType *pFS = GetTransition(osm,"StartResources","INITCOMMODITY","PromptMegaResources",1);
  OutpostGameInfo ogi(oset,sgi,top);
  ActionParser ap("");

  // should never be a legal action.
  BOOST_CHECK_EQUAL(pFS->IsLegal("Player1","StartResources",ogi) , false);
  // should always be an auto action
  BOOST_CHECK_EQUAL(pFS->IsAuto("StartResources",ogi) , true );

  ogi.GetPlayers().add("Player1");
  ogi.GetPlayers()[0].GetFactories().AddFactory(ORE_FACTORY);
  ogi.GetPlayers()[0].GetFactories().AddFactory(ORE_FACTORY);
  ogi.GetPlayers()[0].GetFactories().AlterManning("HU",1,0);
  ogi.GetPlayers().add("Player2");
  ogi.GetPlayers().DetermineTurnOrder();
  ogi.SetCurTurn(1);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetCommodityHand().GetHandDescription(false,true) , "||");
  BOOST_CHECK_EQUAL( ogi.GetCommodities().GetDeck(ORE_COMMODITY).GetDeckSize() , 26);
  BOOST_CHECK_EQUAL( ogi.GetCommodities().GetDeck(ORE_COMMODITY).GetDiscardSize() , 0);
  BOOST_CHECK_EQUAL(pFS->ExecuteAction("StartResources",ogi,"",ap) , true);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetCommodityHand().GetHandDescription(false,true) , "|OR*-OR*|");
  BOOST_CHECK_EQUAL( ogi.GetCommodities().GetDeck(ORE_COMMODITY).GetDeckSize() , 24);
  BOOST_CHECK_EQUAL( ogi.GetCommodities().GetDeck(ORE_COMMODITY).GetDiscardSize() , 0);
  BOOST_CHECK_EQUAL( ogi.GetProductionManager().IsProductionDone() , true );
  
  std::ostringstream oss;
  oss << "broadcast: PLAYERCARDBACKS,0," << 
    ogi.GetPlayers()[0].GetCommodityHand().GetHandDescription(false,true) << std::endl;
  oss << "broadcast: PLAYERCARDBACKS,1," << 
    ogi.GetPlayers()[1].GetCommodityHand().GetHandDescription(false,true) << std::endl;
  oss << "unicast: Player1 -- PLAYERCARDS," << 
    ogi.GetPlayers()[0].GetCommodityHand().GetHandDescription(true,true) << std::endl;
  oss << "unicast: Player2 -- PLAYERCARDS," << 
    ogi.GetPlayers()[1].GetCommodityHand().GetHandDescription(true,true) << std::endl;

  oss << "broadcast: PLAYERSTATE,0,1,0,5,5,0,10,0,0,3,3,0,-1,-1\n";
  oss << "broadcast: PLAYERSTATE,1,0,0,5,5,0,10,0,0,3,3,0,-1,-1\n";

  oss << 
                    "broadcast: COMMODITYSTOCK,1,24,0,0,3\n"
                    "broadcast: COMMODITYSTOCK,2,39,0,30,7\n"
                    "broadcast: COMMODITYSTOCK,3,53,0,44,10\n"
                    "broadcast: COMMODITYSTOCK,4,34,0,56,13\n"
                    "broadcast: COMMODITYSTOCK,5,16,0,0,17\n"
                    "broadcast: COMMODITYSTOCK,6,23,0,88,20\n"
                    "broadcast: COMMODITYSTOCK,7,14,0,0,30\n"
                    "broadcast: COMMODITYSTOCK,8,12,0,0,40\n"
                    "broadcast: COMMODITYSTOCK,9,12,0,0,50\n"
    ;


  BOOST_CHECK_EQUAL( top.GetOutput() , oss.str() );
  top.ResetOutput();

  // also going to check the two completion transitions for Production, since
  // we're here and they don't do anything, the false cases will be tested when we have
  // a non-complete Production Manager

  const TransitionType *pDI = GetTransition(osm,"PromptMegaResources","COMMODITYDONETODISCARD","StartDiscard",3);
  const TransitionType *pPU = GetTransition(osm,"PromptMegaResources","COMMODITYDONETOPURCHASE","StartPurchase",3);
  // should never be a legal action.
  BOOST_CHECK_EQUAL(pDI->IsLegal("Player1","PromptMegaResources",ogi) , false);
  BOOST_CHECK_EQUAL(pPU->IsLegal("Player1","PromptMegaResources",ogi) , false);
  // should be an auto action when we're complete and earlydiscard is true.
  BOOST_CHECK_EQUAL(pDI->IsAuto("PromptMegaResources",ogi) , false );
  BOOST_CHECK_EQUAL(pPU->IsAuto("PromptMegaResources",ogi) , true );
  ogi.GetOptions().SetEarlyDiscard(true);
  BOOST_CHECK_EQUAL(pDI->IsAuto("PromptMegaResources",ogi) , true );
  BOOST_CHECK_EQUAL(pPU->IsAuto("PromptMegaResources",ogi) , false );

  // these aren't supposed to do anything but return true.
  BOOST_CHECK_EQUAL(pDI->ExecuteAction("PromptMegaResources",ogi,"",ap) , true);
  BOOST_CHECK_EQUAL(top.GetOutput() , "");
  BOOST_CHECK_EQUAL(pPU->ExecuteAction("PromptMegaResources",ogi,"",ap) , true);
  BOOST_CHECK_EQUAL(top.GetOutput() , "");
  
}


BOOST_AUTO_TEST_CASE( TransitionTestINITCOMMODITY_Refineries )
{
  OutpostSet oset("");
  ServerGameInfo sgi("Outpost","testOutpost","XMLLOC","XMLFILE");

  std::vector<std::string> vcs;
  vcs.push_back("Player1");
  vcs.push_back("Player2");
  vcs.push_back("Player3");

  TestOutputPort top(vcs);

  OutpostStateMachine osm;
  const TransitionType *pFS = GetTransition(osm,"StartResources","INITCOMMODITY","PromptMegaResources",1);
  OutpostGameInfo ogi(oset,sgi,top);
  ActionParser ap("");

  // should never be a legal action.
  BOOST_CHECK_EQUAL(pFS->IsLegal("Player1","StartResources",ogi) , false);
  // should always be an auto action
  BOOST_CHECK_EQUAL(pFS->IsAuto("StartResources",ogi) , true );

  ogi.GetPlayers().add("Player1");
  ogi.GetPlayers()[0].GetFactories().AddFactory(ORE_FACTORY);
  ogi.GetPlayers()[0].GetFactories().AddFactory(ORE_FACTORY);
  ogi.GetPlayers()[0].GetFactories().AlterManning("HU",1,0);
  ogi.GetPlayers().add("Player2");
  ogi.GetPlayers().DetermineTurnOrder();
  ogi.GetOptions().SetRefineries(true);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetCommodityHand().GetHandDescription(false,true) , "||");
  BOOST_CHECK_EQUAL( ogi.GetCommodities().GetDeck(ORE_COMMODITY).GetDeckSize() , 26);
  BOOST_CHECK_EQUAL( ogi.GetCommodities().GetDeck(ORE_COMMODITY).GetDiscardSize() , 0);
  BOOST_CHECK_EQUAL(pFS->ExecuteAction("StartResources",ogi,"",ap) , true);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetCommodityHand().GetHandDescription(false,true) , "|OR*|");
  BOOST_CHECK_EQUAL( ogi.GetCommodities().GetDeck(ORE_COMMODITY).GetDeckSize() , 24);
  BOOST_CHECK_EQUAL( ogi.GetCommodities().GetDeck(ORE_COMMODITY).GetDiscardSize() , 1);
  BOOST_CHECK_EQUAL( ogi.GetProductionManager().IsProductionDone() , true );
  
  std::ostringstream oss;
  oss << "broadcast: PLAYERCARDBACKS,0," << 
    ogi.GetPlayers()[0].GetCommodityHand().GetHandDescription(false,true) << std::endl;
  oss << "broadcast: PLAYERCARDBACKS,1," << 
    ogi.GetPlayers()[1].GetCommodityHand().GetHandDescription(false,true) << std::endl;
  oss << "unicast: Player1 -- PLAYERCARDS," << 
    ogi.GetPlayers()[0].GetCommodityHand().GetHandDescription(true,true) << std::endl;
  oss << "unicast: Player2 -- PLAYERCARDS," << 
    ogi.GetPlayers()[1].GetCommodityHand().GetHandDescription(true,true) << std::endl;
  oss << "broadcast: PLAYERSTATE,0,1,0,5,5,0,10,0,0,3,3,0,-1,-1\n";
  oss << "broadcast: PLAYERSTATE,1,0,0,5,5,0,10,0,0,3,3,0,-1,-1\n";

  oss << 
                    "broadcast: COMMODITYSTOCK,1,24,1,0,3\n"
                    "broadcast: COMMODITYSTOCK,2,39,0,30,7\n"
                    "broadcast: COMMODITYSTOCK,3,53,0,44,10\n"
                    "broadcast: COMMODITYSTOCK,4,34,0,56,13\n"
                    "broadcast: COMMODITYSTOCK,5,16,0,0,17\n"
                    "broadcast: COMMODITYSTOCK,6,23,0,88,20\n"
                    "broadcast: COMMODITYSTOCK,7,14,0,0,30\n"
                    "broadcast: COMMODITYSTOCK,8,12,0,0,40\n"
                    "broadcast: COMMODITYSTOCK,9,12,0,0,50\n"
    ;


  BOOST_CHECK_EQUAL( top.GetOutput() , oss.str() );
  top.ResetOutput();

  // also going to check the two completion transitions for Production, since
  // we're here and they don't do anything, the false cases will be tested when we have
  // a non-complete Production Manager

  const TransitionType *pDI = GetTransition(osm,"PromptMegaResources","COMMODITYDONETODISCARD","StartDiscard",3);
  const TransitionType *pPU = GetTransition(osm,"PromptMegaResources","COMMODITYDONETOPURCHASE","StartPurchase",3);
  // should never be a legal action.
  BOOST_CHECK_EQUAL(pDI->IsLegal("Player1","PromptMegaResources",ogi) , false);
  BOOST_CHECK_EQUAL(pPU->IsLegal("Player1","PromptMegaResources",ogi) , false);
  // should be an auto action when we're complete and earlydiscard is true.
  BOOST_CHECK_EQUAL(pDI->IsAuto("PromptMegaResources",ogi) , false );
  BOOST_CHECK_EQUAL(pPU->IsAuto("PromptMegaResources",ogi) , true );
  ogi.GetOptions().SetEarlyDiscard(true);
  BOOST_CHECK_EQUAL(pDI->IsAuto("PromptMegaResources",ogi) , true );
  BOOST_CHECK_EQUAL(pPU->IsAuto("PromptMegaResources",ogi) , false );

  // these aren't supposed to do anything but return true.
  BOOST_CHECK_EQUAL(pDI->ExecuteAction("PromptMegaResources",ogi,"",ap) , true);
  BOOST_CHECK_EQUAL(top.GetOutput() , "");
  BOOST_CHECK_EQUAL(pPU->ExecuteAction("PromptMegaResources",ogi,"",ap) , true);
  BOOST_CHECK_EQUAL(top.GetOutput() , "");
  
}


// validating correct behaviour for Blind Draws
BOOST_AUTO_TEST_CASE( TransitionTestBlindDraws )
{
  OutpostSet oset("");
  ServerGameInfo sgi("Outpost","testOutpost","XMLLOC","XMLFILE");

  TestOutputPort top;

  OutpostStateMachine osm;
  OutpostGameInfo ogi(oset,sgi,top);
  ActionParser ap("");
  
  // player 1 has 5 vp
  ogi.GetPlayers().add("Player1");
  ogi.GetPlayers()[0].GetFactories().AddFactory(WATER_FACTORY);
  ogi.GetPlayers()[0].GetFactories().AddFactory(WATER_FACTORY);
  ogi.GetPlayers()[0].GetFactories().AddFactory(WATER_FACTORY);
  ogi.GetPlayers()[0].GetFactories().AddFactory(WATER_FACTORY);
  ogi.GetPlayers()[0].GetFactories().AddFactory(WATER_FACTORY);
  ogi.GetPlayers()[0].GetFactories().AlterManning("HHHHH",5,0);

  Commodity c1(RESEARCH_COMMODITY,17,false,false);
  c1.MakeOld();
  ogi.GetPlayers()[0].GetCommodityHand().AddCommodity(c1);

  // player 2 has 11 vp
  ogi.GetPlayers().add("Player2");
  ogi.GetPlayers()[1].GetOwnedItems().AddItem(ECOPLANTS);
  ogi.GetPlayers()[1].GetOwnedItems().AddItem(ECOPLANTS);
  ogi.GetPlayers()[1].GetFactories().AddFactory(WATER_FACTORY);
  ogi.GetPlayers()[1].GetFactories().AlterManning("H",1,0);

  Commodity c2(TITANIUM_COMMODITY,12,false,false);
  c2.MakeOld();
  ogi.GetPlayers()[1].GetCommodityHand().AddCommodity(c2);

  ogi.GetPlayers().DetermineTurnOrder();

  // set the blind draw flag
  ogi.GetOptions().SetBlindDraws(true);

  const TransitionType *pInit = GetTransition(osm,"StartResources","INITCOMMODITY","PromptMegaResources",1);
  BOOST_CHECK_EQUAL(pInit->ExecuteAction("StartResources",ogi,"",ap) , true);
  BOOST_CHECK_EQUAL(top.GetOutput(),
                    "broadcast: PLAYERCARDBACKS,0,|RE*|\n"
                    "broadcast: PLAYERCARDBACKS,1,|TI*-WA*|\n"
                    "unicast: Player1 -- PLAYERCARDS,|RE17|\n"
                    "unicast: Player2 -- PLAYERCARDS,|TI12-WA*|\n"
                    "broadcast: PLAYERSTATE,0,5,0,5,5,0,10,0,0,1,3,0,-1,-1\n"
                    "broadcast: PLAYERSTATE,1,11,100,5,5,0,5,0,0,3,3,0,-1,-1\n"
                    "broadcast: COMMODITYSTOCK,1,26,0,0,3\n"
                    "broadcast: COMMODITYSTOCK,2,38,0,30,7\n"
                    "broadcast: COMMODITYSTOCK,3,53,0,44,10\n"
                    "broadcast: COMMODITYSTOCK,4,34,0,56,13\n"
                    "broadcast: COMMODITYSTOCK,5,16,0,0,17\n"
                    "broadcast: COMMODITYSTOCK,6,23,0,88,20\n"
                    "broadcast: COMMODITYSTOCK,7,14,0,0,30\n"
                    "broadcast: COMMODITYSTOCK,8,12,0,0,40\n"
                    "broadcast: COMMODITYSTOCK,9,12,0,0,50\n"
                    );
}







  
// getting into the state for REQUESTMEGA should allow us to test COMMODITYDONE false states, too.
BOOST_AUTO_TEST_CASE( TransitionTestREQUESTMEGA )
{
  OutpostSet oset("");
  ServerGameInfo sgi("Outpost","testOutpost","XMLLOC","XMLFILE");

  std::vector<std::string> vcs;
  vcs.push_back("Player1");
  vcs.push_back("Player2");
  vcs.push_back("Player3");

  TestOutputPort top(vcs);

  OutpostStateMachine osm;
  const TransitionType *pFS = GetTransition(osm,"PromptMegaResources","REQUESTMEGA","MegaResources",3);
  OutpostGameInfo ogi(oset,sgi,top);
  ActionParser ap("");

  // should never be a legal action.
  BOOST_CHECK_EQUAL(pFS->IsLegal("Player1","PromptMegaResources",ogi) , false);

  ogi.GetPlayers().add("Player1");
  ogi.GetPlayers()[0].GetFactories().AddFactory(WATER_FACTORY);
  ogi.GetPlayers()[0].GetFactories().AddFactory(WATER_FACTORY);
  ogi.GetPlayers()[0].GetFactories().AddFactory(WATER_FACTORY);
  ogi.GetPlayers()[0].GetFactories().AddFactory(WATER_FACTORY);
  ogi.GetPlayers()[0].GetFactories().AddFactory(WATER_FACTORY);
  ogi.GetPlayers()[0].GetFactories().AlterManning("HHHHH",5,0);
  ogi.GetPlayers().add("Player2");
  // futzing with VP to give a turn order.
  ogi.GetPlayers()[1].GetOwnedItems().AddItem(ECOPLANTS);
  ogi.GetPlayers()[1].GetOwnedItems().AddItem(ECOPLANTS);
  ogi.GetPlayers().DetermineTurnOrder();

  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetCommodityHand().GetHandDescription(false,true) , "||");
  BOOST_CHECK_EQUAL( ogi.GetCommodities().GetDeck(ORE_COMMODITY).GetDeckSize() , 26);

  // if production is done, this is false.
  BOOST_CHECK_EQUAL(pFS->IsAuto("PromptMegaResources",ogi) , false);

  // before we call this, the production manager reports IsProductionDone = true
  ogi.GetProductionManager().StartProduction(ogi.GetPlayers(),
                                             ogi.GetCommodities(),
                                             ogi.GetCurTurn() == 1,
                                             ogi.GetOptions().GetRefineries());

  // if production is not done, this is true.
  BOOST_CHECK_EQUAL(pFS->IsAuto("PromptMegaResources",ogi) , true);
  // this should generate a message.
  BOOST_CHECK_EQUAL(pFS->ExecuteAction("PromptMegaResources",ogi,"",ap) , true);
  BOOST_CHECK_EQUAL(top.GetOutput() , 
                    "unicast: Player1 -- MEGAREQUEST,2,1\n"
                    );
  top.ResetOutput();

  // also going to check the two false completion transitions for Production, since
  // we're here and they don't do anything.

  const TransitionType *pDI = GetTransition(osm,"PromptMegaResources","COMMODITYDONETODISCARD","StartDiscard",3);
  const TransitionType *pPU = GetTransition(osm,"PromptMegaResources","COMMODITYDONETOPURCHASE","StartPurchase",3);
  // should never be a legal action.
  BOOST_CHECK_EQUAL(pDI->IsLegal("Player1","PromptMegaResources",ogi) , false);
  BOOST_CHECK_EQUAL(pPU->IsLegal("Player1","PromptMegaResources",ogi) , false);
  // should be false when ProductionManager is not complete.
  BOOST_CHECK_EQUAL(pDI->IsAuto("PromptMegaResources",ogi) , false );
  BOOST_CHECK_EQUAL(pPU->IsAuto("PromptMegaResources",ogi) , false );
  ogi.GetOptions().SetEarlyDiscard(true);
  BOOST_CHECK_EQUAL(pDI->IsAuto("PromptMegaResources",ogi) , false );
  BOOST_CHECK_EQUAL(pPU->IsAuto("PromptMegaResources",ogi) , false );

  // since the system is set up correctly, we're going to test NUMMEGAS as well.
  const TransitionType *pNM = GetTransition(osm,"MegaResources","NUMMEGAS","PromptMegaResources",1);

  BOOST_CHECK_EQUAL(pNM->IsAuto("MegaResources",ogi) , false );
  // must be the player who's mega it is.
  BOOST_CHECK_EQUAL(pNM->IsLegal("Player1","MegaResources",ogi) , true );
  BOOST_CHECK_EQUAL(pNM->IsLegal("Player2","MegaResources",ogi) , false );

  // bad input should keep it from executing.
  ActionParser ap2("NUMMEGAS,-1");
  BOOST_CHECK_EQUAL(pNM->ExecuteAction("MegaResources",ogi,"Player1",ap2) , false);
  BOOST_CHECK_EQUAL(ogi.GetProductionManager().IsProductionDone() , false);
  BOOST_CHECK_EQUAL(top.GetOutput(), "unicast: Player1 -- ERROR,Action NUMMEGAS variable i_count is below minimum\n");
  top.ResetOutput();

  // bad input should keep it from executing.
  ActionParser ap3("NUMMEGAS,2");
  BOOST_CHECK_EQUAL(pNM->ExecuteAction("MegaResources",ogi,"Player1",ap3) , false);
  BOOST_CHECK_EQUAL(ogi.GetProductionManager().IsProductionDone() , false);
  BOOST_CHECK_EQUAL(top.GetOutput(), "unicast: Player1 -- ERROR,Action NUMMEGAS variable i_count is above maximum\n");
  top.ResetOutput();

  ActionParser ap4("NUMMEGAS,1");
  BOOST_CHECK_EQUAL(pNM->ExecuteAction("MegaResources",ogi,"Player1",ap4) , true);
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[0].GetCommodityHand().GetHandDescription(false,true),"|WA30M-WA*|");

  std::ostringstream oss;
  oss << "broadcast: PLAYERCARDBACKS,0," << 
    ogi.GetPlayers()[0].GetCommodityHand().GetHandDescription(false,true) << std::endl;
  oss << "broadcast: PLAYERCARDBACKS,1," << 
    ogi.GetPlayers()[1].GetCommodityHand().GetHandDescription(false,true) << std::endl;
  oss << "unicast: Player1 -- PLAYERCARDS," << 
    ogi.GetPlayers()[0].GetCommodityHand().GetHandDescription(true,true) << std::endl;
  oss << "unicast: Player2 -- PLAYERCARDS," << 
    ogi.GetPlayers()[1].GetCommodityHand().GetHandDescription(true,true) << std::endl;
  oss << "broadcast: PLAYERSTATE,0,5,0,10,5,0,10,0,0,3,3,0,-1,-1\n";
  oss << "broadcast: PLAYERSTATE,1,10,100,10,5,0,5,0,0,3,3,0,-1,-1\n";

  oss << 
                    "broadcast: COMMODITYSTOCK,1,26,0,0,3\n"
                    "broadcast: COMMODITYSTOCK,2,38,0,30,7\n"
                    "broadcast: COMMODITYSTOCK,3,53,0,44,10\n"
                    "broadcast: COMMODITYSTOCK,4,34,0,56,13\n"
                    "broadcast: COMMODITYSTOCK,5,16,0,0,17\n"
                    "broadcast: COMMODITYSTOCK,6,23,0,88,20\n"
                    "broadcast: COMMODITYSTOCK,7,14,0,0,30\n"
                    "broadcast: COMMODITYSTOCK,8,12,0,0,40\n"
                    "broadcast: COMMODITYSTOCK,9,12,0,0,50\n"
    ;

  BOOST_CHECK_EQUAL( top.GetOutput() , oss.str() );
  
}

BOOST_AUTO_TEST_CASE( TransitionTestDISCARD1 )
{
  OutpostSet oset("");
  ServerGameInfo sgi("Outpost","testOutpost","XMLLOC","XMLFILE");

  std::vector<std::string> vcs;
  vcs.push_back("Player1");
  vcs.push_back("Player2");
  vcs.push_back("Player3");

  TestOutputPort top(vcs);

  OutpostStateMachine osm;
  const TransitionType *pSD = GetTransition(osm,"StartDiscard","STARTDISCARD","DoDiscard",1);
  OutpostGameInfo ogi(oset,sgi,top);
  ActionParser ap("");

  ogi.GetPlayers().add("Player1");
  ogi.GetPlayers()[0].GetOwnedItems().AddItem(OUTPOST);
  ogi.GetPlayers().add("Player2");
  ogi.GetPlayers()[1].GetOwnedItems().AddItem(DATA_LIBRARY);
  ogi.GetPlayers().add("Player3");
  ogi.GetPlayers().DetermineTurnOrder(); // 0,1,2
  ogi.GetPlayers().StartTurnOrder();
  std::string pl1name = ogi.GetPlayers().GetCurTurnPlayer().GetName();

  ogi.GetPlayers().IncrementTurnOrder(); // points to the second player now
  std::string pl2name = ogi.GetPlayers().GetCurTurnPlayer().GetName();

  // just for sanity, make sure they are different.
  BOOST_CHECK( pl1name != pl2name );

  // should never be a legal action.
  BOOST_CHECK_EQUAL(pSD->IsLegal("Player1","StartDiscard",ogi) , false);
  BOOST_CHECK_EQUAL(pSD->IsLegal("Player2","StartDiscard",ogi) , false);
  BOOST_CHECK_EQUAL(pSD->IsLegal("Player3","StartDiscard",ogi) , false);
  // should always be an auto action
  BOOST_CHECK_EQUAL(pSD->IsAuto("StartDiscard",ogi) , true );

  BOOST_CHECK_EQUAL(pSD->ExecuteAction("StartDiscard",ogi,"",ap) , true);
  BOOST_CHECK_EQUAL(top.GetOutput() , 
                    "broadcast: PLAYERSTATE,0,5,100,8,10,0,10,0,0,1,3,0,-1,-1\n"
                    "broadcast: PLAYERSTATE,1,1,15,5,5,0,10,0,0,0,3,0,-1,-1\n"
                    "broadcast: PLAYERSTATE,2,0,0,5,5,0,10,0,0,0,3,0,-1,-1\n"
                    );
  BOOST_CHECK_EQUAL( ogi.GetPlayers().GetCurTurnPlayer().GetName() , pl1name );

  const TransitionType *pDP = GetTransition(osm,"DoDiscard","ENDDISCARDTOPURCHASE","StartPurchase",4);
  const TransitionType *pDE = GetTransition(osm,"DoDiscard","ENDDISCARDTOTURNEND","TurnEnd",4);

  // neither are allowed
  BOOST_CHECK_EQUAL(pDP->IsLegal("Player1","DoDiscard",ogi) , false);
  BOOST_CHECK_EQUAL(pDP->IsLegal("Player2","DoDiscard",ogi) , false);
  BOOST_CHECK_EQUAL(pDP->IsLegal("Player3","DoDiscard",ogi) , false);
  BOOST_CHECK_EQUAL(pDE->IsLegal("Player1","DoDiscard",ogi) , false);
  BOOST_CHECK_EQUAL(pDE->IsLegal("Player2","DoDiscard",ogi) , false);
  BOOST_CHECK_EQUAL(pDE->IsLegal("Player3","DoDiscard",ogi) , false);

  // neither are auto when we're not to the end yet.
  ogi.GetPlayers().StartTurnOrder(); // points to player 1
  BOOST_CHECK_EQUAL(pDP->IsAuto("DoDiscard",ogi) , false );
  BOOST_CHECK_EQUAL(pDE->IsAuto("DoDiscard",ogi) , false );
  ogi.GetPlayers().IncrementTurnOrder(); // points to player 2
  BOOST_CHECK_EQUAL(pDP->IsAuto("DoDiscard",ogi) , false );
  BOOST_CHECK_EQUAL(pDE->IsAuto("DoDiscard",ogi) , false );
  ogi.GetPlayers().IncrementTurnOrder(); // points to player 3
  BOOST_CHECK_EQUAL(pDP->IsAuto("DoDiscard",ogi) , false );
  BOOST_CHECK_EQUAL(pDE->IsAuto("DoDiscard",ogi) , false );
  ogi.GetPlayers().IncrementTurnOrder(); // points to done.
  // now we're done, one of these should be true.
  BOOST_CHECK_EQUAL(pDP->IsAuto("DoDiscard",ogi) , false );
  BOOST_CHECK_EQUAL(pDE->IsAuto("DoDiscard",ogi) , true );
  ogi.GetOptions().SetEarlyDiscard(true);
  BOOST_CHECK_EQUAL(pDP->IsAuto("DoDiscard",ogi) , true );
  BOOST_CHECK_EQUAL(pDE->IsAuto("DoDiscard",ogi) , false );
}

BOOST_AUTO_TEST_CASE( TransitionTestDISCARD2 )
{
  OutpostSet oset("");
  ServerGameInfo sgi("Outpost","testOutpost","XMLLOC","XMLFILE");

  std::vector<std::string> vcs;
  vcs.push_back("Player1");
  vcs.push_back("Player2");
  vcs.push_back("Player3");

  TestOutputPort top(vcs);

  OutpostStateMachine osm;
  OutpostGameInfo ogi(oset,sgi,top);
  ActionParser ap("");

  ogi.GetPlayers().add("Player1");
  ogi.GetPlayers().DetermineTurnOrder(); // points to the first player
  ogi.GetPlayers().StartTurnOrder();

  const TransitionType *pAS = GetTransition(osm,"DoDiscard","AUTOSKIPDISCARD","DoDiscard",4);
  const TransitionType *pMD = GetTransition(osm,"DoDiscard","MANUALDISCARD","ManualDiscard",4);

  // neither are allowed
  BOOST_CHECK_EQUAL(pAS->IsLegal("Player1","DoDiscard",ogi) , false);
  BOOST_CHECK_EQUAL(pAS->IsLegal("Player2","DoDiscard",ogi) , false);
  BOOST_CHECK_EQUAL(pAS->IsLegal("Player3","DoDiscard",ogi) , false);
  BOOST_CHECK_EQUAL(pMD->IsLegal("Player1","DoDiscard",ogi) , false);
  BOOST_CHECK_EQUAL(pMD->IsLegal("Player2","DoDiscard",ogi) , false);
  BOOST_CHECK_EQUAL(pMD->IsLegal("Player3","DoDiscard",ogi) , false);

  ogi.GetPlayers().StartTurnOrder(); // points to player 1
  // AS is auto when a) we aren't done with turn order, and
  // b) our card weight is no greater than our hand limit.
  // MD is auto when a) we aren't done with turn order, and
  // b) our card weight is greater than our hand limit.
  // current state: not done, not early discard (HL = 5), large Research and Micro
  // 3 research, 2 micro should go to the edge nicely.
  ogi.GetPlayers()[0].GetCommodityHand().AddCommodity(Commodity(RESEARCH_COMMODITY,17,false,false));  
  ogi.GetPlayers()[0].GetCommodityHand().AddCommodity(Commodity(RESEARCH_COMMODITY,18,false,false));
  ogi.GetPlayers()[0].GetCommodityHand().AddCommodity(Commodity(RESEARCH_COMMODITY,19,false,false));
  BOOST_CHECK_EQUAL(pAS->IsAuto("DoDiscard",ogi) , true );
  BOOST_CHECK_EQUAL(pMD->IsAuto("DoDiscard",ogi) , false );

  ogi.GetPlayers().IncrementTurnOrder(); // should make us done.
  BOOST_CHECK_EQUAL(pAS->IsAuto("DoDiscard",ogi) , false );
  BOOST_CHECK_EQUAL(pMD->IsAuto("DoDiscard",ogi) , false );
  ogi.GetPlayers().StartTurnOrder();

  ogi.GetPlayers()[0].GetCommodityHand().AddCommodity(Commodity(MICROBIOTICS_COMMODITY,18,false,false));
  ogi.GetPlayers()[0].GetCommodityHand().AddCommodity(Commodity(MICROBIOTICS_COMMODITY,19,false,false));
  BOOST_CHECK_EQUAL(pAS->IsAuto("DoDiscard",ogi) , true );
  BOOST_CHECK_EQUAL(pMD->IsAuto("DoDiscard",ogi) , false );

  ogi.GetPlayers().IncrementTurnOrder(); // should make us done.
  BOOST_CHECK_EQUAL(pAS->IsAuto("DoDiscard",ogi) , false );
  BOOST_CHECK_EQUAL(pMD->IsAuto("DoDiscard",ogi) , false );
  ogi.GetPlayers().StartTurnOrder();

  // one more puts us over?
  ogi.GetPlayers()[0].GetCommodityHand().AddCommodity(Commodity(MICROBIOTICS_COMMODITY,19,false,false));
  BOOST_CHECK_EQUAL(pAS->IsAuto("DoDiscard",ogi) , false );
  BOOST_CHECK_EQUAL(pMD->IsAuto("DoDiscard",ogi) , true );

  ogi.GetPlayers().IncrementTurnOrder(); // should make us done.
  BOOST_CHECK_EQUAL(pAS->IsAuto("DoDiscard",ogi) , false );
  BOOST_CHECK_EQUAL(pMD->IsAuto("DoDiscard",ogi) , false );
  ogi.GetPlayers().StartTurnOrder();

  // if we set small microbiotics, we should be below again.
  ogi.GetOptions().SetMicroIsSmall(true);
  BOOST_CHECK_EQUAL(pAS->IsAuto("DoDiscard",ogi) , true );
  BOOST_CHECK_EQUAL(pMD->IsAuto("DoDiscard",ogi) , false );
  // three more research should do it.
  ogi.GetPlayers()[0].GetCommodityHand().AddCommodity(Commodity(RESEARCH_COMMODITY,17,false,false));  
  ogi.GetPlayers()[0].GetCommodityHand().AddCommodity(Commodity(RESEARCH_COMMODITY,18,false,false));
  ogi.GetPlayers()[0].GetCommodityHand().AddCommodity(Commodity(RESEARCH_COMMODITY,19,false,false));

  BOOST_CHECK_EQUAL(pAS->IsAuto("DoDiscard",ogi) , false );
  BOOST_CHECK_EQUAL(pMD->IsAuto("DoDiscard",ogi) , true );

  ogi.GetPlayers().IncrementTurnOrder(); // should make us done.
  BOOST_CHECK_EQUAL(pAS->IsAuto("DoDiscard",ogi) , false );
  BOOST_CHECK_EQUAL(pMD->IsAuto("DoDiscard",ogi) , false );
  ogi.GetPlayers().StartTurnOrder();

  ogi.GetOptions().SetMicroIsSmall(false);
  ogi.GetOptions().SetResearchIsSmall(true);
  BOOST_CHECK_EQUAL(pAS->IsAuto("DoDiscard",ogi) , true );
  BOOST_CHECK_EQUAL(pMD->IsAuto("DoDiscard",ogi) , false );

  ogi.GetPlayers()[0].GetCommodityHand().AddCommodity(Commodity(MICROBIOTICS_COMMODITY,18,false,false));
  ogi.GetPlayers()[0].GetCommodityHand().AddCommodity(Commodity(MICROBIOTICS_COMMODITY,19,false,false));
  ogi.GetPlayers()[0].GetCommodityHand().AddCommodity(Commodity(MICROBIOTICS_COMMODITY,18,false,false));

  BOOST_CHECK_EQUAL(pAS->IsAuto("DoDiscard",ogi) , false );
  BOOST_CHECK_EQUAL(pMD->IsAuto("DoDiscard",ogi) , true );

  ogi.GetPlayers().IncrementTurnOrder(); // should make us done.
  BOOST_CHECK_EQUAL(pAS->IsAuto("DoDiscard",ogi) , false );
  BOOST_CHECK_EQUAL(pMD->IsAuto("DoDiscard",ogi) , false );
  ogi.GetPlayers().StartTurnOrder();

  // finally, EarlyDiscard being true will cause the limit to be ten, not five.
  ogi.GetOptions().SetEarlyDiscard(true);
  BOOST_CHECK_EQUAL(pAS->IsAuto("DoDiscard",ogi) , true );
  BOOST_CHECK_EQUAL(pMD->IsAuto("DoDiscard",ogi) , false );

  ogi.GetPlayers()[0].GetCommodityHand().AddCommodity(Commodity(TITANIUM_COMMODITY,18,false,false));
  ogi.GetPlayers()[0].GetCommodityHand().AddCommodity(Commodity(TITANIUM_COMMODITY,19,false,false));
  ogi.GetPlayers()[0].GetCommodityHand().AddCommodity(Commodity(TITANIUM_COMMODITY,18,false,false));
  ogi.GetPlayers()[0].GetCommodityHand().AddCommodity(Commodity(TITANIUM_COMMODITY,18,false,false));

  BOOST_CHECK_EQUAL(pAS->IsAuto("DoDiscard",ogi) , true );
  BOOST_CHECK_EQUAL(pMD->IsAuto("DoDiscard",ogi) , false );

  ogi.GetPlayers()[0].GetCommodityHand().AddCommodity(Commodity(MOON_ORE_COMMODITY,18,false,false));

  BOOST_CHECK_EQUAL(pAS->IsAuto("DoDiscard",ogi) , false );
  BOOST_CHECK_EQUAL(pMD->IsAuto("DoDiscard",ogi) , true );

  ogi.GetPlayers().IncrementTurnOrder(); // should make us done.
  BOOST_CHECK_EQUAL(pAS->IsAuto("DoDiscard",ogi) , false );
  BOOST_CHECK_EQUAL(pMD->IsAuto("DoDiscard",ogi) , false );
  ogi.GetPlayers().StartTurnOrder();

  // executing the skip should cause an increment. 
  BOOST_CHECK_EQUAL(pAS->ExecuteAction("DoDiscard",ogi,"",ap) , true);
  BOOST_CHECK_EQUAL(top.GetOutput() , 
                    "broadcast: PLAYERSTATE,0,0,0,10,5,0,10,0,0,3,3,0,-1,-1\n"
                    );
  top.ResetOutput();
  BOOST_CHECK_EQUAL(ogi.GetPlayers().AllPlayersDone() , true );

  // executing the Manual Discard should cause it to not skip, and to generate output.
  ogi.GetPlayers().StartTurnOrder();
  BOOST_CHECK_EQUAL(pMD->ExecuteAction("DoDiscard",ogi,"",ap) , true);
  BOOST_CHECK_EQUAL(ogi.GetPlayers().AllPlayersDone() , false );
  BOOST_CHECK_EQUAL(top.GetOutput() , "");
  top.ResetOutput();

  ogi.GetOptions().SetEarlyDiscard(false);
  
  BOOST_CHECK_EQUAL(pMD->ExecuteAction("DoDiscard",ogi,"",ap) , true);
  BOOST_CHECK_EQUAL(ogi.GetPlayers().AllPlayersDone() , false );
  BOOST_CHECK_EQUAL(top.GetOutput() , "" );
                    
}

BOOST_AUTO_TEST_CASE( TransitionTestMANUALDISCARD )
{
  OutpostSet oset("");
  ServerGameInfo sgi("Outpost","testOutpost","XMLLOC","XMLFILE");

  std::vector<std::string> vcs;
  vcs.push_back("Player1");
  vcs.push_back("Player2");
  vcs.push_back("Player3");

  TestOutputPort top(vcs);

  OutpostStateMachine osm;
  OutpostGameInfo ogi(oset,sgi,top);

  ogi.GetPlayers().add("Player1");
  ogi.GetPlayers().DetermineTurnOrder(); // points to the first player
  ogi.GetPlayers().StartTurnOrder();

  const TransitionType *pMD = GetTransition(osm,"ManualDiscard","DISCARD","DoDiscard",1);
  // always false.
  BOOST_CHECK_EQUAL(pMD->IsAuto("ManualDiscard",ogi) , false );
  // allowed is true if it's the player in question.
  BOOST_CHECK_EQUAL(pMD->IsLegal("Player1","ManualDiscard",ogi) , true);
  BOOST_CHECK_EQUAL(pMD->IsLegal("Player2","ManualDiscard",ogi) , false);

  ogi.GetPlayers()[0].GetCommodityHand().AddCommodity(Commodity(TITANIUM_COMMODITY,15,false,false));
  ogi.GetPlayers()[0].GetCommodityHand().AddCommodity(Commodity(TITANIUM_COMMODITY,16,false,false));
  ogi.GetPlayers()[0].GetCommodityHand().AddCommodity(Commodity(TITANIUM_COMMODITY,17,false,false));
  ogi.GetPlayers()[0].GetCommodityHand().AddCommodity(Commodity(TITANIUM_COMMODITY,18,false,false));
  ogi.GetPlayers()[0].GetCommodityHand().AddCommodity(Commodity(TITANIUM_COMMODITY,19,false,false));
  ogi.GetPlayers()[0].GetCommodityHand().AddCommodity(Commodity(TITANIUM_COMMODITY,20,false,false));
  ogi.GetPlayers()[0].GetCommodityHand().AddCommodity(Commodity(TITANIUM_COMMODITY,21,false,false));
  ogi.GetPlayers()[0].GetCommodityHand().AddCommodity(Commodity(TITANIUM_COMMODITY,22,false,false));

  // these things should not change if we get do a bad result.
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetCommodityHand().GetHandDescription(true,true) , 
                     "|TI22-TI21-TI20-TI19-TI18-TI17-TI16-TI15|");
  BOOST_CHECK_EQUAL( ogi.GetCommodities().GetDeck(TITANIUM_COMMODITY).GetDiscardSize() , 0 );
  BOOST_CHECK_EQUAL( ogi.GetPlayers().AllPlayersDone() , false );

  ActionParser ap("DISCARD,001100100");
  BOOST_CHECK_EQUAL(pMD->ExecuteAction("ManualDiscard",ogi,"Player1",ap) , false);
  BOOST_CHECK_EQUAL( top.GetOutput() , "unicast: Player1 -- ERROR,Mismatched delete string length\n" );
  top.ResetOutput();

  // these things should not change if we get do a bad result.
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetCommodityHand().GetHandDescription(true,true) , 
                     "|TI22-TI21-TI20-TI19-TI18-TI17-TI16-TI15|");
  BOOST_CHECK_EQUAL( ogi.GetCommodities().GetDeck(TITANIUM_COMMODITY).GetDiscardSize() , 0 );
  BOOST_CHECK_EQUAL( ogi.GetPlayers().AllPlayersDone() , false );

  ActionParser ap2("DISCARD,001B0000");
  BOOST_CHECK_EQUAL(pMD->ExecuteAction("ManualDiscard",ogi,"Player1",ap2) , false);
  BOOST_CHECK_EQUAL( top.GetOutput() , "unicast: Player1 -- ERROR,Illegal characters in delete string\n" );
  top.ResetOutput();

  // these things should not change if we get do a bad result.
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetCommodityHand().GetHandDescription(true,true) , 
                     "|TI22-TI21-TI20-TI19-TI18-TI17-TI16-TI15|");
  BOOST_CHECK_EQUAL( ogi.GetCommodities().GetDeck(TITANIUM_COMMODITY).GetDiscardSize() , 0 );
  BOOST_CHECK_EQUAL( ogi.GetPlayers().AllPlayersDone() , false );

  ActionParser ap3("DISCARD,00000011");
  BOOST_CHECK_EQUAL(pMD->ExecuteAction("ManualDiscard",ogi,"Player1",ap3) , false);
  BOOST_CHECK_EQUAL( top.GetOutput() , 
                     "unicast: Player1 -- ERROR,You need to discard more to get to your hand limit.\n" );
  top.ResetOutput();

  // these things should not change if we get do a bad result.
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetCommodityHand().GetHandDescription(true,true) , 
                     "|TI22-TI21-TI20-TI19-TI18-TI17-TI16-TI15|");
  BOOST_CHECK_EQUAL( ogi.GetCommodities().GetDeck(TITANIUM_COMMODITY).GetDiscardSize() , 0 );
  BOOST_CHECK_EQUAL( ogi.GetPlayers().AllPlayersDone() , false );

  ActionParser ap4("DISCARD,00000111");
  BOOST_CHECK_EQUAL(pMD->ExecuteAction("ManualDiscard",ogi,"Player1",ap4) , true);
  BOOST_CHECK_EQUAL( top.GetOutput() , 
                     "broadcast: SPENDS,0,0,2,0,3,|TI17-TI16-TI15|\n"
                     "broadcast: PLAYERCARDBACKS,0,|TI*-TI*-TI*-TI*-TI*|\n"
                     "unicast: Player1 -- PLAYERCARDS,|TI22-TI21-TI20-TI19-TI18|\n"
                     "broadcast: PLAYERSTATE,0,0,0,5,5,0,10,0,0,3,3,0,-1,-1\n"
                     "broadcast: COMMODITYSTOCK,1,26,0,0,3\n"
                     "broadcast: COMMODITYSTOCK,2,39,0,30,7\n"
                     "broadcast: COMMODITYSTOCK,3,53,3,44,10\n"
                     "broadcast: COMMODITYSTOCK,4,34,0,56,13\n"
                     "broadcast: COMMODITYSTOCK,5,16,0,0,17\n"
                     "broadcast: COMMODITYSTOCK,6,23,0,88,20\n"
                     "broadcast: COMMODITYSTOCK,7,14,0,0,30\n"
                     "broadcast: COMMODITYSTOCK,8,12,0,0,40\n"
                     "broadcast: COMMODITYSTOCK,9,12,0,0,50\n"
                     );
  
  top.ResetOutput();

  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetCommodityHand().GetHandDescription(true,true) , 
                     "|TI22-TI21-TI20-TI19-TI18|");
  BOOST_CHECK_EQUAL( ogi.GetCommodities().GetDeck(TITANIUM_COMMODITY).GetDiscardSize() , 3 );
  BOOST_CHECK_EQUAL( ogi.GetPlayers().AllPlayersDone() , true );

}


BOOST_AUTO_TEST_CASE( TransitionTestMANUALDISCARD2 )
{
  OutpostSet oset("");
  ServerGameInfo sgi("Outpost","testOutpost","XMLLOC","XMLFILE");

  std::vector<std::string> vcs;
  vcs.push_back("Player1");
  vcs.push_back("Player2");
  vcs.push_back("Player3");

  TestOutputPort top(vcs);

  OutpostStateMachine osm;
  OutpostGameInfo ogi(oset,sgi,top);

  ogi.GetPlayers().add("Player1");
  ogi.GetPlayers()[0].GetOwnedItems().AddItem(DATA_LIBRARY);
  ogi.GetPlayers().add("Player2");
  ogi.GetPlayers().DetermineTurnOrder(); // points to the first player
  ogi.GetPlayers().StartTurnOrder();

  const TransitionType *pMD = GetTransition(osm,"ManualDiscard","DISCARD","DoDiscard",1);
  // always false.
  BOOST_CHECK_EQUAL(pMD->IsAuto("ManualDiscard",ogi) , false );
  // allowed is true if it's the player in question.
  BOOST_CHECK_EQUAL(pMD->IsLegal("Player1","ManualDiscard",ogi) , true);
  BOOST_CHECK_EQUAL(pMD->IsLegal("Player2","ManualDiscard",ogi) , false);

  ogi.GetPlayers()[0].GetCommodityHand().AddCommodity(Commodity(TITANIUM_COMMODITY,15,false,false));
  ogi.GetPlayers()[0].GetCommodityHand().AddCommodity(Commodity(TITANIUM_COMMODITY,16,false,false));
  ogi.GetPlayers()[0].GetCommodityHand().AddCommodity(Commodity(TITANIUM_COMMODITY,17,false,false));
  ogi.GetPlayers()[0].GetCommodityHand().AddCommodity(Commodity(TITANIUM_COMMODITY,18,false,false));
  ogi.GetPlayers()[0].GetCommodityHand().AddCommodity(Commodity(TITANIUM_COMMODITY,19,false,false));
  ogi.GetPlayers()[0].GetCommodityHand().AddCommodity(Commodity(TITANIUM_COMMODITY,20,false,false));
  ogi.GetPlayers()[0].GetCommodityHand().AddCommodity(Commodity(TITANIUM_COMMODITY,21,false,false));
  ogi.GetPlayers()[0].GetCommodityHand().AddCommodity(Commodity(TITANIUM_COMMODITY,22,false,false));

  // these things should not change if we get do a bad result.
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetCommodityHand().GetHandDescription(true,true) , 
                     "|TI22-TI21-TI20-TI19-TI18-TI17-TI16-TI15|");
  BOOST_CHECK_EQUAL( ogi.GetCommodities().GetDeck(TITANIUM_COMMODITY).GetDiscardSize() , 0 );
  BOOST_CHECK_EQUAL( ogi.GetPlayers().AllPlayersDone() , false );

  ActionParser ap("DISCARD,001100100");
  BOOST_CHECK_EQUAL(pMD->ExecuteAction("ManualDiscard",ogi,"Player1",ap) , false);
  BOOST_CHECK_EQUAL( top.GetOutput() , "unicast: Player1 -- ERROR,Mismatched delete string length\n" );
  top.ResetOutput();

  // these things should not change if we get do a bad result.
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetCommodityHand().GetHandDescription(true,true) , 
                     "|TI22-TI21-TI20-TI19-TI18-TI17-TI16-TI15|");
  BOOST_CHECK_EQUAL( ogi.GetCommodities().GetDeck(TITANIUM_COMMODITY).GetDiscardSize() , 0 );
  BOOST_CHECK_EQUAL( ogi.GetPlayers().AllPlayersDone() , false );

  ActionParser ap2("DISCARD,001B0000");
  BOOST_CHECK_EQUAL(pMD->ExecuteAction("ManualDiscard",ogi,"Player1",ap2) , false);
  BOOST_CHECK_EQUAL( top.GetOutput() , "unicast: Player1 -- ERROR,Illegal characters in delete string\n" );
  top.ResetOutput();

  // these things should not change if we get do a bad result.
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetCommodityHand().GetHandDescription(true,true) , 
                     "|TI22-TI21-TI20-TI19-TI18-TI17-TI16-TI15|");
  BOOST_CHECK_EQUAL( ogi.GetCommodities().GetDeck(TITANIUM_COMMODITY).GetDiscardSize() , 0 );
  BOOST_CHECK_EQUAL( ogi.GetPlayers().AllPlayersDone() , false );

  ActionParser ap3("DISCARD,00000011");
  BOOST_CHECK_EQUAL(pMD->ExecuteAction("ManualDiscard",ogi,"Player1",ap3) , false);
  BOOST_CHECK_EQUAL( top.GetOutput() , 
                     "unicast: Player1 -- ERROR,You need to discard more to get to your hand limit.\n" );
  top.ResetOutput();

  // these things should not change if we get do a bad result.
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetCommodityHand().GetHandDescription(true,true) , 
                     "|TI22-TI21-TI20-TI19-TI18-TI17-TI16-TI15|");
  BOOST_CHECK_EQUAL( ogi.GetCommodities().GetDeck(TITANIUM_COMMODITY).GetDiscardSize() , 0 );
  BOOST_CHECK_EQUAL( ogi.GetPlayers().AllPlayersDone() , false );

  BOOST_CHECK_EQUAL( ogi.GetPlayers().GetTurnOrder()[0] , 0);
  BOOST_CHECK_EQUAL( ogi.GetPlayers().GetTurnOrder()[1] , 1);

  BOOST_CHECK_EQUAL( ogi.GetPlayers().GetCurTurnPlayer().GetId() , 0);


  ActionParser ap4("DISCARD,00000111");
  BOOST_CHECK_EQUAL(pMD->ExecuteAction("ManualDiscard",ogi,"Player1",ap4) , true);
  BOOST_CHECK_EQUAL( top.GetOutput() , 
                     "broadcast: SPENDS,0,0,2,0,3,|TI17-TI16-TI15|\n"
                     "broadcast: PLAYERCARDBACKS,0,|TI*-TI*-TI*-TI*-TI*|\n"
                     "unicast: Player1 -- PLAYERCARDS,|TI22-TI21-TI20-TI19-TI18|\n"
                     "broadcast: PLAYERSTATE,0,1,15,5,5,0,10,0,0,3,3,0,-1,-1\n"
                     "broadcast: PLAYERSTATE,1,0,0,5,5,0,10,0,0,1,3,0,-1,-1\n"
                     "broadcast: COMMODITYSTOCK,1,26,0,0,3\n"
                     "broadcast: COMMODITYSTOCK,2,39,0,30,7\n"
                     "broadcast: COMMODITYSTOCK,3,53,3,44,10\n"
                     "broadcast: COMMODITYSTOCK,4,34,0,56,13\n"
                     "broadcast: COMMODITYSTOCK,5,16,0,0,17\n"
                     "broadcast: COMMODITYSTOCK,6,23,0,88,20\n"
                     "broadcast: COMMODITYSTOCK,7,14,0,0,30\n"
                     "broadcast: COMMODITYSTOCK,8,12,0,0,40\n"
                     "broadcast: COMMODITYSTOCK,9,12,0,0,50\n"
                     );
  
  top.ResetOutput();

  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetCommodityHand().GetHandDescription(true,true) , 
                     "|TI22-TI21-TI20-TI19-TI18|");
  BOOST_CHECK_EQUAL( ogi.GetCommodities().GetDeck(TITANIUM_COMMODITY).GetDiscardSize() , 3 );
  BOOST_CHECK_EQUAL( ogi.GetPlayers().AllPlayersDone() , false );

}



BOOST_AUTO_TEST_CASE( TransitionTestSTARTMANNING )
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
  ogi.GetPlayers()[5].SetPurchaseState(INTERNAL_FINAL_DONE);

  const TransitionType *pMD = GetTransition(osm,"StartFinalManning","STARTMANNING","FinalManning",1);

  BOOST_CHECK_EQUAL( ogi.GetPlayers().GetCurTurnPlayer().GetId() , 4);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetPurchaseState() , INTERNALIZING);

  BOOST_CHECK_EQUAL(pMD->IsAuto("StartFinalManning",ogi) , true );
  BOOST_CHECK_EQUAL(pMD->IsLegal("Player1","StartFinalManning",ogi) , false);

  BOOST_CHECK_EQUAL(pMD->ExecuteAction("StartFinalManning",ogi,"",ap) , true);

  BOOST_CHECK_EQUAL( ogi.GetPlayers().GetCurTurnPlayer().GetId() , 4);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetPurchaseState() , UNPLAYED);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetPurchaseState() , ACTIVE);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetPurchaseState() , INTERNAL_FINAL_DONE);

  // PLAYERSTATE: plid,vp,costsum,handlimt,poplimit,mancost,#hum,#rob,purstate,bidstate,haspurchased
  BOOST_CHECK_EQUAL( top.GetOutput() , 
                     "broadcast: PLAYERSTATE,0,3,65,8,5,0,10,0,0,0,3,0,-1,-1\n"
                     "broadcast: PLAYERSTATE,1,4,65,5,8,0,10,0,0,0,3,0,-1,-1\n"
                     "broadcast: PLAYERSTATE,2,5,90,5,5,-1,10,0,0,0,3,0,-1,-1\n"
                     "broadcast: PLAYERSTATE,3,8,100,5,5,0,5,0,0,0,3,0,-1,-1\n"
                     "broadcast: PLAYERSTATE,4,8,150,8,10,-1,10,0,0,1,3,1,-1,-1\n"
                     "broadcast: PLAYERSTATE,5,10,200,11,15,0,10,0,0,6,3,0,-1,-1\n"
                     );
}


BOOST_AUTO_TEST_CASE( TransitionTestDONEMANNING )
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

  // cyclic on FinalManning
  const TransitionType *pMD = GetTransition(osm,"FinalManning","DONEMANNING","FinalManning",4);

  BOOST_CHECK_EQUAL( ogi.GetPlayers().GetCurTurnPlayer().GetId() , 4);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetPurchaseState() , ACTIVE);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetPurchaseState() , UNPLAYED);

  BOOST_CHECK_EQUAL(pMD->IsAuto("FinalManning",ogi) , false );
  BOOST_CHECK_EQUAL(pMD->IsLegal("Player9","FinalManning",ogi) , false); // has to be a player
  BOOST_CHECK_EQUAL(pMD->IsLegal("Player1","FinalManning",ogi) , false); // and in fact, the active player
  BOOST_CHECK_EQUAL(pMD->IsLegal("Player4","FinalManning",ogi) , true); // and in fact, the active player

  // new special case.  if the number of people the player has is greater than the number
  // of people he can have (case arises from unmanning a mannable item), DONEMANNING is not legal
  // until the player unmans factories to where the number of unused men is at least the difference
  // between #hum-poplimit, and if there is a limit to the number of robots they can have and the removal
  // of said men gives them more robots than their limit, that must be appropriately altered too.

  ogi.GetOptions().SetRobotMechanism(SECOND_UNLIMITED); // this gives us a limited # of robots
  // player 4 has outpost and robots, so # people = 10 and # robots = 10
  // we'll need 23 factories for this test, including a mannable.
  
  int i;
  for (i = 0 ; i < 22 ; ++i)
  {
    ogi.GetPlayers()[4].GetFactories().AddFactory(WATER_FACTORY);
  }
  ogi.GetPlayers()[4].GetFactories().AddFactory(MOON_BASE_FACTORY);

  BOOST_CHECK_EQUAL(ogi.GetPlayers()[4].GetFactories().GetFactoryDescription(),
                    "|WU-WU-WU-WU-WU-WU-WU-WU-WU-WU-WU-WU-WU-WU-WU-WU-WU-WU-WU-WU-WU-WU-MU|");

  ogi.GetPlayers()[4].AddHumans(10);
  ogi.GetPlayers()[4].AddRobots(10);
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[4].GetFactories().AlterManning("UUUBBBBBBBBBBHHHHHHHHHH",10,10),0);

  BOOST_CHECK_EQUAL(ogi.GetPlayers()[4].GetFactories().GetFactoryDescription(),
                    "|WU-WU-WU-WB-WB-WB-WB-WB-WB-WB-WB-WB-WB-WH-WH-WH-WH-WH-WH-WH-WH-WH-MH|");

  ogi.GetPlayers()[4].AddHumans(1);
  ogi.GetPlayers()[4].AddRobots(1);
  
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[4].GetFactories().AlterManning("UHBBBBBBBBBBBHHHHHHHHHH",11,11),0);

  BOOST_CHECK_EQUAL(ogi.GetPlayers()[4].GetFactories().GetFactoryDescription(),
                    "|WU-WH-WB-WB-WB-WB-WB-WB-WB-WB-WB-WB-WB-WH-WH-WH-WH-WH-WH-WH-WH-WH-MH|");

  // now, our men and robots are only legal because of our manning the mannable.
  BOOST_CHECK_EQUAL(pMD->IsLegal("Player4","FinalManning",ogi) , true); 
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[4].GetFactories().AlterManning("UHBBBBBBBBBBHHHHHHHHHHB",11,11),0);
  BOOST_CHECK_EQUAL(pMD->IsLegal("Player4","FinalManning",ogi) , false); // above our limit on both
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[4].GetFactories().AlterManning("UUBBBBBBBBBBHHHHHHHHHHB",11,11),0);
  BOOST_CHECK_EQUAL(pMD->IsLegal("Player4","FinalManning",ogi) , false); // above our limit on robots
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[4].GetFactories().AlterManning("UUUBBBBBBBBBHHHHHHHHHHB",11,11),0);
  BOOST_CHECK_EQUAL(pMD->IsLegal("Player4","FinalManning",ogi) , true); // should be ok now

  BOOST_CHECK_EQUAL(pMD->ExecuteAction("FinalManning",ogi,"Player4",ap) , true);

  BOOST_CHECK_EQUAL( ogi.GetPlayers().GetCurTurnPlayer().GetId() , 3);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetPurchaseState() , ACTIVE);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetPurchaseState() , FINAL_DONE);

  // PLAYERSTATE: plid,vp,costsum,handlimt,poplimit,robotlimit,mancost,#hum,#rob,purstate,bidstate,haspurchased,highbid,lastbid
  // given the above machinations, we should see that the # of robots and humans get adjusted down
  BOOST_CHECK_EQUAL( top.GetOutput() , 
                     "broadcast: PLAYERSTATE,4,47,150,8,10,10,10,10,10,5,3,0,-1,-1\n"
                     "broadcast: PLAYERSTATE,3,8,100,5,5,0,5,0,0,1,3,0,-1,-1\n"
                     );
  top.ResetOutput();

  
  ogi.GetPlayers().IncrementTurnOrder();
  ogi.GetPlayers().IncrementTurnOrder();
  ogi.GetPlayers().IncrementTurnOrder();

  BOOST_CHECK_EQUAL(pMD->ExecuteAction("FinalManning",ogi,"Player0",ap) , true);
  BOOST_CHECK_EQUAL( ogi.GetPlayers().AllPlayersDone() , true );

  BOOST_CHECK_EQUAL( top.GetOutput() , 
                     "broadcast: PLAYERSTATE,0,3,65,8,5,0,10,0,0,5,3,0,-1,-1\n"
                     );

  
  BOOST_CHECK_EQUAL(pMD->IsLegal("Player5","FinalManning",ogi) , false); // needs to execute, return false, even after all players done


}


BOOST_AUTO_TEST_CASE( TransitionTestENDMANNING )
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
  ogi.GetPlayers().IncrementTurnOrder();
  ogi.GetPlayers().IncrementTurnOrder();
  ogi.GetPlayers().IncrementTurnOrder();
  ogi.GetPlayers().IncrementTurnOrder();

  const TransitionType *pMD = GetTransition(osm,"FinalManning","ENDMANNINGTODISCARD","StartDiscard",4);
  const TransitionType *pME = GetTransition(osm,"FinalManning","ENDMANNINGTOTURNEND","TurnEnd",4);

  // neither of them are legal
  BOOST_CHECK_EQUAL(pMD->IsLegal("Player9","FinalManning",ogi) , false); // has to be a player?
  BOOST_CHECK_EQUAL(pMD->IsLegal("Player1","FinalManning",ogi) , false); 
  BOOST_CHECK_EQUAL(pMD->IsLegal("Player4","FinalManning",ogi) , false); 
  BOOST_CHECK_EQUAL(pMD->IsLegal("Player0","FinalManning",ogi) , false); // not even active player

  BOOST_CHECK_EQUAL(pME->IsLegal("Player9","FinalManning",ogi) , false); // has to be a player?
  BOOST_CHECK_EQUAL(pME->IsLegal("Player1","FinalManning",ogi) , false); 
  BOOST_CHECK_EQUAL(pME->IsLegal("Player4","FinalManning",ogi) , false); 
  BOOST_CHECK_EQUAL(pME->IsLegal("Player0","FinalManning",ogi) , false); // not even active player

  // neither of them are auto...for now.
  BOOST_CHECK_EQUAL(pMD->IsAuto("FinalManning",ogi) , false );
  BOOST_CHECK_EQUAL(pME->IsAuto("FinalManning",ogi) , false );

  ogi.GetPlayers().IncrementTurnOrder(); // this should make the last player done.

  // with earlydiscard not true, we auto to discard
  BOOST_CHECK_EQUAL(pMD->IsAuto("FinalManning",ogi) , true );
  BOOST_CHECK_EQUAL(pME->IsAuto("FinalManning",ogi) , false );

  ogi.GetOptions().SetEarlyDiscard(true);

  // with earlydiscard true, we auto to turn end
  BOOST_CHECK_EQUAL(pMD->IsAuto("FinalManning",ogi) , false );
  BOOST_CHECK_EQUAL(pME->IsAuto("FinalManning",ogi) , true );

  // neither of them does anything interesting.
  BOOST_CHECK_EQUAL(pMD->ExecuteAction("FinalManning",ogi,"",ap) , true);
  BOOST_CHECK_EQUAL( top.GetOutput() , "" );
  top.ResetOutput();

  BOOST_CHECK_EQUAL(pME->ExecuteAction("FinalManning",ogi,"",ap) , true);
  BOOST_CHECK_EQUAL( top.GetOutput() , "" );
  top.ResetOutput();
}

BOOST_AUTO_TEST_CASE( TransitionTestENDTURN )
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

  // std::set<size_t> GetPlayers().GetWinningPlayerIndices()
  const TransitionType *pEG = GetTransition(osm,"TurnEnd","GAMEOVER","TerminalState",2);
  const TransitionType *pNT = GetTransition(osm,"TurnEnd","NEWTURN","TurnOrder",2);

  // neither are ever legal
  BOOST_CHECK_EQUAL(pEG->IsLegal("Player9","TurnEnd",ogi) , false); // has to be a player?
  BOOST_CHECK_EQUAL(pEG->IsLegal("Player1","TurnEnd",ogi) , false); 
  BOOST_CHECK_EQUAL(pEG->IsLegal("Player4","TurnEnd",ogi) , false); 
  BOOST_CHECK_EQUAL(pEG->IsLegal("Player0","TurnEnd",ogi) , false); // not even active player

  BOOST_CHECK_EQUAL(pNT->IsLegal("Player9","TurnEnd",ogi) , false); // has to be a player?
  BOOST_CHECK_EQUAL(pNT->IsLegal("Player1","TurnEnd",ogi) , false); 
  BOOST_CHECK_EQUAL(pNT->IsLegal("Player4","TurnEnd",ogi) , false); 
  BOOST_CHECK_EQUAL(pNT->IsLegal("Player0","TurnEnd",ogi) , false); // not even active player

  // new turn is auto if no one has achieved 75 VP
  BOOST_CHECK_EQUAL(pEG->IsAuto("TurnEnd",ogi) , false );
  BOOST_CHECK_EQUAL(pNT->IsAuto("TurnEnd",ogi) , true );

  ogi.SetCurTurn(5);
  BOOST_CHECK_EQUAL(ogi.GetCurTurn() , 5);

  // new thing to happen at turn end... all players cards should age.
  ogi.GetPlayers()[0].GetCommodityHand().AddCommodity(Commodity(RESEARCH_COMMODITY,17,false,false));
  ogi.GetPlayers()[3].GetCommodityHand().AddCommodity(Commodity(MOON_ORE_COMMODITY,40,false,true));
  ogi.GetPlayers()[3].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,3,false,false));
  ogi.GetPlayers()[5].GetCommodityHand().AddCommodity(Commodity(NEW_CHEMICALS_COMMODITY,88,true,false));

  BOOST_CHECK_EQUAL(ogi.GetPlayers()[0].GetCommodityHand().GetHand()[0].IsNew() , true);
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[3].GetCommodityHand().GetHand()[0].IsNew() , true);
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[3].GetCommodityHand().GetHand()[1].IsNew() , true);
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[5].GetCommodityHand().GetHand()[0].IsNew() , true);

  ogi.GetPlayers()[0].SetLastBid(15);
  ogi.GetPlayers()[3].SetLastBid(20);
  ogi.GetPlayers()[5].SetLastBid(24);

  BOOST_CHECK_EQUAL(pNT->ExecuteAction("TurnEnd",ogi,"",ap) , true);

  BOOST_CHECK_EQUAL(ogi.GetPlayers()[0].GetLastBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[3].GetLastBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[5].GetLastBid() , NOT_HIGH_BID );

  BOOST_CHECK_EQUAL(ogi.GetPlayers()[0].GetCommodityHand().GetHand()[0].IsNew() , false);
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[3].GetCommodityHand().GetHand()[0].IsNew() , false);
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[3].GetCommodityHand().GetHand()[1].IsNew() , false);
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[5].GetCommodityHand().GetHand()[0].IsNew() , false);

  BOOST_CHECK_EQUAL( top.GetOutput() , "broadcast: CURTURN,6\n" );
  BOOST_CHECK_EQUAL(ogi.GetCurTurn() , 6);
  top.ResetOutput();

  // let's give a player some VP.
  ogi.GetPlayers()[2].GetFactories().AddFactory(MOON_BASE_FACTORY);
  ogi.GetPlayers()[2].GetFactories().AddFactory(MOON_BASE_FACTORY);
  ogi.GetPlayers()[2].GetFactories().AddFactory(MOON_BASE_FACTORY);
  ogi.GetPlayers()[2].GetFactories().AddFactory(SPACE_STATION_FACTORY); // 20 + 20 + 20 + 10 + 5 (from above = 75!)
  ogi.GetPlayers()[2].GetFactories().AlterManning("HHHH",4,0);

  // game end is auto if someone has achieved 75 VP
  BOOST_CHECK_EQUAL(pEG->IsAuto("TurnEnd",ogi) , true );
  BOOST_CHECK_EQUAL(pNT->IsAuto("TurnEnd",ogi) , false );
  
  BOOST_CHECK_EQUAL(pEG->ExecuteAction("TurnEnd",ogi,"",ap) , true);
  BOOST_CHECK_EQUAL( top.GetOutput() , "broadcast: PLAYERWINS,2\n" );
  top.ResetOutput();

  ogi.GetPlayers()[5].GetFactories().AddFactory(MOON_BASE_FACTORY);
  ogi.GetPlayers()[5].GetFactories().AddFactory(MOON_BASE_FACTORY);
  ogi.GetPlayers()[5].GetFactories().AddFactory(MOON_BASE_FACTORY);
  ogi.GetPlayers()[5].GetFactories().AlterManning("HHH",4,0);
  ogi.GetPlayers()[5].GetOwnedItems().AddItem(ECOPLANTS); // should also be 75 VP

  // game end is auto if someone has achieved 75 VP
  BOOST_CHECK_EQUAL(pEG->IsAuto("TurnEnd",ogi) , true );
  BOOST_CHECK_EQUAL(pNT->IsAuto("TurnEnd",ogi) , false );
  
  BOOST_CHECK_EQUAL(pEG->ExecuteAction("TurnEnd",ogi,"",ap) , true);
  BOOST_CHECK_EQUAL( top.GetOutput() , 
                     "broadcast: PLAYERWINS,2\n" 
                     "broadcast: PLAYERWINS,5\n" 
                     );
  top.ResetOutput();

  // give someone more than 75....
  ogi.GetPlayers()[4].GetFactories().AddFactory(MOON_BASE_FACTORY);
  ogi.GetPlayers()[4].GetFactories().AddFactory(MOON_BASE_FACTORY);
  ogi.GetPlayers()[4].GetFactories().AddFactory(MOON_BASE_FACTORY);
  ogi.GetPlayers()[4].GetFactories().AddFactory(MOON_BASE_FACTORY);
  ogi.GetPlayers()[4].GetFactories().AlterManning("HHHH",4,0); // should have 88 now.

  // game end is auto if someone has achieved 75 VP
  BOOST_CHECK_EQUAL(pEG->IsAuto("TurnEnd",ogi) , true );
  BOOST_CHECK_EQUAL(pNT->IsAuto("TurnEnd",ogi) , false );
  
  BOOST_CHECK_EQUAL(pEG->ExecuteAction("TurnEnd",ogi,"",ap) , true);
  BOOST_CHECK_EQUAL( top.GetOutput() , 
                     "broadcast: PLAYERWINS,4\n" 
                     );
  top.ResetOutput();
}


// replaces all instances of ## with i_replace in i_basestring
std::string ReplacePlayerName(const std::string &i_replace,const std::string &i_basestring)
{
  std::string input = i_basestring;
  boost::algorithm::replace_all(input,"##",i_replace);
  return input;
}

void TestRefresh(const OutpostGameInfo &i_ogi,TestOutputPort &i_top,
                 const std::string &i_statename,const std::string &i_playername,
                 const std::set<std::string> &i_lines,const std::string &i_TestName)
{
  // error message
  std::ostringstream erross;
  erross << std::endl << "Refresh Test: " << i_TestName << std::endl;

  // special: must always be first:
  //   RESET 
  std::string rbase("unicast: ## -- RESET,Outpost");
  std::string testreset(ReplacePlayerName(i_playername,rbase));

  // Want my own map of test records.
  std::map<std::string,bool>::iterator mapit;
  std::set<std::string>::const_iterator setit;

  std::map<std::string,bool> testlines;
    
  for (setit = i_lines.begin() ; setit != i_lines.end() ; ++setit)
  {
    std::string newline = ReplacePlayerName(i_playername,*setit);
    BOOST_REQUIRE( testlines.find(newline) == testlines.end() );
    testlines[newline] = false;
  }

  // let's get the real data
  i_ogi.SendFullState(i_playername,i_statename);
  std::istringstream iss(i_top.GetOutput());

  // check the first line specially.
  std::string realreset;
  BOOST_REQUIRE(std::getline(iss,realreset));
  BOOST_REQUIRE_EQUAL(testreset,realreset);

  // create a map for the rest of the lines
  std::map<std::string,bool> reallines;
  std::string curline;

  while(std::getline(iss,curline))
  {
    BOOST_REQUIRE( reallines.find(curline) == reallines.end() );
    reallines[curline + "\n"] = false;
  }

  bool fail = false;

  for (mapit = reallines.begin() ; mapit != reallines.end() ; ++mapit)
  {
    if (testlines.find(mapit->first) == testlines.end()) 
    {
      fail = true;
    }
    else
    {
      mapit->second = true;
    }
  }

  for (mapit = testlines.begin() ; mapit != testlines.end() ; ++mapit)
  {
    if (reallines.find(mapit->first) == reallines.end()) 
    {
      fail = true;
    }
    else
    {
      mapit->second = true;
    }
  }

  erross << "--TEST----" << std::endl;
  for (mapit = testlines.begin() ; mapit != testlines.end() ; ++mapit)
  {
    erross << (mapit->second ? "   : " : "xxx: ");
    erross << mapit->first;
  }

  erross << "--REAL----" << std::endl;
  for (mapit = reallines.begin() ; mapit != reallines.end() ; ++mapit)
  {
    erross << (mapit->second ? "   : " : "xxx: ");
    erross << mapit->first;
  }
  erross << "---------" << std::endl;

  if (fail) BOOST_FAIL(erross.str());

  i_top.ResetOutput();

}



BOOST_AUTO_TEST_CASE( StateMachineRefresh )
{
  OutpostSet oset("");
  ServerGameInfo sgi("Outpost","testOutpost","XMLLOC","XMLFILE");
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
  
  ogi.GetOptions().SetEarlyDiscard(true);
  ogi.GetOptions().SetSupplyHarshness(4);
  ogi.GetOptions().SetResearchIsSmall(true);
  ogi.GetOptions().SetMicroIsSmall(true);
  ogi.GetOptions().SetBlindDraws(true);

  ogi.GetPlayers()[1].SetOkStartup(true);

  // always:
  //   PLAYER
  //   OPTIONS

  std::set<std::string> always;
  always.insert("unicast: ## -- PLAYER,Player0,0\n");
  always.insert("unicast: ## -- PLAYER,Player1,1\n");
  always.insert("unicast: ## -- PLAYER,Player2,2\n");
  always.insert("unicast: ## -- OPTIONS,1,0,0,0,5,1,0,1,1,1,1\n");

  ogi.SetCurTurn(3);
  ogi.GetPlayers()[0].GetOwnedItems().AddItem(OUTPOST);
  ogi.GetPlayers()[1].GetOwnedItems().AddItem(WAREHOUSE);
  ogi.GetPlayers()[1].GetOwnedItems().AddItem(NODULE);
  ogi.GetPlayers()[2].GetOwnedItems().AddItem(SCIENTISTS);
  ogi.GetPlayers()[2].GetOwnedItems().AddItem(SCIENTISTS);
  ogi.GetUnownedItems().Initialize(3);
  ogi.GetUnownedItems().DecrementItemCount(OUTPOST);
  ogi.GetUnownedItems().DecrementItemCount(WAREHOUSE);
  ogi.GetUnownedItems().DecrementItemCount(NODULE);
  ogi.GetUnownedItems().DecrementItemCount(SCIENTISTS);
  ogi.GetUnownedItems().DecrementItemCount(SCIENTISTS);
  ogi.GetUnownedItems().SetShipItem(2,MOON_BASE);
  ogi.GetUnownedItems().SetShipItem(0,ECOPLANTS);
  // to this point, VPs are: 0 has 5, 1 has 3, 2 has 4
  ogi.GetPlayers()[0].GetCommodityHand().AddCommodity(Commodity(WATER_COMMODITY,7,false,false));
  ogi.GetPlayers()[0].GetCommodityHand().AddCommodity(Commodity(MOON_ORE_COMMODITY,30,false,false));
  ogi.GetPlayers()[1].GetCommodityHand().AddCommodity(Commodity(RESEARCH_COMMODITY,38,false,true));
  ogi.GetPlayers()[1].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,3,false,false));
  ogi.GetPlayers()[2].GetCommodityHand().AddCommodity(Commodity(MICROBIOTICS_COMMODITY,18,true,false));
  // Player 2: |MI18A|
  // Player 1: |RE38M-OR3|
  // Player 0: |MO30-WA7|

  ogi.GetPlayers()[0].AddHumans(3);
  ogi.GetPlayers()[1].AddHumans(4);
  ogi.GetPlayers()[2].AddHumans(5);
  ogi.GetPlayers()[1].AddRobots(1);
  ogi.GetPlayers()[0].GetFactories().AddFactory(WATER_FACTORY);
  ogi.GetPlayers()[0].GetFactories().AddFactory(WATER_FACTORY);
  ogi.GetPlayers()[0].GetFactories().AddFactory(WATER_FACTORY);
  ogi.GetPlayers()[0].GetFactories().AddFactory(WATER_FACTORY);
  ogi.GetPlayers()[0].GetFactories().AddFactory(RESEARCH_FACTORY);
  ogi.GetPlayers()[0].GetFactories().AlterManning("HHHHH",5,0); // 6 more VP
  ogi.GetPlayers()[1].GetFactories().AddFactory(TITANIUM_FACTORY); // no manning, so no VP
  ogi.GetPlayers()[2].GetFactories().AddFactory(MOON_BASE_FACTORY);
  ogi.GetPlayers()[2].GetFactories().AddFactory(MOON_BASE_FACTORY);
  ogi.GetPlayers()[2].GetFactories().AddFactory(MOON_BASE_FACTORY);
  ogi.GetPlayers()[2].GetFactories().AddFactory(MOON_BASE_FACTORY);
  ogi.GetPlayers()[2].GetFactories().AddFactory(SPACE_STATION_FACTORY);
  ogi.GetPlayers()[2].GetFactories().AlterManning("HBBBB",1,4); // 90 VP, + 1 population
  // to this point, VPs are: 0 has 5+6, 1 has 3, 2 has 4+90
  ogi.GetPlayers().DetermineTurnOrder();
  ogi.GetPlayers().StartPurchasePhase(); // turn order: 2,0,1  active 2
  ogi.GetPlayers().StartBidPhase(false,false,10); // turn order: 2,0,1, 2 has bid 10, 0 is next to bid
  ogi.GetUnownedItems().SetBidIndex(2);

  ogi.GetUnownedItems().SetPurchased((size_t)2,ROBOTS,(size_t)0,75);

  ogi.GetCommodities().GetDeck(RESEARCH_COMMODITY).DrawCommodity(false);
  ogi.GetCommodities().GetDeck(RESEARCH_COMMODITY).DrawCommodity(false);
  ogi.GetCommodities().GetDeck(MOON_ORE_COMMODITY).DrawCommodity(false);
  ogi.GetCommodities().GetDeck(NEW_CHEMICALS_COMMODITY).DiscardCommodity(Commodity(NEW_CHEMICALS_COMMODITY,17,false,false));
  // research draw pile is 2 down, moon ore is 1 down, and newchem has one discard.

  // during normal game, public:
  //   CURTURN
  //   PLAYERITEMS
  //   ITEMSTOCK
  //   SUPPLYSHIP
  //   PLAYERCARDBACKS
  //   PLAYERFACTORIES
  //   TURNORDER
  //   PLAYERSTATE
  //   COMMODITYSTOCK

  std::set<std::string> normalpublic;
  normalpublic.insert("unicast: ## -- CURTURN,3\n");
  normalpublic.insert("unicast: ## -- PLAYERITEMS,0,0,10\n");
  normalpublic.insert("unicast: ## -- PLAYERITEMS,1,0,2\n");
  normalpublic.insert("unicast: ## -- PLAYERITEMS,1,1,4\n");
  normalpublic.insert("unicast: ## -- PLAYERITEMS,2,0,5\n");
  normalpublic.insert("unicast: ## -- PLAYERITEMS,2,1,5\n");
  normalpublic.insert("unicast: ## -- ITEMSTOCK,1,2\n");
  normalpublic.insert("unicast: ## -- ITEMSTOCK,2,1\n");
  normalpublic.insert("unicast: ## -- ITEMSTOCK,3,2\n");
  normalpublic.insert("unicast: ## -- ITEMSTOCK,4,1\n");
  normalpublic.insert("unicast: ## -- ITEMSTOCK,5,0\n");
  normalpublic.insert("unicast: ## -- ITEMSTOCK,6,2\n");
  normalpublic.insert("unicast: ## -- ITEMSTOCK,7,2\n");
  normalpublic.insert("unicast: ## -- ITEMSTOCK,8,2\n");
  normalpublic.insert("unicast: ## -- ITEMSTOCK,9,2\n");
  normalpublic.insert("unicast: ## -- ITEMSTOCK,10,1\n");
  normalpublic.insert("unicast: ## -- ITEMSTOCK,11,2\n");
  normalpublic.insert("unicast: ## -- ITEMSTOCK,12,2\n");
  normalpublic.insert("unicast: ## -- ITEMSTOCK,13,2\n");
  normalpublic.insert("unicast: ## -- SUPPLYSHIP,0,9\n");
  normalpublic.insert("unicast: ## -- SUPPLYSHIP,1,0\n");
  normalpublic.insert("unicast: ## -- SUPPLYSHIP,2,13\n");
  normalpublic.insert("unicast: ## -- PLAYERFACTORIES,0,|WH-WH-WH-WH-RH|\n");
  normalpublic.insert("unicast: ## -- PLAYERFACTORIES,1,|TU|\n");
  normalpublic.insert("unicast: ## -- PLAYERFACTORIES,2,|SH-MB-MB-MB-MB|\n");
  normalpublic.insert("unicast: ## -- TURNORDER,0,2\n");
  normalpublic.insert("unicast: ## -- TURNORDER,1,3\n");
  normalpublic.insert("unicast: ## -- TURNORDER,2,1\n");
  normalpublic.insert("unicast: ## -- PURCHASEDATA,0,3000,0,-1\n");
  normalpublic.insert("unicast: ## -- PURCHASEDATA,1,3000,0,-1\n");
  normalpublic.insert("unicast: ## -- PURCHASEDATA,2,0,7,75\n");

  std::set<std::string> cardset1;
  cardset1.insert("unicast: ## -- PLAYERCARDBACKS,0,|MO*-WA*|\n");
  cardset1.insert("unicast: ## -- PLAYERCARDBACKS,1,|RE38M-OR*|\n");
  cardset1.insert("unicast: ## -- PLAYERCARDBACKS,2,|MI18A|\n");
  cardset1.insert("unicast: ## -- PLAYERSTATE,0,11,100,13,10,0,10,3,0,0,1,0,-1,-1\n");
  cardset1.insert("unicast: ## -- PLAYERSTATE,1,3,50,13,8,0,10,4,1,0,0,0,-1,-1\n");
  cardset1.insert("unicast: ## -- PLAYERSTATE,2,94,80,10,6,0,10,5,0,1,4,0,10,-1\n");
  cardset1.insert("unicast: ## -- COMMODITYSTOCK,1,26,0,0,3\n");
  cardset1.insert("unicast: ## -- COMMODITYSTOCK,2,39,0,30,7\n");
  cardset1.insert("unicast: ## -- COMMODITYSTOCK,3,53,0,44,10\n");
  cardset1.insert("unicast: ## -- COMMODITYSTOCK,4,32,0,56,13\n");
  cardset1.insert("unicast: ## -- COMMODITYSTOCK,5,16,0,0,17\n");
  cardset1.insert("unicast: ## -- COMMODITYSTOCK,6,23,1,88,20\n");
  cardset1.insert("unicast: ## -- COMMODITYSTOCK,7,14,0,0,30\n");
  cardset1.insert("unicast: ## -- COMMODITYSTOCK,8,12,0,0,40\n");
  cardset1.insert("unicast: ## -- COMMODITYSTOCK,9,11,0,0,50\n");

  std::set<std::string> cardset2;
  cardset2.insert("unicast: ## -- PLAYERCARDBACKS,0,|MO*-RE*-WA*|\n");
  cardset2.insert("unicast: ## -- PLAYERCARDBACKS,1,|RE38M-OR*|\n");
  cardset2.insert("unicast: ## -- PLAYERCARDBACKS,2,|MO*-MO*-MO*-MO*-OM*-MI18A-RE*-RE*|\n");
  cardset2.insert("unicast: ## -- PLAYERSTATE,0,11,100,13,10,0,10,3,0,1,1,0,-1,-1\n");
  cardset2.insert("unicast: ## -- PLAYERSTATE,1,3,50,13,8,0,10,4,1,0,0,0,-1,-1\n");
  cardset2.insert("unicast: ## -- PLAYERSTATE,2,94,80,10,6,0,10,5,0,3,4,0,10,-1\n");
  cardset2.insert("unicast: ## -- COMMODITYSTOCK,1,26,0,0,3\n");
  cardset2.insert("unicast: ## -- COMMODITYSTOCK,2,39,0,30,7\n");
  cardset2.insert("unicast: ## -- COMMODITYSTOCK,3,53,0,44,10\n");
  cardset2.insert("unicast: ## -- COMMODITYSTOCK,4,29,0,56,13\n");
  cardset2.insert("unicast: ## -- COMMODITYSTOCK,5,16,0,0,17\n");
  cardset2.insert("unicast: ## -- COMMODITYSTOCK,6,23,1,88,20\n");
  cardset2.insert("unicast: ## -- COMMODITYSTOCK,7,13,0,0,30\n");
  cardset2.insert("unicast: ## -- COMMODITYSTOCK,8,12,0,0,40\n");
  cardset2.insert("unicast: ## -- COMMODITYSTOCK,9,7,0,0,50\n");



  // private: (only sent to a particular player)
  //   PLAYERCARDS
  //   PLAYERPURCHASEINFO

  // rob,men,ore,water,titan,res,newchem
  // 37, 0 research, 3/8 H 0/0 R
  std::set<std::string> player0private;
  player0private.insert("unicast: Player0 -- PLAYERCARDS,|MO30-WA7|\n");
  player0private.insert("unicast: Player0 -- PLAYERPURCHASEINFO,0,3,3,1,0,0,0\n");

  // 41, 4 research 4/8 H 1/0 R
  std::set<std::string> player1private;
  player1private.insert("unicast: Player1 -- PLAYERCARDS,|RE38M-OR3|\n");
  player1private.insert("unicast: Player1 -- PLAYERPURCHASEINFO,0,4,4,2,0,0,0\n");

  // 18, 0 research 5/5 H 0/0 R
  std::set<std::string> player2private;
  player2private.insert("unicast: Player2 -- PLAYERCARDS,|MI18A|\n");
  player2private.insert("unicast: Player2 -- PLAYERPURCHASEINFO,0,1,1,0,0,0,0\n");



  
  std::set<std::string> player0initialprivate;
  std::set<std::string> player1initialprivate;
  std::set<std::string> player2initialprivate;
  player0initialprivate.insert("unicast: Player0 -- IAM,0\n");
  player1initialprivate.insert("unicast: Player1 -- IAM,1\n");
  player2initialprivate.insert("unicast: Player2 -- IAM,2\n");




  // specials:
  //   PLAYEROKSTARTUP (only in ValidateSetup)
  std::set<std::string> vsetup;
  vsetup.insert("unicast: ## -- PLAYEROKSTARTUP,0,0\n");
  vsetup.insert("unicast: ## -- PLAYEROKSTARTUP,1,1\n");
  vsetup.insert("unicast: ## -- PLAYEROKSTARTUP,2,0\n");

  //   BIDITEM (only in Bid)
  std::set<std::string> biditemstate;
  biditemstate.insert("unicast: ## -- BIDITEM,2\n");
  biditemstate.insert("unicast: ## -- PURCHASEPRICE,10\n");

  //   PLAYERWINS (only in TerminalState)
  std::set<std::string> terminalstate;
  terminalstate.insert("unicast: ## -- PLAYERWINS,2\n");

  //   MEGAREQUEST (only in MegaResources)
  std::set<std::string> requestmega;
  requestmega.insert("unicast: Player0 -- MEGAREQUEST,2,1\n");

  // non-player, initial state
  TestRefresh(ogi,top,"InitialState","Player9",always,"Initial State Non-Player");

  std::set<std::string> initial2;
  initial2.insert(always.begin(),always.end());
  initial2.insert(player2initialprivate.begin(),player2initialprivate.end());

  // player, initial state
  TestRefresh(ogi,top,"InitialState","Player2",initial2,"Initial State, Player 2");

  std::set<std::string> validateset;
  validateset.insert(always.begin(),always.end());
  validateset.insert(vsetup.begin(),vsetup.end());

  // non-player, 
  TestRefresh(ogi,top,"ValidateSetup","Player9",validateset,"Validate Setup Non-Player");

  validateset.insert(player2initialprivate.begin(),player2initialprivate.end());

  // player, 2
  TestRefresh(ogi,top,"ValidateSetup","Player2",validateset,"Validate Setup Player 2");

  // normal part of the game.
  std::set<std::string> normalset;
  normalset.insert(always.begin(),always.end());
  normalset.insert(normalpublic.begin(),normalpublic.end());
  normalset.insert(cardset1.begin(),cardset1.end());

  // non-player, 
  TestRefresh(ogi,top,"Purchase","Player9",normalset,"Normal non-player");

  // a player will see all those, plus their own stuff.
  normalset.insert(player1private.begin(),player1private.end());
  normalset.insert(player1initialprivate.begin(),player1initialprivate.end());
  TestRefresh(ogi,top,"Purchase","Player1",normalset,"Normal Player 1");
  
  // in Bid state
  std::set<std::string> bidstateset;
  bidstateset.insert(always.begin(),always.end());
  bidstateset.insert(normalpublic.begin(),normalpublic.end());
  bidstateset.insert(biditemstate.begin(),biditemstate.end());
  bidstateset.insert(cardset1.begin(),cardset1.end());

  TestRefresh(ogi,top,"Bid","Player9",bidstateset,"Bid state, Non-player");
  bidstateset.insert(player2private.begin(),player2private.end());
  bidstateset.insert(player2initialprivate.begin(),player2initialprivate.end());
  TestRefresh(ogi,top,"Bid","Player2",bidstateset,"Bid State, Player 2");

  // terminalstate (TerminalState)
  std::set<std::string> terminalstateset;
  terminalstateset.insert(always.begin(),always.end());
  terminalstateset.insert(normalpublic.begin(),normalpublic.end());
  terminalstateset.insert(terminalstate.begin(),terminalstate.end());
  terminalstateset.insert(cardset1.begin(),cardset1.end());

  TestRefresh(ogi,top,"TerminalState","Player9",terminalstateset,"Terminal state, non-player");
  terminalstateset.insert(player0private.begin(),player0private.end());
  terminalstateset.insert(player0initialprivate.begin(),player0initialprivate.end());
  TestRefresh(ogi,top,"TerminalState","Player0",terminalstateset,"Terminal state, player 0");

  BOOST_CHECK_EQUAL(ogi.GetCommodities().GetDeck(ORE_COMMODITY).GetDeckSize() , 26);
  BOOST_CHECK_EQUAL(ogi.GetCommodities().GetDeck(ORE_COMMODITY).GetDiscardSize() , 0);
  BOOST_CHECK_EQUAL(ogi.GetCommodities().GetDeck(WATER_COMMODITY).GetDeckSize() , 39);
  BOOST_CHECK_EQUAL(ogi.GetCommodities().GetDeck(WATER_COMMODITY).GetDiscardSize() , 0);
  BOOST_CHECK_EQUAL(ogi.GetCommodities().GetDeck(TITANIUM_COMMODITY).GetDeckSize() , 53);
  BOOST_CHECK_EQUAL(ogi.GetCommodities().GetDeck(TITANIUM_COMMODITY).GetDiscardSize() , 0);
  BOOST_CHECK_EQUAL(ogi.GetCommodities().GetDeck(RESEARCH_COMMODITY).GetDeckSize() , 32);
  BOOST_CHECK_EQUAL(ogi.GetCommodities().GetDeck(RESEARCH_COMMODITY).GetDiscardSize() , 0);
  BOOST_CHECK_EQUAL(ogi.GetCommodities().GetDeck(MICROBIOTICS_COMMODITY).GetDeckSize() , 16);
  BOOST_CHECK_EQUAL(ogi.GetCommodities().GetDeck(MICROBIOTICS_COMMODITY).GetDiscardSize() , 0);
  BOOST_CHECK_EQUAL(ogi.GetCommodities().GetDeck(NEW_CHEMICALS_COMMODITY).GetDeckSize() , 23);
  BOOST_CHECK_EQUAL(ogi.GetCommodities().GetDeck(NEW_CHEMICALS_COMMODITY).GetDiscardSize() , 1);
  BOOST_CHECK_EQUAL(ogi.GetCommodities().GetDeck(ORBITAL_MEDICINE_COMMODITY).GetDeckSize() , 14);
  BOOST_CHECK_EQUAL(ogi.GetCommodities().GetDeck(ORBITAL_MEDICINE_COMMODITY).GetDiscardSize() , 0);
  BOOST_CHECK_EQUAL(ogi.GetCommodities().GetDeck(RING_ORE_COMMODITY).GetDeckSize() , 12);
  BOOST_CHECK_EQUAL(ogi.GetCommodities().GetDeck(RING_ORE_COMMODITY).GetDiscardSize() , 0);
  BOOST_CHECK_EQUAL(ogi.GetCommodities().GetDeck(MOON_ORE_COMMODITY).GetDeckSize() , 11);
  BOOST_CHECK_EQUAL(ogi.GetCommodities().GetDeck(MOON_ORE_COMMODITY).GetDiscardSize() , 0);


  ogi.GetProductionManager().StartProduction(ogi.GetPlayers(),ogi.GetCommodities(),false,false);

  BOOST_CHECK_EQUAL(ogi.GetCommodities().GetDeck(ORE_COMMODITY).GetDeckSize() , 26);
  BOOST_CHECK_EQUAL(ogi.GetCommodities().GetDeck(ORE_COMMODITY).GetDiscardSize() , 0);
  BOOST_CHECK_EQUAL(ogi.GetCommodities().GetDeck(WATER_COMMODITY).GetDeckSize() , 39);
  BOOST_CHECK_EQUAL(ogi.GetCommodities().GetDeck(WATER_COMMODITY).GetDiscardSize() , 0);
  BOOST_CHECK_EQUAL(ogi.GetCommodities().GetDeck(TITANIUM_COMMODITY).GetDeckSize() , 53);
  BOOST_CHECK_EQUAL(ogi.GetCommodities().GetDeck(TITANIUM_COMMODITY).GetDiscardSize() , 0);
  BOOST_CHECK_EQUAL(ogi.GetCommodities().GetDeck(RESEARCH_COMMODITY).GetDeckSize() , 29);
  BOOST_CHECK_EQUAL(ogi.GetCommodities().GetDeck(RESEARCH_COMMODITY).GetDiscardSize() , 0);
  BOOST_CHECK_EQUAL(ogi.GetCommodities().GetDeck(MICROBIOTICS_COMMODITY).GetDeckSize() , 16);
  BOOST_CHECK_EQUAL(ogi.GetCommodities().GetDeck(MICROBIOTICS_COMMODITY).GetDiscardSize() , 0);
  BOOST_CHECK_EQUAL(ogi.GetCommodities().GetDeck(NEW_CHEMICALS_COMMODITY).GetDeckSize() , 23);
  BOOST_CHECK_EQUAL(ogi.GetCommodities().GetDeck(NEW_CHEMICALS_COMMODITY).GetDiscardSize() , 1);
  BOOST_CHECK_EQUAL(ogi.GetCommodities().GetDeck(ORBITAL_MEDICINE_COMMODITY).GetDeckSize() , 13);
  BOOST_CHECK_EQUAL(ogi.GetCommodities().GetDeck(ORBITAL_MEDICINE_COMMODITY).GetDiscardSize() , 0);
  BOOST_CHECK_EQUAL(ogi.GetCommodities().GetDeck(RING_ORE_COMMODITY).GetDeckSize() , 12);
  BOOST_CHECK_EQUAL(ogi.GetCommodities().GetDeck(RING_ORE_COMMODITY).GetDiscardSize() , 0);
  BOOST_CHECK_EQUAL(ogi.GetCommodities().GetDeck(MOON_ORE_COMMODITY).GetDeckSize() , 7);
  BOOST_CHECK_EQUAL(ogi.GetCommodities().GetDeck(MOON_ORE_COMMODITY).GetDiscardSize() , 0);


  std::set<std::string> mrset;
  mrset.insert(always.begin(),always.end());
  mrset.insert(normalpublic.begin(),normalpublic.end());
  mrset.insert(cardset2.begin(),cardset2.end());

  TestRefresh(ogi,top,"MegaResources","Player9",mrset,"MegaResources, non-player");


  mrset.insert(std::string("unicast: Player0 -- PLAYERCARDS,") +
               ogi.GetPlayers()[0].GetCommodityHand().GetHandDescription(true,false) +
               std::string("\n"));

  std::ostringstream oss;
  oss << "unicast: Player0 -- PLAYERPURCHASEINFO";
  oss << "," << ogi.GetPlayers()[0].GetMaxPurchasable(ROBOT_PURCHASABLE,FIRST_UNLIMITED);
  oss << "," << ogi.GetPlayers()[0].GetMaxPurchasable(MAN_PURCHASABLE,FIRST_UNLIMITED);
  oss << "," << ogi.GetPlayers()[0].GetMaxPurchasable(ORE_FACTORY_PURCHASABLE,FIRST_UNLIMITED);
  oss << "," << ogi.GetPlayers()[0].GetMaxPurchasable(WATER_FACTORY_PURCHASABLE,FIRST_UNLIMITED);
  oss << "," << ogi.GetPlayers()[0].GetMaxPurchasable(TITANIUM_FACTORY_PURCHASABLE,FIRST_UNLIMITED);
  oss << "," << ogi.GetPlayers()[0].GetMaxPurchasable(RESEARCH_FACTORY_PURCHASABLE,FIRST_UNLIMITED);
  oss << "," << ogi.GetPlayers()[0].GetMaxPurchasable(NEW_CHEMICALS_FACTORY_PURCHASABLE,FIRST_UNLIMITED);
  oss << "\n";

  mrset.insert(oss.str());
               

  mrset.insert(requestmega.begin(),requestmega.end());
  mrset.insert(player0initialprivate.begin(),player0initialprivate.end());

  TestRefresh(ogi,top,"MegaResources","Player0",mrset,"MegaResources, player 0");
  
               


}

