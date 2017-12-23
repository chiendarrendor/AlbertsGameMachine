#ifndef OUTPUTPORTHPP
#define OUTPUTPORTHPP

#include <string>

class NameBoolean
{
public:
  virtual bool operator()(const std::string &i_PlayerName) const = 0;
};




class OutputPort
{
public:
  virtual void UniCast(const std::string &i_Name,const std::string &i_Message) const = 0;
  virtual void BroadCast(const std::string &i_Message) const = 0;
  virtual void VariCast(const NameBoolean &i_Variator,const std::string &i_Message) const = 0;
};

#endif
