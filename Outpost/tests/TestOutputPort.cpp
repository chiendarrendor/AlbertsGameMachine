#include "TestOutputPort.hpp"

TestOutputPort::TestOutputPort() :
  OutputPort()
{
}

TestOutputPort::~TestOutputPort()
{
}

void TestOutputPort::UniCast(const std::string &i_Name,const std::string &i_Message) const
{
  m_oss << "unicast: " << i_Name << " -- " << i_Message << std::endl;
}

void TestOutputPort::BroadCastFullState(const std::string &i_GuiName,
                                        const std::string &i_XMLLoc,const std::string &i_XMLFile,
                                        const boost::json::object &i_Envelope) const
{
  m_oss << "broadcastfullstate: " << i_GuiName << " -- " << boost::json::serialize(i_Envelope) << std::endl;
}

std::string TestOutputPort::GetOutput() const
{
  return m_oss.str();
}

void TestOutputPort::ResetOutput()
{
  m_oss.str("");
}

