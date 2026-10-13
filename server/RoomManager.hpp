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
  // Returns "" on success, or an error description on rejection -- the
  // caller (RoomManager::HandleAction) owns actually wrapping/addressing/
  // sending that, since it's the one that has XMLLoc/XMLFile and talks to
  // GameServerConnectionHandlerFactory. See .claude/server_game_interface_spec.md's
  // ROOMGUI-envelope-unification design.
  std::string HandleAction(const std::string &i_Name,const ActionParser &i_ap);

  std::string GetGameName();
  bool GetGameIsDone();
  boost::json::array GetSavedGameNames();

  // Which of the "room"-namespace actions (NEWGAME/LOADGAME/SAVEGAME) are
  // currently legal in this room, mirroring the same preconditions
  // HandleNewGame/HandleLoadGame/HandleSaveGame already enforce. See the
  // spec doc section above -- this is deliberately room-scoped, not
  // per-individual, since there's no player-vs-observer distinction for
  // any room-level action. Takes the room's IsDone() as a parameter rather
  // than calling GetGameIsDone() itself, so a caller that's already paying
  // for that (potentially real-RPC) call once per room -- BuildRoomGuiState
  // -- never pays for it again per recipient.
  boost::json::array GetLegalRoomActions(bool i_GameIsDone);

  std::string GetName();
  const std::set<std::string> &GetInhabitantSet() const;

  static void Initialize(const GameCloset &i_rGameCloset);
private:

  std::string HandleNewGame(const std::string &i_Name,const ActionParser &i_ap);
  std::string HandleLoadGame(const std::string &i_Name,const ActionParser &i_ap);
  std::string HandleSaveGame(const std::string &i_Name,const ActionParser &i_ap);

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
  // "" on success, error text on rejection -- same convention as
  // Room::HandleAction above.
  std::string HandleNewRoom(const std::string &i_Name,const ActionParser &i_ap,
                     GameServerConnectionHandlerFactory &i_Connections);
  std::string HandleChangeRoom(const std::string &i_Name,const ActionParser &i_ap,
                     GameServerConnectionHandlerFactory &i_Connections);

  Room *GetRoomOf(const std::string &i_Name);

  // The roommanager-level actions (NEWROOM/CHANGEROOM, always legal) plus
  // whatever room i_Name is currently in reports in i_PerRoomActions (a
  // room-name-keyed lookup, built once per broadcast pass by
  // BuildRoomGuiState -- see below). Room-scoped, not per-individual: two
  // inhabitants of the same room always get the identical answer, and
  // looking it up here rather than recomputing it is what keeps a
  // whole-room broadcast from paying for GetLegalRoomActions' IsDone()
  // call once per recipient instead of once per room.
  boost::json::array LegalActionsFor(const std::string &i_Name,
                     const std::map<std::string,boost::json::array> &i_PerRoomActions);

  // Single-recipient convenience wrapper around LegalActionsFor, for the
  // one-off case (a rejected action's targeted error envelope) where
  // there's no existing batch of per-room answers to look up -- computing
  // this one room's answer fresh here is fine, since it's a single send,
  // not a whole-room fan-out.
  boost::json::array ComputeLegalActions(const std::string &i_Name);

  // Sends a single targeted {state,stateDesc,legalActions,error} ROOMGUI
  // envelope to one recipient -- the error-path counterpart to
  // BroadcastRoomGuiIfChanged's success-path envelope below. Used instead
  // of the old ROOMVALIDATE/SENDLINE macros, which sent a bare non-JSON
  // "ERROR,..." line incompatible with the all-JSON wire protocol.
  void SendRoomActionError(const std::string &i_Name,const std::string &i_Error,
                     GameServerConnectionHandlerFactory &i_Connections);

  // Computes the current global lobby snapshot -- every room's name/current
  // game/IsDone()/available game types, plus the full inhabitant list --
  // and, in the same pass (so each room's IsDone() is only ever asked once
  // per broadcast, not once per room-list-field and again per recipient),
  // every room's own legal "room"-namespace actions into o_PerRoomActions.
  // See .claude/server_game_interface_spec.md's ROOMGUI design.
  boost::json::object BuildRoomGuiState(std::map<std::string,boost::json::array> &o_PerRoomActions);

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
