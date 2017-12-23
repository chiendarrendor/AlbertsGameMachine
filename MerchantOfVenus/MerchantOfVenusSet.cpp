#include "MerchantOfVenusSet.hpp"
#include "MapData.hpp"
#include <iostream>

MerchantOfVenusSet::MerchantOfVenusSet(const std::string &i_DataDir) :
  m_pmapdata(NULL),
  m_valid(false)
{
  try
  {
    m_pmapdata = new MapData(i_DataDir + "/MerchantOfVenusMap.xml");
  }
  catch(std::exception& e)
  {
    std::cout << "Merchant Of Venus Map Load Exception: " << e.what() << std::endl;
    return;
  }
  m_valid = true;
}

bool MerchantOfVenusSet::IsValid() const
{
  return m_valid;
}

const MapData& MerchantOfVenusSet::GetMapData() const
{
  return *m_pmapdata;
}

