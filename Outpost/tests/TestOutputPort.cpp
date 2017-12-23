#include "TestOutputPort.hpp"

TestOutputPort::TestOutputPort() :
  OutputPort(),
  m_varicasttests()
{
}

TestOutputPort::TestOutputPort(const std::vector<std::string>& i_varis) :
  OutputPort(),
  m_varicasttests(i_varis)
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

void TestOutputPort::VariCast(const NameBoolean &i_Variator,const std::string &i_Message) const
{
  m_oss << "varicast: " << "(";
  size_t i;
  for (i = 0 ; i < m_varicasttests.size() ; ++i)
  {
    if (i_Variator(m_varicasttests[i]))
    {
      m_oss << "1";
    } 
    else
    {
      m_oss << "0";
    }
  }
  m_oss << ") -- " << i_Message << std::endl;
}

std::string TestOutputPort::GetOutput() const
{
  return m_oss.str();
}

void TestOutputPort::ResetOutput()
{
  m_oss.str("");
}

