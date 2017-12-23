#include "Serialize.hpp"
#include <sstream>


template<typename T_Class>
void SerialTransfer(const T_Class &i_in,T_Class &i_out)
{
  std::ostringstream oss;
  {
    SaveArchive i_ar(oss);
    SERIALIZE(i_in);
  }

  std::istringstream iss(oss.str());
  {
    LoadArchive i_ar(iss);
    SERIALIZE(i_out);
  }
}
