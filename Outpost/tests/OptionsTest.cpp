#include "Options.hpp"
#include "SerializeUtility.hpp"
#include <boost/test/auto_unit_test.hpp>
#include <iostream>

BOOST_AUTO_TEST_CASE( OptionsConstructor )
{

  Options opt;

  BOOST_CHECK_EQUAL(opt.GetEarlyDiscard() , false);
  BOOST_CHECK_EQUAL(opt.GetRefineries() , false);
  BOOST_CHECK_EQUAL(opt.GetOneUpgradePerTurn() , false);
  BOOST_CHECK_EQUAL(opt.GetMustBeActiveToBid() , false);
  BOOST_CHECK_EQUAL(opt.GetSupplyHarshness() , 0);
  BOOST_CHECK_EQUAL(opt.GetRobotMechanism() , FIRST_UNLIMITED);
  BOOST_CHECK_EQUAL(opt.GetResearchMega() , false);
  BOOST_CHECK_EQUAL(opt.GetResearchIsSmall() , false);
  BOOST_CHECK_EQUAL(opt.GetMicroIsSmall() , false);
  BOOST_CHECK_EQUAL(opt.GetBlindDraws(), false);
  BOOST_CHECK_EQUAL(opt.GetStockMechanism() , NORMAL_STOCK );

  opt.SetEarlyDiscard(true);
  opt.SetRefineries(true);
  opt.SetOneUpgradePerTurn(true);
  opt.SetMustBeActiveToBid(true);
  opt.SetSupplyHarshness(3);
  opt.SetRobotMechanism(ALWAYS_MULTIPLICATIVE);
  opt.SetResearchMega(true);
  opt.SetResearchIsSmall(true);
  opt.SetMicroIsSmall(true);
  opt.SetBlindDraws(true);
  opt.SetStockMechanism(SOME_ONE_EXTRA);

  BOOST_CHECK_EQUAL(opt.GetEarlyDiscard() , true);
  BOOST_CHECK_EQUAL(opt.GetRefineries() , true);
  BOOST_CHECK_EQUAL(opt.GetOneUpgradePerTurn() , true);
  BOOST_CHECK_EQUAL(opt.GetMustBeActiveToBid() , true);
  BOOST_CHECK_EQUAL(opt.GetSupplyHarshness() , 3);
  BOOST_CHECK_EQUAL(opt.GetRobotMechanism() , ALWAYS_MULTIPLICATIVE);
  BOOST_CHECK_EQUAL(opt.GetResearchMega() , true);
  BOOST_CHECK_EQUAL(opt.GetResearchIsSmall() , true);
  BOOST_CHECK_EQUAL(opt.GetMicroIsSmall() , true);
  BOOST_CHECK_EQUAL(opt.GetBlindDraws(), true);
  BOOST_CHECK_EQUAL(opt.GetStockMechanism() , SOME_ONE_EXTRA );



  opt.SetStandardBasic();

  BOOST_CHECK_EQUAL(opt.GetEarlyDiscard() , false);
  BOOST_CHECK_EQUAL(opt.GetRefineries() , false);
  BOOST_CHECK_EQUAL(opt.GetOneUpgradePerTurn() , false);
  BOOST_CHECK_EQUAL(opt.GetMustBeActiveToBid() , false);
  BOOST_CHECK_EQUAL(opt.GetSupplyHarshness() , 0);
  BOOST_CHECK_EQUAL(opt.GetRobotMechanism() , FIRST_UNLIMITED);
  BOOST_CHECK_EQUAL(opt.GetResearchMega() , false);
  BOOST_CHECK_EQUAL(opt.GetResearchIsSmall() , false);
  BOOST_CHECK_EQUAL(opt.GetMicroIsSmall() , false);
  BOOST_CHECK_EQUAL(opt.GetBlindDraws(), false);
  BOOST_CHECK_EQUAL(opt.GetStockMechanism() , NORMAL_STOCK );

  opt.SetStandardExpert();

  BOOST_CHECK_EQUAL(opt.GetEarlyDiscard() , true);
  BOOST_CHECK_EQUAL(opt.GetRefineries() , false);
  BOOST_CHECK_EQUAL(opt.GetOneUpgradePerTurn() , false);
  BOOST_CHECK_EQUAL(opt.GetMustBeActiveToBid() , false);
  BOOST_CHECK_EQUAL(opt.GetSupplyHarshness() , 0);
  BOOST_CHECK_EQUAL(opt.GetRobotMechanism() , FIRST_UNLIMITED);
  BOOST_CHECK_EQUAL(opt.GetResearchMega() , false);
  BOOST_CHECK_EQUAL(opt.GetResearchIsSmall() , true);
  BOOST_CHECK_EQUAL(opt.GetMicroIsSmall() , true);
  BOOST_CHECK_EQUAL(opt.GetBlindDraws(), false);
  BOOST_CHECK_EQUAL(opt.GetStockMechanism() , NORMAL_STOCK );

  {
    Options opt1;
    opt1.SetEarlyDiscard(true);
    opt1.SetRobotMechanism(SECOND_UNLIMITED);

    BOOST_CHECK_EQUAL(opt1.GetEarlyDiscard() , true);
    BOOST_CHECK_EQUAL(opt1.GetRefineries() , false);
    BOOST_CHECK_EQUAL(opt1.GetOneUpgradePerTurn() , false);
    BOOST_CHECK_EQUAL(opt1.GetMustBeActiveToBid() , false);
    BOOST_CHECK_EQUAL(opt1.GetSupplyHarshness() , 0);
    BOOST_CHECK_EQUAL(opt1.GetRobotMechanism() , SECOND_UNLIMITED);
    BOOST_CHECK_EQUAL(opt1.GetResearchMega() , false);
    BOOST_CHECK_EQUAL(opt1.GetResearchIsSmall() , false);
    BOOST_CHECK_EQUAL(opt1.GetMicroIsSmall() , false);
    BOOST_CHECK_EQUAL(opt1.GetBlindDraws(), false);
    BOOST_CHECK_EQUAL(opt1.GetStockMechanism() , NORMAL_STOCK );

    Options opt2;
    SerialTransfer(opt1,opt2);

    BOOST_CHECK_EQUAL(opt2.GetEarlyDiscard() , true);
    BOOST_CHECK_EQUAL(opt2.GetRefineries() , false);
    BOOST_CHECK_EQUAL(opt2.GetOneUpgradePerTurn() , false);
    BOOST_CHECK_EQUAL(opt2.GetMustBeActiveToBid() , false);
    BOOST_CHECK_EQUAL(opt2.GetSupplyHarshness() , 0);
    BOOST_CHECK_EQUAL(opt2.GetRobotMechanism() , SECOND_UNLIMITED);
    BOOST_CHECK_EQUAL(opt2.GetResearchMega() , false);
    BOOST_CHECK_EQUAL(opt2.GetResearchIsSmall() , false);
    BOOST_CHECK_EQUAL(opt2.GetMicroIsSmall() , false);
    BOOST_CHECK_EQUAL(opt2.GetBlindDraws(), false);
    BOOST_CHECK_EQUAL(opt2.GetStockMechanism() , NORMAL_STOCK );
  }

  {
    Options opt1;
    opt1.SetRefineries(true);
    opt1.SetRobotMechanism(ALWAYS_MULTIPLICATIVE);

    BOOST_CHECK_EQUAL(opt1.GetEarlyDiscard() , false);
    BOOST_CHECK_EQUAL(opt1.GetRefineries() , true);
    BOOST_CHECK_EQUAL(opt1.GetOneUpgradePerTurn() , false);
    BOOST_CHECK_EQUAL(opt1.GetMustBeActiveToBid() , false);
    BOOST_CHECK_EQUAL(opt1.GetSupplyHarshness() , 0);
    BOOST_CHECK_EQUAL(opt1.GetRobotMechanism() , ALWAYS_MULTIPLICATIVE);
    BOOST_CHECK_EQUAL(opt1.GetResearchMega() , false);
    BOOST_CHECK_EQUAL(opt1.GetResearchIsSmall() , false);
    BOOST_CHECK_EQUAL(opt1.GetMicroIsSmall() , false);
    BOOST_CHECK_EQUAL(opt1.GetBlindDraws(), false);
    BOOST_CHECK_EQUAL(opt1.GetStockMechanism() , NORMAL_STOCK );

    Options opt2;
    SerialTransfer(opt1,opt2);

    BOOST_CHECK_EQUAL(opt2.GetEarlyDiscard() , false);
    BOOST_CHECK_EQUAL(opt2.GetRefineries() , true);
    BOOST_CHECK_EQUAL(opt2.GetOneUpgradePerTurn() , false);
    BOOST_CHECK_EQUAL(opt2.GetMustBeActiveToBid() , false);
    BOOST_CHECK_EQUAL(opt2.GetSupplyHarshness() , 0);
    BOOST_CHECK_EQUAL(opt2.GetRobotMechanism() , ALWAYS_MULTIPLICATIVE);
    BOOST_CHECK_EQUAL(opt2.GetResearchMega() , false);
    BOOST_CHECK_EQUAL(opt2.GetResearchIsSmall() , false);
    BOOST_CHECK_EQUAL(opt2.GetMicroIsSmall() , false);
    BOOST_CHECK_EQUAL(opt2.GetBlindDraws(), false);
    BOOST_CHECK_EQUAL(opt2.GetStockMechanism() , NORMAL_STOCK );
  }

  {
    Options opt1;
    opt1.SetOneUpgradePerTurn(true);

    BOOST_CHECK_EQUAL(opt1.GetEarlyDiscard() , false);
    BOOST_CHECK_EQUAL(opt1.GetRefineries() , false);
    BOOST_CHECK_EQUAL(opt1.GetOneUpgradePerTurn() , true);
    BOOST_CHECK_EQUAL(opt1.GetMustBeActiveToBid() , false);
    BOOST_CHECK_EQUAL(opt1.GetSupplyHarshness() , 0);
    BOOST_CHECK_EQUAL(opt1.GetRobotMechanism() , FIRST_UNLIMITED);
    BOOST_CHECK_EQUAL(opt1.GetResearchMega() , false);
    BOOST_CHECK_EQUAL(opt1.GetResearchIsSmall() , false);
    BOOST_CHECK_EQUAL(opt1.GetMicroIsSmall() , false);
    BOOST_CHECK_EQUAL(opt1.GetBlindDraws(), false);
    BOOST_CHECK_EQUAL(opt1.GetStockMechanism() , NORMAL_STOCK );

    Options opt2;
    SerialTransfer(opt1,opt2);

    BOOST_CHECK_EQUAL(opt2.GetEarlyDiscard() , false);
    BOOST_CHECK_EQUAL(opt2.GetRefineries() , false);
    BOOST_CHECK_EQUAL(opt2.GetOneUpgradePerTurn() , true);
    BOOST_CHECK_EQUAL(opt2.GetMustBeActiveToBid() , false);
    BOOST_CHECK_EQUAL(opt2.GetSupplyHarshness() , 0);
    BOOST_CHECK_EQUAL(opt2.GetRobotMechanism() , FIRST_UNLIMITED);
    BOOST_CHECK_EQUAL(opt2.GetResearchMega() , false);
    BOOST_CHECK_EQUAL(opt2.GetResearchIsSmall() , false);
    BOOST_CHECK_EQUAL(opt2.GetMicroIsSmall() , false);
    BOOST_CHECK_EQUAL(opt2.GetBlindDraws(), false);
    BOOST_CHECK_EQUAL(opt2.GetStockMechanism() , NORMAL_STOCK );
  }

  {
    Options opt1;
    opt1.SetMustBeActiveToBid(true);
    opt1.SetRobotMechanism(SECOND_UNLIMITED);

    BOOST_CHECK_EQUAL(opt1.GetEarlyDiscard() , false);
    BOOST_CHECK_EQUAL(opt1.GetRefineries() , false);
    BOOST_CHECK_EQUAL(opt1.GetOneUpgradePerTurn() , false);
    BOOST_CHECK_EQUAL(opt1.GetMustBeActiveToBid() , true);
    BOOST_CHECK_EQUAL(opt1.GetSupplyHarshness() , 0);
    BOOST_CHECK_EQUAL(opt1.GetRobotMechanism() , SECOND_UNLIMITED);
    BOOST_CHECK_EQUAL(opt1.GetResearchMega() , false);
    BOOST_CHECK_EQUAL(opt1.GetResearchIsSmall() , false);
    BOOST_CHECK_EQUAL(opt1.GetMicroIsSmall() , false);
    BOOST_CHECK_EQUAL(opt1.GetBlindDraws(), false);
    BOOST_CHECK_EQUAL(opt1.GetStockMechanism() , NORMAL_STOCK );

    Options opt2;
    SerialTransfer(opt1,opt2);

    BOOST_CHECK_EQUAL(opt2.GetEarlyDiscard() , false);
    BOOST_CHECK_EQUAL(opt2.GetRefineries() , false);
    BOOST_CHECK_EQUAL(opt2.GetOneUpgradePerTurn() , false);
    BOOST_CHECK_EQUAL(opt2.GetMustBeActiveToBid() , true);
    BOOST_CHECK_EQUAL(opt2.GetSupplyHarshness() , 0);
    BOOST_CHECK_EQUAL(opt2.GetRobotMechanism() , SECOND_UNLIMITED);
    BOOST_CHECK_EQUAL(opt2.GetResearchMega() , false);
    BOOST_CHECK_EQUAL(opt2.GetResearchIsSmall() , false);
    BOOST_CHECK_EQUAL(opt2.GetMicroIsSmall() , false);
    BOOST_CHECK_EQUAL(opt2.GetBlindDraws(), false);
    BOOST_CHECK_EQUAL(opt2.GetStockMechanism() , NORMAL_STOCK );
  }

  {
    Options opt1;
    opt1.SetSupplyHarshness(4);
    opt1.SetRobotMechanism(ALWAYS_MULTIPLICATIVE);

    BOOST_CHECK_EQUAL(opt1.GetEarlyDiscard() , false);
    BOOST_CHECK_EQUAL(opt1.GetRefineries() , false);
    BOOST_CHECK_EQUAL(opt1.GetOneUpgradePerTurn() , false);
    BOOST_CHECK_EQUAL(opt1.GetMustBeActiveToBid() , false);
    BOOST_CHECK_EQUAL(opt1.GetSupplyHarshness() , 4);
    BOOST_CHECK_EQUAL(opt1.GetRobotMechanism() , ALWAYS_MULTIPLICATIVE);
    BOOST_CHECK_EQUAL(opt1.GetResearchMega() , false);
    BOOST_CHECK_EQUAL(opt1.GetResearchIsSmall() , false);
    BOOST_CHECK_EQUAL(opt1.GetMicroIsSmall() , false);
    BOOST_CHECK_EQUAL(opt1.GetBlindDraws(), false);
    BOOST_CHECK_EQUAL(opt1.GetStockMechanism() , NORMAL_STOCK );

    Options opt2;
    SerialTransfer(opt1,opt2);

    BOOST_CHECK_EQUAL(opt2.GetEarlyDiscard() , false);
    BOOST_CHECK_EQUAL(opt2.GetRefineries() , false);
    BOOST_CHECK_EQUAL(opt2.GetOneUpgradePerTurn() , false);
    BOOST_CHECK_EQUAL(opt2.GetMustBeActiveToBid() , false);
    BOOST_CHECK_EQUAL(opt2.GetSupplyHarshness() , 4);
    BOOST_CHECK_EQUAL(opt2.GetRobotMechanism() , ALWAYS_MULTIPLICATIVE);
    BOOST_CHECK_EQUAL(opt2.GetResearchMega() , false);
    BOOST_CHECK_EQUAL(opt2.GetResearchIsSmall() , false);
    BOOST_CHECK_EQUAL(opt2.GetMicroIsSmall() , false);
    BOOST_CHECK_EQUAL(opt2.GetBlindDraws(), false);
    BOOST_CHECK_EQUAL(opt2.GetStockMechanism() , NORMAL_STOCK );
  }

  {
    Options opt1;
    opt1.SetResearchMega(true);
    opt1.SetRobotMechanism(SECOND_UNLIMITED);

    BOOST_CHECK_EQUAL(opt1.GetEarlyDiscard() , false);
    BOOST_CHECK_EQUAL(opt1.GetRefineries() , false);
    BOOST_CHECK_EQUAL(opt1.GetOneUpgradePerTurn() , false);
    BOOST_CHECK_EQUAL(opt1.GetMustBeActiveToBid() , false);
    BOOST_CHECK_EQUAL(opt1.GetSupplyHarshness() , 0);
    BOOST_CHECK_EQUAL(opt1.GetRobotMechanism() , SECOND_UNLIMITED);
    BOOST_CHECK_EQUAL(opt1.GetResearchMega() , true);
    BOOST_CHECK_EQUAL(opt1.GetResearchIsSmall() , false);
    BOOST_CHECK_EQUAL(opt1.GetMicroIsSmall() , false);
    BOOST_CHECK_EQUAL(opt1.GetBlindDraws(), false);
    BOOST_CHECK_EQUAL(opt1.GetStockMechanism() , NORMAL_STOCK );

    Options opt2;
    SerialTransfer(opt1,opt2);

    BOOST_CHECK_EQUAL(opt2.GetEarlyDiscard() , false);
    BOOST_CHECK_EQUAL(opt2.GetRefineries() , false);
    BOOST_CHECK_EQUAL(opt2.GetOneUpgradePerTurn() , false);
    BOOST_CHECK_EQUAL(opt2.GetMustBeActiveToBid() , false);
    BOOST_CHECK_EQUAL(opt2.GetSupplyHarshness() , 0);
    BOOST_CHECK_EQUAL(opt2.GetRobotMechanism() , SECOND_UNLIMITED);
    BOOST_CHECK_EQUAL(opt2.GetResearchMega() , true);
    BOOST_CHECK_EQUAL(opt2.GetResearchIsSmall() , false);
    BOOST_CHECK_EQUAL(opt2.GetMicroIsSmall() , false);
    BOOST_CHECK_EQUAL(opt2.GetBlindDraws(), false);
    BOOST_CHECK_EQUAL(opt2.GetStockMechanism() , NORMAL_STOCK );
  }

  {
    Options opt1;
    opt1.SetResearchIsSmall(true);

    BOOST_CHECK_EQUAL(opt1.GetEarlyDiscard() , false);
    BOOST_CHECK_EQUAL(opt1.GetRefineries() , false);
    BOOST_CHECK_EQUAL(opt1.GetOneUpgradePerTurn() , false);
    BOOST_CHECK_EQUAL(opt1.GetMustBeActiveToBid() , false);
    BOOST_CHECK_EQUAL(opt1.GetSupplyHarshness() , 0);
    BOOST_CHECK_EQUAL(opt1.GetRobotMechanism() , FIRST_UNLIMITED);
    BOOST_CHECK_EQUAL(opt1.GetResearchMega() , false);
    BOOST_CHECK_EQUAL(opt1.GetResearchIsSmall() , true);
    BOOST_CHECK_EQUAL(opt1.GetMicroIsSmall() , false);
    BOOST_CHECK_EQUAL(opt1.GetBlindDraws(), false);
    BOOST_CHECK_EQUAL(opt1.GetStockMechanism() , NORMAL_STOCK );

    Options opt2;
    SerialTransfer(opt1,opt2);

    BOOST_CHECK_EQUAL(opt2.GetEarlyDiscard() , false);
    BOOST_CHECK_EQUAL(opt2.GetRefineries() , false);
    BOOST_CHECK_EQUAL(opt2.GetOneUpgradePerTurn() , false);
    BOOST_CHECK_EQUAL(opt2.GetMustBeActiveToBid() , false);
    BOOST_CHECK_EQUAL(opt2.GetSupplyHarshness() , 0);
    BOOST_CHECK_EQUAL(opt2.GetRobotMechanism() , FIRST_UNLIMITED);
    BOOST_CHECK_EQUAL(opt2.GetResearchMega() , false);
    BOOST_CHECK_EQUAL(opt2.GetResearchIsSmall() , true);
    BOOST_CHECK_EQUAL(opt2.GetMicroIsSmall() , false);
    BOOST_CHECK_EQUAL(opt2.GetBlindDraws(), false);
    BOOST_CHECK_EQUAL(opt2.GetStockMechanism() , NORMAL_STOCK );
  }

  {
    Options opt1;
    opt1.SetMicroIsSmall(true);

    BOOST_CHECK_EQUAL(opt1.GetEarlyDiscard() , false);
    BOOST_CHECK_EQUAL(opt1.GetRefineries() , false);
    BOOST_CHECK_EQUAL(opt1.GetOneUpgradePerTurn() , false);
    BOOST_CHECK_EQUAL(opt1.GetMustBeActiveToBid() , false);
    BOOST_CHECK_EQUAL(opt1.GetSupplyHarshness() , 0);
    BOOST_CHECK_EQUAL(opt1.GetRobotMechanism() , FIRST_UNLIMITED);
    BOOST_CHECK_EQUAL(opt1.GetResearchMega() , false);
    BOOST_CHECK_EQUAL(opt1.GetResearchIsSmall() , false);
    BOOST_CHECK_EQUAL(opt1.GetMicroIsSmall() , true);
    BOOST_CHECK_EQUAL(opt1.GetBlindDraws(), false);
    BOOST_CHECK_EQUAL(opt1.GetStockMechanism() , NORMAL_STOCK );

    Options opt2;
    SerialTransfer(opt1,opt2);

    BOOST_CHECK_EQUAL(opt2.GetEarlyDiscard() , false);
    BOOST_CHECK_EQUAL(opt2.GetRefineries() , false);
    BOOST_CHECK_EQUAL(opt2.GetOneUpgradePerTurn() , false);
    BOOST_CHECK_EQUAL(opt2.GetMustBeActiveToBid() , false);
    BOOST_CHECK_EQUAL(opt2.GetSupplyHarshness() , 0);
    BOOST_CHECK_EQUAL(opt2.GetRobotMechanism() , FIRST_UNLIMITED);
    BOOST_CHECK_EQUAL(opt2.GetResearchMega() , false);
    BOOST_CHECK_EQUAL(opt2.GetResearchIsSmall() , false);
    BOOST_CHECK_EQUAL(opt2.GetMicroIsSmall() , true);
    BOOST_CHECK_EQUAL(opt2.GetBlindDraws(), false);
    BOOST_CHECK_EQUAL(opt2.GetStockMechanism() , NORMAL_STOCK );
  }

  {
    Options opt1;
    opt1.SetBlindDraws(true);
    opt1.SetRobotMechanism(ALWAYS_MULTIPLICATIVE);

    BOOST_CHECK_EQUAL(opt1.GetEarlyDiscard() , false);
    BOOST_CHECK_EQUAL(opt1.GetRefineries() , false);
    BOOST_CHECK_EQUAL(opt1.GetOneUpgradePerTurn() , false);
    BOOST_CHECK_EQUAL(opt1.GetMustBeActiveToBid() , false);
    BOOST_CHECK_EQUAL(opt1.GetSupplyHarshness() , 0);
    BOOST_CHECK_EQUAL(opt1.GetRobotMechanism() , ALWAYS_MULTIPLICATIVE);
    BOOST_CHECK_EQUAL(opt1.GetResearchMega() , false);
    BOOST_CHECK_EQUAL(opt1.GetResearchIsSmall() , false);
    BOOST_CHECK_EQUAL(opt1.GetMicroIsSmall() , false);
    BOOST_CHECK_EQUAL(opt1.GetBlindDraws(), true);
    BOOST_CHECK_EQUAL(opt1.GetStockMechanism() , NORMAL_STOCK );

    Options opt2;
    SerialTransfer(opt1,opt2);

    BOOST_CHECK_EQUAL(opt2.GetEarlyDiscard() , false);
    BOOST_CHECK_EQUAL(opt2.GetRefineries() , false);
    BOOST_CHECK_EQUAL(opt2.GetOneUpgradePerTurn() , false);
    BOOST_CHECK_EQUAL(opt2.GetMustBeActiveToBid() , false);
    BOOST_CHECK_EQUAL(opt2.GetSupplyHarshness() , 0);
    BOOST_CHECK_EQUAL(opt2.GetRobotMechanism() , ALWAYS_MULTIPLICATIVE);
    BOOST_CHECK_EQUAL(opt2.GetResearchMega() , false);
    BOOST_CHECK_EQUAL(opt2.GetResearchIsSmall() , false);
    BOOST_CHECK_EQUAL(opt2.GetMicroIsSmall() , false);
    BOOST_CHECK_EQUAL(opt2.GetBlindDraws(), true);
    BOOST_CHECK_EQUAL(opt2.GetStockMechanism() , NORMAL_STOCK );
  }

  {
    Options opt1;
    opt1.SetEarlyDiscard(true);
    opt1.SetMustBeActiveToBid(true);
    opt1.SetResearchMega(true);
    opt1.SetResearchIsSmall(true);
    opt1.SetMicroIsSmall(true);
    opt1.SetStockMechanism(SOME_ONE_EXTRA);
  
    BOOST_CHECK_EQUAL(opt1.GetEarlyDiscard() , true);
    BOOST_CHECK_EQUAL(opt1.GetRefineries() , false);
    BOOST_CHECK_EQUAL(opt1.GetOneUpgradePerTurn() , false);
    BOOST_CHECK_EQUAL(opt1.GetMustBeActiveToBid() , true);
    BOOST_CHECK_EQUAL(opt1.GetSupplyHarshness() , 0);
    BOOST_CHECK_EQUAL(opt1.GetRobotMechanism() , FIRST_UNLIMITED);
    BOOST_CHECK_EQUAL(opt1.GetResearchMega() , true);
    BOOST_CHECK_EQUAL(opt1.GetResearchIsSmall() , true);
    BOOST_CHECK_EQUAL(opt1.GetMicroIsSmall() , true);
    BOOST_CHECK_EQUAL(opt1.GetBlindDraws(), false);
    BOOST_CHECK_EQUAL(opt1.GetStockMechanism() , SOME_ONE_EXTRA );

    Options opt2;
    SerialTransfer(opt1,opt2);

    BOOST_CHECK_EQUAL(opt2.GetEarlyDiscard() , true);
    BOOST_CHECK_EQUAL(opt2.GetRefineries() , false);
    BOOST_CHECK_EQUAL(opt2.GetOneUpgradePerTurn() , false);
    BOOST_CHECK_EQUAL(opt2.GetMustBeActiveToBid() , true);
    BOOST_CHECK_EQUAL(opt2.GetSupplyHarshness() , 0);
    BOOST_CHECK_EQUAL(opt2.GetRobotMechanism() , FIRST_UNLIMITED);
    BOOST_CHECK_EQUAL(opt2.GetResearchMega() , true);
    BOOST_CHECK_EQUAL(opt2.GetResearchIsSmall() , true);
    BOOST_CHECK_EQUAL(opt2.GetMicroIsSmall() , true);
    BOOST_CHECK_EQUAL(opt2.GetBlindDraws(), false);
    BOOST_CHECK_EQUAL(opt2.GetStockMechanism() , SOME_ONE_EXTRA );
  }

}
