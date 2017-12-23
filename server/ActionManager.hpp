#ifndef ACTIONMANAGERHPP
#define ACTIONMANAGERHPP

#include <string>

class GameServerConnectionHandlerFactory;
class ActionParser;

class ActionManager
{
public:
  virtual void HandleInitialConnection(const std::string &i_Name,
                          GameServerConnectionHandlerFactory &i_Connections) = 0;
  virtual void HandleAction(const std::string &i_Name,
               const ActionParser &i_ap,
               GameServerConnectionHandlerFactory &i_Connections) = 0;
  virtual void HandleDisconnect(const std::string &i_Name,
                   GameServerConnectionHandlerFactory &i_Connections) = 0;
};

#endif
