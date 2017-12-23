#include "SpaceInfo.hpp"

SpaceInfo::SpaceInfo(const std::string &i_Name,SpaceType i_Type,
				   int i_Area,int i_MarketSize) :
  m_Name(i_Name),
  m_Type(i_Type),
  m_MarketSize(i_MarketSize),
  m_Area(i_Area),
  m_IsOverseas(false)
{
}

void SpaceInfo::SetOverseas()
{
	m_IsOverseas = true;
}

bool SpaceInfo::IsOverseas() const
{
	return m_IsOverseas;
}

const std::string &SpaceInfo::GetName() const
{
	return m_Name;
}

SpaceType SpaceInfo::GetSpaceType() const
{
	return m_Type;
}

int SpaceInfo::GetArea() const
{
	return m_Area;
}

int SpaceInfo::GetMarketSize() const
{
	return m_MarketSize;
}

bool SpaceInfo::HasLinkTo(const SpaceInfo &i_rLink) const
{
	return m_Links.GetSpace(i_rLink.GetName()) != NULL;
}

void SpaceInfo::AddSatellite(SpaceInfo &i_rSatellite)
{
	m_Satellites.Insert(i_rSatellite);
}


void SpaceInfo::AddLink(SpaceInfo &i_pLink)
{
	m_Links.Insert(i_pLink);
}

void SpaceInfo::AddCommodity(const Commodity &i_rCommodity)
{
  m_Commodities.push_back(&i_rCommodity);
}


const std::vector<const Commodity *> &SpaceInfo::GetCommodities() const
{
	return m_Commodities;
}

const SpaceInfoMap &SpaceInfo::GetLinks() const
{
	return m_Links;
}

const SpaceInfoMap &SpaceInfo::GetSatellites() const
{
	return m_Satellites;
}


