#include "CommodityInfo.hpp"

const std::string CommodityInfo::STONE("Stone");
const std::string CommodityInfo::WOOL("Wool");
const std::string CommodityInfo::TIMBER("Timber");
const std::string CommodityInfo::GRAIN("Grain");
const std::string CommodityInfo::CLOTH("Cloth");
const std::string CommodityInfo::WINE("Wine");
const std::string CommodityInfo::METAL("Metal");
const std::string CommodityInfo::FUR("Fur");
const std::string CommodityInfo::SILK("Silk");
const std::string CommodityInfo::SPICE("Spice");
const std::string CommodityInfo::GOLD("Gold");
const std::string CommodityInfo::IVORY("Ivory");

// ------------------- Commodity -------------------------

Commodity::Commodity(const std::string &i_Name,int i_Value,int i_SSNum) :
m_Name(i_Name),
m_Value(i_Value),
m_SSNum(i_SSNum)
{
}

Commodity::~Commodity()
{
}

const std::string &Commodity::GetName() const
{
	return m_Name;
}

int Commodity::GetSSNum() const
{
	return m_SSNum;
}

int Commodity::GetValue() const
{
	return m_Value;
}

// ----------------------- CommodityLog --------------------------

CommodityInfo::CommodityInfo() 
{
	m_Commodities[STONE]= new Commodity(STONE, 1, 2);
	m_Commodities[WOOL]=  new Commodity(WOOL,  2, 3);
	m_Commodities[TIMBER]=new Commodity(TIMBER,3, 4);
	m_Commodities[GRAIN]= new Commodity(GRAIN, 4, 5);
	m_Commodities[CLOTH]= new Commodity(CLOTH, 5, 6);
	m_Commodities[WINE]=  new Commodity(WINE,  5, 7);
	m_Commodities[METAL]= new Commodity(METAL, 6, 8);
	m_Commodities[FUR]=   new Commodity(FUR,   7, 9);
	m_Commodities[SILK]=  new Commodity(SILK,  8,10);
	m_Commodities[SPICE]= new Commodity(SPICE, 9,11);
	m_Commodities[GOLD]=  new Commodity(GOLD, 10,12);
	m_Commodities[IVORY]= new Commodity(IVORY,10,12);
}

Commodity *CommodityInfo::GetCommodityByName(const std::string &i_Name) const
{
	std::map<std::string,Commodity*>::const_iterator findit = m_Commodities.find(i_Name);
	
	if (findit == m_Commodities.end())
	{
		return NULL;
	}

	return findit->second;
}

CommodityInfo::~CommodityInfo()
{
	std::map<std::string,Commodity*>::iterator doomed;
	for (doomed = m_Commodities.begin() ; doomed != m_Commodities.end() ; doomed++)
	{
		delete doomed->second;
	}
}

const std::map<std::string,Commodity *> &CommodityInfo::GetCommodities() const
{
  return m_Commodities;
}

