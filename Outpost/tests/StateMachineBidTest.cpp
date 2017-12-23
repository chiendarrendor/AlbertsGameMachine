#include "OutpostGameInfo.hpp"
#include <boost/test/auto_unit_test.hpp>
#include <iostream>

#include "ActionParser.hpp"
#include "ServerGameInfo.hpp"
#include <sstream>
#include <stdexcept>
#include "TestOutputPort.hpp"
#include "TransitionTestUtilities.hpp"

const int numPurchaseTransitions = 14;
const int numBidTransitions = 16;

BOOST_AUTO_TEST_CASE( TransitionTestPurchaseDualTransitions )
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

  const TransitionType *pMP = GetTransition(osm,"Purchase","BUYMEN","Purchase",numPurchaseTransitions);
  const TransitionType *pRP = GetTransition(osm,"Purchase","BUYROBOTS","Purchase",numPurchaseTransitions);
  const TransitionType *pOP = GetTransition(osm,"Purchase","BUYOREFACTORIES","Purchase",numPurchaseTransitions);
  const TransitionType *pWP = GetTransition(osm,"Purchase","BUYWATERFACTORIES","Purchase",numPurchaseTransitions);
  const TransitionType *pTP = GetTransition(osm,"Purchase","BUYTITANIUMFACTORIES","Purchase",numPurchaseTransitions);
  const TransitionType *pEP = GetTransition(osm,"Purchase","BUYRESEARCHFACTORIES","Purchase",numPurchaseTransitions);
  const TransitionType *pNP = GetTransition(osm,"Purchase","BUYNEWCHEMFACTORIES","Purchase",numPurchaseTransitions);
  const TransitionType *pIP = GetTransition(osm,"Purchase","INTERNALIZE","Purchase",numPurchaseTransitions);
  const TransitionType *pDP = GetTransition(osm,"Purchase","DONE","Purchase",numPurchaseTransitions);
  const TransitionType *pDPF = GetTransition(osm,"Purchase","FINALDONE","Purchase",numPurchaseTransitions);
  const TransitionType *pWMP = GetTransition(osm,"Purchase","WATERMULLIGAN","Purchase",numPurchaseTransitions);

  const TransitionType *pMB = GetTransition(osm,"Bid","BUYMEN","Bid",numBidTransitions);
  const TransitionType *pRB = GetTransition(osm,"Bid","BUYROBOTS","Bid",numBidTransitions);
  const TransitionType *pOB = GetTransition(osm,"Bid","BUYOREFACTORIES","Bid",numBidTransitions);
  const TransitionType *pWB = GetTransition(osm,"Bid","BUYWATERFACTORIES","Bid",numBidTransitions);
  const TransitionType *pTB = GetTransition(osm,"Bid","BUYTITANIUMFACTORIES","Bid",numBidTransitions);
  const TransitionType *pEB = GetTransition(osm,"Bid","BUYRESEARCHFACTORIES","Bid",numBidTransitions);
  const TransitionType *pNB = GetTransition(osm,"Bid","BUYNEWCHEMFACTORIES","Bid",numBidTransitions);
  const TransitionType *pIB = GetTransition(osm,"Bid","INTERNALIZE","Bid",numBidTransitions);
  const TransitionType *pDB = GetTransition(osm,"Bid","DONE","Bid",numBidTransitions);
  const TransitionType *pDBF = GetTransition(osm,"Bid","FINALDONE","Bid",numBidTransitions);
  const TransitionType *pWMB = GetTransition(osm,"Bid","WATERMULLIGAN","Bid",numBidTransitions);

  BOOST_CHECK_EQUAL( pMP , pMB );
  BOOST_CHECK_EQUAL( pRP , pRB );
  BOOST_CHECK_EQUAL( pOP , pOB );
  BOOST_CHECK_EQUAL( pWP , pWB );
  BOOST_CHECK_EQUAL( pTP , pTB );
  BOOST_CHECK_EQUAL( pEP , pEB );
  BOOST_CHECK_EQUAL( pNP , pNB );
  BOOST_CHECK_EQUAL( pIP , pIB );
  BOOST_CHECK_EQUAL( pDP , pDB );
  BOOST_CHECK_EQUAL( pDPF , pDBF );
  BOOST_CHECK_EQUAL( pWMP , pWMB );
}



  

BOOST_AUTO_TEST_CASE( TransitionTestOPENBIDDING )
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

  ogi.GetUnownedItems().Initialize(6);

  ogi.GetPlayers().DetermineTurnOrder();

  ogi.GetPlayers().StartPurchasePhase();
  ogi.GetPlayers()[2].SetPurchaseState(INTERNAL_DONE);
  ogi.GetPlayers()[1].SetPurchaseState(INTERNALIZING);
  ogi.GetPlayers().PurchaseIncrementTurnOrder();
  // at this point, 5 is done, 4 is active, 3 and 0 are unplayed, 2 is INTERNAL_DONE, and 1 is INTERNALIZING

  // transits from Purchase to Bid
  const TransitionType *pBM = GetTransition(osm,"Purchase","OPENBID","Bid",numPurchaseTransitions);

  // never auto
  // auto only if all players are done or internal done.
  BOOST_CHECK_EQUAL(pBM->IsAuto("Purchase",ogi) , false );

  // allowed if:
  //   player name is actually in the game
  //   player name's purchase state is ACTIVE
  //   there must be at least one item in the supply ship
  //   if OneUpgradePerTurn is true, the player must not have purchased an item this turn already.
  //   the player must have funds+discounts to afford at least one of the items on the ship
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Purchase",ogi) , false); // not in game
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Purchase",ogi) , false); // not active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , false); // active, but Supply Ship Empty
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , false); // not active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , false); // not active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , false); // not active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Purchase",ogi) , false); // not active

  ogi.GetUnownedItems().SetShipItem(1,LABORATORY);
  ogi.GetUnownedItems().SetShipItem(4,MOON_BASE);

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Purchase",ogi) , false); // not in game
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Purchase",ogi) , false); // not active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , false); // active, but cannot afford
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , false); // not active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , false); // not active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , false); // not active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Purchase",ogi) , false); // not active

  ogi.GetPlayers()[0].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,90,false,false));
  ogi.GetPlayers()[1].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,90,false,false));
  ogi.GetPlayers()[2].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,90,false,false));
  ogi.GetPlayers()[3].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,90,false,false));
  ogi.GetPlayers()[4].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,90,false,false));
  ogi.GetPlayers()[5].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,90,false,false));

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Purchase",ogi) , false); // not in game
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Purchase",ogi) , false); // not active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , false); // active, but cannot afford
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , false); // not active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , false); // not active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , false); // not active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Purchase",ogi) , false); // not active

  ogi.GetPlayers()[4].GetOwnedItems().AddItem(DATA_LIBRARY);

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Purchase",ogi) , false); // not in game
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Purchase",ogi) , false); // not active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , true); // active, and can afford, with discount
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , false); // not active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , false); // not active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , false); // not active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Purchase",ogi) , false); // not active
  
  ogi.GetOptions().SetOneUpgradePerTurn(true);
  ogi.GetPlayers()[4].SetHasPurchased(false);

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Purchase",ogi) , false); // not in game
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Purchase",ogi) , false); // not active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , true); // active, and can afford
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , false); // not active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , false); // not active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , false); // not active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Purchase",ogi) , false); // not active

  ogi.GetOptions().SetOneUpgradePerTurn(false);
  ogi.GetOptions().SetRobotMechanism(SECOND_UNLIMITED);
  ogi.GetPlayers()[4].SetHasPurchased(true);

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Purchase",ogi) , false); // not in game
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Purchase",ogi) , false); // not active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , true); // active, and can afford
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , false); // not active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , false); // not active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , false); // not active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Purchase",ogi) , false); // not active

  ogi.GetOptions().SetOneUpgradePerTurn(true);
  ogi.GetOptions().SetRobotMechanism(ALWAYS_MULTIPLICATIVE);
  ogi.GetPlayers()[4].SetHasPurchased(true);

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Purchase",ogi) , false); // not in game
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Purchase",ogi) , false); // not active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , false); // active, and can afford
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Purchase",ogi) , false); // not active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Purchase",ogi) , false); // not active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Purchase",ogi) , false); // not active
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Purchase",ogi) , false); // not active

  // putting system back in state for a legal transition
  ogi.GetPlayers()[4].SetHasPurchased(false); 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Purchase",ogi) , true); // active, and can afford

  //
  //   incoming action is:
  //     int i_shipslot -- index into supply ship -- must be a non-empty slot (const std::set<size_t>)
  //     int i_openingbid -- must be at least the face cost of the item, 
  //                         and no more than funds+discounts for that player.
  
  // openbid should clear all players LastBid if successful, and set the LastBid of the bidding player to their bid
  ogi.GetPlayers()[0].SetLastBid(25);
  ogi.GetPlayers()[1].SetLastBid(25);
  ogi.GetPlayers()[2].SetLastBid(25);
  ogi.GetPlayers()[3].SetLastBid(25);
  ogi.GetPlayers()[4].SetLastBid(25);
  ogi.GetPlayers()[5].SetLastBid(25);

  ActionParser ap1("OPENBID,2,30");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player4",ap1) , false); // fail, slot is not valid
  BOOST_CHECK_EQUAL( top.GetOutput(),"unicast: Player4 -- ERROR,Action OPENBID variable i_shipslot has illegal value\n");
  top.ResetOutput();

  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetLastBid() , 25);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetLastBid() , 25);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetLastBid() , 25);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetLastBid() , 25);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetLastBid() , 25);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetLastBid() , 25);


  ActionParser ap2("OPENBID,1,90");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player4",ap2) , false); // fail, less than face cost
  BOOST_CHECK_EQUAL( top.GetOutput(),"unicast: Player4 -- ERROR,Action OPENBID variable i_openingbid is below minimum\n");
  top.ResetOutput();

  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetLastBid() , 25);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetLastBid() , 25);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetLastBid() , 25);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetLastBid() , 25);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetLastBid() , 25);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetLastBid() , 25);

  ActionParser ap2a("OPENBID,1,106");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player4",ap2a) , false); // fail, above funds + discounts (90 + 15)
  BOOST_CHECK_EQUAL( top.GetOutput(),"unicast: Player4 -- ERROR,Action OPENBID variable i_openingbid is above maximum\n");
  top.ResetOutput();

  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetLastBid() , 25);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetLastBid() , 25);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetLastBid() , 25);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetLastBid() , 25);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetLastBid() , 25);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetLastBid() , 25);


  //
  //   body:
  //     a. must set all players bid states appropriately, based on:
  //        - if player is INTERNALIZING or INTERNAL_DONE -- bid state is PASS_OUT
  //        - if player is ACTIVE -- bid state is HASBID
  //        - if player is UNPLAYED -- bid state is UNBID
  //        - if player is DONE:
  //          if MustBeActiveToBid -- bid state is PASS_OUT
  //          if OneUpgradePerTurn and player has purchased -- bid state is PASS_OUT
  //          if OneUpgradePerTurn and player has not purchased -- bid state is UNBID
  //          else -- bid state is UNBID
  //     b. must set all players highbid attributes correctly:
  //        if bid state is HASBID, set to i_openingbid
  //        else set to NOT_HIGH_BID
  //     c. current bidder index must be set to the next player in sequence (descending turn order, wrapping
  //        around) whose bid state is not PASS_OUT, bid state set to CURBID
  //     d. current bid item = i_shipslot (UnownedItems.SetBidIndex())
  //
  //     report states for all players
  //     report index of item being bid on. BroadcastBIDITEM


  // checking to see that we know the prior state.
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetPurchaseState() , ACTIVE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetPurchaseState() , UNPLAYED );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetPurchaseState() , INTERNAL_DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetPurchaseState() , INTERNALIZING );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetPurchaseState() , UNPLAYED );

  ActionParser ap3("OPENBID,1,100");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player4",ap3) , true); // should work...

  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetBidState() , UNBID);     BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetBidState() , HASBID);    BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetHighBid() , 100 );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetBidState() , CURBID);    BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetBidState() , UNBID);     BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetHighBid() , NOT_HIGH_BID );

  BOOST_CHECK_EQUAL( ogi.GetPlayers().GetCurBidPlayer().GetId() , 3 );
  BOOST_CHECK_EQUAL( ogi.GetUnownedItems().GetBidIndex() , (size_t)1);

  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetLastBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetLastBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetLastBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetLastBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetLastBid() , 100);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetLastBid() , NOT_HIGH_BID);



  BOOST_CHECK_EQUAL( top.GetOutput(),
                     "broadcast: PLAYERSTATE,0,3,65,8,5,0,10,0,0,0,0,0,-1,-1\n"
                     "broadcast: PLAYERSTATE,1,4,65,5,8,0,10,0,0,2,3,0,-1,-1\n"
                     "broadcast: PLAYERSTATE,2,5,90,5,5,0,10,0,0,4,3,0,-1,-1\n"
                     "broadcast: PLAYERSTATE,3,8,100,5,5,0,5,0,0,0,1,0,-1,-1\n"
                     "broadcast: PLAYERSTATE,4,9,165,8,10,0,10,0,0,1,4,0,100,100\n"
                     "broadcast: PLAYERSTATE,5,10,200,11,15,0,10,0,0,3,0,0,-1,-1\n"
                     "broadcast: BIDITEM,1\n"
                     "broadcast: PURCHASEPRICE,90\n"
                     );
  top.ResetOutput();

  ogi.GetOptions().SetOneUpgradePerTurn(false);
  ogi.GetOptions().SetMustBeActiveToBid(true);
  ogi.GetOptions().SetRobotMechanism(ALWAYS_MULTIPLICATIVE);

  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player4",ap3) , true); // should work...

  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetBidState() , HASBID);    BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetHighBid() , 100 );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetBidState() , CURBID);    BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetBidState() , UNBID);     BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetHighBid() , NOT_HIGH_BID );

  BOOST_CHECK_EQUAL( ogi.GetPlayers().GetCurBidPlayer().GetId() , 3 );
  BOOST_CHECK_EQUAL( ogi.GetUnownedItems().GetBidIndex() , (size_t)1);


  BOOST_CHECK_EQUAL( top.GetOutput(),
                     "broadcast: PLAYERSTATE,0,3,65,8,5,0,10,0,0,0,0,0,-1,-1\n"
                     "broadcast: PLAYERSTATE,1,4,65,5,8,0,10,0,0,2,3,0,-1,-1\n"
                     "broadcast: PLAYERSTATE,2,5,90,5,5,0,10,0,0,4,3,0,-1,-1\n"
                     "broadcast: PLAYERSTATE,3,8,100,5,5,0,5,0,0,0,1,0,-1,-1\n"
                     "broadcast: PLAYERSTATE,4,9,165,8,10,0,10,0,0,1,4,0,100,100\n"
                     "broadcast: PLAYERSTATE,5,10,200,11,15,0,10,0,0,3,3,0,-1,-1\n"
                     "broadcast: BIDITEM,1\n"
                     "broadcast: PURCHASEPRICE,90\n"
                     );
  top.ResetOutput();

  ogi.GetOptions().SetOneUpgradePerTurn(true);
  ogi.GetOptions().SetMustBeActiveToBid(false);
  ogi.GetOptions().SetRobotMechanism(SECOND_UNLIMITED);

  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player4",ap3) , true); // should work...

  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetBidState() , UNBID);     BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetBidState() , HASBID);    BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetHighBid() , 100 );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetBidState() , CURBID);    BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetBidState() , UNBID);     BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetHighBid() , NOT_HIGH_BID );

  BOOST_CHECK_EQUAL( ogi.GetPlayers().GetCurBidPlayer().GetId() , 3 );
  BOOST_CHECK_EQUAL( ogi.GetUnownedItems().GetBidIndex() , (size_t)1);


  BOOST_CHECK_EQUAL( top.GetOutput(),
                     "broadcast: PLAYERSTATE,0,3,65,8,5,0,10,0,0,0,0,0,-1,-1\n"
                     "broadcast: PLAYERSTATE,1,4,65,5,8,0,10,0,0,2,3,0,-1,-1\n"
                     "broadcast: PLAYERSTATE,2,5,90,5,5,0,10,0,0,4,3,0,-1,-1\n"
                     "broadcast: PLAYERSTATE,3,8,100,5,5,0,5,0,0,0,1,0,-1,-1\n"
                     "broadcast: PLAYERSTATE,4,9,165,8,10,0,10,0,0,1,4,0,100,100\n"
                     "broadcast: PLAYERSTATE,5,10,200,11,15,0,10,0,0,3,0,0,-1,-1\n"
                     "broadcast: BIDITEM,1\n"
                     "broadcast: PURCHASEPRICE,90\n"
                     );
  top.ResetOutput();

  ogi.GetPlayers()[5].SetHasPurchased(true);

  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Purchase",ogi,"Player4",ap3) , true); // should work...

  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetBidState() , HASBID);    BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetHighBid() , 100 );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetBidState() , CURBID);    BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetBidState() , UNBID);     BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetHighBid() , NOT_HIGH_BID );

  BOOST_CHECK_EQUAL( ogi.GetPlayers().GetCurBidPlayer().GetId() , 3 );
  BOOST_CHECK_EQUAL( ogi.GetUnownedItems().GetBidIndex() , (size_t)1);


  BOOST_CHECK_EQUAL( top.GetOutput(),
                     "broadcast: PLAYERSTATE,0,3,65,8,5,0,10,0,0,0,0,0,-1,-1\n"
                     "broadcast: PLAYERSTATE,1,4,65,5,8,0,10,0,0,2,3,0,-1,-1\n"
                     "broadcast: PLAYERSTATE,2,5,90,5,5,0,10,0,0,4,3,0,-1,-1\n"
                     "broadcast: PLAYERSTATE,3,8,100,5,5,0,5,0,0,0,1,0,-1,-1\n"
                     "broadcast: PLAYERSTATE,4,9,165,8,10,0,10,0,0,1,4,0,100,100\n"
                     "broadcast: PLAYERSTATE,5,10,200,11,15,0,10,0,0,3,3,1,-1,-1\n"
                     "broadcast: BIDITEM,1\n"
                     "broadcast: PURCHASEPRICE,90\n"
                     );
  top.ResetOutput();
}

BOOST_AUTO_TEST_CASE( TransitionTestPURCHASEBID )
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

  ogi.GetUnownedItems().Initialize(6);

  ogi.GetPlayers().DetermineTurnOrder();

  ogi.GetPlayers().StartPurchasePhase();
  ogi.GetPlayers()[2].SetPurchaseState(INTERNAL_DONE);
  ogi.GetPlayers()[1].SetPurchaseState(INTERNALIZING);
  ogi.GetPlayers().PurchaseIncrementTurnOrder();
  // at this point, 5 is done, 4 is active, 3 and 0 are unplayed, 2 is INTERNAL_DONE, and 1 is INTERNALIZING

  // transits from bid back to purchase
  const TransitionType *pBM = GetTransition(osm,"Bid","PURCHASEBID","Purchase",numBidTransitions);

  // never auto
  BOOST_CHECK_EQUAL(pBM->IsAuto("Bid",ogi) , false );


  // the winner (player 4) should owe 90 (bid of 100 with a discount of 10)
  ogi.GetPlayers()[4].GetOwnedItems().AddItem(DATA_LIBRARY);
  ogi.GetUnownedItems().SetShipItem(1,LABORATORY);
  ogi.GetUnownedItems().SetBidIndex(1);

  // and that we know that the bid phase is correct
  // make sure we know about purchase state
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetPurchaseState() , ACTIVE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetPurchaseState() , UNPLAYED );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetPurchaseState() , INTERNAL_DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetPurchaseState() , INTERNALIZING );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetPurchaseState() , UNPLAYED );

  ogi.GetPlayers().StartBidPhase(false,false,100);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetBidState() , UNBID);     BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetBidState() , HASBID);    BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetHighBid() , 100 );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetBidState() , CURBID);    BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetBidState() , UNBID);     BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetHighBid() , NOT_HIGH_BID );

  // allowed if:
  //   i_PlayerName is current bidder index's name
  //   current bidder index's highbid is not NOT_HIGH_BID
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Bid",ogi) , false); // not a player
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Bid",ogi) , false); // not bid yet
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Bid",ogi) , false); // current high bid, but not current bidder.
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Bid",ogi) , false); // current bidder, not high bid
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Bid",ogi) , false); // not in bidding
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Bid",ogi) , false); // not in bidding
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Bid",ogi) , false); // not not bid yet.

  ogi.GetPlayers().IncrementBid(PASS);

  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetPurchaseState() , ACTIVE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetPurchaseState() , UNPLAYED );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetPurchaseState() , INTERNAL_DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetPurchaseState() , INTERNALIZING );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetPurchaseState() , UNPLAYED );

  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetBidState() , UNBID);     BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetBidState() , HASBID);    BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetHighBid() , 100 );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetBidState() , PASS);      BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetBidState() , CURBID);    BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetHighBid() , NOT_HIGH_BID );

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Bid",ogi) , false); // not a player
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Bid",ogi) , false); // not bid yet
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Bid",ogi) , false); // current high bid, but not current bidder.
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Bid",ogi) , false); // passed
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Bid",ogi) , false); // not in bidding
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Bid",ogi) , false); // not in bidding
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Bid",ogi) , false); // cur bidder

  ogi.GetPlayers().IncrementBid(PASS_OUT);

  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetPurchaseState() , ACTIVE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetPurchaseState() , UNPLAYED );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetPurchaseState() , INTERNAL_DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetPurchaseState() , INTERNALIZING );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetPurchaseState() , UNPLAYED );

  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetBidState() , CURBID);    BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetBidState() , HASBID);    BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetHighBid() , 100 );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetBidState() , PASS);      BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetHighBid() , NOT_HIGH_BID );

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Bid",ogi) , false); // not a player
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Bid",ogi) , false); // cur bidder
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Bid",ogi) , false); // current high bid, but not current bidder.
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Bid",ogi) , false); // passed
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Bid",ogi) , false); // not in bidding
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Bid",ogi) , false); // not in bidding
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Bid",ogi) , false); // passed out

  ogi.GetPlayers().IncrementBid(PASS_OUT);

  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetPurchaseState() , ACTIVE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetPurchaseState() , UNPLAYED );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetPurchaseState() , INTERNAL_DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetPurchaseState() , INTERNALIZING );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetPurchaseState() , UNPLAYED );

  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetBidState() , CURBID);    BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetHighBid() , 100 );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetBidState() , PASS);      BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetHighBid() , NOT_HIGH_BID );

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Bid",ogi) , false); // not a player
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Bid",ogi) , false); // passed out
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Bid",ogi) , true); // current high bid, and current bidder...win!
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Bid",ogi) , false); // passed
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Bid",ogi) , false); // not in bidding
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Bid",ogi) , false); // not in bidding
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Bid",ogi) , false); // passed out

  ogi.GetPlayers()[4].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,30,false,false));
  ogi.GetPlayers()[4].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,40,false,false));
  ogi.GetPlayers()[4].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,20,false,false));

  // 
  // incoming action:
  //   std::string discardstring
  // body:
  //   fail on any error in discardstring, including if amount discard is less than 
  //     current bidder index's highbid - current bidder index's discounts

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Bid",ogi) , true); // active
  ActionParser ap3("PURCHASEBID,1111");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Bid",ogi,"Player4",ap3) , false); // bad delete string
  BOOST_CHECK_EQUAL(top.GetOutput() , "unicast: Player4 -- ERROR,Mismatched delete string length\n" );
  top.ResetOutput();

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Bid",ogi) , true); // active
  ActionParser ap4("PURCHASEBID,11");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Bid",ogi,"Player4",ap4) , false); // bad delete string
  BOOST_CHECK_EQUAL(top.GetOutput() , "unicast: Player4 -- ERROR,Mismatched delete string length\n" );
  top.ResetOutput();

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Bid",ogi) , true); // active
  ActionParser ap5("PURCHASEBID,11V");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Bid",ogi,"Player4",ap5) , false); // bad delete string
  BOOST_CHECK_EQUAL(top.GetOutput() ,  "unicast: Player4 -- ERROR,Illegal characters in delete string\n" );
  top.ResetOutput();

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Bid",ogi) , true); // active
  ActionParser ap6("PURCHASEBID,110");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Bid",ogi,"Player4",ap6) , false); // didn't spend enough.
  BOOST_CHECK_EQUAL(top.GetOutput() ,  "unicast: Player4 -- ERROR,You need to spend more to buy that.\n" );
  top.ResetOutput();

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Bid",ogi) , true); // active
  ActionParser ap7("PURCHASEBID,111");

  ogi.GetPlayers()[4].AddHumans(1);

  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Bid",ogi,"Player4",ap7) , true); // should be fine

  //
  // Factory newfac = player.GetOwnedItems().AddItem(item bid on)
  // if newfac != NO_FACTORY player.GetFactories().AddFactory(newfac);
  //
  // player's highbid = NOT_HIGH_BID
  // player's HasPurchased = true
  // player's Bid State = PASS_OUT
  // current bid item = NO_ITEM (UnownedItems.SetShipItem())
  // current bid item (UnownedItems.SetBidIndex()) = NO_BID_INDEX
  //
  // report update Player's Commodity Hand (Back and Front)
  // report update Player's Owned Items
  // report update Player's Factories
  // report update Player's state

  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetFactories().GetFactoryDescription() , "|RH|");
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetHighBid() , NOT_HIGH_BID);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].HasPurchased() , true );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetBidState() , PASS_OUT );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetPurchaseState() , ACTIVE );
  BOOST_CHECK_EQUAL( ogi.GetUnownedItems().GetShipItem(1) , NO_ITEM );
  BOOST_CHECK_EQUAL( ogi.GetUnownedItems().GetBidIndex() , NO_BID_INDEX );

  BOOST_CHECK_EQUAL(top.GetOutput() ,  
                    "broadcast: SPENDS,0,4,1,8,90,|OR40-OR30-OR20|\n"
                    "broadcast: SUPPLYSHIP,1,0\n"
                    "broadcast: PLAYERCARDBACKS,4,||\n"
                    "unicast: Player4 -- PLAYERCARDS,||\n"
                    "unicast: Player4 -- PLAYERPURCHASEINFO,0,0,0,0,0,0,0\n"
                    "broadcast: PLAYERITEMS,4,0,10\n"
                    "broadcast: PLAYERITEMS,4,1,7\n"
                    "broadcast: PLAYERITEMS,4,2,1\n"
                    "broadcast: PLAYERITEMS,4,3,8\n"
                    "broadcast: PLAYERFACTORIES,4,|RH|\n"
                    "broadcast: PLAYERSTATE,4,16,265,8,10,-1,10,1,0,1,3,1,-1,-1\n"
                    "broadcast: BIDITEM,1000\n"
                    "broadcast: COMMODITYSTOCK,1,26,3,0,3\n"
                    "broadcast: COMMODITYSTOCK,2,39,0,30,7\n"
                    "broadcast: COMMODITYSTOCK,3,53,0,44,10\n"
                    "broadcast: COMMODITYSTOCK,4,34,0,56,13\n"
                    "broadcast: COMMODITYSTOCK,5,16,0,0,17\n"
                    "broadcast: COMMODITYSTOCK,6,23,0,88,20\n"
                    "broadcast: COMMODITYSTOCK,7,14,0,0,30\n"
                    "broadcast: COMMODITYSTOCK,8,12,0,0,40\n"
                    "broadcast: COMMODITYSTOCK,9,12,0,0,50\n"
                    "broadcast: PURCHASEDATA,1,4,8,90\n"
                    );
  top.ResetOutput();
}


BOOST_AUTO_TEST_CASE( TransitionTestBID )
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

  ogi.GetUnownedItems().Initialize(6);

  ogi.GetPlayers().DetermineTurnOrder();

  ogi.GetPlayers().StartPurchasePhase();
  ogi.GetPlayers()[2].SetPurchaseState(INTERNAL_DONE);
  ogi.GetPlayers()[1].SetPurchaseState(INTERNALIZING);
  ogi.GetPlayers().PurchaseIncrementTurnOrder();
  // at this point, 5 is done, 4 is active, 3 and 0 are unplayed, 2 is INTERNAL_DONE, and 1 is INTERNALIZING

  // cyclic on bid.
  const TransitionType *pBM = GetTransition(osm,"Bid","BID","Bid",numBidTransitions);

  // never auto
  BOOST_CHECK_EQUAL(pBM->IsAuto("Bid",ogi) , false );


  ogi.GetPlayers()[3].GetOwnedItems().AddItem(DATA_LIBRARY);
  ogi.GetUnownedItems().SetShipItem(1,LABORATORY);
  ogi.GetUnownedItems().SetBidIndex(1);

  // and that we know that the bid phase is correct
  // make sure we know about purchase state
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetPurchaseState() , ACTIVE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetPurchaseState() , UNPLAYED );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetPurchaseState() , INTERNAL_DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetPurchaseState() , INTERNALIZING );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetPurchaseState() , UNPLAYED );

  ogi.GetPlayers().StartBidPhase(false,false,50);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetBidState() , UNBID);     BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetBidState() , HASBID);    BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetHighBid() , 50 );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetBidState() , CURBID);    BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetBidState() , UNBID);     BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetHighBid() , NOT_HIGH_BID );

  // allowed if
  //   i_PlayerName is current bidder index's name
  //   player has more funds than current bid - discounts
  //   player's highbid is NOT_HIGH_BID
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Bid",ogi) , false); // not a player
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Bid",ogi) , false); // not bid yet
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Bid",ogi) , false); // current high bid, but not current bidder.
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Bid",ogi) , false); // current bidder, but not enough money.
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Bid",ogi) , false); // not in bidding
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Bid",ogi) , false); // not in bidding
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Bid",ogi) , false); // not not bid yet.

  // player 4 bid 50, as long as player 3 has at least 41 (51 - discount of 10)
  ogi.GetPlayers()[3].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,30,false,false));
  ogi.GetPlayers()[3].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,10,false,false));

  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Bid",ogi) , false); // current bidder, but not enough money.
  ogi.GetPlayers()[3].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,1,false,false));
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Bid",ogi) , true); // current bidder, has enough money.

  ogi.GetPlayers()[3].SetHighBid(13);
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Bid",ogi) , false); // current bidder, has enough money, but already has high bid.
  ogi.GetPlayers()[3].SetHighBid(NOT_HIGH_BID);

  // incoming action:
  //   int i_newbid min = current bid + 1 , max = player funds + discounts

  // player's funds are 30+10+1+10, so max bid is that + 10 discount...
  ogi.GetPlayers()[3].GetCommodityHand().AddCommodity(Commodity(ORE_COMMODITY,10,false,false));

  // setting last bids to something
  ogi.GetPlayers()[0].SetLastBid(10);
  ogi.GetPlayers()[1].SetLastBid(11);
  ogi.GetPlayers()[2].SetLastBid(12);
  ogi.GetPlayers()[3].SetLastBid(13);
  ogi.GetPlayers()[4].SetLastBid(14);
  ogi.GetPlayers()[5].SetLastBid(15);


  ActionParser ap1("BID,50");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Bid",ogi,"Player3",ap1) , false); // should be fine
  BOOST_CHECK_EQUAL(top.GetOutput() ,  "unicast: Player3 -- ERROR,Action BID variable i_bid is below minimum\n");
  top.ResetOutput();

  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetLastBid() , 10);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetLastBid() , 11);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetLastBid() , 12);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetLastBid() , 13);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetLastBid() , 14);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetLastBid() , 15);


  ActionParser ap2("BID,62");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Bid",ogi,"Player3",ap2) , false); // should be fine
  BOOST_CHECK_EQUAL(top.GetOutput() ,  "unicast: Player3 -- ERROR,Action BID variable i_bid is above maximum\n");
  top.ResetOutput();

  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetLastBid() , 10);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetLastBid() , 11);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetLastBid() , 12);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetLastBid() , 13);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetLastBid() , 14);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetLastBid() , 15);


  ActionParser ap3("BID,55");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Bid",ogi,"Player3",ap3) , true); // should be fine

  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetLastBid() , 10);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetLastBid() , 11);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetLastBid() , 12);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetLastBid() , 55);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetLastBid() , 14);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetLastBid() , 15);



  //
  // body:
  //   prior players highbid = NOT_HIGH_BID
  //   current player's highbid = i_newbid
  //   current player's bid state = HASBID
  //
  //   increment current bidder index to next player in sequence not in PASS_OUT state, set state of new index to CURBID
  //
  //   report update prior high bid player's state
  //   report update i_PlayerName's state
  //   report update current bidder index's state

  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetBidState() , UNBID);     BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetBidState() , HASBID);    BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetBidState() , HASBID);    BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetHighBid() , 55 );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetBidState() , CURBID);    BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers().GetCurBidPlayer().GetId() , 0 );
  
  BOOST_CHECK_EQUAL( top.GetOutput() ,
                     "broadcast: PLAYERSTATE,4,8,150,8,10,-1,10,0,0,1,4,0,-1,14\n"
                     "broadcast: PLAYERSTATE,3,9,115,5,5,0,5,0,0,0,4,0,55,55\n"
                     "broadcast: PLAYERSTATE,0,3,65,8,5,0,10,0,0,0,1,0,-1,10\n"
                     "broadcast: PURCHASEPRICE,45\n"
                     );

}

BOOST_AUTO_TEST_CASE( TransitionTestPASS )
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

  ogi.GetUnownedItems().Initialize(6);

  ogi.GetPlayers().DetermineTurnOrder();

  ogi.GetPlayers().StartPurchasePhase();
  ogi.GetPlayers()[2].SetPurchaseState(INTERNAL_DONE);
  ogi.GetPlayers()[1].SetPurchaseState(INTERNALIZING);
  ogi.GetPlayers().PurchaseIncrementTurnOrder();
  // at this point, 5 is done, 4 is active, 3 and 0 are unplayed, 2 is INTERNAL_DONE, and 1 is INTERNALIZING

  // cyclic on bid.
  const TransitionType *pBM = GetTransition(osm,"Bid","PASS","Bid",numBidTransitions);

  // never auto
  BOOST_CHECK_EQUAL(pBM->IsAuto("Bid",ogi) , false );


  ogi.GetPlayers()[3].GetOwnedItems().AddItem(DATA_LIBRARY);
  ogi.GetUnownedItems().SetShipItem(1,LABORATORY);
  ogi.GetUnownedItems().SetBidIndex(1);

  // and that we know that the bid phase is correct
  // make sure we know about purchase state
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetPurchaseState() , ACTIVE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetPurchaseState() , UNPLAYED );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetPurchaseState() , INTERNAL_DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetPurchaseState() , INTERNALIZING );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetPurchaseState() , UNPLAYED );

  ogi.GetPlayers().StartBidPhase(false,false,50);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetBidState() , UNBID);     BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetBidState() , HASBID);    BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetHighBid() , 50 );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetBidState() , CURBID);    BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetBidState() , UNBID);     BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetHighBid() , NOT_HIGH_BID );

  // allowed if
  //   i_PlayerName is current bidder index's name
  //   player's highbid is NOT_HIGH_BID
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Bid",ogi) , false); // not a player
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Bid",ogi) , false); // not bid yet
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Bid",ogi) , false); // current high bid, but not current bidder.
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Bid",ogi) , true); // current bidder, 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Bid",ogi) , false); // not in bidding
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Bid",ogi) , false); // not in bidding
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Bid",ogi) , false); // not not bid yet.

  ogi.GetPlayers()[3].SetHighBid(13);
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Bid",ogi) , false); // current bidder, has enough money, but already has high bid.
  ogi.GetPlayers()[3].SetHighBid(NOT_HIGH_BID);

  ActionParser ap3("PASS");
  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Bid",ogi,"Player4",ap3) , true); // should be fine

  // body:
  //   i_PlayerName's bid state becomes PASS
  //   increment current bidder index to next player in sequence not in PASS_OUT state, set state of new index to CURBID
  // 
  //   report update i_PlayerName's state
  //   report update current bidder index's state

  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetBidState() , UNBID);     BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetBidState() , HASBID);    BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetHighBid() , 50 );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetBidState() , PASS);      BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetBidState() , CURBID);    BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers().GetCurBidPlayer().GetId() , 0 );
  
  BOOST_CHECK_EQUAL( top.GetOutput() ,
                     "broadcast: PLAYERSTATE,3,9,115,5,5,0,5,0,0,0,2,0,-1,-1\n"
                     "broadcast: PLAYERSTATE,0,3,65,8,5,0,10,0,0,0,1,0,-1,-1\n"
                     );

}

BOOST_AUTO_TEST_CASE( TransitionTestPASSOUT )
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

  ogi.GetUnownedItems().Initialize(6);

  ogi.GetPlayers().DetermineTurnOrder();

  ogi.GetPlayers().StartPurchasePhase();
  ogi.GetPlayers()[2].SetPurchaseState(INTERNAL_DONE);
  ogi.GetPlayers()[1].SetPurchaseState(INTERNALIZING);
  ogi.GetPlayers().PurchaseIncrementTurnOrder();
  // at this point, 5 is done, 4 is active, 3 and 0 are unplayed, 2 is INTERNAL_DONE, and 1 is INTERNALIZING

  // cyclic on bid.
  const TransitionType *pBM = GetTransition(osm,"Bid","PASSOUT","Bid",numBidTransitions);

  // never auto
  BOOST_CHECK_EQUAL(pBM->IsAuto("Bid",ogi) , false );


  ogi.GetPlayers()[3].GetOwnedItems().AddItem(DATA_LIBRARY);
  ogi.GetUnownedItems().SetShipItem(1,LABORATORY);
  ogi.GetUnownedItems().SetBidIndex(1);

  // and that we know that the bid phase is correct
  // make sure we know about purchase state
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetPurchaseState() , DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetPurchaseState() , ACTIVE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetPurchaseState() , UNPLAYED );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetPurchaseState() , INTERNAL_DONE );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetPurchaseState() , INTERNALIZING );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetPurchaseState() , UNPLAYED );

  ogi.GetPlayers().StartBidPhase(false,false,50);
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetBidState() , UNBID);     BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetBidState() , HASBID);    BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetHighBid() , 50 );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetBidState() , CURBID);    BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetBidState() , UNBID);     BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetHighBid() , NOT_HIGH_BID );

  // allowed if
  //   i_PlayerName is current bidder index's name
  //   player's highbid is NOT_HIGH_BID
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player9","Bid",ogi) , false); // not a player
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player5","Bid",ogi) , true); // not bid yet
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player4","Bid",ogi) , false); // current high bid, but not current bidder.
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Bid",ogi) , true); // current bidder, 
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player2","Bid",ogi) , false); // not in bidding
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player1","Bid",ogi) , false); // not in bidding
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player0","Bid",ogi) , true); // not not bid yet.

  ogi.GetPlayers()[3].SetHighBid(13);
  BOOST_CHECK_EQUAL(pBM->IsLegal("Player3","Bid",ogi) , false); // current bidder, has enough money, but already has high bid.
  ogi.GetPlayers()[3].SetHighBid(NOT_HIGH_BID);

  ActionParser ap3("PASS");

  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Bid",ogi,"Player5",ap3) , true); // should be fine
  // body for non-active player passing out:
  //   i_PlayerName's bid state becomes PASS_OUT
  // 
  //   report update i_PlayerName's state
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetBidState() , HASBID);    BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetHighBid() , 50 );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetBidState() , CURBID);    BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetBidState() , UNBID);     BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers().GetCurBidPlayer().GetId() , 3 );

  BOOST_CHECK_EQUAL( top.GetOutput() ,
                     "broadcast: PLAYERSTATE,5,10,200,11,15,0,10,0,0,3,3,0,-1,-1\n"
                     );
  top.ResetOutput();


  BOOST_CHECK_EQUAL(pBM->ExecuteAction("Bid",ogi,"Player3",ap3) , true); // should be fine

  // body:
  //   i_PlayerName's bid state becomes PASS_OUT
  //   increment current bidder index to next player in sequence not in PASS_OUT state, set state of new index to CURBID
  // 
  //   report update i_PlayerName's state
  //   report update current bidder index's state

  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( ogi.GetPlayers()[5].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetBidState() , HASBID);    BOOST_CHECK_EQUAL( ogi.GetPlayers()[4].GetHighBid() , 50 );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( ogi.GetPlayers()[3].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( ogi.GetPlayers()[2].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetBidState() , PASS_OUT);  BOOST_CHECK_EQUAL( ogi.GetPlayers()[1].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetBidState() , CURBID);    BOOST_CHECK_EQUAL( ogi.GetPlayers()[0].GetHighBid() , NOT_HIGH_BID );
  BOOST_CHECK_EQUAL( ogi.GetPlayers().GetCurBidPlayer().GetId() , 0 );
  
  BOOST_CHECK_EQUAL( top.GetOutput() ,
                     "broadcast: PLAYERSTATE,3,9,115,5,5,0,5,0,0,0,3,0,-1,-1\n"
                     "broadcast: PLAYERSTATE,0,3,65,8,5,0,10,0,0,0,1,0,-1,-1\n"
                     );

}

