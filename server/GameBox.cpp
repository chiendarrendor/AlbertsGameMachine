#include "GameBox.hpp"
#include "GameProcessProxy.hpp"
#include <iostream>
#include <unistd.h>

GameBox::GameBox(const std::string &i_Name,
                 const std::string &i_DataDir,
                 const std::string &i_XMLLoc,
                 const std::string &i_XMLFile,
                 const std::string &i_LaunchCommand) :
  ServerGameInfo(i_Name,i_DataDir,i_XMLLoc,i_XMLFile),
  m_LaunchCommand(i_LaunchCommand),
  m_IsValid(false)
{
  if (access(i_LaunchCommand.c_str(), X_OK) != 0)
  {
    m_ErrorString = "Launch command not found or not executable: " + i_LaunchCommand;
    std::cout << "Invalid Launch Command " << i_Name << std::endl;
    return;
  }

  m_IsValid = true;
}

GameBox::~GameBox()
{
}

Game *GameBox::CreateGame(OutputPort &i_rConnections) const
{
  return new GameProcessProxy(*this, i_rConnections, m_LaunchCommand, GetDataDir());
}

bool GameBox::IsValid() const
{
  return m_IsValid;
}

std::string GameBox::GetErrorString() const
{
  return m_ErrorString;
}
