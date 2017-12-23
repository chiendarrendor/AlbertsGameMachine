#include "CommodityManager.hpp"
#include "SerializeUtility.hpp"
#include <boost/test/auto_unit_test.hpp>
#include <iostream>

class CommodityDeckData
{
public:
  CommodityDeckData(CommodityType i_ctype,int i_AverageValue,int i_MegaValue) :
    m_ctype(i_ctype),
    m_AverageValue(i_AverageValue),
    m_MegaValue(i_MegaValue),
    m_NumCards(0)
  {}
  CommodityDeckData &operator()(int i_Value,int i_Count)
  {
    m_NumCards += i_Count;
    m_Cards.push_back(std::pair<int,int>(i_Value,i_Count));
    return *this;
  }

  void GatherDeckA(CommodityManager &i_cm)
  {
    int i;
    for (i = 0 ; i < 7 ; ++i)
    {
      Commodity c = i_cm.GetDeck(m_ctype).DrawCommodity(false);
      BOOST_CHECK(c.GetType() == m_ctype);
      BOOST_CHECK(c.IsAverage() == false);
      BOOST_CHECK(c.IsMega() == false);
      m_CardCount[c.GetValue()]++;
    }
  }
  void GatherDeckB(CommodityManager &i_cm)
  {
    int i;
    for (i = 0 ; i < m_NumCards - 7 ; ++i)
    {
      Commodity c = i_cm.GetDeck(m_ctype).DrawCommodity(false);
      BOOST_CHECK(c.GetType() == m_ctype);
      BOOST_CHECK(c.IsAverage() == false);
      BOOST_CHECK(c.IsMega() == false);
      m_CardCount[c.GetValue()]++;
    }
    
    {
      Commodity c = i_cm.GetDeck(m_ctype).DrawCommodity(false);
      BOOST_CHECK(c.GetType() == m_ctype);
      BOOST_CHECK(c.IsAverage() == true);
      BOOST_CHECK(c.IsMega() == false);
      BOOST_CHECK(c.GetValue() == m_AverageValue);
    }

    if (m_MegaValue == 0)
    {
      i_cm.GetDeck(m_ctype).HasMega() == false;
    }
    else
    {
      Commodity c = i_cm.GetDeck(m_ctype).DrawCommodity(true);
      BOOST_CHECK(c.GetType() == m_ctype);
      BOOST_CHECK(c.IsAverage() == false);
      BOOST_CHECK(c.IsMega() == true);
      BOOST_CHECK(c.GetValue() == m_MegaValue);
    }      
  }

  void ValidateDeck()
  {
    BOOST_CHECK(m_Cards.size() == m_CardCount.size());
    size_t i;

    for (i = 0  ; i < m_Cards.size() ; ++i)
    {
      BOOST_CHECK(m_CardCount[m_Cards[i].first] == m_Cards[i].second);
    }
  }

private:
  CommodityType m_ctype;
  int m_AverageValue;
  int m_MegaValue;
  std::vector<std::pair<int,int> > m_Cards;
  std::map<int,int> m_CardCount;
  int m_NumCards;
};

BOOST_AUTO_TEST_CASE( CommodityManagerResearchMega )
{
  std::vector<CommodityDeckData *> decks;

  CommodityDeckData oredeck(ORE_COMMODITY,3,0);
  oredeck(1,4)(2,6)(3,6)(4,6)(5,4);
  decks.push_back(&oredeck);

  CommodityDeckData waterdeck(WATER_COMMODITY,7,30);
  waterdeck(4,3)(5,5)(6,7)(7,9)(8,7)(9,5)(10,3);
  decks.push_back(&waterdeck);
  
  CommodityDeckData titdeck(TITANIUM_COMMODITY,10,44);
  titdeck(7,5)(8,7)(9,9)(10,11)(11,9)(12,7)(13,5);
  decks.push_back(&titdeck);

  CommodityDeckData resdeck(RESEARCH_COMMODITY,13,56);
  resdeck(9,2)(10,3)(11,4)(12,5)(13,6)(14,5)(15,4)(16,3)(17,2);
  decks.push_back(&resdeck);

  CommodityDeckData microdeck(MICROBIOTICS_COMMODITY,17,0);
  microdeck(14,1)(15,2)(16,3)(17,4)(18,3)(19,2)(20,1);
  decks.push_back(&microdeck);

  CommodityDeckData ncdeck(NEW_CHEMICALS_COMMODITY,20,88);
  ncdeck(14,2)(16,3)(18,4)(20,5)(22,4)(24,3)(26,2);
  decks.push_back(&ncdeck);

  CommodityDeckData omdeck(ORBITAL_MEDICINE_COMMODITY,30,0);
  omdeck(20,2)(25,3)(30,4)(35,3)(40,2);
  decks.push_back(&omdeck);

  CommodityDeckData rodeck(RING_ORE_COMMODITY,40,0);
  rodeck(30,1)(35,3)(40,4)(45,3)(50,1);
  decks.push_back(&rodeck);

  CommodityDeckData modeck(MOON_ORE_COMMODITY,50,0);
  modeck(40,1)(45,3)(50,4)(55,3)(60,1);
  decks.push_back(&modeck);

  CommodityManager cm;

  size_t i;

  for (i = 0 ; i < decks.size() ; ++i)
  {
    decks[i]->GatherDeckA(cm);
  }

  CommodityManager cm2;

  SerialTransfer(cm,cm2);

  for (i = 0 ; i < decks.size() ; ++i)
  {
    decks[i]->GatherDeckB(cm2);
  }

  for (i = 0 ; i < decks.size() ; ++i)
  {
    decks[i]->ValidateDeck();
  }
}  


BOOST_AUTO_TEST_CASE( CommodityManagerResearchNoMega )
{
  std::vector<CommodityDeckData *> decks;
  CommodityDeckData oredeck(ORE_COMMODITY,3,0);
  oredeck(1,4)(2,6)(3,6)(4,6)(5,4);
  decks.push_back(&oredeck);

  CommodityDeckData waterdeck(WATER_COMMODITY,7,30);
  waterdeck(4,3)(5,5)(6,7)(7,9)(8,7)(9,5)(10,3);
  decks.push_back(&waterdeck);
  
  CommodityDeckData titdeck(TITANIUM_COMMODITY,10,44);
  titdeck(7,5)(8,7)(9,9)(10,11)(11,9)(12,7)(13,5);
  decks.push_back(&titdeck);

  CommodityDeckData resdeck(RESEARCH_COMMODITY,13,0);
  resdeck(9,2)(10,3)(11,4)(12,5)(13,6)(14,5)(15,4)(16,3)(17,2);
  decks.push_back(&resdeck);

  CommodityDeckData microdeck(MICROBIOTICS_COMMODITY,17,0);
  microdeck(14,1)(15,2)(16,3)(17,4)(18,3)(19,2)(20,1);
  decks.push_back(&microdeck);

  CommodityDeckData ncdeck(NEW_CHEMICALS_COMMODITY,20,88);
  ncdeck(14,2)(16,3)(18,4)(20,5)(22,4)(24,3)(26,2);
  decks.push_back(&ncdeck);

  CommodityDeckData omdeck(ORBITAL_MEDICINE_COMMODITY,30,0);
  omdeck(20,2)(25,3)(30,4)(35,3)(40,2);
  decks.push_back(&omdeck);

  CommodityDeckData rodeck(RING_ORE_COMMODITY,40,0);
  rodeck(30,1)(35,3)(40,4)(45,3)(50,1);
  decks.push_back(&rodeck);

  CommodityDeckData modeck(MOON_ORE_COMMODITY,50,0);
  modeck(40,1)(45,3)(50,4)(55,3)(60,1);
  decks.push_back(&modeck);

  CommodityManager cm;
  cm.ApplyResearchMegaStatus(false);

  size_t i;

  for (i = 0 ; i < decks.size() ; ++i)
  {
    decks[i]->GatherDeckA(cm);
  }

  CommodityManager cm2;

  SerialTransfer(cm,cm2);

  for (i = 0 ; i < decks.size() ; ++i)
  {
    decks[i]->GatherDeckB(cm2);
  }

  for (i = 0 ; i < decks.size() ; ++i)
  {
    decks[i]->ValidateDeck();
  }
}  
