#include "OutputPort.hpp"
#include <vector>
#include <string>
#include <sstream>


class TestOutputPort : public OutputPort
{
public:
  TestOutputPort();
  TestOutputPort(const std::vector<std::string> &i_varis);
  virtual ~TestOutputPort();
  virtual void UniCast(const std::string &i_Name,const std::string &i_Message) const;
  virtual void BroadCast(const std::string &i_Message) const;
  virtual void VariCast(const NameBoolean &i_Variator,const std::string &i_Message) const;
  std::string GetOutput() const;
  void ResetOutput();

private:
  mutable std::ostringstream m_oss;
  std::vector<std::string> m_varicasttests;
};
