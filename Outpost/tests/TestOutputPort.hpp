#include "OutputPort.hpp"
#include <string>
#include <sstream>


class TestOutputPort : public OutputPort
{
public:
  TestOutputPort();
  virtual ~TestOutputPort();
  virtual void UniCast(const std::string &i_Name,const std::string &i_Message) const;
  virtual void BroadCast(const std::string &i_Message) const;
  std::string GetOutput() const;
  void ResetOutput();

private:
  mutable std::ostringstream m_oss;
};
