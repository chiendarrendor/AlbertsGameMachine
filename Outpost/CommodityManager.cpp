#include "CommodityManager.hpp"
#include <stdexcept>


CommodityManager::CommodityManager() 
{
  m_Commodities[ORE_COMMODITY]=CommodityDeck(ORE_COMMODITY,0,3);
  m_Commodities[ORE_COMMODITY].AddCards(1,4);
  m_Commodities[ORE_COMMODITY].AddCards(2,6);
  m_Commodities[ORE_COMMODITY].AddCards(3,6);
  m_Commodities[ORE_COMMODITY].AddCards(4,6);
  m_Commodities[ORE_COMMODITY].AddCards(5,4);

  m_Commodities[WATER_COMMODITY]=CommodityDeck(WATER_COMMODITY,30,7);
  m_Commodities[WATER_COMMODITY].AddCards(4,3);
  m_Commodities[WATER_COMMODITY].AddCards(5,5);
  m_Commodities[WATER_COMMODITY].AddCards(6,7);
  m_Commodities[WATER_COMMODITY].AddCards(7,9);
  m_Commodities[WATER_COMMODITY].AddCards(8,7);
  m_Commodities[WATER_COMMODITY].AddCards(9,5);
  m_Commodities[WATER_COMMODITY].AddCards(10,3);

  m_Commodities[TITANIUM_COMMODITY]=CommodityDeck(TITANIUM_COMMODITY,44,10);
  m_Commodities[TITANIUM_COMMODITY].AddCards(7,5);
  m_Commodities[TITANIUM_COMMODITY].AddCards(8,7);
  m_Commodities[TITANIUM_COMMODITY].AddCards(9,9);
  m_Commodities[TITANIUM_COMMODITY].AddCards(10,11);
  m_Commodities[TITANIUM_COMMODITY].AddCards(11,9);
  m_Commodities[TITANIUM_COMMODITY].AddCards(12,7);
  m_Commodities[TITANIUM_COMMODITY].AddCards(13,5);

  m_Commodities[RESEARCH_COMMODITY]=CommodityDeck(RESEARCH_COMMODITY,56,13);
  m_Commodities[RESEARCH_COMMODITY].AddCards(9,2);
  m_Commodities[RESEARCH_COMMODITY].AddCards(10,3);
  m_Commodities[RESEARCH_COMMODITY].AddCards(11,4);
  m_Commodities[RESEARCH_COMMODITY].AddCards(12,5);
  m_Commodities[RESEARCH_COMMODITY].AddCards(13,6);
  m_Commodities[RESEARCH_COMMODITY].AddCards(14,5);
  m_Commodities[RESEARCH_COMMODITY].AddCards(15,4);
  m_Commodities[RESEARCH_COMMODITY].AddCards(16,3);
  m_Commodities[RESEARCH_COMMODITY].AddCards(17,2);
  
  m_Commodities[MICROBIOTICS_COMMODITY]=CommodityDeck(MICROBIOTICS_COMMODITY,0,17);
  m_Commodities[MICROBIOTICS_COMMODITY].AddCards(14,1);
  m_Commodities[MICROBIOTICS_COMMODITY].AddCards(15,2);
  m_Commodities[MICROBIOTICS_COMMODITY].AddCards(16,3);
  m_Commodities[MICROBIOTICS_COMMODITY].AddCards(17,4);
  m_Commodities[MICROBIOTICS_COMMODITY].AddCards(18,3);
  m_Commodities[MICROBIOTICS_COMMODITY].AddCards(19,2);
  m_Commodities[MICROBIOTICS_COMMODITY].AddCards(20,1);

  m_Commodities[NEW_CHEMICALS_COMMODITY]=CommodityDeck(NEW_CHEMICALS_COMMODITY,88,20);
  m_Commodities[NEW_CHEMICALS_COMMODITY].AddCards(14,2);
  m_Commodities[NEW_CHEMICALS_COMMODITY].AddCards(16,3);
  m_Commodities[NEW_CHEMICALS_COMMODITY].AddCards(18,4);
  m_Commodities[NEW_CHEMICALS_COMMODITY].AddCards(20,5);
  m_Commodities[NEW_CHEMICALS_COMMODITY].AddCards(22,4);
  m_Commodities[NEW_CHEMICALS_COMMODITY].AddCards(24,3);
  m_Commodities[NEW_CHEMICALS_COMMODITY].AddCards(26,2);

  m_Commodities[ORBITAL_MEDICINE_COMMODITY]=CommodityDeck(ORBITAL_MEDICINE_COMMODITY,0,30);
  m_Commodities[ORBITAL_MEDICINE_COMMODITY].AddCards(20,2);
  m_Commodities[ORBITAL_MEDICINE_COMMODITY].AddCards(25,3);
  m_Commodities[ORBITAL_MEDICINE_COMMODITY].AddCards(30,4);
  m_Commodities[ORBITAL_MEDICINE_COMMODITY].AddCards(35,3);
  m_Commodities[ORBITAL_MEDICINE_COMMODITY].AddCards(40,2);

  m_Commodities[RING_ORE_COMMODITY]=CommodityDeck(RING_ORE_COMMODITY,0,40);
  m_Commodities[RING_ORE_COMMODITY].AddCards(30,1);
  m_Commodities[RING_ORE_COMMODITY].AddCards(35,3);
  m_Commodities[RING_ORE_COMMODITY].AddCards(40,4);
  m_Commodities[RING_ORE_COMMODITY].AddCards(45,3);
  m_Commodities[RING_ORE_COMMODITY].AddCards(50,1);

  m_Commodities[MOON_ORE_COMMODITY]=CommodityDeck(MOON_ORE_COMMODITY,0,50);
  m_Commodities[MOON_ORE_COMMODITY].AddCards(40,1);
  m_Commodities[MOON_ORE_COMMODITY].AddCards(45,3);
  m_Commodities[MOON_ORE_COMMODITY].AddCards(50,4);
  m_Commodities[MOON_ORE_COMMODITY].AddCards(55,3);
  m_Commodities[MOON_ORE_COMMODITY].AddCards(60,1);
}

CommodityDeck &CommodityManager::GetDeck(CommodityType i_ctype)
{
  return m_Commodities[i_ctype];
}

const CommodityDeck &CommodityManager::GetDeck(CommodityType i_ctype) const
{
  std::map<CommodityType,CommodityDeck>::const_iterator cit = m_Commodities.find(i_ctype);
  if (cit == m_Commodities.end()) throw std::runtime_error("Unknown CommodityType!");
  return cit->second;
}

void CommodityManager::ApplyResearchMegaStatus(bool i_hasmega)
{
  if (!i_hasmega) m_Commodities[RESEARCH_COMMODITY].ZeroMegaValue();
}
