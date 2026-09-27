#ifndef OUTPUTPORTHPP
#define OUTPUTPORTHPP

#include <string>

class OutputPort
{
public:
  virtual void UniCast(const std::string &i_Name,const std::string &i_Message) const = 0;
  virtual void BroadCast(const std::string &i_Message) const = 0;
};

#endif
