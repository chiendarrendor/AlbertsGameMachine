#ifndef ROOMMANAGERHPP
#define ROOMMANAGERHPP

#include <set>
#include <map>
#include <boost/json.hpp>
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
  bool GetGameIsDone();
  boost::json::array GetSavedGameNames();

  std::string GetName();
  const std::set<std::string> &GetInhabitantSet() const;

  static void Initialize(const GameCloset &i_rGameCloset);
private:

  void HandleNewGame(const std::string &i_Name,const ActionParser &i_ap);
  void HandleLoadGame(const std::string &i_Name,const ActionParser &i_ap);
  void HandleSaveGame(const std::string &i_Name,const ActionParser &i_ap);

  void BroadcastFullState();


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

  Room *GetRoomOf(const std::string &i_Name);

  // Computes the current global lobby snapshot -- every room's name/current
  // game/IsDone()/available game types, plus the full inhabitant list. See
  // .claude/server_game_interface_spec.md's ROOMGUI design.
  boost::json::object BuildRoomGuiState();

  // Recomputes the snapshot and broadcasts it (wrapped the same way as a
  // per-game full-state, via WrapFullStateEnvelope) to every connected
  // player, but only if it actually differs from the last one sent --
  // most actions touch nothing lobby-relevant. Called at the end of every
  // place that can change lobby-relevant state: initial connection, new
  // room, change room, disconnect, and after every in-room action (the only
  // room-action-triggered change that's ever lobby-relevant is IsDone()
  // flipping, which this re-checks for every room's game unconditionally --
  // cheap at this project's scale, see the spec doc's size estimate).
  void BroadcastRoomGuiIfChanged(GameServerConnectionHandlerFactory &i_Connections);

  std::map<std::string,Room *> m_Rooms;
  std::set<std::string> m_Inhabitants;
  const GameCloset &m_rGameCloset;
  boost::json::object m_LastRoomGuiState;

  std::string m_XMLLoc;
  std::string m_XMLName;

};





#endif
