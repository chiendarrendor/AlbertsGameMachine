#include <string>

// this class contains game information that 
// is const.

class MapData;

class MerchantOfVenusSet
{
public:
  MerchantOfVenusSet(const std::string &i_DataDir);
  bool IsValid() const;
  // const accessors of data
  const MapData& GetMapData() const;

private:
  MapData* m_pmapdata;
  bool m_valid;
};
