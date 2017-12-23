#include <string>

// this class contains game information that 
// is const.

class OutpostSet
{
public:
  OutpostSet(const std::string &i_DataDir);
  bool IsValid() const;

  // const accessors of data

private:
};
