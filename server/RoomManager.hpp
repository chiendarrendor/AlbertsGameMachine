#ifndef ROOMMANAGERHPP
#define ROOMMANAGERHPP

#include <set>
#include <map>
#include "ActionManager.hpp"
#include "RoomOutputPort.hpp"

class GameCloset;
class Game;

class Room
{
public:
  Room(const std::string &i_RoomName,GameServerConnectionHandlerFactory &i_rConnections);
  void AddPlayerToRoom(const std::string &i_Name);
  void RemovePlayerFromRoom(const std::string &i_Name);
  bool IsPlayerInRoom(const std::string &i_Name);
  void HandleAction(const std::string &i_Name,const ActionParser &i_ap);
  
  std::string GetGameName();
  std::string GetGameStatus();

  std::string GetName();
  const std::set<std::string> &GetInhabitantSet() const;

  static void Initialize(const GameCloset &i_rGameCloset);
private:

  void HandleNewGame(const std::string &i_Name,const ActionParser &i_ap);
  void HandleLoadGame(const std::string &i_Name,const ActionParser &i_ap);
  void HandleSaveGame(const std::string &i_Name,const ActionParser &i_ap);

  void BroadcastFullState();

  std::string GetSavedGameString();


  static const GameCloset *s_pGameCloset;
  std::string m_RoomName;
  RoomOutputPort m_Connections;
  std::set<std::string> m_Inhabitants;
  Game *m_pGame;
			    
};

class RoomManager : public ActionManager
{
public:
RoomManager(const std::string &i_XMLLoc,const std::string &i_XMLName,const GameCloset &i_GameCloset);
  virtual ~RoomManager();
  virtual void HandleInitialConnection(const std::string &i_Name,
                          GameServerConnectionHandlerFactory &i_Connections);
  virtual void HandleAction(const std::string &i_Name,
               const ActionParser &i_ap,
               GameServerConnectionHandlerFactory &i_Connections);
  virtual void HandleDisconnect(const std::string &i_Name,
                   GameServerConnectionHandlerFactory &i_Connections);

private:
  void HandleNewRoom(const std::string &i_Name,const ActionParser &i_ap,
                     GameServerConnectionHandlerFactory &i_Connections);
  void HandleChangeRoom(const std::string &i_Name,const ActionParser &i_ap,
                     GameServerConnectionHandlerFactory &i_Connections);
  void HandleRoomTalk(const std::string &i_Name,const ActionParser &i_ap,
                     GameServerConnectionHandlerFactory &i_Connections);
  void HandlePlayerTalk(const std::string &i_Name,const ActionParser &i_ap,
                     GameServerConnectionHandlerFactory &i_Connections);

  Room *GetRoomOf(const std::string &i_Name);

  std::map<std::string,Room *> m_Rooms;
  std::set<std::string> m_Inhabitants;
  
  std::string m_XMLLoc;
  std::string m_XMLName;

};





#endif
