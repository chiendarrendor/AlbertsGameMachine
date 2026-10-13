#include "OutputPort.hpp"
#include <set>

class GameServerConnectionHandlerFactory;

class RoomOutputPort : public OutputPort
{
public:
  RoomOutputPort(GameServerConnectionHandlerFactory &i_Connections);
  virtual ~RoomOutputPort();

  void AddBroadcastTarget(const std::string &i_Name);
  void RemoveBroadcastTarget(const std::string &i_Name);

  virtual void UniCast(const std::string &i_Name,const std::string &i_Message) const;
  virtual void BroadCastFullState(const std::string &i_GuiName,
                                  const std::string &i_XMLLoc,const std::string &i_XMLFile,
                                  const boost::json::object &i_Envelope) const;
private:
  std::set<std::string> m_BroadcastTargets;
  GameServerConnectionHandlerFactory &m_rConnections;
};

    
  
