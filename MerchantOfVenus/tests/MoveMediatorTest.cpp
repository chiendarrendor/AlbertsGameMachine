#include "MoveMediator.hpp"
#include "Players.hpp"
#include "MapData.hpp"
#include "MapOverlay.hpp"
#include "Options.hpp"
#include "Ship.hpp"

#include "AppendShuffleDraws.hpp"
#include "ScopedFixedRandom.hpp"

#include <boost/test/unit_test.hpp>

namespace
{
  // MapOverlay's constructor now needs an Options reference (see
  // MerchantOfVenus/Options.hpp) to decide whether to hide the unused
  // combat items -- these tests are about movement, not that option, so
  // they pin it to false to keep the 10-relic/10-asteroid canonical layout
  // below (g_QboxDeckOrder/g_QboxLayout) unchanged.
  Options MakeTestOptions()
  {
    Options opts;
    opts.SetHideUnusedWeapons(false);
    return opts;
  }
  const Options g_TestOptions = MakeTestOptions();

  // MakeQBoxList()'s exact push order (MerchantOfVenus/MapOverlay.cpp) --
  // the "deck" of 36 qbox-content candidates its constructor shuffles and
  // truncates to the 24 real qbox positions on the board.
  const std::vector<std::string> g_QboxDeckOrder{
    "telegate1","telegate2","telegate3","telegate4","telegate5","telegate6",
    "openport1","openport2","openport3",
    "red10","red20a","red20b","red30a","red30b","red40",
    "yellow10","yellow20","yellow30","yellow40",
    "blue10a","blue10b","blue20a","blue20b","blue30a","blue30b","blue40",
    "asteroid1","asteroid2","asteroid3","asteroid4","asteroid5",
    "asteroid6","asteroid7","asteroid8","asteroid9","asteroid10"
  };

  // What we want the deck to look like after the shuffle: the first 24
  // entries are the real qbox space names in the same sorted order
  // MapOverlay's constructor iterates them (std::map<std::string,MapSpace*>),
  // followed by 12 "didn't make it onto the board this game" leftovers.
  // Pins the 6 telegates onto the same named spaces the tests below were
  // originally written against (documented, pre-refactor, as "srand seed 5
  // has the following telegates"), plus cloud_body_2/cloud_body_4 onto a
  // red/yellow penalty respectively (needed by
  // TestApplyMPNextToColoredQBoxPenalty). Every other position is a "don't
  // care" filler -- no test below references it by name.
  const std::vector<std::string> g_QboxLayout{
    /*AS_3_4*/        "telegate1",
    /*AS_4_1*/        "openport1",
    /*AS_5_5*/        "openport2",
    /*AS_6_4*/        "openport3",
    /*AS_7_6*/        "telegate2",
    /*GP_loop_08*/    "red20a",
    /*IM_loop_03*/    "telegate4",
    /*IM_loop_10*/    "red20b",
    /*PP_loop_05*/    "telegate5",
    /*PP_loop_09*/    "red30a",
    /*WTS_loop_2*/    "telegate3",
    /*WTS_loop_5*/    "red30b",
    /*WTS_loop_9*/    "red40",
    /*WW_loop_08*/    "yellow20",
    /*cloud_body_2*/  "red10",
    /*cloud_body_4*/  "yellow10",
    /*cloud_body_5*/  "yellow30",
    /*cloud_north_1*/ "yellow40",
    /*cloud_wing_3*/  "blue10a",
    /*dw_loop_3*/     "blue10b",
    /*ip_loop_4*/     "blue20a",
    /*ip_loop_9*/     "blue20b",
    /*jw_loop_5*/     "blue30a",
    /*jw_loop_8*/     "telegate6",
    // leftover -- not placed on the board this game
    "blue30b","blue40",
    "asteroid1","asteroid2","asteroid3","asteroid4","asteroid5",
    "asteroid6","asteroid7","asteroid8","asteroid9","asteroid10"
  };

  // Builds the full draw script MapOverlay's constructor needs (the qbox
  // shuffle above, immediately followed by its relic shuffle -- 10 relics,
  // scripted as an identity/no-op since no test here checks relic
  // placement), followed by whatever additional draws (e.g. dice rolls) the
  // rest of a test needs, in the exact order the code under test will
  // consume them.
  std::vector<int> BuildDraws(const std::vector<int>& i_extraDraws = std::vector<int>())
  {
    std::vector<int> draws;
    // MapOverlay's constructor shuffles relics before qboxes (so it knows
    // how many asteroids to generate) -- the relic shuffle's draws come
    // first, then the qbox shuffle's.
    draws.insert(draws.end(),10,0);
    AppendShuffleDraws(draws,g_QboxDeckOrder,g_QboxLayout);
    draws.insert(draws.end(),i_extraDraws.begin(),i_extraDraws.end());
    return draws;
  }
}

BOOST_AUTO_TEST_CASE( TestStartMove )
{
  Players pls;
  pls.add("Foo Bar");
  pls[0].AddToken(Ship::GetShipOfClass(SCOUT));
  pls.RandomizeTurnOrder();
  MapData md("../MerchantOfVenusMap.xml");
  MapOverlay mo(md,g_TestOptions);

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
  MapOverlay mo(md,g_TestOptions);

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
  MapOverlay mo(md,g_TestOptions);

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

  ScopedFixedRandom randomguard(BuildDraws({0,0,0,0,0,0}));
  MapOverlay mo(md,g_TestOptions);

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

  //   jumpstart and first, some telegates: before dice are visible, the
  //   blind first-move list must be Telegates only, not walk options too
  mo.MakeSpacePublic("AS_3_4");
  mo.MakeSpacePublic("WTS_loop_2");
  mo.MakeSpacePublic("PP_loop_05");
  mm.AddAdjacents();
  mm.AddTelegates();
  BOOST_CHECK(mm.GetCurrentDests() ==
              "AS_3_4!jump!0!1/WTS_loop_2!jump!0!3/PP_loop_05!jump!0!5");

  //   once dice become visible (e.g. after a rejected blind guess forces
  //   PrepareForStep() to recompute), the walk options reappear alongside
  //   the telegates -- not a filter of the blind list, a fresh recompute
  mm.MakeDiceVisible();
  mm.AddAdjacents();
  mm.AddTelegates();
  BOOST_CHECK(mm.GetCurrentDests() ==
              "ib_gb_3!walk!0!246/cloud_body_8!walk!0!246/gb_cw_1!walk!0!246/"
              "AS_3_4!jump!0!1/WTS_loop_2!jump!0!3/PP_loop_05!jump!0!5");

  //   jumpstart and first, all telegates (dice already visible from above)
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

  ScopedFixedRandom randomguard(BuildDraws({0,0,0,0,0,0}));
  MapOverlay mo(md,g_TestOptions);

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

  // before dice are visible, the blind first-move list is Telegates only
  BOOST_CHECK(mm.GetCurrentDests() ==
              "AS_3_4!jump!0!1/WTS_loop_2!jump!0!3/PP_loop_05!jump!0!5");

  // once dice become visible, the walk options reappear alongside the telegates
  mm.MakeDiceVisible();
  mm.AddAdjacents();
  mm.AddTelegates();
  BOOST_CHECK(mm.GetCurrentDests() ==
              "cloud_body_4!walk!0!46/Galactic Base!walk!0!2/"
              "AS_3_4!jump!0!1/WTS_loop_2!jump!0!3/PP_loop_05!jump!0!5");

  //   jumpstart and first, all telegates (dice already visible from above)
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

  ScopedFixedRandom randomguard(BuildDraws({0,0,0}));
  MapOverlay mo(md,g_TestOptions);

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

  ScopedFixedRandom randomguard(BuildDraws({0,0,0}));
  MapOverlay mo(md,g_TestOptions);
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

  // before dice are visible, the blind first-move list is Telegates only --
  // and even under Jump Start, the telegate the player is standing on
  // (AS_3_4) is still excluded from its own jump list
  BOOST_CHECK(mm.GetCurrentDests() ==
              "WTS_loop_2!jump!0!3/PP_loop_05!jump!0!5");

  // once dice become visible, the walk options reappear alongside the telegates
  mm.MakeDiceVisible();
  mm.AddAdjacents();
  mm.AddTelegates();

  BOOST_CHECK(mm.GetCurrentDests() ==
              "AS_2_3!walk!0!1246/AS_3_3!walk!0!1246/AS_4_4!walk!0!1246/"
              "WTS_loop_2!jump!0!3/PP_loop_05!jump!0!5");
}

// a Jump Start move to a telegate launched from a ground city costs 2 MP,
// same as any other move out of a city -- but only 1 MP with Air Foil
BOOST_AUTO_TEST_CASE( TestAddTelegatesJumpStartFromCityMPCost )
{
  Players pls;
  pls.add("Foo Bar");
  pls[0].AddToken(Ship::GetShipOfClass(SCOUT));
  pls.RandomizeTurnOrder();
  MapData md("../MerchantOfVenusMap.xml");

  ScopedFixedRandom randomguard(BuildDraws({0,0,0}));
  MapOverlay mo(md,g_TestOptions);
  MoveMediator mm(pls,mo);

  pls[0].SetLocation("Rumble City");
  pls[0].AddToken(Token::Relic("Jump Start",100));
  pls[0].GetAdvances().SetSwitchables("JS");
  mo.MakeSpacePublic("AS_3_4");

  mm.StartMove();
  mm.PrepareForStep();

  BOOST_CHECK(mm.GetCurrentDests() == "AS_3_4!jump!2!1");

  pls[0].AddToken(Token::Relic("Air Foil",200));
  mm.PrepareForStep();

  BOOST_CHECK(mm.GetCurrentDests() == "AS_3_4!jump!1!1");
}

BOOST_AUTO_TEST_CASE( TestRemoveBacktracks )
{
  Players pls;
  pls.add("Foo Bar");
  pls[0].AddToken(Ship::GetShipOfClass(SCOUT));
  pls.RandomizeTurnOrder();
  MapData md("../MerchantOfVenusMap.xml");

  ScopedFixedRandom randomguard(BuildDraws({0,0,0}));
  MapOverlay mo(md,g_TestOptions);
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

  // dice = {1,4,3} -- see CullByPilotNumber() in MoveMediator.cpp: the tests
  // below depend on the exact set of pilot numbers this roll produces.
  ScopedFixedRandom randomguard(BuildDraws({0,3,2}));
  MapOverlay mo(md,g_TestOptions);
  MoveMediator mm(pls,mo);

  // test set 1. a set with only pilotless items should be unchanged.
  // start move leaves us with hidden dice

  mm.StartMove();

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

  // dice = {1,4,3} -- see CullByPilotNumber() in MoveMediator.cpp: the tests
  // below depend on the exact set of pilot numbers this roll produces.
  ScopedFixedRandom randomguard(BuildDraws({0,3,2}));
  MapOverlay mo(md,g_TestOptions);
  MoveMediator mm(pls,mo);
  pls[0].SetLocation("cloud_body_8");

  // test set 1. a set with only pilotless items should be unchanged.
  // start move leaves us with hidden dice

  mm.StartMove();

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

  ScopedFixedRandom randomguard(BuildDraws({0,0,0}));
  MapOverlay mo(md,g_TestOptions);
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

  ScopedFixedRandom randomguard(BuildDraws({0,0,0}));
  MapOverlay mo(md,g_TestOptions);
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

  ScopedFixedRandom randomguard(BuildDraws({0,0,0}));
  MapOverlay mo(md,g_TestOptions);
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

  ScopedFixedRandom randomguard(BuildDraws({0,0,0}));
  MapOverlay mo(md,g_TestOptions);
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

// MapOverlay's constructor now sizes the asteroid pool off the relic list
// (built first), so hiding the Laser relic should also drop the asteroid
// count by one, keeping the two in lockstep.
BOOST_AUTO_TEST_CASE( TestHideUnusedWeaponsRemovesLaserRelic )
{
  MapData md("../MerchantOfVenusMap.xml");

  Options hiddenoptions;
  hiddenoptions.SetHideUnusedWeapons(true);
  MapOverlay mo(md,hiddenoptions);

  BOOST_CHECK(mo.GetRelicList().size() == 9);
  for (size_t i = 0 ; i < mo.GetRelicList().size() ; ++i)
  {
    BOOST_CHECK(mo.GetRelicList()[i].GetName() != "Laser 20");
  }
}

BOOST_AUTO_TEST_CASE( TestShowUnusedWeaponsIncludesLaserRelic )
{
  MapData md("../MerchantOfVenusMap.xml");

  Options shownoptions;
  shownoptions.SetHideUnusedWeapons(false);
  MapOverlay mo(md,shownoptions);

  BOOST_CHECK(mo.GetRelicList().size() == 10);
  bool foundlaser = false;
  for (size_t i = 0 ; i < mo.GetRelicList().size() ; ++i)
  {
    if (mo.GetRelicList()[i].GetName() == "Laser 20") foundlaser = true;
  }
  BOOST_CHECK(foundlaser);
}
