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

void TestOutputPort::BroadCast(const std::string &i_Message) const
{
  m_oss << "broadcast: " << i_Message << std::endl;
}    

std::string TestOutputPort::GetOutput() const
{
  return m_oss.str();
}

void TestOutputPort::ResetOutput()
{
  m_oss.str("");
}

