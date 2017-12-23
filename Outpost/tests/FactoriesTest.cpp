#include "Factories.hpp"
#include "SerializeUtility.hpp"
#include <boost/test/auto_unit_test.hpp>
#include <iostream>


void TestFactory(FactoryType i_ftype,const std::string &i_fchar,int i_vp,int i_increase)
{
  Factories fac;

  BOOST_CHECK_EQUAL( fac.size() , (size_t)0 );

  fac.AddFactory(i_ftype);

  BOOST_CHECK_EQUAL( fac.size() , (size_t)1 );

  std::string udesc = std::string("|") + i_fchar + "U|";
  std::string rdesc = std::string("|") + i_fchar + "B|";
  std::string mdesc = std::string("|") + i_fchar + "H|";
  
  BOOST_CHECK(fac.GetFactoryDescription() == udesc);
  BOOST_CHECK(fac.GetVPs() == 0);
  BOOST_CHECK(fac.GetPeopleIncrease() == 0);
  BOOST_CHECK(fac.GetMannedFactoryCount(i_ftype) == 0);
  BOOST_CHECK(fac.GetUnmannedFactoryCount(i_ftype) == 1);
  BOOST_CHECK(fac.GetUsedMen() == 0);
  BOOST_CHECK(fac.GetUsedRobots() == 0);

  BOOST_CHECK(fac.AlterManning("B",1,1) == 0);

  BOOST_CHECK(fac.GetFactoryDescription() == rdesc);
  BOOST_CHECK(fac.GetVPs() == i_vp);
  BOOST_CHECK(fac.GetPeopleIncrease() == 0);
  BOOST_CHECK(fac.GetMannedFactoryCount(i_ftype) == 1);
  BOOST_CHECK(fac.GetUnmannedFactoryCount(i_ftype) == 0);
  BOOST_CHECK(fac.GetUsedMen() == 0);
  BOOST_CHECK(fac.GetUsedRobots() == 1);

  BOOST_CHECK(fac.AlterManning("H",1,1) == 0);

  BOOST_CHECK(fac.GetFactoryDescription() == mdesc);
  BOOST_CHECK(fac.GetVPs() == i_vp);
  BOOST_CHECK(fac.GetPeopleIncrease() == i_increase);
  BOOST_CHECK(fac.GetMannedFactoryCount(i_ftype) == 1);
  BOOST_CHECK(fac.GetUnmannedFactoryCount(i_ftype) == 0);
  BOOST_CHECK(fac.GetUsedMen() == 1);
  BOOST_CHECK(fac.GetUsedRobots() == 0);

  if (i_increase != 0)
  {
    BOOST_CHECK(fac.AlterManning("B",1,1) == 0);
    BOOST_CHECK(fac.AlterManning("H",1,1) == 0);
    BOOST_CHECK(fac.AlterManning("U",1,1) == 0);
    BOOST_CHECK(fac.AlterManning("H",1,1) == 0);
    
    BOOST_CHECK(fac.GetFactoryDescription() == mdesc);
    BOOST_CHECK(fac.GetVPs() == i_vp);
    BOOST_CHECK(fac.GetPeopleIncrease() == i_increase);
    BOOST_CHECK(fac.GetMannedFactoryCount(i_ftype) == 1);
    BOOST_CHECK(fac.GetUnmannedFactoryCount(i_ftype) == 0);
    BOOST_CHECK(fac.GetUsedMen() == 1);
    BOOST_CHECK(fac.GetUsedRobots() == 0);
  }
  else
  {
    BOOST_CHECK(fac.AlterManning("B",1,1) == 0);
    BOOST_CHECK(fac.GetFactoryDescription() == rdesc);
    BOOST_CHECK(fac.GetVPs() == i_vp);
    BOOST_CHECK(fac.GetPeopleIncrease() == 0);
    BOOST_CHECK(fac.GetMannedFactoryCount(i_ftype) == 1);
    BOOST_CHECK(fac.GetUnmannedFactoryCount(i_ftype) == 0);
    BOOST_CHECK(fac.GetUsedMen() == 0);
    BOOST_CHECK(fac.GetUsedRobots() == 1);
  }    
    

}
  



BOOST_AUTO_TEST_CASE( FactoriesManning )
{
  Factories fac;
  BOOST_CHECK( fac.GetFactoryDescription() == "||");

  TestFactory(ORE_FACTORY,"O",1,0);
  TestFactory(WATER_FACTORY,"W",1,0);
  TestFactory(TITANIUM_FACTORY,"T",2,0);
  TestFactory(RESEARCH_FACTORY,"R",2,0);
  TestFactory(NEW_CHEMICALS_FACTORY,"N",3,0);
  TestFactory(SPACE_STATION_FACTORY,"S",10,1);
  TestFactory(PLANETARY_CRUISER_FACTORY,"P",15,1);
  TestFactory(MOON_BASE_FACTORY,"M",20,1);

  BOOST_CHECK_EQUAL( fac.size() , (size_t)0 );


  fac.AddFactory(RESEARCH_FACTORY);
  fac.AddFactory(NEW_CHEMICALS_FACTORY);
  fac.AddFactory(NEW_CHEMICALS_FACTORY);
  fac.AddFactory(MOON_BASE_FACTORY);
  fac.AddFactory(SPACE_STATION_FACTORY);
  fac.AddFactory(WATER_FACTORY);
  fac.AddFactory(WATER_FACTORY);

  BOOST_CHECK_EQUAL( fac.size() , (size_t)7 );


  BOOST_CHECK (fac.GetFactoryDescription() == "|WU-WU-RU-NU-NU-SU-MU|");
  BOOST_CHECK (fac.GetVPs() == 0);
  BOOST_CHECK (fac.GetPeopleIncrease() == 0);
  BOOST_CHECK (fac.GetMannedFactoryCount(WATER_FACTORY) == 0);
  BOOST_CHECK (fac.GetMannedFactoryCount(NEW_CHEMICALS_FACTORY) == 0);
  BOOST_CHECK (fac.GetUnmannedFactoryCount(WATER_FACTORY) == 2);
  BOOST_CHECK (fac.GetUnmannedFactoryCount(NEW_CHEMICALS_FACTORY) == 2);
  BOOST_CHECK(fac.GetUsedMen() == 0);
  BOOST_CHECK(fac.GetUsedRobots() == 0);
  
  
  BOOST_CHECK(fac.AlterManning("HHHHHHHH",10,0) == -1);
  BOOST_CHECK (fac.GetFactoryDescription() == "|WU-WU-RU-NU-NU-SU-MU|");
  BOOST_CHECK (fac.GetVPs() == 0);
  BOOST_CHECK (fac.GetPeopleIncrease() == 0);
  BOOST_CHECK (fac.GetMannedFactoryCount(WATER_FACTORY) == 0);
  BOOST_CHECK (fac.GetMannedFactoryCount(NEW_CHEMICALS_FACTORY) == 0);
  BOOST_CHECK (fac.GetUnmannedFactoryCount(WATER_FACTORY) == 2);
  BOOST_CHECK (fac.GetUnmannedFactoryCount(NEW_CHEMICALS_FACTORY) == 2);
  BOOST_CHECK(fac.GetUsedMen() == 0);
  BOOST_CHECK(fac.GetUsedRobots() == 0);



  BOOST_CHECK(fac.AlterManning("HHHHHH",10,0) == -1);
  BOOST_CHECK (fac.GetFactoryDescription() == "|WU-WU-RU-NU-NU-SU-MU|");
  BOOST_CHECK (fac.GetVPs() == 0);
  BOOST_CHECK (fac.GetPeopleIncrease() == 0);
  BOOST_CHECK (fac.GetMannedFactoryCount(WATER_FACTORY) == 0);
  BOOST_CHECK (fac.GetMannedFactoryCount(NEW_CHEMICALS_FACTORY) == 0);
  BOOST_CHECK (fac.GetUnmannedFactoryCount(WATER_FACTORY) == 2);
  BOOST_CHECK (fac.GetUnmannedFactoryCount(NEW_CHEMICALS_FACTORY) == 2);
  BOOST_CHECK(fac.GetUsedMen() == 0);
  BOOST_CHECK(fac.GetUsedRobots() == 0);

  BOOST_CHECK(fac.AlterManning("HHHHHHB",10,0) == -2);
  BOOST_CHECK (fac.GetFactoryDescription() == "|WU-WU-RU-NU-NU-SU-MU|");
  BOOST_CHECK (fac.GetVPs() == 0);
  BOOST_CHECK (fac.GetPeopleIncrease() == 0);
  BOOST_CHECK (fac.GetMannedFactoryCount(WATER_FACTORY) == 0);
  BOOST_CHECK (fac.GetMannedFactoryCount(NEW_CHEMICALS_FACTORY) == 0);
  BOOST_CHECK (fac.GetUnmannedFactoryCount(WATER_FACTORY) == 2);
  BOOST_CHECK (fac.GetUnmannedFactoryCount(NEW_CHEMICALS_FACTORY) == 2);
  BOOST_CHECK(fac.GetUsedMen() == 0);
  BOOST_CHECK(fac.GetUsedRobots() == 0);

  BOOST_CHECK(fac.AlterManning("BBBBBHH",1,5) == -2);
  BOOST_CHECK (fac.GetFactoryDescription() == "|WU-WU-RU-NU-NU-SU-MU|");
  BOOST_CHECK (fac.GetVPs() == 0);
  BOOST_CHECK (fac.GetPeopleIncrease() == 0);
  BOOST_CHECK (fac.GetMannedFactoryCount(WATER_FACTORY) == 0);
  BOOST_CHECK (fac.GetMannedFactoryCount(NEW_CHEMICALS_FACTORY) == 0);
  BOOST_CHECK (fac.GetUnmannedFactoryCount(WATER_FACTORY) == 2);
  BOOST_CHECK (fac.GetUnmannedFactoryCount(NEW_CHEMICALS_FACTORY) == 2);
  BOOST_CHECK(fac.GetUsedMen() == 0);
  BOOST_CHECK(fac.GetUsedRobots() == 0);

  BOOST_CHECK(fac.AlterManning("BBURRHH",2,2) == -3);
  BOOST_CHECK (fac.GetFactoryDescription() == "|WU-WU-RU-NU-NU-SU-MU|");
  BOOST_CHECK (fac.GetVPs() == 0);
  BOOST_CHECK (fac.GetPeopleIncrease() == 0);
  BOOST_CHECK (fac.GetMannedFactoryCount(WATER_FACTORY) == 0);
  BOOST_CHECK (fac.GetMannedFactoryCount(NEW_CHEMICALS_FACTORY) == 0);
  BOOST_CHECK (fac.GetUnmannedFactoryCount(WATER_FACTORY) == 2);
  BOOST_CHECK (fac.GetUnmannedFactoryCount(NEW_CHEMICALS_FACTORY) == 2);
  BOOST_CHECK(fac.GetUsedMen() == 0);
  BOOST_CHECK(fac.GetUsedRobots() == 0);

  BOOST_CHECK(fac.AlterManning("UHHHUHB",4,1) == 0);
  BOOST_CHECK (fac.GetFactoryDescription() == "|WU-WH-RH-NH-NU-SH-MB|");
  BOOST_CHECK (fac.GetVPs() == 36);
  BOOST_CHECK (fac.GetPeopleIncrease() == 1);
  BOOST_CHECK (fac.GetMannedFactoryCount(WATER_FACTORY) == 1);
  BOOST_CHECK (fac.GetMannedFactoryCount(NEW_CHEMICALS_FACTORY) == 1);
  BOOST_CHECK (fac.GetUnmannedFactoryCount(WATER_FACTORY) == 1);
  BOOST_CHECK (fac.GetUnmannedFactoryCount(NEW_CHEMICALS_FACTORY) == 1);
  BOOST_CHECK(fac.GetUsedMen() == 4);
  BOOST_CHECK(fac.GetUsedRobots() == 1);

  BOOST_CHECK(fac.AlterManning("UBHHHHU",4,1) == 0);
  BOOST_CHECK (fac.GetFactoryDescription() == "|WU-WB-RH-NH-NH-SH-MU|");
  BOOST_CHECK (fac.GetVPs() == 19);
  BOOST_CHECK (fac.GetPeopleIncrease() == 1);
  BOOST_CHECK (fac.GetMannedFactoryCount(WATER_FACTORY) == 1);
  BOOST_CHECK (fac.GetMannedFactoryCount(NEW_CHEMICALS_FACTORY) == 2);
  BOOST_CHECK (fac.GetUnmannedFactoryCount(WATER_FACTORY) == 1);
  BOOST_CHECK (fac.GetUnmannedFactoryCount(NEW_CHEMICALS_FACTORY) == 0);
  BOOST_CHECK(fac.GetUsedMen() == 4);
  BOOST_CHECK(fac.GetUsedRobots() == 1);

  BOOST_CHECK(fac.AlterManning("UBHHHHH",5,1) == 0);
  BOOST_CHECK (fac.GetFactoryDescription() == "|WU-WB-RH-NH-NH-SH-MH|");
  BOOST_CHECK (fac.GetVPs() == 39);
  BOOST_CHECK (fac.GetPeopleIncrease() == 2);
  BOOST_CHECK (fac.GetMannedFactoryCount(WATER_FACTORY) == 1);
  BOOST_CHECK (fac.GetMannedFactoryCount(NEW_CHEMICALS_FACTORY) == 2);
  BOOST_CHECK (fac.GetUnmannedFactoryCount(WATER_FACTORY) == 1);
  BOOST_CHECK (fac.GetUnmannedFactoryCount(NEW_CHEMICALS_FACTORY) == 0);
  BOOST_CHECK(fac.GetUsedMen() == 5);
  BOOST_CHECK(fac.GetUsedRobots() == 1);

  BOOST_CHECK(fac.AlterManning("UBHHHHB",4,2) == 0);
  BOOST_CHECK (fac.GetFactoryDescription() == "|WU-WB-RH-NH-NH-SH-MB|");
  BOOST_CHECK (fac.GetVPs() == 39);
  BOOST_CHECK (fac.GetPeopleIncrease() == 1);
  BOOST_CHECK (fac.GetMannedFactoryCount(WATER_FACTORY) == 1);
  BOOST_CHECK (fac.GetMannedFactoryCount(NEW_CHEMICALS_FACTORY) == 2);
  BOOST_CHECK (fac.GetUnmannedFactoryCount(WATER_FACTORY) == 1);
  BOOST_CHECK (fac.GetUnmannedFactoryCount(NEW_CHEMICALS_FACTORY) == 0);
  BOOST_CHECK(fac.GetUsedMen() == 4);
  BOOST_CHECK(fac.GetUsedRobots() == 2);

  Factories tfac;

  SerialTransfer(fac,tfac);

  BOOST_CHECK (tfac.GetFactoryDescription() == "|WU-WB-RH-NH-NH-SH-MB|");
  BOOST_CHECK (tfac.GetVPs() == 39);
  BOOST_CHECK (tfac.GetPeopleIncrease() == 1);
  BOOST_CHECK (tfac.GetMannedFactoryCount(WATER_FACTORY) == 1);
  BOOST_CHECK (tfac.GetMannedFactoryCount(NEW_CHEMICALS_FACTORY) == 2);
  BOOST_CHECK (tfac.GetUnmannedFactoryCount(WATER_FACTORY) == 1);
  BOOST_CHECK (tfac.GetUnmannedFactoryCount(NEW_CHEMICALS_FACTORY) == 0);
  BOOST_CHECK(tfac.GetUsedMen() == 4);
  BOOST_CHECK(tfac.GetUsedRobots() == 2);

  BOOST_CHECK_EQUAL( tfac.size() , (size_t)7 );


}
  
