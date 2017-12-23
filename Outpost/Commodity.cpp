
#include "Commodity.hpp"

Commodity::Commodity(CommodityType i_Type,int i_Value,bool i_IsAverage,bool i_IsMega) :
  m_ctype(i_Type),
  m_IsAverage(i_IsAverage),
  m_IsMega(i_IsMega),
  m_value(i_Value),
  m_IsNew(true)
{
}

Commodity::Commodity() :
  m_ctype(ORE_COMMODITY),
  m_IsAverage(false),
  m_IsMega(false),
  m_value(0),
  m_IsNew(true)
{
}


CommodityType Commodity::GetType() const
{
  return m_ctype;
}

bool Commodity::IsAverage() const
{
  return m_IsAverage;
}

bool Commodity::IsMega() const
{
  return m_IsMega;
}

int Commodity::GetValue() const
{
  return m_value;
}

bool Commodity::IsNew() const
{
  return m_IsNew;
}

void Commodity::MakeNew()
{
  m_IsNew = true;
}

void Commodity::MakeOld()
{
  m_IsNew = false;
}

