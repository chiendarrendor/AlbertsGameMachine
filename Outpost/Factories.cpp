#include "Factories.hpp"
#include <sstream>

Factories::Factories() :
  m_Factories()
{
}

struct FactSort
{
  bool operator()(const Factories::FactoryDatum &i_left,
                  const Factories::FactoryDatum &i_right)
  {
    return i_left.first < i_right.first;
  }
};


void Factories::AddFactory(FactoryType i_FType)
{
  m_Factories.push_back(FactoryDatum(i_FType,UNMANNED));
  std::stable_sort(m_Factories.begin(),m_Factories.end(),FactSort());
}

std::string Factories::GetFactoryDescription() const
{
  std::ostringstream oss;

  oss << "|";
  
  size_t i;

  for (i = 0 ; i < m_Factories.size(); ++i)
  {
    if (i != 0)
    {
      oss << "-";
    }

    const FactoryDatum &fd = m_Factories[i];
    switch(fd.first)
    {
    case ORE_FACTORY:      oss << "O"; break;
    case WATER_FACTORY:    oss << "W"; break;
    case TITANIUM_FACTORY: oss << "T"; break;
    case RESEARCH_FACTORY: oss << "R"; break;
    case NEW_CHEMICALS_FACTORY: oss << "N"; break;
    case SPACE_STATION_FACTORY: oss << "S"; break;
    case PLANETARY_CRUISER_FACTORY: oss << "P"; break;
    case MOON_BASE_FACTORY: oss << "M"; break;
    case NO_FACTORY: oss << "?"; break;
    }

    switch(fd.second)
    {
    case UNMANNED: oss << "U" ; break;
    case HUMAN: oss << "H" ; break;
    case ROBOT: oss << "B" ; break;
    }
  }
  
  oss << "|";
  
  return oss.str();
}

// -1 = invalid manninglist...not the same size as the Factory List
// -2 = invalid manninglist...too many robots or people.
// -3 = invalid manninglist...illegal character
// -4 = invalid manninglist...moving a human off of a mannable item
//    -- obsoleting this error
//  0 = valid manning alteration

int Factories::AlterManning(const std::string &i_ManningList,int i_NPeople,int i_NRobots)
{
  size_t i;

  if (i_ManningList.size() != m_Factories.size()) return -1;
  
  int pcount = 0;
  int rcount = 0;
  int ucount = 0;

  for (i = 0 ; i < i_ManningList.size() ; ++i)
  {
    if (i_ManningList[i] == 'H') pcount++;
    else if (i_ManningList[i] == 'B') rcount++;
    else if (i_ManningList[i] == 'U') ucount++;
    else return -3;

    /*
    if (m_Factories[i].first == SPACE_STATION_FACTORY ||
        m_Factories[i].first == PLANETARY_CRUISER_FACTORY ||
        m_Factories[i].first == MOON_BASE_FACTORY)
    {
      if (m_Factories[i].second == HUMAN &&
          i_ManningList[i] != 'H') return -4;
    }
    */
  }
  
  if (pcount > i_NPeople || rcount > i_NRobots) return -2;

  for (i = 0 ; i < i_ManningList.size() ; ++i)
  {
    if (i_ManningList[i] == 'H') m_Factories[i].second = HUMAN;
    if (i_ManningList[i] == 'B') m_Factories[i].second = ROBOT;
    if (i_ManningList[i] == 'U') m_Factories[i].second = UNMANNED;
  }

  return 0;
}

int Factories::GetVPs() const
{
  size_t i;
  int result = 0;

  for (i = 0 ; i < m_Factories.size() ; ++i)
  {
    const FactoryDatum &fd = m_Factories[i];
    if (fd.second == UNMANNED) continue;

    switch(fd.first)
    {
    case NO_FACTORY:
      result += 0;
      break;
    case ORE_FACTORY:
    case WATER_FACTORY:
      result += 1;
      break;
    case TITANIUM_FACTORY:
    case RESEARCH_FACTORY:
      result += 2;
      break;
    case NEW_CHEMICALS_FACTORY:
      result += 3;
      break;
    case SPACE_STATION_FACTORY:
      result += 10;
      break;
    case PLANETARY_CRUISER_FACTORY:
      result += 15;
      break;
    case MOON_BASE_FACTORY:
      result += 20;
      break;
    }
  }
  return result;
}


int Factories::GetPeopleIncrease() const
{
  size_t i;
  int result = 0;

  for (i = 0 ; i < m_Factories.size() ; ++i)
  {
    const FactoryDatum &fd = m_Factories[i];
    if (fd.second != HUMAN) continue;

    switch(fd.first)
    {
    case ORE_FACTORY:
    case WATER_FACTORY:
    case TITANIUM_FACTORY:
    case RESEARCH_FACTORY:
    case NEW_CHEMICALS_FACTORY:
    case NO_FACTORY:
      break;
    case SPACE_STATION_FACTORY:
    case PLANETARY_CRUISER_FACTORY:
    case MOON_BASE_FACTORY:
      result += 1;
      break;
    }
  }
  return result;
}


    
int Factories::GetMannedFactoryCount(FactoryType i_FType) const
{
  size_t i;
  int result = 0;

  for (i = 0 ; i < m_Factories.size() ; ++i)
  {
    const FactoryDatum &fd = m_Factories[i];
    if (fd.second == UNMANNED) continue;
    if (fd.first != i_FType) continue;
    result++;
  }

  return result;
}

int Factories::GetUnmannedFactoryCount(FactoryType i_FType) const
{
  size_t i;
  int result = 0;

  for (i = 0 ; i < m_Factories.size() ; ++i)
  {
    const FactoryDatum &fd = m_Factories[i];
    if (fd.second != UNMANNED) continue;
    if (fd.first != i_FType) continue;
    result++;
  }

  return result;
}

int Factories::GetUsedMen() const
{
  size_t i;
  int result = 0;
  for (i = 0 ; i < m_Factories.size() ; ++i)
  {
    const FactoryDatum &fd = m_Factories[i];
    if (fd.second == HUMAN) ++result;
  }

  return result;
}

int Factories::GetUsedRobots() const
{
  size_t i;
  int result = 0;
  for (i = 0 ; i < m_Factories.size() ; ++i)
  {
    const FactoryDatum &fd = m_Factories[i];
    if (fd.second == ROBOT) ++result;
  }

  return result;
}

size_t Factories::size() const
{
  return m_Factories.size();
}
