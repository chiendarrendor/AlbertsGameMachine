#include "RoomManager.hpp"
#include "GameServerConnectionHandler.hpp"
#include "StringUtilities.hpp"
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

  if (m_RoomName == INITIALROOM)
  {
    m_Connections.UniCast(i_Name,std::string("GAMES, "));
    m_Connections.UniCast(i_Name,"SAVEGAMES,");
    return;
  }

  std::string gamelist;

  std::set<std::string>::const_iterator nameit;
  bool firsttime = true;

  for (nameit = s_pGameCloset->GetGameBoxNames().begin() ; 
       nameit != s_pGameCloset->GetGameBoxNames().end() ; ++nameit)
  {
    if (firsttime)
    {
      firsttime = false;
    }
    else
    {
      gamelist += ",";
    }
    gamelist += UnComma(*nameit);
  }
  gamelist = UnComma(gamelist);
  m_Connections.UniCast(i_Name,std::string("GAMES,") + gamelist);

  if (m_pGame != NULL)
  {
    std::string newguievent = 
      std::string("NEWGUI,") +
      m_pGame->GetServerGameInfo().GetName() + std::string(",") +
      m_pGame->GetServerGameInfo().GetXMLLoc() + std::string(",") +
      m_pGame->GetServerGameInfo().GetXMLFile();
    m_Connections.UniCast(i_Name,newguievent);
    m_pGame->SendFullState(i_Name);
    m_Connections.UniCast(i_Name,GetSavedGameString());
  }
  else
  {
    m_Connections.UniCast(i_Name,"SAVEGAMES,");
  }
}

void Room::RemovePlayerFromRoom(const std::string &i_Name)
{

  if (m_pGame != NULL)
  {
    m_Connections.UniCast(i_Name,std::string("DROPGUI,") + m_pGame->GetServerGameInfo().GetName());
  }
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

std::string Room::GetSavedGameString()
{
  if (!m_pGame)
  {
    return "ERROR,Request for Saved Games with no game!";
  }

  std::vector<std::string> dirents;

  if (!GetDirectoryContents(m_pGame->GetServerGameInfo().GetSaveDir(),dirents))
  {
    return std::string("ERROR,Can't read Save Game Directory for Game ") + m_pGame->GetServerGameInfo().GetName();
  }

  bool first = true;

  std::vector<std::string>::iterator dirit;
  std::string args;
  for (dirit = dirents.begin() ; dirit != dirents.end() ; dirit++)
  {
    if (!first)
    {
      args += ",";
    }
    first = false;

    args += *dirit;
  }

  return std::string("SAVEGAMES,") + UnComma(args);
}


void Room::HandleAction(const std::string &i_Name,const ActionParser &i_ap)
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
    if (m_pGame)
    {
      m_pGame->HandleAction(i_Name,i_ap);
    }
  }
}

void Room::HandleNewGame(const std::string &i_Name,const ActionParser &i_ap)
{
  ROOMVALIDATE(i_ap.GetNumArguments() == 1,"Illegal NEWGAME action.");
  const GameBox *pGameBox = s_pGameCloset->GetGameBoxByName(i_ap[0]);

  ROOMVALIDATE(pGameBox != NULL,"Unknown Game: " + i_ap[0]);
  ROOMVALIDATE(m_pGame == NULL || m_pGame->IsDone(),"Room already has active game!");

  if (m_pGame != NULL)
  {
    m_Connections.BroadCast(std::string("DROPGUI,") + m_pGame->GetServerGameInfo().GetName());
    delete m_pGame;
    m_pGame = NULL;
  }

  srand(time(NULL));

  m_pGame = pGameBox->CreateGame(m_Connections);

  ROOMVALIDATE(m_pGame,"Unknown Error: could not create game!");

  std::string newguievent = 
    std::string("NEWGUI,") +
    pGameBox->GetName() + std::string(",") +
    pGameBox->GetXMLLoc() + std::string(",") +
    pGameBox->GetXMLFile();

  m_Connections.BroadCast(newguievent);
  BroadcastFullState();
  m_Connections.BroadCast(GetSavedGameString());
}

void Room::HandleLoadGame(const std::string &i_Name,const ActionParser &i_ap)
{
  ROOMVALIDATE(i_ap.GetNumArguments() == 1,"Illegal LOADGAME action.");
  ROOMVALIDATE(m_pGame,"No Game to Load.");
  ROOMVALIDATE(m_pGame->Load(i_ap[0]),"Game " + i_ap[0] + " failed to load.");
  BroadcastFullState();
}

void Room::HandleSaveGame(const std::string &i_Name,const ActionParser &i_ap)
{
  ROOMVALIDATE(i_ap.GetNumArguments() == 1,"Illegal SAVEGAME action.");
  ROOMVALIDATE(m_pGame,"No Game to Save.");
  ROOMVALIDATE(m_pGame->Save(i_ap[0]),"Game " + i_ap[0] + " failed to save.");
  m_Connections.BroadCast(GetSavedGameString());
}

std::string Room::GetGameName()
{
  if (!m_pGame) return "";
  return m_pGame->GetName();
}

std::string Room::GetGameStatus()
{
  if (!m_pGame) return "";
  return m_pGame->GetStatusString();
}



RoomManager::RoomManager(const std::string &i_XMLLoc,const std::string &i_XMLName,const GameCloset &i_rGameCloset) :
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

void RoomManager::HandleInitialConnection(const std::string &i_Name,
                          GameServerConnectionHandlerFactory &i_Connections)
{
  std::string initialPacket;
  initialPacket += "NEWGUI,ROOMGUI,";
  initialPacket += m_XMLLoc;
  initialPacket += ",";
  initialPacket += m_XMLName;

  i_Connections.SendLineToName(i_Name,initialPacket);
  m_Inhabitants.insert(i_Name);

  if (m_Rooms.size() == 0)
  {
    m_Rooms[INITIALROOM] = new Room(INITIALROOM,i_Connections);
  }
  m_Rooms[INITIALROOM]->AddPlayerToRoom(i_Name);

  // tell new player what their name is
  SENDLINE(i_Name,"GUIIAM," << UnComma(i_Name));

  // tell new player about rooms
  std::map<std::string,Room *>::iterator roomit;
  for (roomit = m_Rooms.begin() ; roomit != m_Rooms.end(); ++roomit)
  {
    Room *pRoom = roomit->second;
    SENDLINE(i_Name, "GUIROOM," << UnComma(roomit->first) 
             << "," << UnComma(pRoom->GetGameName())
             << "," << UnComma(pRoom->GetGameStatus()));
  }


  // tell everyone about new player, and tell new player about everyone
  std::set<std::string>::iterator setit;
  for (setit = m_Inhabitants.begin() ; setit != m_Inhabitants.end() ; ++setit)
  {
    SENDLINE(*setit,"INHABITANT," << i_Name << "," << INITIALROOM );
    SENDLINE(i_Name,"INHABITANT," << *setit << "," << GetRoomOf(*setit)->GetName() );
  }
}

void RoomManager::HandleAction(const std::string &i_Name,const ActionParser &i_ap,
			       GameServerConnectionHandlerFactory &i_Connections)
{
  if (i_ap.GetActionName() == "NEWROOM") HandleNewRoom(i_Name,i_ap,i_Connections);
  else if (i_ap.GetActionName() == "CHANGEROOM") HandleChangeRoom(i_Name,i_ap,i_Connections);
  else if (i_ap.GetActionName() == "ROOMTALK") HandleRoomTalk(i_Name,i_ap,i_Connections);
  else if (i_ap.GetActionName() == "PLAYERTALK") HandlePlayerTalk(i_Name,i_ap,i_Connections);
  else
  {
    Room *pRoom = GetRoomOf(i_Name);
    if (!pRoom) return;
    pRoom->HandleAction(i_Name,i_ap);

    // disseminate new room state ROOMSTATE
    std::set<std::string>::iterator setit;
    int ctr;
    for (setit = m_Inhabitants.begin() ; setit != m_Inhabitants.end() ; ++setit)
    {
      SENDLINE(i_Name, "GUIROOM," << UnComma(pRoom->GetName())
               << "," << UnComma(pRoom->GetGameName())
               << "," << UnComma(pRoom->GetGameStatus()));
      ctr++;
    }
  }
}

void RoomManager::HandleNewRoom(const std::string &i_Name,const ActionParser &i_ap,
                     GameServerConnectionHandlerFactory &i_Connections)
{
  if (i_ap.GetNumArguments() == 0)
  {
    SENDLINE(i_Name,"ERROR,Bad NEWROOM action");
    return;
  }
  std::string newName = i_ap[0];

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

  // tell everyone about new room ROOMINDEX
  std::set<std::string>::iterator setit;
  for (setit = m_Inhabitants.begin() ; setit != m_Inhabitants.end() ; ++setit)
  {
    SENDLINE(*setit,"GUIROOM," << UnComma(newName) << ",,");
  }
}

void RoomManager::HandleChangeRoom(const std::string &i_Name,const ActionParser &i_ap,
                     GameServerConnectionHandlerFactory &i_Connections)
{
  if (i_ap.GetNumArguments() == 0)
  {
    SENDLINE(i_Name,"ERROR,Bad CHANGEROOM action");
    return;
  }

  std::string newRoomName = i_ap[0].c_str();
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

  // tell everyone that player has new room INHABITANT
  std::set<std::string>::iterator setit;
  for (setit = m_Inhabitants.begin() ; setit != m_Inhabitants.end() ; ++setit)
  {
    SENDLINE(*setit,"INHABITANT," << UnComma(i_Name) << "," << UnComma(newRoomName));
  }

}


void RoomManager::HandleRoomTalk(const std::string &i_Name,const ActionParser &i_ap,
                     GameServerConnectionHandlerFactory &i_Connections)
{
  if (i_ap.GetNumArguments() == 0)
  {
    i_Connections.SendLineToName(i_Name,"ERROR,Bad ROOMTALK action");
    return;
  }

  Room *pRoom = GetRoomOf(i_Name);
  if (!pRoom) return;

  std::string message;
  message += "(";
  message += i_Name;
  message += " to room '";
  message += pRoom->GetName();
  message += "') ";
  message += i_ap[0];

  std::string packet;
  packet += "MESSAGE,";
  packet += UnComma(message);

  std::set<std::string>::iterator beginit = pRoom->GetInhabitantSet().begin();
  std::set<std::string>::iterator endit = pRoom->GetInhabitantSet().end();

  for (; beginit != endit ; ++beginit)
  {
    i_Connections.SendLineToName(*beginit,packet);
  }
}

void RoomManager::HandlePlayerTalk(const std::string &i_Name,const ActionParser &i_ap,
                     GameServerConnectionHandlerFactory &i_Connections)
{
  if (i_ap.GetNumArguments() != 2)
  {
    i_Connections.SendLineToName(i_Name,"ERROR,Bad PLAYERTALK action");
    return;
  }

  std::string target = i_ap[0];

  std::string message;
  message += "(";
  message += i_Name;
  message += ") ";
  message += i_ap[1];

  std::string packet;
  packet += "MESSAGE,";
  packet += UnComma(message);


  i_Connections.SendLineToName(target,packet);
}

void RoomManager::HandleDisconnect(const std::string &i_Name,GameServerConnectionHandlerFactory &i_Connections)
{
  m_Inhabitants.erase(i_Name);
  Room *pRoom = GetRoomOf(i_Name);
  if (!pRoom) return;
  pRoom->RemovePlayerFromRoom(i_Name);

  // tell everyone player is gone DROPINHABITANT
  std::set<std::string>::iterator setit;
  for (setit = m_Inhabitants.begin() ; setit != m_Inhabitants.end() ; ++setit)
  {
    SENDLINE(*setit,"DROPINHABITANT," << i_Name);
  }

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

