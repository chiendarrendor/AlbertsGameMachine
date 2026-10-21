#include "RoomManager.hpp"
#include "GameServerConnectionHandler.hpp"
#include "StringUtilities.hpp"
#include "JsonParamExtraction.hpp"
#include "OutputPort.hpp"
#include <iostream>
#include <time.h>
#include "GameCloset.hpp"
#include "GameBox.hpp"
#include "Game.hpp"
#include "FileUtilities.hpp"
#include <boost/uuid/nil_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

const char *INITIALROOM = "Great Hall";

// ROOMGUI's synthetic, permanently-fixed FSM state -- see
// .claude/server_game_interface_spec.md's "ROOMGUI's full-state shape
// decided to match a real game's exactly" section. Mapping the room
// subsystem's real action set onto an FSM the way a game's is modeled
// shows it's genuinely a single-state cyclic FSM (every action loops back
// to the same state), so this never varies -- varying it would be making
// it do double-duty with legalActions, which is where per-request
// conditional availability actually belongs.
const char *ROOMGUI_STATE = "ROOM";
const char *ROOMGUI_STATEDESC = "Room Navigation";

// Same spirit: ROOMGUI isn't a "game instance" that can ever be reloaded
// from an unrelated save file (see StateWalker::GetInstanceId -- a real
// game's unique per-instance id, added to its envelope so the client can
// detect it's looking at a different instance than before), so there's
// nothing for this to actually distinguish. But keeping ROOMGUI's envelope
// shape identical to every real game's means GuiPacketParser never needs
// to treat gameInstanceId as GUI-type-conditional -- fixed nil UUID,
// always present, never varies.
const std::string ROOMGUI_INSTANCE_ID = boost::uuids::to_string(boost::uuids::nil_uuid());

const GameCloset *Room::s_pGameCloset(NULL);




void Room::Initialize(const GameCloset &i_rGameCloset)
{
  s_pGameCloset = &i_rGameCloset;
}

Room::Room(const std::string &i_RoomName,GameServerConnectionHandlerFactory &i_rCommPort) :
  m_RoomName(i_RoomName),
  m_Connections(i_rCommPort),
  m_pGame(NULL)
{
}

std::string Room::GetName()
{
  return m_RoomName;
}

const std::set<std::string> &Room::GetInhabitantSet() const
{
  return m_Inhabitants;
}


void Room::AddPlayerToRoom(const std::string &i_Name)
{
  m_Connections.AddBroadcastTarget(i_Name);
  m_Inhabitants.insert(i_Name);

  // The available-games list and saved-game list are now both part of
  // ROOMGUI (per-room gameTypes/savedGames, empty for a gameless room, and
  // gameTypes empty for Great Hall); NEWGUI is retired entirely -- a
  // per-game full-state already self-describes via its own layers 3+4, so a
  // client bootstraps it by inference, no separate signal needed. See
  // .claude/server_game_interface_spec.md.
  if (m_pGame != NULL)
  {
    m_pGame->SendFullState(i_Name);
  }
}

void Room::RemovePlayerFromRoom(const std::string &i_Name)
{
  m_Inhabitants.erase(i_Name);
  m_Connections.RemoveBroadcastTarget(i_Name);
}

bool Room::IsPlayerInRoom(const std::string &i_Name)
{
  std::set<std::string>::iterator findit = m_Inhabitants.find(i_Name);
  return findit != m_Inhabitants.end();
}

void Room::BroadcastFullState()
{
  if (!m_pGame)
  {
    return;
  }
  std::set<std::string>::iterator playerit;
  for (playerit = m_Inhabitants.begin() ; playerit != m_Inhabitants.end() ; ++playerit)
  {
    m_pGame->SendFullState(*playerit);
  }
}

boost::json::array Room::GetSavedGameNames()
{
  boost::json::array result;

  if (!m_pGame)
  {
    return result;
  }

  std::vector<std::string> dirents;

  if (!GetDirectoryContents(m_pGame->GetServerGameInfo().GetSaveDir(),dirents))
  {
    std::cerr << "Can't read Save Game Directory for Game " << m_pGame->GetServerGameInfo().GetName() << std::endl;
    return result;
  }

  std::vector<std::string>::iterator dirit;
  for (dirit = dirents.begin() ; dirit != dirents.end() ; dirit++)
  {
    result.push_back(boost::json::value(*dirit));
  }

  return result;
}


std::string Room::HandleAction(const std::string &i_Name,const ActionParser &i_ap)
{
  if (i_ap.GetNamespace() == "room")
  {
    if (i_ap.GetActionName() == "NEWGAME")
    {
      return HandleNewGame(i_Name,i_ap);
    }
    else if (i_ap.GetActionName() == "LOADGAME")
    {
      return HandleLoadGame(i_Name,i_ap);
    }
    else if (i_ap.GetActionName() == "SAVEGAME")
    {
      return HandleSaveGame(i_Name,i_ap);
    }
    else
    {
      return std::string("Unknown room action ") + i_ap.GetActionName();
    }
  }

  if (m_pGame == NULL) return "No active game in this room.";
  if (i_ap.GetNamespace() != m_pGame->GetServerGameInfo().GetName())
  {
    return "Action namespace '" + i_ap.GetNamespace() + "' doesn't match this room's active game.";
  }

  // The real game's own HandleAction already reports a rejection as a
  // JSON full-state envelope straight to the acting player via
  // UniCastJson (see DLLGame::HandleAction) -- nothing left for this
  // room-level wrapper to do on top of that.
  m_pGame->HandleAction(i_Name,i_ap,m_Inhabitants);
  return "";
}

std::string Room::HandleNewGame(const std::string &i_Name,const ActionParser &i_ap)
{
  if (i_ap.GetParams().size() != 1) return "Illegal NEWGAME action.";
  std::string gameName = ExtractJsonParam<std::string>(i_ap.GetParams(),"NewGame");

  const GameBox *pGameBox = s_pGameCloset->GetGameBoxByName(gameName);

  if (pGameBox == NULL) return "Unknown Game: " + gameName;
  if (!(m_pGame == NULL || m_pGame->IsDone())) return "Room already has active game!";

  if (m_pGame != NULL)
  {
    delete m_pGame;
    m_pGame = NULL;
  }

  m_pGame = pGameBox->CreateGame(m_Connections);

  if (!m_pGame) return "Unknown Error: could not create game!";

  BroadcastFullState();
  return "";
}

std::string Room::HandleLoadGame(const std::string &i_Name,const ActionParser &i_ap)
{
  if (i_ap.GetParams().size() != 1) return "Illegal LOADGAME action.";
  std::string fileName = ExtractJsonParam<std::string>(i_ap.GetParams(),"NameToLoad");
  if (!m_pGame) return "No Game to Load.";
  if (!m_pGame->Load(fileName)) return "Game " + fileName + " failed to load.";
  BroadcastFullState();
  return "";
}

std::string Room::HandleSaveGame(const std::string &i_Name,const ActionParser &i_ap)
{
  if (i_ap.GetParams().size() != 1) return "Illegal SAVEGAME action.";
  std::string fileName = ExtractJsonParam<std::string>(i_ap.GetParams(),"NameToSave");
  if (!m_pGame) return "No Game to Save.";
  if (!m_pGame->Save(fileName)) return "Game " + fileName + " failed to save.";
  return "";
}

boost::json::array Room::GetLegalRoomActions(bool i_GameIsDone)
{
  boost::json::array result;

  if (m_pGame == NULL || i_GameIsDone)
  {
    result.push_back(boost::json::value("NEWGAME"));
  }
  if (m_pGame != NULL)
  {
    result.push_back(boost::json::value("LOADGAME"));
    result.push_back(boost::json::value("SAVEGAME"));
  }

  return result;
}

std::string Room::GetGameName()
{
  if (!m_pGame) return "";
  return m_pGame->GetName();
}

bool Room::GetGameIsDone()
{
  if (!m_pGame) return true;
  return m_pGame->IsDone();
}



RoomManager::RoomManager(const std::string &i_XMLLoc,const std::string &i_XMLName,const GameCloset &i_rGameCloset) :
  m_rGameCloset(i_rGameCloset),
  m_XMLLoc(i_XMLLoc),
  m_XMLName(i_XMLName)
{
  Room::Initialize(i_rGameCloset);
}

RoomManager::~RoomManager()
{
  std::map<std::string,Room *>::iterator doomed;

  for (doomed = m_Rooms.begin() ; doomed != m_Rooms.end() ; ++doomed)
  {
    delete doomed->second;
  }
}

boost::json::object RoomManager::BuildRoomGuiState(std::map<std::string,boost::json::array> &o_PerRoomActions)
{
  boost::json::array gameTypes;
  std::set<std::string>::const_iterator gameit;
  for (gameit = m_rGameCloset.GetGameBoxNames().begin() ;
       gameit != m_rGameCloset.GetGameBoxNames().end() ; ++gameit)
  {
    gameTypes.push_back(boost::json::value(*gameit));
  }

  boost::json::array rooms;
  std::map<std::string,Room *>::iterator roomit;
  for (roomit = m_Rooms.begin() ; roomit != m_Rooms.end() ; ++roomit)
  {
    Room *pRoom = roomit->second;
    bool isDone = pRoom->GetGameIsDone();

    boost::json::object roomobj;
    roomobj["name"] = roomit->first;
    roomobj["gameName"] = pRoom->GetGameName();
    roomobj["isDone"] = isDone;
    // Great Hall never allows a game to start in it -- the client's own
    // available-games list is empty/greyed out there, driven purely by this
    // being empty (there's no separate server-side NEWGAME block; it's
    // enforced by never offering a choice).
    roomobj["gameTypes"] = (roomit->first == INITIALROOM) ? boost::json::array() : gameTypes;
    roomobj["savedGames"] = pRoom->GetSavedGameNames();

    // Nested per-room, not a top-level flat {name,room} list cross-referenced
    // by the client -- confirmed (2026-10-19) that the room-navigation client
    // widget is the only consumer of "who's in which room" anywhere in the
    // client, so there's no reason to carry two representations of the same
    // fact. This also matches Room's own data model directly (GetInhabitantSet()),
    // rather than rebuilding the same information by cross-referencing the
    // server-wide m_Inhabitants set against GetRoomOf() per name.
    boost::json::array roomInhabitants;
    const std::set<std::string> &roomNames = pRoom->GetInhabitantSet();
    std::set<std::string>::const_iterator inhabit;
    for (inhabit = roomNames.begin() ; inhabit != roomNames.end() ; ++inhabit)
    {
      roomInhabitants.push_back(boost::json::value(*inhabit));
    }
    roomobj["inhabitants"] = roomInhabitants;

    rooms.push_back(roomobj);

    o_PerRoomActions[roomit->first] = pRoom->GetLegalRoomActions(isDone);
  }

  boost::json::object state;
  state["rooms"] = rooms;
  return state;
}

boost::json::array RoomManager::LegalActionsFor(const std::string &i_Name,
                     const std::map<std::string,boost::json::array> &i_PerRoomActions)
{
  // NEWROOM/CHANGEROOM are dispatched before any room lookup (see
  // HandleAction below), so they have no room-dependent precondition at
  // all -- always legal for anyone connected.
  boost::json::array result;
  result.push_back(boost::json::value("NEWROOM"));
  result.push_back(boost::json::value("CHANGEROOM"));

  Room *pRoom = GetRoomOf(i_Name);
  if (pRoom)
  {
    std::map<std::string,boost::json::array>::const_iterator findit =
      i_PerRoomActions.find(pRoom->GetName());
    if (findit != i_PerRoomActions.end())
    {
      const boost::json::array &roomActions = findit->second;
      for (std::size_t i = 0 ; i < roomActions.size() ; ++i)
      {
        result.push_back(roomActions[i]);
      }
    }
  }

  return result;
}

boost::json::array RoomManager::ComputeLegalActions(const std::string &i_Name)
{
  std::map<std::string,boost::json::array> perRoomActions;
  Room *pRoom = GetRoomOf(i_Name);
  if (pRoom)
  {
    perRoomActions[pRoom->GetName()] = pRoom->GetLegalRoomActions(pRoom->GetGameIsDone());
  }
  return LegalActionsFor(i_Name,perRoomActions);
}

void RoomManager::SendRoomActionError(const std::string &i_Name,const std::string &i_Error,
                                      GameServerConnectionHandlerFactory &i_Connections)
{
  boost::json::object envelope;
  envelope["state"] = ROOMGUI_STATE;
  envelope["stateDesc"] = ROOMGUI_STATEDESC;
  envelope["legalActions"] = ComputeLegalActions(i_Name);
  envelope["gameInstanceId"] = ROOMGUI_INSTANCE_ID;
  envelope["error"] = i_Error;

  i_Connections.SendLineToName(i_Name,
      boost::json::serialize(WrapFullStateEnvelope(i_Name,"ROOMGUI",m_XMLLoc,m_XMLName,envelope)));
}

void RoomManager::BroadcastRoomGuiIfChanged(GameServerConnectionHandlerFactory &i_Connections)
{
  std::map<std::string,boost::json::array> perRoomActions;
  boost::json::object current = BuildRoomGuiState(perRoomActions);
  if (current == m_LastRoomGuiState)
  {
    return;
  }
  m_LastRoomGuiState = current;

  // "myroom" is a per-viewer convenience field, same spirit as Outpost's
  // myHand/myPurchaseInfo (see .claude/TODO.md's Phase 3 item) -- an exact
  // duplicate of whichever "rooms" entry the recipient currently occupies,
  // so the client never has to search the rooms array by name itself.
  // Keyed here (not folded into BuildRoomGuiState/the cached+diffed
  // "current") because it varies per recipient, unlike the rest of the body.
  std::map<std::string,boost::json::value> roomsByName;
  const boost::json::array &rooms = current.at("rooms").as_array();
  for (std::size_t i = 0 ; i < rooms.size() ; ++i)
  {
    const boost::json::object &roomobj = rooms[i].as_object();
    roomsByName[std::string(roomobj.at("name").as_string().c_str())] = rooms[i];
  }

  std::set<std::string>::const_iterator nameit;
  for (nameit = m_Inhabitants.begin() ; nameit != m_Inhabitants.end() ; ++nameit)
  {
    Room *pRoom = GetRoomOf(*nameit);
    std::map<std::string,boost::json::value>::const_iterator foundit =
      pRoom ? roomsByName.find(pRoom->GetName()) : roomsByName.end();

    boost::json::object body = current;
    body["myroom"] = (foundit != roomsByName.end()) ? foundit->second : boost::json::value();

    boost::json::object envelope;
    envelope["state"] = ROOMGUI_STATE;
    envelope["stateDesc"] = ROOMGUI_STATEDESC;
    envelope["legalActions"] = LegalActionsFor(*nameit,perRoomActions);
    envelope["gameInstanceId"] = ROOMGUI_INSTANCE_ID;
    envelope["body"] = body;

    i_Connections.SendLineToName(*nameit,
        boost::json::serialize(WrapFullStateEnvelope(*nameit,"ROOMGUI",m_XMLLoc,m_XMLName,envelope)));
  }
}

void RoomManager::HandleInitialConnection(const std::string &i_Name,
                          GameServerConnectionHandlerFactory &i_Connections)
{
  m_Inhabitants.insert(i_Name);

  if (m_Rooms.size() == 0)
  {
    m_Rooms[INITIALROOM] = new Room(INITIALROOM,i_Connections);
  }
  m_Rooms[INITIALROOM]->AddPlayerToRoom(i_Name);

  BroadcastRoomGuiIfChanged(i_Connections);
}

void RoomManager::HandleAction(const std::string &i_Name,const ActionParser &i_ap,
			       GameServerConnectionHandlerFactory &i_Connections)
{
  if (i_ap.GetNamespace() == "roommanager")
  {
    std::string error;
    if (i_ap.GetActionName() == "NEWROOM") error = HandleNewRoom(i_Name,i_ap,i_Connections);
    else if (i_ap.GetActionName() == "CHANGEROOM") error = HandleChangeRoom(i_Name,i_ap,i_Connections);
    else error = std::string("Unknown roommanager action ") + i_ap.GetActionName();

    if (!error.empty())
    {
      SendRoomActionError(i_Name,error,i_Connections);
    }
    return;
  }

  Room *pRoom = GetRoomOf(i_Name);
  if (!pRoom) return;
  std::string error = pRoom->HandleAction(i_Name,i_ap);
  if (!error.empty())
  {
    // A rejected room-level action is a targeted error straight to the
    // actor -- nothing lobby-relevant changed, so no broadcast follows,
    // same "rejected actions produce zero full-state churn" rule a real
    // game's HandleAction already follows.
    SendRoomActionError(i_Name,error,i_Connections);
    return;
  }

  BroadcastRoomGuiIfChanged(i_Connections);
}

std::string RoomManager::HandleNewRoom(const std::string &i_Name,const ActionParser &i_ap,
                     GameServerConnectionHandlerFactory &i_Connections)
{
  if (i_ap.GetParams().size() != 1)
  {
    return "Bad NEWROOM action";
  }
  std::string newName = ExtractJsonParam<std::string>(i_ap.GetParams(),"RoomName");

  if (newName.size() == 0)
  {
    return "Empty NEWROOM name";
  }

  std::map<std::string,Room *>::iterator findit = m_Rooms.find(newName);
  if (findit != m_Rooms.end())
  {
    return "NEWROOM name already exists";
  }

  m_Rooms[newName] = new Room(newName,i_Connections);

  BroadcastRoomGuiIfChanged(i_Connections);
  return "";
}

std::string RoomManager::HandleChangeRoom(const std::string &i_Name,const ActionParser &i_ap,
                     GameServerConnectionHandlerFactory &i_Connections)
{
  if (i_ap.GetParams().size() != 1)
  {
    return "Bad CHANGEROOM action";
  }

  std::string newRoomName = ExtractJsonParam<std::string>(i_ap.GetParams(),"TargetRoom");
  std::map<std::string,Room *>::iterator findit = m_Rooms.find(newRoomName);
  if (findit == m_Rooms.end())
  {
    return "Don't know which room you mean..try again...";
  }

  Room *pOldRoom = GetRoomOf(i_Name);
  if (pOldRoom)
  {
    pOldRoom->RemovePlayerFromRoom(i_Name);
  }
  Room *pNewRoom = findit->second;
  pNewRoom->AddPlayerToRoom(i_Name);

  BroadcastRoomGuiIfChanged(i_Connections);
  return "";
}


void RoomManager::HandleDisconnect(const std::string &i_Name,GameServerConnectionHandlerFactory &i_Connections)
{
  m_Inhabitants.erase(i_Name);
  Room *pRoom = GetRoomOf(i_Name);
  if (!pRoom) return;
  pRoom->RemovePlayerFromRoom(i_Name);

  BroadcastRoomGuiIfChanged(i_Connections);
}

Room *RoomManager::GetRoomOf(const std::string &i_Name)
{
  std::map<std::string,Room *>::iterator mapit;

  for (mapit = m_Rooms.begin(); mapit != m_Rooms.end() ; ++mapit)
  {
    if (mapit->second->IsPlayerInRoom(i_Name))
    {
      return mapit->second;
    }
  }
  return NULL;
}

