#include "RoomManager.hpp"
#include "GameServerConnectionHandler.hpp"
#include "StringUtilities.hpp"
#include "JsonParamExtraction.hpp"
#include "OutputPort.hpp"
#include <sstream>
#include <iostream>
#include <time.h>
#include "GameCloset.hpp"
#include "GameBox.hpp"
#include "Game.hpp"
#include "FileUtilities.hpp"

const char *INITIALROOM = "Great Hall";

const GameCloset *Room::s_pGameCloset(NULL);

#define ROOMVALIDATE(_vtest_,_verror_) \
if (!(_vtest_))                         \
{                                      \
  m_Connections.UniCast(i_Name,std::string("ERROR,") + _verror_); \
  return; \
}

#define SENDLINE(M_TARGET,M_STREAM) \
{ \
  std::ostringstream sloss; \
  sloss << M_STREAM; \
  i_Connections.SendLineToName(M_TARGET,sloss.str() ); \
}




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


void Room::HandleAction(const std::string &i_Name,const ActionParser &i_ap)
{
  if (i_ap.GetNamespace() == "room")
  {
    if (i_ap.GetActionName() == "NEWGAME")
    {
      HandleNewGame(i_Name,i_ap);
    }
    else if (i_ap.GetActionName() == "LOADGAME")
    {
      HandleLoadGame(i_Name,i_ap);
    }
    else if (i_ap.GetActionName() == "SAVEGAME")
    {
      HandleSaveGame(i_Name,i_ap);
    }
    else
    {
      m_Connections.UniCast(i_Name,std::string("ERROR,Unknown room action ") + i_ap.GetActionName());
    }
    return;
  }

  ROOMVALIDATE(m_pGame != NULL,"No active game in this room.");
  ROOMVALIDATE(i_ap.GetNamespace() == m_pGame->GetServerGameInfo().GetName(),
               "Action namespace '" + i_ap.GetNamespace() + "' doesn't match this room's active game.");

  m_pGame->HandleAction(i_Name,i_ap,m_Inhabitants);
}

void Room::HandleNewGame(const std::string &i_Name,const ActionParser &i_ap)
{
  ROOMVALIDATE(i_ap.GetParams().size() == 1,"Illegal NEWGAME action.");
  std::string gameName = ExtractJsonParam<std::string>(i_ap.GetParams(),"NewGame");

  const GameBox *pGameBox = s_pGameCloset->GetGameBoxByName(gameName);

  ROOMVALIDATE(pGameBox != NULL,"Unknown Game: " + gameName);
  ROOMVALIDATE(m_pGame == NULL || m_pGame->IsDone(),"Room already has active game!");

  if (m_pGame != NULL)
  {
    delete m_pGame;
    m_pGame = NULL;
  }

  m_pGame = pGameBox->CreateGame(m_Connections);

  ROOMVALIDATE(m_pGame,"Unknown Error: could not create game!");

  BroadcastFullState();
}

void Room::HandleLoadGame(const std::string &i_Name,const ActionParser &i_ap)
{
  ROOMVALIDATE(i_ap.GetParams().size() == 1,"Illegal LOADGAME action.");
  std::string fileName = ExtractJsonParam<std::string>(i_ap.GetParams(),"NameToLoad");
  ROOMVALIDATE(m_pGame,"No Game to Load.");
  ROOMVALIDATE(m_pGame->Load(fileName),"Game " + fileName + " failed to load.");
  BroadcastFullState();
}

void Room::HandleSaveGame(const std::string &i_Name,const ActionParser &i_ap)
{
  ROOMVALIDATE(i_ap.GetParams().size() == 1,"Illegal SAVEGAME action.");
  std::string fileName = ExtractJsonParam<std::string>(i_ap.GetParams(),"NameToSave");
  ROOMVALIDATE(m_pGame,"No Game to Save.");
  ROOMVALIDATE(m_pGame->Save(fileName),"Game " + fileName + " failed to save.");
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

boost::json::object RoomManager::BuildRoomGuiState()
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
    boost::json::object roomobj;
    roomobj["name"] = roomit->first;
    roomobj["gameName"] = pRoom->GetGameName();
    roomobj["isDone"] = pRoom->GetGameIsDone();
    // Great Hall never allows a game to start in it -- the client's own
    // available-games list is empty/greyed out there, driven purely by this
    // being empty (there's no separate server-side NEWGAME block; it's
    // enforced by never offering a choice).
    roomobj["gameTypes"] = (roomit->first == INITIALROOM) ? boost::json::array() : gameTypes;
    roomobj["savedGames"] = pRoom->GetSavedGameNames();
    rooms.push_back(roomobj);
  }

  boost::json::array inhabitants;
  std::set<std::string>::const_iterator nameit;
  for (nameit = m_Inhabitants.begin() ; nameit != m_Inhabitants.end() ; ++nameit)
  {
    Room *pRoom = GetRoomOf(*nameit);
    boost::json::object inhab;
    inhab["name"] = *nameit;
    inhab["room"] = pRoom ? pRoom->GetName() : "";
    inhabitants.push_back(inhab);
  }

  boost::json::object state;
  state["rooms"] = rooms;
  state["inhabitants"] = inhabitants;
  return state;
}

void RoomManager::BroadcastRoomGuiIfChanged(GameServerConnectionHandlerFactory &i_Connections)
{
  boost::json::object current = BuildRoomGuiState();
  if (current == m_LastRoomGuiState)
  {
    return;
  }
  m_LastRoomGuiState = current;

  std::set<std::string>::const_iterator nameit;
  for (nameit = m_Inhabitants.begin() ; nameit != m_Inhabitants.end() ; ++nameit)
  {
    i_Connections.SendLineToName(*nameit,
        boost::json::serialize(WrapFullStateEnvelope(*nameit,"ROOMGUI",m_XMLLoc,m_XMLName,current)));
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
    if (i_ap.GetActionName() == "NEWROOM") HandleNewRoom(i_Name,i_ap,i_Connections);
    else if (i_ap.GetActionName() == "CHANGEROOM") HandleChangeRoom(i_Name,i_ap,i_Connections);
    else
    {
      i_Connections.SendLineToName(i_Name,"ERROR,Unknown roommanager action " + i_ap.GetActionName());
    }
    return;
  }

  Room *pRoom = GetRoomOf(i_Name);
  if (!pRoom) return;
  pRoom->HandleAction(i_Name,i_ap);

  BroadcastRoomGuiIfChanged(i_Connections);
}

void RoomManager::HandleNewRoom(const std::string &i_Name,const ActionParser &i_ap,
                     GameServerConnectionHandlerFactory &i_Connections)
{
  if (i_ap.GetParams().size() != 1)
  {
    SENDLINE(i_Name,"ERROR,Bad NEWROOM action");
    return;
  }
  std::string newName = ExtractJsonParam<std::string>(i_ap.GetParams(),"RoomName");

  if (newName.size() == 0)
  {
    SENDLINE(i_Name,"ERROR,Empty NEWROOM name");
    return;
  }

  std::map<std::string,Room *>::iterator findit = m_Rooms.find(newName);
  if (findit != m_Rooms.end())
  {
    SENDLINE(i_Name,"ERROR,NEWROOM name already exists");
    return;
  }

  m_Rooms[newName] = new Room(newName,i_Connections);

  BroadcastRoomGuiIfChanged(i_Connections);
}

void RoomManager::HandleChangeRoom(const std::string &i_Name,const ActionParser &i_ap,
                     GameServerConnectionHandlerFactory &i_Connections)
{
  if (i_ap.GetParams().size() != 1)
  {
    SENDLINE(i_Name,"ERROR,Bad CHANGEROOM action");
    return;
  }

  std::string newRoomName = ExtractJsonParam<std::string>(i_ap.GetParams(),"TargetRoom");
  std::map<std::string,Room *>::iterator findit = m_Rooms.find(newRoomName);
  if (findit == m_Rooms.end())
  {
    SENDLINE(i_Name,"ERROR,Don't know which room you mean..try again...");
    return;
  }

  Room *pOldRoom = GetRoomOf(i_Name);
  if (pOldRoom)
  {
    pOldRoom->RemovePlayerFromRoom(i_Name);
  }
  Room *pNewRoom = findit->second;
  pNewRoom->AddPlayerToRoom(i_Name);

  BroadcastRoomGuiIfChanged(i_Connections);
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

