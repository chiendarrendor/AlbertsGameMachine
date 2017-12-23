#ifndef SPACEINFOMAPHPP
#define SPACEINFOMAPHPP

#include <string>
#include <map>

class SpaceInfo;

class SpaceInfoMap : public std::map<std::string,SpaceInfo *>
{
public:
  SpaceInfo *GetSpace(const std::string &i_Name);
  const SpaceInfo *GetSpace(const std::string &i_Name) const;

  void Insert(SpaceInfo &i_rSpace);
};

#endif
