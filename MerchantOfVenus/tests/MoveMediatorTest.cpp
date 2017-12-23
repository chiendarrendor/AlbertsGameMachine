#include "MoveMediator.hpp"
#include "Players.hpp"
#include "MapData.hpp"
#include "MapOverlay.hpp"
#include "Ship.hpp"

#include <boost/test/auto_unit_test.hpp>

BOOST_AUTO_TEST_CASE( TestStartMove )
{
  Players pls;
  pls.add("Foo Bar");
  pls[0].AddToken(Ship::GetShipOfClass(SCOUT));
  pls.RandomizeTurnOrder();
  MapData md("../MerchantOfVenusMap.xml");
  MapOverlay mo(md);

  MoveMediator mm(pls,mo);

  // normal case, no Mulligan or Auto Pilot (scout rolls 3 dice)
  mm.StartMove();
  BOOST_CHECK(mm.GetDice().size() == 3);
  int ct = 0;
  for ( size_t i = 0 ; i < mm.GetDice().size() ; ++i)
  {
    ct += mm.GetDice()[i];
  }
  BOOST_CHECK(mm.GetMP() == ct);
  BOOST_CHECK(mm.CanMulligan() == false);

  // add Mulligan Gear
  pls[0].AddToken(Token::Relic("Mulligan Gear",150));
  // clipper has 4 dice.
  pls[0].ExchangeShip(Ship::GetShipOfClass(CLIPPER));

  mm.StartMove();
  BOOST_CHECK(mm.GetDice().size() == 4);
  ct = 0;
  for ( size_t i = 0 ; i < mm.GetDice().size() ; ++i)
  {
    ct += mm.GetDice()[i];
  }
  BOOST_CHECK(mm.GetMP() == ct);
  BOOST_CHECK(mm.CanMulligan() == true);

  // add Auto Pilot (but don't turn it on)
  pls[0].AddToken(Token::Relic("Auto Pilot",150));

  // this is stochastic....the first die might be 4,
  // but shouldn't be 4 every time.
  int ctr = 0;
  while(ctr < 10)
  {
    mm.StartMove();
    if (mm.GetDice()[0] != 4) break;
    ++ctr;
  }
  BOOST_CHECK(ctr < 10);
  BOOST_CHECK(mm.GetDice().size() == 4);
  ct = 0;
  for ( size_t i = 0 ; i < mm.GetDice().size() ; ++i)
  {
    ct += mm.GetDice()[i];
  }
  BOOST_CHECK(mm.GetMP() == ct);
  BOOST_CHECK(mm.CanMulligan() == true);

  // again, stochastic, but with AP on, _every_ time
  // we roll, the first die should be 4.
  pls[0].GetAdvances().SetSwitchables("AP");
  // freighter has 2 dice.
  pls[0].ExchangeShip(Ship::GetShipOfClass(FREIGHTER));
  ctr = 0;
  while(ctr < 10)
  {
    mm.StartMove();
    BOOST_CHECK(mm.GetDice()[0] == 4);
    ++ctr;
  }

  BOOST_CHECK(mm.GetDice().size() == 2);
  ct = 0;
  for ( size_t i = 0 ; i < mm.GetDice().size() ; ++i)
  {
    ct += mm.GetDice()[i];
  }
  BOOST_CHECK(mm.GetMP() == ct);
  BOOST_CHECK(mm.CanMulligan() == true);
}

BOOST_AUTO_TEST_CASE( TestGetPenaltyCost )
{
  Players pls;
  pls.add("Foo Bar");
  pls[0].AddToken(Ship::GetShipOfClass(SCOUT));
  pls.RandomizeTurnOrder();
  MapData md("../MerchantOfVenusMap.xml");
  MapOverlay mo(md);

  MoveMediator mm(pls,mo);

  // cost depends on size of penalty space, and
  // how much shield player has.
  // (extra wrinkle...a colored space matching
  // a player's drives is zero cost)
  BOOST_CHECK(mm.GetPenaltyCost() == 0);

  // a red 30 point space
  pls[0].SetLocation("SN_loop_01");
  BOOST_CHECK(mm.GetPenaltyCost() == 30);

  pls[0].AddToken(Token::Drive(true,true,false,100));
  pls[0].GetAdvances().SetSwitchables("CD");
  BOOST_CHECK(mm.GetPenaltyCost() == 0);

  pls[0].GetAdvances().SetSwitchables("");

  BOOST_CHECK(mm.GetPenaltyCost() == 30);

  pls[0].AddToken(Token::Shield(true,100));
  
  BOOST_CHECK(mm.GetPenaltyCost() == 10);

  pls[0].AddToken(Token::Shield(true,100));
  
  BOOST_CHECK(mm.GetPenaltyCost() == 0);
}

  
BOOST_AUTO_TEST_CASE( TestAddAdjacents )
{
  Players pls;
  pls.add("Foo Bar");
  pls[0].AddToken(Ship::GetShipOfClass(SCOUT));
  pls.RandomizeTurnOrder();
  MapData md("../MerchantOfVenusMap.xml");
  MapOverlay mo(md);

  MoveMediator mm(pls,mo);
  mm.StartMove();

  BOOST_CHECK(mm.GetCurrentDests() == "");
  
  mm.AddAdjacents();

  BOOST_CHECK(mm.GetCurrentDests() ==
              "ib_gb_3!walk!0/cloud_body_8!walk!0/gb_cw_1!walk!0");
  
  pls[0].SetLocation("cloud_body_8");
  mm.AddAdjacents();

  BOOST_CHECK(mm.GetCurrentDests() ==
              "cloud_body_4!walk!0!1456/Galactic Base!walk!0!2/cloud_body_7!walk!0!3");

  pls[0].SetLocation("Southern Comfort");
  mm.AddAdjacents();

  BOOST_CHECK(mm.GetCurrentDests() ==
              "cw_loop_4!walk!0");

  pls[0].AddToken(Token::Relic("Air Foil",100));
  mm.AddAdjacents();

  BOOST_CHECK(mm.GetCurrentDests() == 
              "Shoemaker Levee!walk!0/cw_loop_4!walk!0");
}


BOOST_AUTO_TEST_CASE( TestAddTelegatesJumpStartPilotless )
{
  Players pls;
  pls.add("Foo Bar");
  pls[0].AddToken(Ship::GetShipOfClass(SCOUT));
  pls.RandomizeTurnOrder();
  MapData md("../MerchantOfVenusMap.xml");

  // srand seed 5 has the following telegates
  // 1: AS_3_4
  // 2: AS_7_6
  // 3: WTS_loop_2
  // 4: IM_loop_03
  // 5: PP_loop_05
  // 6: jw_loop_8

  srand(5);
  MapOverlay mo(md);
  
  MoveMediator mm(pls,mo);

  // cases: 
  //   first, but no jumpstart
  mm.StartMove();
  mm.AddAdjacents();
  mm.AddTelegates();
  BOOST_CHECK(mm.GetCurrentDests() ==
              "ib_gb_3!walk!0/cloud_body_8!walk!0/gb_cw_1!walk!0");

  //   jumpstart, but not first
  mm.ClearFirst();
  pls[0].AddToken(Token::Relic("Jump Start",50));
  pls[0].GetAdvances().SetSwitchables("JS");
  mm.AddAdjacents();
  mm.AddTelegates();
  BOOST_CHECK(mm.GetCurrentDests() ==
              "ib_gb_3!walk!0/cloud_body_8!walk!0/gb_cw_1!walk!0");

  //   jumpstart and first, but no telegates
  mm.StartMove();
  mm.AddAdjacents();
  mm.AddTelegates();
  BOOST_CHECK(mm.GetCurrentDests() ==
              "ib_gb_3!walk!0/cloud_body_8!walk!0/gb_cw_1!walk!0");

  //   jumpstart and first, some telegates
  mo.MakeSpacePublic("AS_3_4");
  mo.MakeSpacePublic("WTS_loop_2");
  mo.MakeSpacePublic("PP_loop_05");
  mm.AddAdjacents();
  mm.AddTelegates();
  BOOST_CHECK(mm.GetCurrentDests() ==
              "ib_gb_3!walk!0!246/cloud_body_8!walk!0!246/gb_cw_1!walk!0!246/"
              "AS_3_4!jump!0!1/WTS_loop_2!jump!0!3/PP_loop_05!jump!0!5");

  //   jumpstart and first, all telegates
  mo.MakeSpacePublic("AS_7_6");
  mo.MakeSpacePublic("IM_loop_03");
  mo.MakeSpacePublic("jw_loop_8");
  mm.AddAdjacents();
  mm.AddTelegates();
  BOOST_CHECK(mm.GetCurrentDests() ==
              "AS_3_4!jump!0!1/AS_7_6!jump!0!2/WTS_loop_2!jump!0!3/"
              "IM_loop_03!jump!0!4/PP_loop_05!jump!0!5/jw_loop_8!jump!0!6");
  
}

BOOST_AUTO_TEST_CASE( TestAddTelegatesJumpStartPiloted )
{
  Players pls;
  pls.add("Foo Bar");
  pls[0].AddToken(Ship::GetShipOfClass(SCOUT));
  pls.RandomizeTurnOrder();
  MapData md("../MerchantOfVenusMap.xml");

  pls[0].SetLocation("cloud_body_8");

  srand(5);
  MapOverlay mo(md);

  MoveMediator mm(pls,mo);

  // cases: 
  //   first, but no jumpstart
  mm.StartMove();
  mm.AddAdjacents();
  mm.AddTelegates();
  BOOST_CHECK(mm.GetCurrentDests() ==
    "cloud_body_4!walk!0!1456/Galactic Base!walk!0!2/cloud_body_7!walk!0!3");

  //   jumpstart, but not first
  mm.ClearFirst();
  pls[0].AddToken(Token::Relic("Jump Start",50));
  pls[0].GetAdvances().SetSwitchables("JS");
  mm.AddAdjacents();
  mm.AddTelegates();
  BOOST_CHECK(mm.GetCurrentDests() ==
    "cloud_body_4!walk!0!1456/Galactic Base!walk!0!2/cloud_body_7!walk!0!3");

  //   jumpstart and first, but no telegates
  mm.StartMove();
  mm.AddAdjacents();
  mm.AddTelegates();
  BOOST_CHECK(mm.GetCurrentDests() ==
    "cloud_body_4!walk!0!1456/Galactic Base!walk!0!2/cloud_body_7!walk!0!3");

  //   jumpstart and first, some telegates
  mo.MakeSpacePublic("AS_3_4");
  mo.MakeSpacePublic("WTS_loop_2");
  mo.MakeSpacePublic("PP_loop_05");
  mm.AddAdjacents();
  mm.AddTelegates();

  BOOST_CHECK(mo.GetTeleGate(1) != NULL);
  BOOST_CHECK(mo.GetTeleGate(2) == NULL);
  BOOST_CHECK(mo.GetTeleGate(3) != NULL);
  BOOST_CHECK(mo.GetTeleGate(4) == NULL);
  BOOST_CHECK(mo.GetTeleGate(5) != NULL);
  BOOST_CHECK(mo.GetTeleGate(6) == NULL);

  BOOST_CHECK(mm.GetCurrentDests() ==
              "cloud_body_4!walk!0!46/Galactic Base!walk!0!2/"
              "AS_3_4!jump!0!1/WTS_loop_2!jump!0!3/PP_loop_05!jump!0!5");

  //   jumpstart and first, all telegates
  mo.MakeSpacePublic("AS_7_6");
  mo.MakeSpacePublic("IM_loop_03");
  mo.MakeSpacePublic("jw_loop_8");
  mm.AddAdjacents();
  mm.AddTelegates();
  BOOST_CHECK(mm.GetCurrentDests() ==
              "AS_3_4!jump!0!1/AS_7_6!jump!0!2/WTS_loop_2!jump!0!3/"
              "IM_loop_03!jump!0!4/PP_loop_05!jump!0!5/jw_loop_8!jump!0!6");
}


// there are no pilot-numbered spaces that have a telegate in them
BOOST_AUTO_TEST_CASE( TestAddTelegatesTeleGate )
{
  Players pls;
  pls.add("Foo Bar");
  pls[0].AddToken(Ship::GetShipOfClass(SCOUT));
  pls.RandomizeTurnOrder();
  MapData md("../MerchantOfVenusMap.xml");

  // srand seed 5 has the following telegates
  // 1: AS_3_4
  // 2: AS_7_6
  // 3: WTS_loop_2
  // 4: IM_loop_03
  // 5: PP_loop_05
  // 6: jw_loop_8

  srand(5);
  MapOverlay mo(md);
  
  MoveMediator mm(pls,mo);

  // cases: 
  //   not telegate
  mm.StartMove();
  mm.AddAdjacents();
  mm.AddTelegates();
  BOOST_CHECK(mm.GetCurrentDests() ==
              "ib_gb_3!walk!0/cloud_body_8!walk!0/gb_cw_1!walk!0");


  // not telegate, with Gate Lock On
  pls[0].AddToken(Token::Relic("Gate Lock",70));
  pls[0].GetAdvances().SetSwitchables("GL");
  mm.AddAdjacents();
  mm.AddTelegates();
  BOOST_CHECK(mm.GetCurrentDests() ==
              "ib_gb_3!walk!0/cloud_body_8!walk!0/gb_cw_1!walk!0");


  // telegate, gate lock on. (and not first, just to check)
  // but no available tgates. (the one we're on should not occur)
  mm.ClearFirst();
  pls[0].SetLocation("AS_3_4");
  mo.MakeSpacePublic("AS_3_4");
  mm.AddAdjacents();
  mm.AddTelegates();

  BOOST_CHECK(mm.GetCurrentDests() ==
              "AS_2_3!walk!0/AS_3_3!walk!0/AS_4_4!walk!0");

  // turn off gate lock, but still no available tgates
  pls[0].GetAdvances().SetSwitchables("");

  mm.AddAdjacents();
  mm.AddTelegates();

  BOOST_CHECK(mm.GetCurrentDests() ==
              "AS_2_3!walk!0/AS_3_3!walk!0/AS_4_4!walk!0");
  
  // add some telegates
  mo.MakeSpacePublic("WTS_loop_2");
  mo.MakeSpacePublic("PP_loop_05");
  mm.AddAdjacents();
  mm.AddTelegates();
  
  BOOST_CHECK(mm.GetCurrentDests() ==
              "AS_2_3!walk!0!1246/AS_3_3!walk!0!1246/AS_4_4!walk!0!1246/"
              "WTS_loop_2!jump!0!3/PP_loop_05!jump!0!5");

  // add the rest. (we will never delete all of these, because
  // we can't jump to the one we're standing on.
  mo.MakeSpacePublic("AS_7_6");
  mo.MakeSpacePublic("IM_loop_03");
  mo.MakeSpacePublic("jw_loop_8");
  mm.AddAdjacents();
  mm.AddTelegates();
  BOOST_CHECK(mm.GetCurrentDests() ==
              "AS_2_3!walk!0!1/AS_3_3!walk!0!1/AS_4_4!walk!0!1/"
              "AS_7_6!jump!0!2/WTS_loop_2!jump!0!3/"
              "IM_loop_03!jump!0!4/PP_loop_05!jump!0!5/jw_loop_8!jump!0!6");
  
}

// next test case.  Jump Start from a telegate space
// (should act no different from a normal telegate)
BOOST_AUTO_TEST_CASE( TestAddTelegatesTeleGateJumpStart )
{
  Players pls;
  pls.add("Foo Bar");
  pls[0].AddToken(Ship::GetShipOfClass(SCOUT));
  pls.RandomizeTurnOrder();
  MapData md("../MerchantOfVenusMap.xml");

  // srand seed 5 has the following telegates
  // 1: AS_3_4
  // 2: AS_7_6
  // 3: WTS_loop_2
  // 4: IM_loop_03
  // 5: PP_loop_05
  // 6: jw_loop_8

  srand(5);
  MapOverlay mo(md);
  MoveMediator mm(pls,mo);

  pls[0].AddToken(Token::Relic("Jump Start",100));
  pls[0].GetAdvances().SetSwitchables("JS");
  pls[0].SetLocation("AS_3_4");
  mo.MakeSpacePublic("AS_3_4");
  mo.MakeSpacePublic("WTS_loop_2");
  mo.MakeSpacePublic("PP_loop_05");
  mm.StartMove();
  mm.AddAdjacents();
  mm.AddTelegates();
  
  BOOST_CHECK(mm.GetCurrentDests() ==
              "AS_2_3!walk!0!1246/AS_3_3!walk!0!1246/AS_4_4!walk!0!1246/"
              "WTS_loop_2!jump!0!3/PP_loop_05!jump!0!5");
}

BOOST_AUTO_TEST_CASE( TestRemoveBacktracks )
{
  Players pls;
  pls.add("Foo Bar");
  pls[0].AddToken(Ship::GetShipOfClass(SCOUT));
  pls.RandomizeTurnOrder();
  MapData md("../MerchantOfVenusMap.xml");

  // srand seed 5 has the following telegates
  // 1: AS_3_4
  // 2: AS_7_6
  // 3: WTS_loop_2
  // 4: IM_loop_03
  // 5: PP_loop_05
  // 6: jw_loop_8

  srand(5);
  MapOverlay mo(md);
  MoveMediator mm(pls,mo);

  mo.MakeSpacePublic("AS_3_4");
  mo.MakeSpacePublic("AS_7_6");

  pls[0].SetLocation("AS_3_4");

  mm.StartMove();
  mm.AddAdjacents();
  mm.AddTelegates();
  mm.CullBacktracks();

  BOOST_CHECK(mm.GetCurrentDests() ==
              "AS_2_3!walk!0!13456/AS_3_3!walk!0!13456/AS_4_4!walk!0!13456/"
              "AS_7_6!jump!0!2");

  // ok...pretending we've moved from AS_2_3 to AS_3_4 before
  mm.AddToBacks("AS_2_3","AS_3_4");
  mm.AddToBacks("AS_7_6","AS_3_4");

  mm.AddAdjacents();
  mm.AddTelegates();
  mm.CullBacktracks();

  BOOST_CHECK(mm.GetCurrentDests() ==
              "AS_3_3!walk!0!13456/AS_4_4!walk!0!13456/"
              "AS_7_6!jump!0!2");
}

BOOST_AUTO_TEST_CASE(TestCullByPilotNumberNoPilotNumbers)
{
  Players pls;
  pls.add("Foo Bar");
  pls[0].AddToken(Ship::GetShipOfClass(SCOUT));
  pls.RandomizeTurnOrder();
  MapData md("../MerchantOfVenusMap.xml");

  // srand seed 5 has the following telegates
  // 1: AS_3_4
  // 2: AS_7_6
  // 3: WTS_loop_2
  // 4: IM_loop_03
  // 5: PP_loop_05
  // 6: jw_loop_8

  srand(5);
  MapOverlay mo(md);
  MoveMediator mm(pls,mo);
  
  // test set 1. a set with only pilotless items should be unchanged.
  // start move leaves us with hidden dice

  srand(7);
  mm.StartMove();
  // this causes the dice roll to be 143

  mm.AddAdjacents();
  mm.AddTelegates();
  mm.CullBacktracks();
  mm.CullByPilotNumber();

  BOOST_CHECK(mm.GetCurrentDests() ==
              "ib_gb_3!walk!0/cloud_body_8!walk!0/gb_cw_1!walk!0");

  // dice visible, but no pilot number
  mm.MakeDiceVisible();
  mm.AddAdjacents();
  mm.AddTelegates();
  mm.CullBacktracks();
  mm.CullByPilotNumber();

  BOOST_CHECK(mm.GetCurrentDests() ==
              "ib_gb_3!walk!0/cloud_body_8!walk!0/gb_cw_1!walk!0");

  // dice visible and pilot #
  mm.SetPilotNumber(1);
  mm.AddAdjacents();
  mm.AddTelegates();
  mm.CullBacktracks();
  mm.CullByPilotNumber();

  BOOST_CHECK(mm.GetCurrentDests() ==
              "ib_gb_3!walk!0/cloud_body_8!walk!0/gb_cw_1!walk!0");

  // and finally, player has Switch Switch
  pls[0].AddToken(Token::Relic("Switch Switch",25));

  mm.AddAdjacents();
  mm.AddTelegates();
  mm.CullBacktracks();
  mm.CullByPilotNumber();

  BOOST_CHECK(mm.GetCurrentDests() ==
              "ib_gb_3!walk!0/cloud_body_8!walk!0/gb_cw_1!walk!0");

}

BOOST_AUTO_TEST_CASE(TestCullByPilotNumberWithPilotNumbers)
{
  Players pls;
  pls.add("Foo Bar");
  pls[0].AddToken(Ship::GetShipOfClass(SCOUT));
  pls.RandomizeTurnOrder();
  MapData md("../MerchantOfVenusMap.xml");

  // srand seed 5 has the following telegates
  // 1: AS_3_4
  // 2: AS_7_6
  // 3: WTS_loop_2
  // 4: IM_loop_03
  // 5: PP_loop_05
  // 6: jw_loop_8

  srand(5);
  MapOverlay mo(md);
  MoveMediator mm(pls,mo);
  pls[0].SetLocation("cloud_body_8");

  // test set 1. a set with only pilotless items should be unchanged.
  // start move leaves us with hidden dice

  srand(7);
  mm.StartMove();
  // this causes the dice roll to be 143

  mm.AddAdjacents();
  mm.AddTelegates();
  mm.CullBacktracks();
  mm.CullByPilotNumber();

  BOOST_CHECK(mm.GetCurrentDests() ==
              "cloud_body_4!walk!0!1456/Galactic Base!walk!0!2/cloud_body_7!walk!0!3");
  

  // dice visible, but no pilot number
  mm.MakeDiceVisible();
  mm.AddAdjacents();
  mm.AddTelegates();
  mm.CullBacktracks();
  mm.CullByPilotNumber();

  BOOST_CHECK(mm.GetCurrentDests() ==
              "cloud_body_4!walk!0!14/cloud_body_7!walk!0!3");

  // dice visible and pilot #
  mm.SetPilotNumber(3);
  mm.AddAdjacents();
  mm.AddTelegates();
  mm.CullBacktracks();
  mm.CullByPilotNumber();

  BOOST_CHECK(mm.GetCurrentDests() ==
              "cloud_body_7!walk!0!3");

  // and finally, player has Switch Switch
  pls[0].AddToken(Token::Relic("Switch Switch",25));

  mm.AddAdjacents();
  mm.AddTelegates();
  mm.CullBacktracks();
  mm.CullByPilotNumber();

  BOOST_CHECK(mm.GetCurrentDests() ==
              "cloud_body_4!walk!0!14/cloud_body_7!walk!0!3");
}

BOOST_AUTO_TEST_CASE(TestApplyMPSimple)
{
  Players pls;
  pls.add("Foo Bar");
  pls[0].AddToken(Ship::GetShipOfClass(SCOUT));
  pls.RandomizeTurnOrder();
  MapData md("../MerchantOfVenusMap.xml");

  // srand seed 5 has the following telegates
  // 1: AS_3_4
  // 2: AS_7_6
  // 3: WTS_loop_2
  // 4: IM_loop_03
  // 5: PP_loop_05
  // 6: jw_loop_8

  srand(5);
  MapOverlay mo(md);
  MoveMediator mm(pls,mo);

  mm.StartMove();
  mm.PrepareForStep();

  BOOST_CHECK(mm.GetCurrentDests() ==
              "ib_gb_3!walk!1/cloud_body_8!walk!1/gb_cw_1!walk!1");

  // skip red drive
  pls[0].AddToken(Token::Drive(true,false,false,50));
  pls[0].GetAdvances().SetSwitchables("RD");

  mm.PrepareForStep();

  BOOST_CHECK(mm.GetCurrentDests() ==
              "ib_gb_3!walk!1/cloud_body_8!walk!1/gb_cw_1!walk!0");

  pls[0].RemoveToken(1);
  pls[0].AddToken(Token::Drive(false,true,false,50));
  pls[0].GetAdvances().SetSwitchables("YD");

  mm.PrepareForStep();

  BOOST_CHECK(mm.GetCurrentDests() ==
              "ib_gb_3!walk!0/cloud_body_8!walk!1/gb_cw_1!walk!1");

  pls[0].RemoveToken(1);
  pls[0].AddToken(Token::Drive(true,true,true,100));
  pls[0].GetAdvances().SetSwitchables("CD");

  mm.PrepareForStep();

  BOOST_CHECK(mm.GetCurrentDests() ==
              "ib_gb_3!walk!0/cloud_body_8!walk!1/gb_cw_1!walk!0");

  // 2 mp left
  mm.SpendMP(mm.GetMP()-2);

  mm.PrepareForStep();

  BOOST_CHECK(mm.GetCurrentDests() ==
              "ib_gb_3!walk!0/cloud_body_8!walk!1/gb_cw_1!walk!0");

  // 1 mp left
  mm.SpendMP(1);

  mm.PrepareForStep();

  BOOST_CHECK(mm.GetCurrentDests() ==
              "ib_gb_3!walk!0/cloud_body_8!walk!1/gb_cw_1!walk!0");
  
  // 0 mp left
  mm.SpendMP(1);

  mm.PrepareForStep();

  BOOST_CHECK(mm.GetCurrentDests() == "");
}

BOOST_AUTO_TEST_CASE(TestApplyMPToCity)
{
  Players pls;
  pls.add("Foo Bar");
  pls[0].AddToken(Ship::GetShipOfClass(SCOUT));
  pls.RandomizeTurnOrder();
  MapData md("../MerchantOfVenusMap.xml");

  // srand seed 5 has the following telegates
  // 1: AS_3_4
  // 2: AS_7_6
  // 3: WTS_loop_2
  // 4: IM_loop_03
  // 5: PP_loop_05
  // 6: jw_loop_8

  srand(5);
  MapOverlay mo(md);
  MoveMediator mm(pls,mo);

  mm.StartMove();
  pls[0].SetLocation("trp_loop_1");
  // skip red drive
  pls[0].AddToken(Token::Drive(true,false,false,50));
  pls[0].GetAdvances().SetSwitchables("RD");

  mm.PrepareForStep();

  BOOST_CHECK(mm.GetCurrentDests() == 
              "Rumble City!walk!2/trp_loop_11!walk!0/trp_loop_2!walk!1");

  // have only 1 mp left.
  mm.SpendMP(mm.GetMP()-1);
  mm.PrepareForStep();

  BOOST_CHECK(mm.GetCurrentDests() == 
              "trp_loop_11!walk!0/trp_loop_2!walk!1");

  // give player Air Foil
  pls[0].AddToken(Token::Relic("Air Foil",200));
  mm.PrepareForStep();

  BOOST_CHECK(mm.GetCurrentDests() == 
              "Rumble City!walk!1/trp_loop_11!walk!0/trp_loop_2!walk!1");
  
}

BOOST_AUTO_TEST_CASE(TestApplyMPFromCity)
{
  Players pls;
  pls.add("Foo Bar");
  pls[0].AddToken(Ship::GetShipOfClass(SCOUT));
  pls.RandomizeTurnOrder();
  MapData md("../MerchantOfVenusMap.xml");

  // srand seed 5 has the following telegates
  // 1: AS_3_4
  // 2: AS_7_6
  // 3: WTS_loop_2
  // 4: IM_loop_03
  // 5: PP_loop_05
  // 6: jw_loop_8

  srand(5);
  MapOverlay mo(md);
  MoveMediator mm(pls,mo);

  mm.StartMove();
  pls[0].SetLocation("Rumble City");

  mm.PrepareForStep();

  BOOST_CHECK(mm.GetCurrentDests() == 
              "trp_loop_1!walk!2");

  pls[0].AddToken(Token::Relic("Air Foil",200));

  mm.PrepareForStep();

  BOOST_CHECK(mm.GetCurrentDests() == 
              "Rain City!walk!1/trp_loop_1!walk!1");
}

BOOST_AUTO_TEST_CASE(TestApplyMPNextToColoredQBoxPenalty)
{
  Players pls;
  pls.add("Foo Bar");
  pls[0].AddToken(Ship::GetShipOfClass(SCOUT));
  pls.RandomizeTurnOrder();
  MapData md("../MerchantOfVenusMap.xml");

  // srand seed 5 has the following telegates
  // 1: AS_3_4
  // 2: AS_7_6
  // 3: WTS_loop_2
  // 4: IM_loop_03
  // 5: PP_loop_05
  // 6: jw_loop_8

  srand(5);
  MapOverlay mo(md);
  MoveMediator mm(pls,mo);

  mm.StartMove();

  // cloud_body_2 is a red qbox penalty
  // cloud_body_4 is a yellow qbox penalty

  pls[0].SetLocation("cloud_body_3");
  pls[0].AddToken(Token::Drive(true,true,true,100));
  pls[0].GetAdvances().SetSwitchables("CD");

  mm.PrepareForStep();

  BOOST_CHECK(mm.GetCurrentDests() ==
              "cloud_body_2!walk!1/cloud_body_4!walk!1/cloud_south_1!walk!0");

  mo.AddPlayerToSecret("cloud_body_2",0);
  mo.MakeSpacePublic("cloud_body_4");

  mm.PrepareForStep();

  BOOST_CHECK(mm.GetCurrentDests() ==
              "cloud_body_2!walk!0/cloud_body_4!walk!0/cloud_south_1!walk!0");

}
