#include "RoomOutputPort.hpp"
#include "GameServerConnectionHandler.hpp"

RoomOutputPort::RoomOutputPort(GameServerConnectionHandlerFactory &i_Connections) :
  m_rConnections(i_Connections)
{
}

RoomOutputPort::~RoomOutputPort()
{
}


void RoomOutputPort::AddBroadcastTarget(const std::string &i_Name)
{
  m_BroadcastTargets.insert(i_Name);
}

void RoomOutputPort::RemoveBroadcastTarget(const std::string &i_Name)
{
  m_BroadcastTargets.erase(i_Name);
}

void RoomOutputPort::UniCast(const std::string &i_Name,const std::string &i_Message) const
{
  if (m_BroadcastTargets.find(i_Name) == m_BroadcastTargets.end()) return;
  m_rConnections.SendLineToName(i_Name,i_Message);
}

void RoomOutputPort::BroadCast(const std::string &i_Message) const
{
  std::set<std::string>::iterator nameit = m_BroadcastTargets.begin();

  for (; nameit != m_BroadcastTargets.end() ; ++nameit)
  {
    m_rConnections.SendLineToName(*nameit,i_Message);
  }
}

void RoomOutputPort::VariCast(const NameBoolean &i_Variator,const std::string &i_Message) const
{
  std::set<std::string>::iterator nameit = m_BroadcastTargets.begin();

  for (; nameit != m_BroadcastTargets.end() ; ++nameit)
  {
    if (i_Variator(*nameit))
    {
      m_rConnections.SendLineToName(*nameit,i_Message);
    }
  }
}
