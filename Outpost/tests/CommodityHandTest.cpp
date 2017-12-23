#include "CommodityHand.hpp"
#include "SerializeUtility.hpp"
#include <boost/test/auto_unit_test.hpp>


BOOST_AUTO_TEST_CASE ( CommodityHandConstructor )
{
  CommodityHand ch;

  BOOST_CHECK_EQUAL( ch.GetHandDescription(true,true) , "||");
  BOOST_CHECK_EQUAL( ch.GetHandDescription(false,true) , "||");
  BOOST_CHECK_EQUAL( ch.GetHandDescription(true,false) , "||");
  BOOST_CHECK_EQUAL( ch.GetHandDescription(false,false) , "||");

  BOOST_CHECK_EQUAL( ch.GetHandWeight(false,false) , 0 );
  BOOST_CHECK_EQUAL( ch.GetHandWeight(false,true) , 0 );
  BOOST_CHECK_EQUAL( ch.GetHandWeight(true,false) , 0 );
  BOOST_CHECK_EQUAL( ch.GetHandWeight(true,true) , 0 );
  BOOST_CHECK_EQUAL( ch.GetHandValue() , 0 );
  BOOST_CHECK_EQUAL( ch.GetNumResearch() , 0);


  Commodity c = Commodity(WATER_COMMODITY,30,false,true);
  c.MakeOld();
  ch.AddCommodity( c );     // WA30M

  ch.AddCommodity( Commodity(MOON_ORE_COMMODITY,55,false,false) ); // MO55
  ch.AddCommodity( Commodity(RESEARCH_COMMODITY,13,false,false) ); // RE13
  ch.AddCommodity( Commodity(MOON_ORE_COMMODITY,45,false,false) ); // MO45

  c = Commodity(WATER_COMMODITY,7,true,false);
  ch.AddCommodity( c );      // WA7A

  ch.AddCommodity( Commodity(RESEARCH_COMMODITY,17,false,false) ); // RE17
  ch.AddCommodity( Commodity(RESEARCH_COMMODITY,56,false,true) );  // RE56M

  c =  Commodity(MICROBIOTICS_COMMODITY,17,false,false);
  c.MakeOld();

  ch.AddCommodity( c ); // MI17
  ch.AddCommodity( Commodity(WATER_COMMODITY,8,false,false) );     // WA8

  c = Commodity(WATER_COMMODITY,4,false,false);
  c.MakeOld();

  ch.AddCommodity( c );     // WA4
  ch.AddCommodity( Commodity(WATER_COMMODITY,7,true,false) );      // WA7A

  BOOST_CHECK_EQUAL( ch.GetHandWeight(false,false) , 17 );
  BOOST_CHECK_EQUAL( ch.GetHandWeight(false,true) , 16 );
  BOOST_CHECK_EQUAL( ch.GetHandWeight(true,false) , 11 );
  BOOST_CHECK_EQUAL( ch.GetHandWeight(true,true) , 10 );
  BOOST_CHECK_EQUAL( ch.GetHandValue() , 30 + 55 + 13 + 45 + 7 + 17 + 56 + 8 + 4 + 7 + 17);

  BOOST_CHECK_EQUAL( ch.GetHandDescription(true,true) , "|MO55-MO45-MI17-RE56M-RE17-RE13-WA30M-WA8-WA4-WA7A-WA7A|");
  BOOST_CHECK_EQUAL( ch.GetHandDescription(false,false) , "|MO*-MO*-MI*-RE56M-RE*-RE*-WA30M-WA*-WA*-WA7A-WA7A|");
  BOOST_CHECK_EQUAL( ch.GetHandDescription(true,false) , "|MO*-MO*-MI17-RE56M-RE*-RE*-WA30M-WA*-WA4-WA7A-WA7A|");
  BOOST_CHECK_EQUAL( ch.GetHandDescription(false,true) , "|MO*-MO*-MI*-RE56M-RE*-RE*-WA30M-WA*-WA*-WA7A-WA7A|");

  BOOST_CHECK_EQUAL( ch.GetNumResearch() , 6);

  // WA30M = 6;
  // WA4 = 8

  size_t i;
  for (i = 0 ; i < ch.GetHand().size() ; ++i)
  {
    if (i == 2 || i == 6 || i == 8 )
    {
      BOOST_CHECK_MESSAGE( ch.GetHand()[i].IsNew() == false , "IsNew, Index " << i );
    }
    else
    {
      BOOST_CHECK_MESSAGE( ch.GetHand()[i].IsNew() == true , "IsNew, Index " << i );
    }
  }

  CommodityHand ch2(ch);
  
  BOOST_CHECK_EQUAL( ch2.GetHandWeight(false,false) , 17 );
  BOOST_CHECK_EQUAL( ch2.GetHandWeight(false,true) , 16 );
  BOOST_CHECK_EQUAL( ch2.GetHandWeight(true,false) , 11 );
  BOOST_CHECK_EQUAL( ch2.GetHandWeight(true,true) , 10 );
  BOOST_CHECK_EQUAL( ch2.GetHandValue() , 30 + 55 + 13 + 45 + 7 + 17 + 56 + 8 + 4 + 7 + 17);
  BOOST_CHECK_EQUAL( ch2.GetHandDescription(true,true) , "|MO55-MO45-MI17-RE56M-RE17-RE13-WA30M-WA8-WA4-WA7A-WA7A|");
  BOOST_CHECK_EQUAL( ch2.GetHandDescription(false,false) , "|MO*-MO*-MI*-RE56M-RE*-RE*-WA30M-WA*-WA*-WA7A-WA7A|");
  BOOST_CHECK_EQUAL( ch2.GetHandDescription(true,false) , "|MO*-MO*-MI17-RE56M-RE*-RE*-WA30M-WA*-WA4-WA7A-WA7A|");
  BOOST_CHECK_EQUAL( ch2.GetHandDescription(false,true) , "|MO*-MO*-MI*-RE56M-RE*-RE*-WA30M-WA*-WA*-WA7A-WA7A|");
  BOOST_CHECK_EQUAL( ch2.GetNumResearch() , 6);

  for (i = 0 ; i < ch2.GetHand().size() ; ++i)
  {
    if (i == 2 || i == 6 || i == 8 )
    {
      BOOST_CHECK_MESSAGE( ch.GetHand()[i].IsNew() == false , "IsNew, Index " << i );
    }
    else
    {
      BOOST_CHECK_MESSAGE( ch.GetHand()[i].IsNew() == true , "IsNew, Index " << i );
    }
  }



  CommodityHand ch3;
  SerialTransfer(ch,ch3);

  BOOST_CHECK_EQUAL( ch3.GetHandWeight(false,false) , 17 );
  BOOST_CHECK_EQUAL( ch3.GetHandWeight(false,true) , 16 );
  BOOST_CHECK_EQUAL( ch3.GetHandWeight(true,false) , 11 );
  BOOST_CHECK_EQUAL( ch3.GetHandWeight(true,true) , 10 );
  BOOST_CHECK_EQUAL( ch3.GetHandValue() , 30 + 55 + 13 + 45 + 7 + 17 + 56 + 8 + 4 + 7 + 17);
  BOOST_CHECK_EQUAL( ch3.GetHandDescription(true,true) , "|MO55-MO45-MI17-RE56M-RE17-RE13-WA30M-WA8-WA4-WA7A-WA7A|");
  BOOST_CHECK_EQUAL( ch3.GetHandDescription(false,false) , "|MO*-MO*-MI*-RE56M-RE*-RE*-WA30M-WA*-WA*-WA7A-WA7A|");
  BOOST_CHECK_EQUAL( ch3.GetHandDescription(true,false) , "|MO*-MO*-MI17-RE56M-RE*-RE*-WA30M-WA*-WA4-WA7A-WA7A|");
  BOOST_CHECK_EQUAL( ch3.GetHandDescription(false,true) , "|MO*-MO*-MI*-RE56M-RE*-RE*-WA30M-WA*-WA*-WA7A-WA7A|");
  BOOST_CHECK_EQUAL( ch3.GetNumResearch() , 6);

  for (i = 0 ; i < ch3.GetHand().size() ; ++i)
  {
    if (i == 2 || i == 6 || i == 8 )
    {
      BOOST_CHECK_MESSAGE( ch.GetHand()[i].IsNew() == false , "IsNew, Index " << i );
    }
    else
    {
      BOOST_CHECK_MESSAGE( ch.GetHand()[i].IsNew() == true , "IsNew, Index " << i );
    }
  }

  CommodityHand ch4;
  ch4 = ch;

  BOOST_CHECK_EQUAL( ch4.GetHandWeight(false,false) , 17 );
  BOOST_CHECK_EQUAL( ch4.GetHandWeight(false,true) , 16 );
  BOOST_CHECK_EQUAL( ch4.GetHandWeight(true,false) , 11 );
  BOOST_CHECK_EQUAL( ch4.GetHandWeight(true,true) , 10 );
  BOOST_CHECK_EQUAL( ch4.GetHandValue() , 30 + 55 + 13 + 45 + 7 + 17 + 56 + 8 + 4 + 7 + 17);
  BOOST_CHECK_EQUAL( ch4.GetHandDescription(true,true) , "|MO55-MO45-MI17-RE56M-RE17-RE13-WA30M-WA8-WA4-WA7A-WA7A|");
  BOOST_CHECK_EQUAL( ch4.GetHandDescription(false,false) , "|MO*-MO*-MI*-RE56M-RE*-RE*-WA30M-WA*-WA*-WA7A-WA7A|");
  BOOST_CHECK_EQUAL( ch4.GetHandDescription(true,false) , "|MO*-MO*-MI17-RE56M-RE*-RE*-WA30M-WA*-WA4-WA7A-WA7A|");
  BOOST_CHECK_EQUAL( ch4.GetHandDescription(false,true) , "|MO*-MO*-MI*-RE56M-RE*-RE*-WA30M-WA*-WA*-WA7A-WA7A|");
  BOOST_CHECK_EQUAL( ch4.GetNumResearch() , 6);

  for (i = 0 ; i < ch4.GetHand().size() ; ++i)
  {
    if (i == 2 || i == 6 || i == 8 )
    {
      BOOST_CHECK_MESSAGE( ch.GetHand()[i].IsNew() == false , "IsNew, Index " << i );
    }
    else
    {
      BOOST_CHECK_MESSAGE( ch.GetHand()[i].IsNew() == true , "IsNew, Index " << i );
    }
  }

  ch4.AgeHand();

  for (i = 0 ; i < ch4.GetHand().size() ; ++i)
  {
      BOOST_CHECK_MESSAGE( ch4.GetHand()[i].IsNew() == false , "IsNew, Index " << i );
  }

  std::vector<Commodity> deletes;
  BOOST_CHECK_EQUAL( ch.ApplyDeletes("111000111000",deletes), -1 );
  BOOST_CHECK_EQUAL( ch.GetHandWeight(false,false) , 17 );
  BOOST_CHECK_EQUAL( ch.GetHandWeight(false,true) , 16 );
  BOOST_CHECK_EQUAL( ch.GetHandWeight(true,false) , 11 );
  BOOST_CHECK_EQUAL( ch.GetHandWeight(true,true) , 10 );
  BOOST_CHECK_EQUAL( ch.GetHandValue() , 30 + 55 + 13 + 45 + 7 + 17 + 56 + 8 + 4 + 7 + 17);
  BOOST_CHECK_EQUAL( ch.GetHandDescription(true,true) , "|MO55-MO45-MI17-RE56M-RE17-RE13-WA30M-WA8-WA4-WA7A-WA7A|");
  BOOST_CHECK_EQUAL( ch.GetHandDescription(false,false) , "|MO*-MO*-MI*-RE56M-RE*-RE*-WA30M-WA*-WA*-WA7A-WA7A|");
  BOOST_CHECK_EQUAL( ch.GetHandDescription(true,false) , "|MO*-MO*-MI17-RE56M-RE*-RE*-WA30M-WA*-WA4-WA7A-WA7A|");
  BOOST_CHECK_EQUAL( ch.GetHandDescription(false,true) , "|MO*-MO*-MI*-RE56M-RE*-RE*-WA30M-WA*-WA*-WA7A-WA7A|");
  BOOST_CHECK_EQUAL( ch.GetNumResearch() , 6);

  BOOST_CHECK_EQUAL( ch.ApplyDeletes("1110001001",deletes), -1 );
  BOOST_CHECK_EQUAL( ch.GetHandWeight(false,false) , 17 );
  BOOST_CHECK_EQUAL( ch.GetHandWeight(false,true) , 16 );
  BOOST_CHECK_EQUAL( ch.GetHandWeight(true,false) , 11 );
  BOOST_CHECK_EQUAL( ch.GetHandWeight(true,true) , 10 );
  BOOST_CHECK_EQUAL( ch.GetHandValue() , 30 + 55 + 13 + 45 + 7 + 17 + 56 + 8 + 4 + 7 + 17);
  BOOST_CHECK_EQUAL( ch.GetHandDescription(true,true) , "|MO55-MO45-MI17-RE56M-RE17-RE13-WA30M-WA8-WA4-WA7A-WA7A|");
  BOOST_CHECK_EQUAL( ch.GetHandDescription(false,false) , "|MO*-MO*-MI*-RE56M-RE*-RE*-WA30M-WA*-WA*-WA7A-WA7A|");
  BOOST_CHECK_EQUAL( ch.GetHandDescription(true,false) , "|MO*-MO*-MI17-RE56M-RE*-RE*-WA30M-WA*-WA4-WA7A-WA7A|");
  BOOST_CHECK_EQUAL( ch.GetHandDescription(false,true) , "|MO*-MO*-MI*-RE56M-RE*-RE*-WA30M-WA*-WA*-WA7A-WA7A|");
  BOOST_CHECK_EQUAL( ch.GetNumResearch() , 6);

  BOOST_CHECK_EQUAL( ch.ApplyDeletes("1110B010000",deletes), -2 );
  BOOST_CHECK_EQUAL( ch.GetHandWeight(false,false) , 17 );
  BOOST_CHECK_EQUAL( ch.GetHandWeight(false,true) , 16 );
  BOOST_CHECK_EQUAL( ch.GetHandWeight(true,false) , 11 );
  BOOST_CHECK_EQUAL( ch.GetHandWeight(true,true) , 10 );
  BOOST_CHECK_EQUAL( ch.GetHandValue() , 30 + 55 + 13 + 45 + 7 + 17 + 56 + 8 + 4 + 7 + 17);
  BOOST_CHECK_EQUAL( ch.GetHandDescription(true,true) , "|MO55-MO45-MI17-RE56M-RE17-RE13-WA30M-WA8-WA4-WA7A-WA7A|");
  BOOST_CHECK_EQUAL( ch.GetHandDescription(false,false) , "|MO*-MO*-MI*-RE56M-RE*-RE*-WA30M-WA*-WA*-WA7A-WA7A|");
  BOOST_CHECK_EQUAL( ch.GetHandDescription(true,false) , "|MO*-MO*-MI17-RE56M-RE*-RE*-WA30M-WA*-WA4-WA7A-WA7A|");
  BOOST_CHECK_EQUAL( ch.GetHandDescription(false,true) , "|MO*-MO*-MI*-RE56M-RE*-RE*-WA30M-WA*-WA*-WA7A-WA7A|");
  BOOST_CHECK_EQUAL( ch.GetNumResearch() , 6);

  BOOST_CHECK_EQUAL( ch.ApplyDeletes("11010001100",deletes),0 );
  BOOST_CHECK_EQUAL( ch.GetHandWeight(false,false) , 9 );
  BOOST_CHECK_EQUAL( ch.GetHandWeight(false,true) , 8 );
  BOOST_CHECK_EQUAL( ch.GetHandWeight(true,false) , 7 );
  BOOST_CHECK_EQUAL( ch.GetHandWeight(true,true) , 6 );
  BOOST_CHECK_EQUAL( ch.GetHandValue() , 17 + 13 + 30 + 7 + 7 + 17);
  BOOST_CHECK_EQUAL( ch.GetHandDescription(true,true) , "|MI17-RE17-RE13-WA30M-WA7A-WA7A|");
  BOOST_CHECK_EQUAL( ch.GetHandDescription(false,false) , "|MI*-RE*-RE*-WA30M-WA7A-WA7A|");
  BOOST_CHECK_EQUAL( ch.GetHandDescription(true,false) , "|MI17-RE*-RE*-WA30M-WA7A-WA7A|");
  BOOST_CHECK_EQUAL( ch.GetHandDescription(false,true) , "|MI*-RE*-RE*-WA30M-WA7A-WA7A|");
  BOOST_CHECK_EQUAL( ch.GetNumResearch() , 2);

  BOOST_CHECK_EQUAL( deletes.size() , (size_t)5);
  BOOST_CHECK_EQUAL( deletes[0].GetType() , MOON_ORE_COMMODITY );
  BOOST_CHECK_EQUAL( deletes[0].GetValue() , 55 );
  BOOST_CHECK_EQUAL( deletes[0].IsMega() , false );
  BOOST_CHECK_EQUAL( deletes[0].IsAverage() , false );
  BOOST_CHECK_EQUAL( deletes[1].GetType() , MOON_ORE_COMMODITY );
  BOOST_CHECK_EQUAL( deletes[1].GetValue() , 45 );
  BOOST_CHECK_EQUAL( deletes[1].IsMega() , false );
  BOOST_CHECK_EQUAL( deletes[1].IsAverage() , false );
  BOOST_CHECK_EQUAL( deletes[2].GetType() , RESEARCH_COMMODITY );
  BOOST_CHECK_EQUAL( deletes[2].GetValue() , 56 );
  BOOST_CHECK_EQUAL( deletes[2].IsMega() , true );
  BOOST_CHECK_EQUAL( deletes[2].IsAverage() , false );
  BOOST_CHECK_EQUAL( deletes[3].GetType() , WATER_COMMODITY );
  BOOST_CHECK_EQUAL( deletes[3].GetValue() , 8 );
  BOOST_CHECK_EQUAL( deletes[3].IsMega() , false );
  BOOST_CHECK_EQUAL( deletes[3].IsAverage() , false );
  BOOST_CHECK_EQUAL( deletes[4].GetType() , WATER_COMMODITY );
  BOOST_CHECK_EQUAL( deletes[4].GetValue() , 4 );
  BOOST_CHECK_EQUAL( deletes[4].IsMega() , false );
  BOOST_CHECK_EQUAL( deletes[4].IsAverage() , false );
}
