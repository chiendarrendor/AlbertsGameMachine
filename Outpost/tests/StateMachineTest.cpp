#include "OutpostGameInfo.hpp"
#include <boost/test/unit_test.hpp>
#include <iostream>

#include "ActionParser.hpp"
#include "DLLGame.hpp"
#include "ServerGameInfo.hpp"
#include "TestOutputPort.hpp"
#include "TransitionTestUtilities.hpp"
#include <sstream>
#include <stdexcept>

#include <boost/algorithm/string/replace.hpp>

const int numInitialStateTransitions = 6;


// Exercises DLLGame::HandleAction directly (every other test here calls
// Transition::ExecuteAction directly, bypassing it) -- this is the one test
// that actually verifies Phase 3's core mechanism: a successful action
// resends full state to every roster member, not just the actor. See
// .claude/server_game_interface_spec.md's Phase 3 design.
BOOST_AUTO_TEST_CASE( HandleActionSendsFullStateToRoster )
{
  OutpostSet oset("");
  ServerGameInfo sgi("Outpost","testOutpost","XMLLOC","XMLFILE");
  TestOutputPort top;
  OutpostStateMachine osm;
  DLLGame<OutpostSet,OutpostGameInfo> game(sgi,top,oset,osm,60);

  ActionParser ap = MakeAction("JOIN");
  std::set<std::string> roster;
  roster.insert("Player1");
  roster.insert("Spectator1");

  game.HandleAction("Player1",ap,roster);

  std::istringstream iss(top.GetOutput());
  std::string line;
  std::set<std::string> seenRecipients;
  while (std::getline(iss,line))
  {
    BOOST_REQUIRE_EQUAL(line.substr(0,9) , "unicast: ");
    size_t dashpos = line.find(" -- ",9);
    BOOST_REQUIRE(dashpos != std::string::npos);
    std::string recipient = line.substr(9,dashpos-9);
    std::string jsonpart = line.substr(dashpos+4);

    // The game process only ever emits the pure envelope -- layers 1-2
    // (state/legalActions/body) -- never Name/XMLLoc/XMLFile (layer 3) or
    // recipient/GUI identity (layer 4): the server already has Name/XMLLoc/
    // XMLFile authoritatively via its own GetServerGameInfo(), so it adds
    // all of layers 3+4 itself, at the one point it's actually writing to a
    // specific socket, rather than having the game process redundantly echo
    // static data back on every single message. TestOutputPort stands in
    // for the game process's own OutputPort here, so it correctly never
    // sees any of that wrapping.
    boost::json::value v = boost::json::parse(jsonpart);
    const boost::json::object &fullState = v.as_object();
    BOOST_CHECK_EQUAL(fullState.at("state").as_string() , "InitialState");
    BOOST_CHECK(fullState.find("legalActions") != fullState.end());
    BOOST_CHECK(fullState.find("body") != fullState.end());
    BOOST_CHECK(fullState.find("Name") == fullState.end());
    BOOST_CHECK(fullState.find("XMLLoc") == fullState.end());
    BOOST_CHECK(fullState.find("XMLFile") == fullState.end());

    const boost::json::object &body = fullState.at("body").as_object();
    BOOST_CHECK(body.find("options") != body.end());
    const boost::json::array &players = body.at("players").as_array();
    BOOST_CHECK_EQUAL(players.size() , 1u);

    if (recipient == "Player1")
    {
      BOOST_CHECK_EQUAL(body.at("myPlayerId").as_int64() , 0);
    }
    else
    {
      BOOST_CHECK(body.at("myPlayerId").is_null());
    }

    seenRecipients.insert(recipient);
  }

  BOOST_CHECK_EQUAL(seenRecipients.size() , roster.size());
  BOOST_CHECK(seenRecipients.find("Player1") != seenRecipients.end());
  BOOST_CHECK(seenRecipients.find("Spectator1") != seenRecipients.end());
}

BOOST_AUTO_TEST_CASE( TransitionTestJOIN )
{
  OutpostSet oset("");
  ServerGameInfo sgi("Outpost","testOutpost","XMLLOC","XMLFILE");
  TestOutputPort top;

  OutpostStateMachine osm;
  const TransitionType *pJoin = GetTransition(osm,"InitialState","JOIN","InitialState",numInitialStateTransitions);

  OutpostGameInfo ogi(oset,sgi,top);
  BOOST_CHECK_EQUAL (ogi.GetPlayers().size() , (size_t)0);
  ActionParser ap = MakeAction("JOIN");

  BOOST_CHECK_EQUAL(pJoin->IsAuto("InitialState",ogi) , false );
  BOOST_CHECK_EQUAL(pJoin->IsLegal("Player1","InitialState",ogi) , true);
  BOOST_CHECK_EQUAL(TryExecuteAction(pJoin,"InitialState",ogi,"Player1",ap) , true);
  BOOST_CHECK_EQUAL (ogi.GetPlayers().size() , (size_t)1);
  BOOST_CHECK_EQUAL (ogi.GetPlayers()[0].GetName() ,"Player1");


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
  BOOST_CHECK_EQUAL(TryExecuteAction(pJoin,"InitialState",ogi,"Player8",ap) , true);

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
  ActionParser ap = MakeAction("UNJOIN");

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

  BOOST_CHECK_EQUAL(TryExecuteAction(pUnJoin,"InitialState",ogi,"PlayerB",ap) , true);


  BOOST_CHECK_EQUAL(ogi.GetPlayers().size() , (size_t)2);

  BOOST_CHECK_EQUAL(ogi.GetPlayers()[0].GetName() , "PlayerA");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[1].GetName() , "PlayerC");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[0].GetId() , 0);
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[1].GetId() , 1);

  BOOST_CHECK_EQUAL(TryExecuteAction(pUnJoin,"InitialState",ogi,"PlayerC",ap) , true);
  


  BOOST_CHECK_EQUAL(ogi.GetPlayers().size() , (size_t)1);

  BOOST_CHECK_EQUAL(ogi.GetPlayers()[0].GetName() , "PlayerA");
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[0].GetId() , 0);


  ogi.GetPlayers().add("PlayerD");
  ogi.GetPlayers().add("PlayerE");

  BOOST_CHECK_EQUAL(TryExecuteAction(pUnJoin,"InitialState",ogi,"PlayerA",ap) , true);



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
  ActionParser ap = MakeAction("OPTIONS",{{"discard",false},{"refinery",false},{"oneupgrade",false},{"activebid",true},{"supplyharshness",3},{"robots",2},{"researchmega",false},{"smallresearch",true},{"smallmicro",false},{"blinddraws",true},{"stock",2}});

  ogi.GetPlayers().add("Player1");

  // only players in the game can change options.
  BOOST_CHECK_EQUAL(pOptions->IsAuto("InitialState",ogi) , false );
  BOOST_CHECK_EQUAL(pOptions->IsLegal("Player2","InitialState",ogi) , false);

  // this should work.
  BOOST_CHECK_EQUAL(pOptions->IsAuto("InitialState",ogi) , false );
  BOOST_CHECK_EQUAL(pOptions->IsLegal("Player1","InitialState",ogi) , true);
  BOOST_CHECK_EQUAL(TryExecuteAction(pOptions,"InitialState",ogi,"Player1",ap) , true);


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

  ActionParser ap2 = MakeAction("STANDARDBASIC");
  const TransitionType *pSB = GetTransition(osm,"InitialState","STANDARDBASIC","InitialState",numInitialStateTransitions);

  // only players in the game can change options.
  BOOST_CHECK_EQUAL(pSB->IsAuto("InitialState",ogi) , false );
  BOOST_CHECK_EQUAL(pSB->IsLegal("Player2","InitialState",ogi) , false);

  // this should work.
  BOOST_CHECK_EQUAL(pSB->IsAuto("InitialState",ogi) , false );
  BOOST_CHECK_EQUAL(pSB->IsLegal("Player1","InitialState",ogi) , true);
  BOOST_CHECK_EQUAL(TryExecuteAction(pSB,"InitialState",ogi,"Player1",ap2) , true);


  BOOST_CHECK_EQUAL(ogi.GetOptions().GetEarlyDiscard() , false);
  BOOST_CHECK_EQUAL(ogi.GetOptions().GetRefineries() , false);
  BOOST_CHECK_EQUAL(ogi.GetOptions().GetOneUpgradePerTurn() , false);
  BOOST_CHECK_EQUAL(ogi.GetOptions().GetMustBeActiveToBid() , false);
  BOOST_CHECK_EQUAL(ogi.GetOptions().GetSupplyHarshness() , 0);
  BOOST_CHECK_EQUAL(ogi.GetOptions().GetRobotMechanism() , FIRST_UNLIMITED);
  BOOST_CHECK_EQUAL(ogi.GetOptions().GetResearchMega() , false);
  BOOST_CHECK_EQUAL(ogi.GetOptions().GetResearchIsSmall() , false);
  BOOST_CHECK_EQUAL(ogi.GetOptions().GetMicroIsSmall() , false);

  ActionParser ap3 = MakeAction("STANDARDEXPERT");
  const TransitionType *pSE = GetTransition(osm,"InitialState","STANDARDEXPERT","InitialState",numInitialStateTransitions);

  // only players in the game can change options.
  BOOST_CHECK_EQUAL(pSE->IsAuto("InitialState",ogi) , false );
  BOOST_CHECK_EQUAL(pSE->IsLegal("Player2","InitialState",ogi) , false);

  // this should work.
  BOOST_CHECK_EQUAL(pSE->IsAuto("InitialState",ogi) , false );
  BOOST_CHECK_EQUAL(pSE->IsLegal("Player1","InitialState",ogi) , true);
  BOOST_CHECK_EQUAL(TryExecuteAction(pSE,"InitialState",ogi,"Player1",ap3) , true);


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
  ActionParser ap = MakeAction("VALIDATESETUP");

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
  BOOST_CHECK_EQUAL(TryExecuteAction(pVS,"InitialState",ogi,"Player1",ap) , true);
  
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
  ActionParser ap = MakeAction("LIKEOPTIONS");
  ActionParser ap2 = MakeAction("DONTLIKEOPTIONS");

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
  BOOST_CHECK_EQUAL(TryExecuteAction(pLO,"ValidateSetup",ogi,"Player1",ap) , true);
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[0].OkStartup() , true);
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[1].OkStartup() , false);
  
  ogi.GetPlayers()[0].SetOkStartup(false);
  BOOST_CHECK_EQUAL(pDL->IsAuto("ValidateSetup",ogi) , false );
  BOOST_CHECK_EQUAL(pDL->IsLegal("Player1","ValidateSetup",ogi) , true);
  BOOST_CHECK_EQUAL(TryExecuteAction(pDL,"ValidateSetup",ogi,"Player1",ap) , true);
}

BOOST_AUTO_TEST_CASE( TransitionTestSTARTGAME )
{
  OutpostSet oset("");
  ServerGameInfo sgi("Outpost","testOutpost","XMLLOC","XMLFILE");
  TestOutputPort top;

  OutpostStateMachine osm;
  const TransitionType *pSG = GetTransition(osm,"ValidateSetup","STARTGAME","TurnOrder",3);
  OutpostGameInfo ogi(oset,sgi,top);
  ActionParser ap = MakeAction("");

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

  BOOST_CHECK_EQUAL(TryExecuteAction(pSG,"ValidateSetup",ogi,"",ap) , true);



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

  BOOST_CHECK_EQUAL(TryExecuteAction(pSG,"ValidateSetup",ogi2,"",ap) , true);



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

  BOOST_CHECK_EQUAL(TryExecuteAction(pSG,"ValidateSetup",ogi3,"",ap) , true);
  
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
  ActionParser ap = MakeAction("");

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

  BOOST_CHECK_EQUAL(TryExecuteAction(pTO,"TurnOrder",ogi,"",ap) , true);

  BOOST_CHECK_EQUAL(ogi.GetPlayers().GetTurnOrder().size() , (size_t)3);
  BOOST_CHECK_EQUAL(ogi.GetPlayers().GetTurnOrder()[0] , 1);
  BOOST_CHECK_EQUAL(ogi.GetPlayers().GetTurnOrder()[1] , 2);
  BOOST_CHECK_EQUAL(ogi.GetPlayers().GetTurnOrder()[2] , 0);

}


BOOST_AUTO_TEST_CASE( TransitionTestFILLSHIP )
{
  OutpostSet oset("");
  ServerGameInfo sgi("Outpost","testOutpost","XMLLOC","XMLFILE");
  TestOutputPort top;

  OutpostStateMachine osm;
  const TransitionType *pFS = GetTransition(osm,"SupplyFill","FILLSHIP","StartResources",1);
  OutpostGameInfo ogi(oset,sgi,top);
  ActionParser ap = MakeAction("");

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

  BOOST_CHECK_EQUAL(TryExecuteAction(pFS,"SupplyFill",ogi,"",ap) , true);

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


}

BOOST_AUTO_TEST_CASE( TransitionTestINITCOMMODITY )
{
  OutpostSet oset("");
  ServerGameInfo sgi("Outpost","testOutpost","XMLLOC","XMLFILE");

  TestOutputPort top;

  OutpostStateMachine osm;
  const TransitionType *pFS = GetTransition(osm,"StartResources","INITCOMMODITY","PromptMegaResources",1);
  OutpostGameInfo ogi(oset,sgi,top);
  ActionParser ap = MakeAction("");

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
  BOOST_CHECK_EQUAL(TryExecuteAction(pFS,"StartResources",ogi,"",ap) , true);
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
  BOOST_CHECK_EQUAL(TryExecuteAction(pDI,"PromptMegaResources",ogi,"",ap) , true);
  BOOST_CHECK_EQUAL(TryExecuteAction(pPU,"PromptMegaResources",ogi,"",ap) , true);
  
}

BOOST_AUTO_TEST_CASE( TransitionTestINITCOMMODITY_Turn1 )
{
  OutpostSet oset("");
  ServerGameInfo sgi("Outpost","testOutpost","XMLLOC","XMLFILE");

  TestOutputPort top;

  OutpostStateMachine osm;
  const TransitionType *pFS = GetTransition(osm,"StartResources","INITCOMMODITY","PromptMegaResources",1);
  OutpostGameInfo ogi(oset,sgi,top);
  ActionParser ap = MakeAction("");

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
  BOOST_CHECK_EQUAL(TryExecuteAction(pFS,"StartResources",ogi,"",ap) , true);
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
  BOOST_CHECK_EQUAL(TryExecuteAction(pDI,"PromptMegaResources",ogi,"",ap) , true);
  BOOST_CHECK_EQUAL(TryExecuteAction(pPU,"PromptMegaResources",ogi,"",ap) , true);
  
}


BOOST_AUTO_TEST_CASE( TransitionTestINITCOMMODITY_Refineries )
{
  OutpostSet oset("");
  ServerGameInfo sgi("Outpost","testOutpost","XMLLOC","XMLFILE");

  TestOutputPort top;

  OutpostStateMachine osm;
  const TransitionType *pFS = GetTransition(osm,"StartResources","INITCOMMODITY","PromptMegaResources",1);
  OutpostGameInfo ogi(oset,sgi,top);
  ActionParser ap = MakeAction("");

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
  BOOST_CHECK_EQUAL(TryExecuteAction(pFS,"StartResources",ogi,"",ap) , true);
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
  BOOST_CHECK_EQUAL(TryExecuteAction(pDI,"PromptMegaResources",ogi,"",ap) , true);
  BOOST_CHECK_EQUAL(TryExecuteAction(pPU,"PromptMegaResources",ogi,"",ap) , true);
  
}


// validating correct behaviour for Blind Draws
BOOST_AUTO_TEST_CASE( TransitionTestBlindDraws )
{
  OutpostSet oset("");
  ServerGameInfo sgi("Outpost","testOutpost","XMLLOC","XMLFILE");

  TestOutputPort top;

  OutpostStateMachine osm;
  OutpostGameInfo ogi(oset,sgi,top);
  ActionParser ap = MakeAction("");
  
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
  BOOST_CHECK_EQUAL(TryExecuteAction(pInit,"StartResources",ogi,"",ap) , true);
}







  
// getting into the state for REQUESTMEGA should allow us to test COMMODITYDONE false states, too.
BOOST_AUTO_TEST_CASE( TransitionTestREQUESTMEGA )
{
  OutpostSet oset("");
  ServerGameInfo sgi("Outpost","testOutpost","XMLLOC","XMLFILE");

  TestOutputPort top;

  OutpostStateMachine osm;
  const TransitionType *pFS = GetTransition(osm,"PromptMegaResources","REQUESTMEGA","MegaResources",3);
  OutpostGameInfo ogi(oset,sgi,top);
  ActionParser ap = MakeAction("");

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
  BOOST_CHECK_EQUAL(TryExecuteAction(pFS,"PromptMegaResources",ogi,"",ap) , true);

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
  ActionParser ap2 = MakeAction("NUMMEGAS",{{"i_count",-1}});
  BOOST_CHECK_EQUAL(TryExecuteAction(pNM,"MegaResources",ogi,"Player1",ap2) , false);
  BOOST_CHECK_EQUAL(ogi.GetProductionManager().IsProductionDone() , false);

  // bad input should keep it from executing.
  ActionParser ap3 = MakeAction("NUMMEGAS",{{"i_count",2}});
  BOOST_CHECK_EQUAL(TryExecuteAction(pNM,"MegaResources",ogi,"Player1",ap3) , false);
  BOOST_CHECK_EQUAL(ogi.GetProductionManager().IsProductionDone() , false);

  ActionParser ap4 = MakeAction("NUMMEGAS",{{"i_count",1}});
  BOOST_CHECK_EQUAL(TryExecuteAction(pNM,"MegaResources",ogi,"Player1",ap4) , true);
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

  
}

BOOST_AUTO_TEST_CASE( TransitionTestDISCARD1 )
{
  OutpostSet oset("");
  ServerGameInfo sgi("Outpost","testOutpost","XMLLOC","XMLFILE");

  TestOutputPort top;

  OutpostStateMachine osm;
  const TransitionType *pSD = GetTransition(osm,"StartDiscard","STARTDISCARD","DoDiscard",1);
  OutpostGameInfo ogi(oset,sgi,top);
  ActionParser ap = MakeAction("");

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

  BOOST_CHECK_EQUAL(TryExecuteAction(pSD,"StartDiscard",ogi,"",ap) , true);
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

  TestOutputPort top;

  OutpostStateMachine osm;
  OutpostGameInfo ogi(oset,sgi,top);
  ActionParser ap = MakeAction("");

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
  BOOST_CHECK_EQUAL(TryExecuteAction(pAS,"DoDiscard",ogi,"",ap) , true);
  BOOST_CHECK_EQUAL(ogi.GetPlayers().AllPlayersDone() , true );

  // executing the Manual Discard should cause it to not skip, and to generate output.
  ogi.GetPlayers().StartTurnOrder();
  BOOST_CHECK_EQUAL(TryExecuteAction(pMD,"DoDiscard",ogi,"",ap) , true);
  BOOST_CHECK_EQUAL(ogi.GetPlayers().AllPlayersDone() , false );

  ogi.GetOptions().SetEarlyDiscard(false);
  
  BOOST_CHECK_EQUAL(TryExecuteAction(pMD,"DoDiscard",ogi,"",ap) , true);
  BOOST_CHECK_EQUAL(ogi.GetPlayers().AllPlayersDone() , false );
                    
}

BOOST_AUTO_TEST_CASE( TransitionTestMANUALDISCARD )
{
  OutpostSet oset("");
  ServerGameInfo sgi("Outpost","testOutpost","XMLLOC","XMLFILE");

  TestOutputPort top;

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

  ActionParser ap = MakeAction("DISCARD",{{"i_discardstring","001100100"}});
  BOOST_CHECK_EQUAL(TryExecuteAction(pMD,"ManualDiscard",ogi,"Player1",ap) , false);

  // these things should not change if we get do a bad result.
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetCommodityHand().GetHandDescription(true,true) , 
                     "|TI22-TI21-TI20-TI19-TI18-TI17-TI16-TI15|");
  BOOST_CHECK_EQUAL( ogi.GetCommodities().GetDeck(TITANIUM_COMMODITY).GetDiscardSize() , 0 );
  BOOST_CHECK_EQUAL( ogi.GetPlayers().AllPlayersDone() , false );

  ActionParser ap2 = MakeAction("DISCARD",{{"i_discardstring","001B0000"}});
  BOOST_CHECK_EQUAL(TryExecuteAction(pMD,"ManualDiscard",ogi,"Player1",ap2) , false);

  // these things should not change if we get do a bad result.
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetCommodityHand().GetHandDescription(true,true) , 
                     "|TI22-TI21-TI20-TI19-TI18-TI17-TI16-TI15|");
  BOOST_CHECK_EQUAL( ogi.GetCommodities().GetDeck(TITANIUM_COMMODITY).GetDiscardSize() , 0 );
  BOOST_CHECK_EQUAL( ogi.GetPlayers().AllPlayersDone() , false );

  ActionParser ap3 = MakeAction("DISCARD",{{"i_discardstring","00000011"}});
  BOOST_CHECK_EQUAL(TryExecuteAction(pMD,"ManualDiscard",ogi,"Player1",ap3) , false);

  // these things should not change if we get do a bad result.
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetCommodityHand().GetHandDescription(true,true) , 
                     "|TI22-TI21-TI20-TI19-TI18-TI17-TI16-TI15|");
  BOOST_CHECK_EQUAL( ogi.GetCommodities().GetDeck(TITANIUM_COMMODITY).GetDiscardSize() , 0 );
  BOOST_CHECK_EQUAL( ogi.GetPlayers().AllPlayersDone() , false );

  ActionParser ap4 = MakeAction("DISCARD",{{"i_discardstring","00000111"}});
  BOOST_CHECK_EQUAL(TryExecuteAction(pMD,"ManualDiscard",ogi,"Player1",ap4) , true);
  

  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetCommodityHand().GetHandDescription(true,true) , 
                     "|TI22-TI21-TI20-TI19-TI18|");
  BOOST_CHECK_EQUAL( ogi.GetCommodities().GetDeck(TITANIUM_COMMODITY).GetDiscardSize() , 3 );
  BOOST_CHECK_EQUAL( ogi.GetPlayers().AllPlayersDone() , true );

}


BOOST_AUTO_TEST_CASE( TransitionTestMANUALDISCARD2 )
{
  OutpostSet oset("");
  ServerGameInfo sgi("Outpost","testOutpost","XMLLOC","XMLFILE");

  TestOutputPort top;

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

  ActionParser ap = MakeAction("DISCARD",{{"i_discardstring","001100100"}});
  BOOST_CHECK_EQUAL(TryExecuteAction(pMD,"ManualDiscard",ogi,"Player1",ap) , false);

  // these things should not change if we get do a bad result.
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetCommodityHand().GetHandDescription(true,true) , 
                     "|TI22-TI21-TI20-TI19-TI18-TI17-TI16-TI15|");
  BOOST_CHECK_EQUAL( ogi.GetCommodities().GetDeck(TITANIUM_COMMODITY).GetDiscardSize() , 0 );
  BOOST_CHECK_EQUAL( ogi.GetPlayers().AllPlayersDone() , false );

  ActionParser ap2 = MakeAction("DISCARD",{{"i_discardstring","001B0000"}});
  BOOST_CHECK_EQUAL(TryExecuteAction(pMD,"ManualDiscard",ogi,"Player1",ap2) , false);

  // these things should not change if we get do a bad result.
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetCommodityHand().GetHandDescription(true,true) , 
                     "|TI22-TI21-TI20-TI19-TI18-TI17-TI16-TI15|");
  BOOST_CHECK_EQUAL( ogi.GetCommodities().GetDeck(TITANIUM_COMMODITY).GetDiscardSize() , 0 );
  BOOST_CHECK_EQUAL( ogi.GetPlayers().AllPlayersDone() , false );

  ActionParser ap3 = MakeAction("DISCARD",{{"i_discardstring","00000011"}});
  BOOST_CHECK_EQUAL(TryExecuteAction(pMD,"ManualDiscard",ogi,"Player1",ap3) , false);

  // these things should not change if we get do a bad result.
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetCommodityHand().GetHandDescription(true,true) , 
                     "|TI22-TI21-TI20-TI19-TI18-TI17-TI16-TI15|");
  BOOST_CHECK_EQUAL( ogi.GetCommodities().GetDeck(TITANIUM_COMMODITY).GetDiscardSize() , 0 );
  BOOST_CHECK_EQUAL( ogi.GetPlayers().AllPlayersDone() , false );

  BOOST_CHECK_EQUAL( ogi.GetPlayers().GetTurnOrder()[0] , 0);
  BOOST_CHECK_EQUAL( ogi.GetPlayers().GetTurnOrder()[1] , 1);

  BOOST_CHECK_EQUAL( ogi.GetPlayers().GetCurTurnPlayer().GetId() , 0);


  ActionParser ap4 = MakeAction("DISCARD",{{"i_discardstring","00000111"}});
  BOOST_CHECK_EQUAL(TryExecuteAction(pMD,"ManualDiscard",ogi,"Player1",ap4) , true);
  

  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetCommodityHand().GetHandDescription(true,true) , 
                     "|TI22-TI21-TI20-TI19-TI18|");
  BOOST_CHECK_EQUAL( ogi.GetCommodities().GetDeck(TITANIUM_COMMODITY).GetDiscardSize() , 3 );
  BOOST_CHECK_EQUAL( ogi.GetPlayers().AllPlayersDone() , false );

}



BOOST_AUTO_TEST_CASE( TransitionTestSTARTMANNING )
{
  OutpostSet oset("");
  ServerGameInfo sgi("Outpost","testOutpost","XMLLOC","XMLFILE");
  ActionParser ap = MakeAction("");

  TestOutputPort top;

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

  BOOST_CHECK_EQUAL(TryExecuteAction(pMD,"StartFinalManning",ogi,"",ap) , true);

  BOOST_CHECK_EQUAL( ogi.GetPlayers().GetCurTurnPlayer().GetId() , 4);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetPurchaseState() , UNPLAYED);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetPurchaseState() , ACTIVE);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetPurchaseState() , INTERNAL_FINAL_DONE);

  // PLAYERSTATE: plid,vp,costsum,handlimt,poplimit,mancost,#hum,#rob,purstate,bidstate,haspurchased
}


BOOST_AUTO_TEST_CASE( TransitionTestDONEMANNING )
{
  OutpostSet oset("");
  ServerGameInfo sgi("Outpost","testOutpost","XMLLOC","XMLFILE");
  ActionParser ap = MakeAction("");

  TestOutputPort top;

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

  BOOST_CHECK_EQUAL(TryExecuteAction(pMD,"FinalManning",ogi,"Player4",ap) , true);

  BOOST_CHECK_EQUAL( ogi.GetPlayers().GetCurTurnPlayer().GetId() , 3);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetPurchaseState() , ACTIVE);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetPurchaseState() , FINAL_DONE);

  // PLAYERSTATE: plid,vp,costsum,handlimt,poplimit,robotlimit,mancost,#hum,#rob,purstate,bidstate,haspurchased,highbid,lastbid
  // given the above machinations, we should see that the # of robots and humans get adjusted down

  
  ogi.GetPlayers().IncrementTurnOrder();
  ogi.GetPlayers().IncrementTurnOrder();
  ogi.GetPlayers().IncrementTurnOrder();

  BOOST_CHECK_EQUAL(TryExecuteAction(pMD,"FinalManning",ogi,"Player0",ap) , true);
  BOOST_CHECK_EQUAL( ogi.GetPlayers().AllPlayersDone() , true );


  
  BOOST_CHECK_EQUAL(pMD->IsLegal("Player5","FinalManning",ogi) , false); // needs to execute, return false, even after all players done


}


BOOST_AUTO_TEST_CASE( TransitionTestENDMANNING )
{
  OutpostSet oset("");
  ServerGameInfo sgi("Outpost","testOutpost","XMLLOC","XMLFILE");
  ActionParser ap = MakeAction("");

  TestOutputPort top;

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
  BOOST_CHECK_EQUAL(TryExecuteAction(pMD,"FinalManning",ogi,"",ap) , true);

  BOOST_CHECK_EQUAL(TryExecuteAction(pME,"FinalManning",ogi,"",ap) , true);
}

BOOST_AUTO_TEST_CASE( TransitionTestENDTURN )
{
  OutpostSet oset("");
  ServerGameInfo sgi("Outpost","testOutpost","XMLLOC","XMLFILE");
  ActionParser ap = MakeAction("");

  TestOutputPort top;

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

  BOOST_CHECK_EQUAL(TryExecuteAction(pNT,"TurnEnd",ogi,"",ap) , true);

  BOOST_CHECK_EQUAL(ogi.GetPlayers()[0].GetLastBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[3].GetLastBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[5].GetLastBid() , NOT_HIGH_BID );

  BOOST_CHECK_EQUAL(ogi.GetPlayers()[0].GetCommodityHand().GetHand()[0].IsNew() , false);
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[3].GetCommodityHand().GetHand()[0].IsNew() , false);
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[3].GetCommodityHand().GetHand()[1].IsNew() , false);
  BOOST_CHECK_EQUAL(ogi.GetPlayers()[5].GetCommodityHand().GetHand()[0].IsNew() , false);

  BOOST_CHECK_EQUAL(ogi.GetCurTurn() , 6);

  // let's give a player some VP.
  ogi.GetPlayers()[2].GetFactories().AddFactory(MOON_BASE_FACTORY);
  ogi.GetPlayers()[2].GetFactories().AddFactory(MOON_BASE_FACTORY);
  ogi.GetPlayers()[2].GetFactories().AddFactory(MOON_BASE_FACTORY);
  ogi.GetPlayers()[2].GetFactories().AddFactory(SPACE_STATION_FACTORY); // 20 + 20 + 20 + 10 + 5 (from above = 75!)
  ogi.GetPlayers()[2].GetFactories().AlterManning("HHHH",4,0);

  // game end is auto if someone has achieved 75 VP
  BOOST_CHECK_EQUAL(pEG->IsAuto("TurnEnd",ogi) , true );
  BOOST_CHECK_EQUAL(pNT->IsAuto("TurnEnd",ogi) , false );
  
  BOOST_CHECK_EQUAL(TryExecuteAction(pEG,"TurnEnd",ogi,"",ap) , true);

  ogi.GetPlayers()[5].GetFactories().AddFactory(MOON_BASE_FACTORY);
  ogi.GetPlayers()[5].GetFactories().AddFactory(MOON_BASE_FACTORY);
  ogi.GetPlayers()[5].GetFactories().AddFactory(MOON_BASE_FACTORY);
  ogi.GetPlayers()[5].GetFactories().AlterManning("HHH",4,0);
  ogi.GetPlayers()[5].GetOwnedItems().AddItem(ECOPLANTS); // should also be 75 VP

  // game end is auto if someone has achieved 75 VP
  BOOST_CHECK_EQUAL(pEG->IsAuto("TurnEnd",ogi) , true );
  BOOST_CHECK_EQUAL(pNT->IsAuto("TurnEnd",ogi) , false );
  
  BOOST_CHECK_EQUAL(TryExecuteAction(pEG,"TurnEnd",ogi,"",ap) , true);

  // give someone more than 75....
  ogi.GetPlayers()[4].GetFactories().AddFactory(MOON_BASE_FACTORY);
  ogi.GetPlayers()[4].GetFactories().AddFactory(MOON_BASE_FACTORY);
  ogi.GetPlayers()[4].GetFactories().AddFactory(MOON_BASE_FACTORY);
  ogi.GetPlayers()[4].GetFactories().AddFactory(MOON_BASE_FACTORY);
  ogi.GetPlayers()[4].GetFactories().AlterManning("HHHH",4,0); // should have 88 now.

  // game end is auto if someone has achieved 75 VP
  BOOST_CHECK_EQUAL(pEG->IsAuto("TurnEnd",ogi) , true );
  BOOST_CHECK_EQUAL(pNT->IsAuto("TurnEnd",ogi) , false );
  
  BOOST_CHECK_EQUAL(TryExecuteAction(pEG,"TurnEnd",ogi,"",ap) , true);
}


// replaces all instances of ## with i_replace in i_basestring
std::string ReplacePlayerName(const std::string &i_replace,const std::string &i_basestring)
{
  std::string input = i_basestring;
  boost::algorithm::replace_all(input,"##",i_replace);
  return input;
}

// Calls GetClientState directly and returns the result for field assertions
// -- replaces the old TestRefresh, which parsed TestOutputPort's captured
// comma-line wire output. See .claude/server_game_interface_spec.md's
// Phase 3 spike.
boost::json::object GetState(const OutpostGameInfo &i_ogi,
                             const std::string &i_statename,const std::string &i_playername)
{
  return i_ogi.GetClientState(i_playername,i_statename);
}

void CheckOptions(const boost::json::object &i_result)
{
  const boost::json::object &options = i_result.at("options").as_object();
  BOOST_CHECK_EQUAL(options.at("discard").as_bool() , true);
  BOOST_CHECK_EQUAL(options.at("refinery").as_bool() , false);
  BOOST_CHECK_EQUAL(options.at("oneupgrade").as_bool() , false);
  BOOST_CHECK_EQUAL(options.at("activebid").as_bool() , false);
  BOOST_CHECK_EQUAL(options.at("supplyharshness").as_int64() , 4);
  BOOST_CHECK_EQUAL(options.at("robots").as_int64() , 1);
  BOOST_CHECK_EQUAL(options.at("researchmega").as_bool() , false);
  BOOST_CHECK_EQUAL(options.at("smallresearch").as_bool() , true);
  BOOST_CHECK_EQUAL(options.at("smallmicro").as_bool() , true);
  BOOST_CHECK_EQUAL(options.at("blinddraws").as_bool() , true);
  BOOST_CHECK_EQUAL(options.at("stock").as_int64() , 1);
}

const boost::json::object &FindPlayer(const boost::json::array &i_players,int i_id)
{
  for (const boost::json::value &pv : i_players)
  {
    const boost::json::object &p = pv.as_object();
    if (p.at("id").as_int64() == i_id)
    {
      return p;
    }
  }
  BOOST_FAIL("player id not found in players array");
  static boost::json::object dummy;
  return dummy;
}

void CheckPlayer(const boost::json::object &i_p,const std::string &i_name,int i_id,bool i_okstartup,
                 int i_vp,int i_itemcostsum,int i_handlimit,int i_poplimit,int i_robotlimit,int i_mancost,
                 int i_numhumans,int i_numrobots,int i_pstate,int i_bstate,bool i_haspurchased,
                 int i_highbid,int i_lastbid,const std::string &i_factorydesc,const std::string &i_cardbackdesc,
                 const std::vector<int> &i_items)
{
  BOOST_CHECK_EQUAL(i_p.at("name").as_string() , i_name);
  BOOST_CHECK_EQUAL(i_p.at("id").as_int64() , i_id);
  BOOST_CHECK_EQUAL(i_p.at("okstartup").as_bool() , i_okstartup);
  BOOST_CHECK_EQUAL(i_p.at("victoryPoints").as_int64() , i_vp);
  BOOST_CHECK_EQUAL(i_p.at("itemCostSum").as_int64() , i_itemcostsum);
  BOOST_CHECK_EQUAL(i_p.at("handLimit").as_int64() , i_handlimit);
  BOOST_CHECK_EQUAL(i_p.at("popLimit").as_int64() , i_poplimit);
  BOOST_CHECK_EQUAL(i_p.at("robotLimit").as_int64() , i_robotlimit);
  BOOST_CHECK_EQUAL(i_p.at("manCost").as_int64() , i_mancost);
  BOOST_CHECK_EQUAL(i_p.at("numHumans").as_int64() , i_numhumans);
  BOOST_CHECK_EQUAL(i_p.at("numRobots").as_int64() , i_numrobots);
  BOOST_CHECK_EQUAL(i_p.at("purchaseState").as_int64() , i_pstate);
  BOOST_CHECK_EQUAL(i_p.at("bidState").as_int64() , i_bstate);
  BOOST_CHECK_EQUAL(i_p.at("hasPurchased").as_bool() , i_haspurchased);
  BOOST_CHECK_EQUAL(i_p.at("highBid").as_int64() , i_highbid);
  BOOST_CHECK_EQUAL(i_p.at("lastBid").as_int64() , i_lastbid);
  BOOST_CHECK_EQUAL(i_p.at("factoryDescription").as_string() , i_factorydesc);
  BOOST_CHECK_EQUAL(i_p.at("cardBackDescription").as_string() , i_cardbackdesc);

  const boost::json::array &items = i_p.at("items").as_array();
  BOOST_REQUIRE_EQUAL(items.size() , i_items.size());
  for (size_t i = 0 ; i < i_items.size() ; ++i)
  {
    BOOST_CHECK_EQUAL(items[i].as_int64() , i_items[i]);
  }
}

// Player stats as of right after setup (before the second StartProduction
// call later in the test) -- used by InitialState/ValidateSetup/Purchase/
// Bid/TerminalState.
void CheckPlayersCardset1(const boost::json::array &i_players)
{
  CheckPlayer(FindPlayer(i_players,0),"Player0",0,false,
             11,100,13,10,0,10,3,0, 0,1,false, -1,-1,
             "|WH-WH-WH-WH-RH|","|MO*-WA*|",{10});
  CheckPlayer(FindPlayer(i_players,1),"Player1",1,true,
             3,50,13,8,0,10,4,1, 0,0,false, -1,-1,
             "|TU|","|RE38M-OR*|",{2,4});
  CheckPlayer(FindPlayer(i_players,2),"Player2",2,false,
             94,80,10,6,0,10,5,0, 1,4,false, 10,-1,
             "|SH-MB-MB-MB-MB|","|MI18A|",{5,5});
}

// Player stats after the second StartProduction call -- used by
// MegaResources. Only purchaseState (0, 2) differs plus the now-more-visible
// card backs; everything else is unchanged from cardset1.
void CheckPlayersCardset2(const boost::json::array &i_players)
{
  CheckPlayer(FindPlayer(i_players,0),"Player0",0,false,
             11,100,13,10,0,10,3,0, 1,1,false, -1,-1,
             "|WH-WH-WH-WH-RH|","|MO*-RE*-WA*|",{10});
  CheckPlayer(FindPlayer(i_players,1),"Player1",1,true,
             3,50,13,8,0,10,4,1, 0,0,false, -1,-1,
             "|TU|","|RE38M-OR*|",{2,4});
  CheckPlayer(FindPlayer(i_players,2),"Player2",2,false,
             94,80,10,6,0,10,5,0, 3,4,false, 10,-1,
             "|SH-MB-MB-MB-MB|","|MO*-MO*-MO*-MO*-OM*-MI18A-RE*-RE*|",{5,5});
}

// The rest of the in-game fields are identical for every viewer at a given
// state (unlike players[], which old PLAYERCARDS/PLAYERPURCHASEINFO kept
// private, these were always "global" events) -- so these only need
// checking once per state, not once per viewer.
void CheckInGamePublicCardset1(const boost::json::object &i_result)
{
  BOOST_CHECK_EQUAL(i_result.at("turnNum").as_int64() , 3);

  static const int expectedItemStock[13] = {2,1,2,1,0,2,2,2,2,1,2,2,2};
  const boost::json::array &itemStock = i_result.at("itemStock").as_array();
  BOOST_REQUIRE_EQUAL(itemStock.size() , 13u);
  for (int i = 0 ; i < 13 ; ++i)
  {
    const boost::json::object &entry = itemStock[i].as_object();
    BOOST_CHECK_EQUAL(entry.at("itemId").as_int64() , i+1);
    BOOST_CHECK_EQUAL(entry.at("count").as_int64() , expectedItemStock[i]);
  }

  static const int expectedTurnOrder[3] = {2,0,1};
  const boost::json::array &turnOrder = i_result.at("turnOrder").as_array();
  BOOST_REQUIRE_EQUAL(turnOrder.size() , 3u);
  for (int i = 0 ; i < 3 ; ++i)
  {
    BOOST_CHECK_EQUAL(turnOrder[i].as_int64() , expectedTurnOrder[i]);
  }

  const boost::json::array &supplyShip = i_result.at("supplyShip").as_array();
  BOOST_REQUIRE_EQUAL(supplyShip.size() , 3u);
  BOOST_CHECK_EQUAL(supplyShip[0].as_int64() , 9);
  BOOST_CHECK_EQUAL(supplyShip[1].as_int64() , 0);
  BOOST_CHECK_EQUAL(supplyShip[2].as_int64() , 13);

  static const int expectedCommodity[9][4] = {
    {26,0,0,3}, {39,0,30,7}, {53,0,44,10}, {32,0,56,13}, {16,0,0,17},
    {23,1,88,20}, {14,0,0,30}, {12,0,0,40}, {11,0,0,50}
  };
  const boost::json::array &commodityStock = i_result.at("commodityStock").as_array();
  BOOST_REQUIRE_EQUAL(commodityStock.size() , 9u);
  for (int i = 0 ; i < 9 ; ++i)
  {
    const boost::json::object &entry = commodityStock[i].as_object();
    BOOST_CHECK_EQUAL(entry.at("commodityId").as_int64() , i+1);
    BOOST_CHECK_EQUAL(entry.at("drawCount").as_int64() , expectedCommodity[i][0]);
    BOOST_CHECK_EQUAL(entry.at("discardCount").as_int64() , expectedCommodity[i][1]);
    BOOST_CHECK_EQUAL(entry.at("megaValue").as_int64() , expectedCommodity[i][2]);
    BOOST_CHECK_EQUAL(entry.at("averageValue").as_int64() , expectedCommodity[i][3]);
  }

  const boost::json::array &purchaseData = i_result.at("purchaseData").as_array();
  BOOST_REQUIRE_EQUAL(purchaseData.size() , 3u);
  BOOST_CHECK_EQUAL(purchaseData[0].as_object().at("shipIndex").as_int64() , 0);
  BOOST_CHECK_EQUAL(purchaseData[0].as_object().at("playerId").as_int64() , 3000);
  BOOST_CHECK_EQUAL(purchaseData[0].as_object().at("itemId").as_int64() , 0);
  BOOST_CHECK_EQUAL(purchaseData[0].as_object().at("cost").as_int64() , -1);
  BOOST_CHECK_EQUAL(purchaseData[1].as_object().at("shipIndex").as_int64() , 1);
  BOOST_CHECK_EQUAL(purchaseData[1].as_object().at("playerId").as_int64() , 3000);
  BOOST_CHECK_EQUAL(purchaseData[1].as_object().at("itemId").as_int64() , 0);
  BOOST_CHECK_EQUAL(purchaseData[1].as_object().at("cost").as_int64() , -1);
  BOOST_CHECK_EQUAL(purchaseData[2].as_object().at("shipIndex").as_int64() , 2);
  BOOST_CHECK_EQUAL(purchaseData[2].as_object().at("playerId").as_int64() , 0);
  BOOST_CHECK_EQUAL(purchaseData[2].as_object().at("itemId").as_int64() , 7);
  BOOST_CHECK_EQUAL(purchaseData[2].as_object().at("cost").as_int64() , 75);
}

// Same as cardset1 except commodities 4/7/9's drawCount, which the second
// StartProduction call (later in the test) draws down further.
void CheckInGamePublicCardset2(const boost::json::object &i_result)
{
  BOOST_CHECK_EQUAL(i_result.at("turnNum").as_int64() , 3);

  static const int expectedItemStock[13] = {2,1,2,1,0,2,2,2,2,1,2,2,2};
  const boost::json::array &itemStock = i_result.at("itemStock").as_array();
  BOOST_REQUIRE_EQUAL(itemStock.size() , 13u);
  for (int i = 0 ; i < 13 ; ++i)
  {
    const boost::json::object &entry = itemStock[i].as_object();
    BOOST_CHECK_EQUAL(entry.at("itemId").as_int64() , i+1);
    BOOST_CHECK_EQUAL(entry.at("count").as_int64() , expectedItemStock[i]);
  }

  static const int expectedTurnOrder[3] = {2,0,1};
  const boost::json::array &turnOrder = i_result.at("turnOrder").as_array();
  BOOST_REQUIRE_EQUAL(turnOrder.size() , 3u);
  for (int i = 0 ; i < 3 ; ++i)
  {
    BOOST_CHECK_EQUAL(turnOrder[i].as_int64() , expectedTurnOrder[i]);
  }

  const boost::json::array &supplyShip = i_result.at("supplyShip").as_array();
  BOOST_REQUIRE_EQUAL(supplyShip.size() , 3u);
  BOOST_CHECK_EQUAL(supplyShip[0].as_int64() , 9);
  BOOST_CHECK_EQUAL(supplyShip[1].as_int64() , 0);
  BOOST_CHECK_EQUAL(supplyShip[2].as_int64() , 13);

  static const int expectedCommodity[9][4] = {
    {26,0,0,3}, {39,0,30,7}, {53,0,44,10}, {29,0,56,13}, {16,0,0,17},
    {23,1,88,20}, {13,0,0,30}, {12,0,0,40}, {7,0,0,50}
  };
  const boost::json::array &commodityStock = i_result.at("commodityStock").as_array();
  BOOST_REQUIRE_EQUAL(commodityStock.size() , 9u);
  for (int i = 0 ; i < 9 ; ++i)
  {
    const boost::json::object &entry = commodityStock[i].as_object();
    BOOST_CHECK_EQUAL(entry.at("commodityId").as_int64() , i+1);
    BOOST_CHECK_EQUAL(entry.at("drawCount").as_int64() , expectedCommodity[i][0]);
    BOOST_CHECK_EQUAL(entry.at("discardCount").as_int64() , expectedCommodity[i][1]);
    BOOST_CHECK_EQUAL(entry.at("megaValue").as_int64() , expectedCommodity[i][2]);
    BOOST_CHECK_EQUAL(entry.at("averageValue").as_int64() , expectedCommodity[i][3]);
  }

  const boost::json::array &purchaseData = i_result.at("purchaseData").as_array();
  BOOST_REQUIRE_EQUAL(purchaseData.size() , 3u);
  BOOST_CHECK_EQUAL(purchaseData[0].as_object().at("shipIndex").as_int64() , 0);
  BOOST_CHECK_EQUAL(purchaseData[0].as_object().at("playerId").as_int64() , 3000);
  BOOST_CHECK_EQUAL(purchaseData[0].as_object().at("itemId").as_int64() , 0);
  BOOST_CHECK_EQUAL(purchaseData[0].as_object().at("cost").as_int64() , -1);
  BOOST_CHECK_EQUAL(purchaseData[1].as_object().at("shipIndex").as_int64() , 1);
  BOOST_CHECK_EQUAL(purchaseData[1].as_object().at("playerId").as_int64() , 3000);
  BOOST_CHECK_EQUAL(purchaseData[1].as_object().at("itemId").as_int64() , 0);
  BOOST_CHECK_EQUAL(purchaseData[1].as_object().at("cost").as_int64() , -1);
  BOOST_CHECK_EQUAL(purchaseData[2].as_object().at("shipIndex").as_int64() , 2);
  BOOST_CHECK_EQUAL(purchaseData[2].as_object().at("playerId").as_int64() , 0);
  BOOST_CHECK_EQUAL(purchaseData[2].as_object().at("itemId").as_int64() , 7);
  BOOST_CHECK_EQUAL(purchaseData[2].as_object().at("cost").as_int64() , 75);
}

void CheckNoPrivateInfo(const boost::json::object &i_result)
{
  BOOST_CHECK(i_result.at("myHand").is_null());
  BOOST_CHECK(i_result.at("myPurchaseInfo").is_null());
}

void CheckPrivateInfo(const boost::json::object &i_result,const std::string &i_hand,
                      int i_numRobots,int i_numMen,int i_numOre,int i_numWater,
                      int i_numTitanium,int i_numResearch,int i_numNewChem)
{
  BOOST_CHECK_EQUAL(i_result.at("myHand").as_string() , i_hand);
  const boost::json::object &pinfo = i_result.at("myPurchaseInfo").as_object();
  BOOST_CHECK_EQUAL(pinfo.at("numRobots").as_int64() , i_numRobots);
  BOOST_CHECK_EQUAL(pinfo.at("numMen").as_int64() , i_numMen);
  BOOST_CHECK_EQUAL(pinfo.at("numOre").as_int64() , i_numOre);
  BOOST_CHECK_EQUAL(pinfo.at("numWater").as_int64() , i_numWater);
  BOOST_CHECK_EQUAL(pinfo.at("numTitanium").as_int64() , i_numTitanium);
  BOOST_CHECK_EQUAL(pinfo.at("numResearch").as_int64() , i_numResearch);
  BOOST_CHECK_EQUAL(pinfo.at("numNewChem").as_int64() , i_numNewChem);
}

BOOST_AUTO_TEST_CASE( StateMachineRefresh )
{
  OutpostSet oset("");
  ServerGameInfo sgi("Outpost","testOutpost","XMLLOC","XMLFILE");
  TestOutputPort top;
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

  // --- InitialState / ValidateSetup: the setup-phase early return. Options
  // and players (full stats included -- GetClientState always computes
  // every player field in one pass, even pre-game, since they're all
  // harmless defaults until the game actually starts) only; nothing else.
  // ValidateSetup behaves identically to InitialState in the new design --
  // okstartup is just always part of each player object now, not a
  // separate state-gated event.
  {
    boost::json::object r = GetState(ogi,"InitialState","Player9");
    CheckOptions(r);
    CheckPlayersCardset1(r.at("players").as_array());
    BOOST_CHECK(r.at("myPlayerId").is_null());
    BOOST_CHECK(r.find("turnNum") == r.end());
  }
  {
    boost::json::object r = GetState(ogi,"InitialState","Player2");
    CheckOptions(r);
    CheckPlayersCardset1(r.at("players").as_array());
    BOOST_CHECK_EQUAL(r.at("myPlayerId").as_int64() , 2);
    BOOST_CHECK(r.find("turnNum") == r.end());
  }
  {
    boost::json::object r = GetState(ogi,"ValidateSetup","Player9");
    CheckOptions(r);
    CheckPlayersCardset1(r.at("players").as_array());
    BOOST_CHECK(r.at("myPlayerId").is_null());
  }
  {
    boost::json::object r = GetState(ogi,"ValidateSetup","Player2");
    CheckOptions(r);
    CheckPlayersCardset1(r.at("players").as_array());
    BOOST_CHECK_EQUAL(r.at("myPlayerId").as_int64() , 2);
  }

  // --- Purchase (normal in-game state)
  {
    boost::json::object r = GetState(ogi,"Purchase","Player9");
    CheckOptions(r);
    CheckPlayersCardset1(r.at("players").as_array());
    BOOST_CHECK(r.at("myPlayerId").is_null());
    CheckInGamePublicCardset1(r);
    CheckNoPrivateInfo(r);
    BOOST_CHECK(r.at("megaRequest").is_null());
    BOOST_CHECK(r.at("bidInfo").is_null());
    BOOST_CHECK_EQUAL(r.at("winners").as_array().size() , 0u);
  }
  {
    boost::json::object r = GetState(ogi,"Purchase","Player1");
    CheckOptions(r);
    CheckPlayersCardset1(r.at("players").as_array());
    BOOST_CHECK_EQUAL(r.at("myPlayerId").as_int64() , 1);
    CheckInGamePublicCardset1(r);
    CheckPrivateInfo(r,"|RE38M-OR3|", 0,4,4,2,0,0,0);
    BOOST_CHECK(r.at("megaRequest").is_null());
    BOOST_CHECK(r.at("bidInfo").is_null());
    BOOST_CHECK_EQUAL(r.at("winners").as_array().size() , 0u);
  }

  // --- Bid
  {
    boost::json::object r = GetState(ogi,"Bid","Player9");
    CheckOptions(r);
    CheckPlayersCardset1(r.at("players").as_array());
    BOOST_CHECK(r.at("myPlayerId").is_null());
    CheckInGamePublicCardset1(r);
    CheckNoPrivateInfo(r);
    BOOST_CHECK(r.at("megaRequest").is_null());
    const boost::json::object &bidInfo = r.at("bidInfo").as_object();
    BOOST_CHECK_EQUAL(bidInfo.at("itemShipIndex").as_int64() , 2);
    BOOST_CHECK_EQUAL(bidInfo.at("price").as_int64() , 10);
    BOOST_CHECK_EQUAL(r.at("winners").as_array().size() , 0u);
  }
  {
    boost::json::object r = GetState(ogi,"Bid","Player2");
    CheckOptions(r);
    CheckPlayersCardset1(r.at("players").as_array());
    BOOST_CHECK_EQUAL(r.at("myPlayerId").as_int64() , 2);
    CheckInGamePublicCardset1(r);
    CheckPrivateInfo(r,"|MI18A|", 0,1,1,0,0,0,0);
    BOOST_CHECK(r.at("megaRequest").is_null());
    const boost::json::object &bidInfo2 = r.at("bidInfo").as_object();
    BOOST_CHECK_EQUAL(bidInfo2.at("itemShipIndex").as_int64() , 2);
    BOOST_CHECK_EQUAL(bidInfo2.at("price").as_int64() , 10);
    BOOST_CHECK_EQUAL(r.at("winners").as_array().size() , 0u);
  }

  // --- TerminalState
  {
    boost::json::object r = GetState(ogi,"TerminalState","Player9");
    CheckOptions(r);
    CheckPlayersCardset1(r.at("players").as_array());
    BOOST_CHECK(r.at("myPlayerId").is_null());
    CheckInGamePublicCardset1(r);
    CheckNoPrivateInfo(r);
    BOOST_CHECK(r.at("megaRequest").is_null());
    BOOST_CHECK(r.at("bidInfo").is_null());
    const boost::json::array &winners = r.at("winners").as_array();
    BOOST_REQUIRE_EQUAL(winners.size() , 1u);
    BOOST_CHECK_EQUAL(winners[0].as_int64() , 2);
  }
  {
    boost::json::object r = GetState(ogi,"TerminalState","Player0");
    CheckOptions(r);
    CheckPlayersCardset1(r.at("players").as_array());
    BOOST_CHECK_EQUAL(r.at("myPlayerId").as_int64() , 0);
    CheckInGamePublicCardset1(r);
    CheckPrivateInfo(r,"|MO30-WA7|", 0,3,3,1,0,0,0);
    BOOST_CHECK(r.at("megaRequest").is_null());
    BOOST_CHECK(r.at("bidInfo").is_null());
    const boost::json::array &winners2 = r.at("winners").as_array();
    BOOST_REQUIRE_EQUAL(winners2.size() , 1u);
    BOOST_CHECK_EQUAL(winners2[0].as_int64() , 2);
  }

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


  // --- MegaResources
  {
    boost::json::object r = GetState(ogi,"MegaResources","Player9");
    CheckOptions(r);
    CheckPlayersCardset2(r.at("players").as_array());
    BOOST_CHECK(r.at("myPlayerId").is_null());
    CheckInGamePublicCardset2(r);
    CheckNoPrivateInfo(r);
    BOOST_CHECK(r.at("megaRequest").is_null());
    BOOST_CHECK(r.at("bidInfo").is_null());
    BOOST_CHECK_EQUAL(r.at("winners").as_array().size() , 0u);
  }
  {
    boost::json::object r = GetState(ogi,"MegaResources","Player0");
    CheckOptions(r);
    CheckPlayersCardset2(r.at("players").as_array());
    BOOST_CHECK_EQUAL(r.at("myPlayerId").as_int64() , 0);
    CheckInGamePublicCardset2(r);

    // computed live, same as the original test did, rather than a
    // hardcoded string -- the second StartProduction call above may have
    // changed Player0's hand.
    std::string expectedHand = ogi.GetPlayers()[0].GetCommodityHand().GetHandDescription(true,false);
    BOOST_CHECK_EQUAL(r.at("myHand").as_string() , expectedHand);
    const boost::json::object &pinfo = r.at("myPurchaseInfo").as_object();
    BOOST_CHECK_EQUAL(pinfo.at("numRobots").as_int64() , ogi.GetPlayers()[0].GetMaxPurchasable(ROBOT_PURCHASABLE,FIRST_UNLIMITED));
    BOOST_CHECK_EQUAL(pinfo.at("numMen").as_int64() , ogi.GetPlayers()[0].GetMaxPurchasable(MAN_PURCHASABLE,FIRST_UNLIMITED));
    BOOST_CHECK_EQUAL(pinfo.at("numOre").as_int64() , ogi.GetPlayers()[0].GetMaxPurchasable(ORE_FACTORY_PURCHASABLE,FIRST_UNLIMITED));
    BOOST_CHECK_EQUAL(pinfo.at("numWater").as_int64() , ogi.GetPlayers()[0].GetMaxPurchasable(WATER_FACTORY_PURCHASABLE,FIRST_UNLIMITED));
    BOOST_CHECK_EQUAL(pinfo.at("numTitanium").as_int64() , ogi.GetPlayers()[0].GetMaxPurchasable(TITANIUM_FACTORY_PURCHASABLE,FIRST_UNLIMITED));
    BOOST_CHECK_EQUAL(pinfo.at("numResearch").as_int64() , ogi.GetPlayers()[0].GetMaxPurchasable(RESEARCH_FACTORY_PURCHASABLE,FIRST_UNLIMITED));
    BOOST_CHECK_EQUAL(pinfo.at("numNewChem").as_int64() , ogi.GetPlayers()[0].GetMaxPurchasable(NEW_CHEMICALS_FACTORY_PURCHASABLE,FIRST_UNLIMITED));

    const boost::json::object &megaRequest = r.at("megaRequest").as_object();
    BOOST_CHECK_EQUAL(megaRequest.at("resourceType").as_int64() , 2);
    BOOST_CHECK_EQUAL(megaRequest.at("maxMegas").as_int64() , 1);
    BOOST_CHECK(r.at("bidInfo").is_null());
    BOOST_CHECK_EQUAL(r.at("winners").as_array().size() , 0u);
  }
}

