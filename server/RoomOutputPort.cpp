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

void RoomOutputPort::BroadCastFullState(const std::string &i_GuiName,
                                        const std::string &i_XMLLoc,const std::string &i_XMLFile,
                                        const boost::json::object &i_Envelope) const
{
  std::set<std::string>::iterator nameit = m_BroadcastTargets.begin();

  for (; nameit != m_BroadcastTargets.end() ; ++nameit)
  {
    UniCastFullState(*nameit,i_GuiName,i_XMLLoc,i_XMLFile,i_Envelope);
  }
}

