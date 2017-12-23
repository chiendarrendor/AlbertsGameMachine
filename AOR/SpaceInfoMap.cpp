#include "SpaceInfoMap.hpp"
#include "SpaceInfo.hpp"

SpaceInfo *SpaceInfoMap::GetSpace(const std::string &i_Name)
{
  std::map<std::string,SpaceInfo *>::iterator spacit = this->find(i_Name);
  if (spacit == this->end())
  {
    return NULL;
  }

  return spacit->second;
}

const SpaceInfo *SpaceInfoMap::GetSpace(const std::string &i_Name) const
{
  std::map<std::string,SpaceInfo *>::const_iterator spacit = this->find(i_Name);
  if (spacit == this->end())
  {
    return NULL;
  }

  return spacit->second;
}

void SpaceInfoMap::Insert(SpaceInfo &i_rSpace)
{
  (*this)[i_rSpace.GetName()] = &i_rSpace;
}

