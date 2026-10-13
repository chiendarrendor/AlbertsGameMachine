#include "OutputPort.hpp"
#include <string>
#include <sstream>


class TestOutputPort : public OutputPort
{
public:
  TestOutputPort();
  virtual ~TestOutputPort();
  virtual void UniCast(const std::string &i_Name,const std::string &i_Message) const;
  virtual void BroadCastFullState(const std::string &i_GuiName,
                                  const std::string &i_XMLLoc,const std::string &i_XMLFile,
                                  const boost::json::object &i_Envelope) const;
  std::string GetOutput() const;
  void ResetOutput();

private:
  mutable std::ostringstream m_oss;
};
