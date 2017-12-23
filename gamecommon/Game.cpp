#include "Game.hpp"
#include "ServerGameInfo.hpp"
#include "FileUtilities.hpp"

Game::Game(const ServerGameInfo &i_rServerGameInfo,OutputPort &i_rOutputPort) :
  m_rOutputPort(i_rOutputPort),
  m_rServerGameInfo(i_rServerGameInfo)
{
}

const ServerGameInfo &Game::GetServerGameInfo() const
{
  return m_rServerGameInfo;
}

OutputPort &Game::GetOutputPort() const
{
  return m_rOutputPort;
}

bool Game::Load(const std::string &i_FileName)
{
  return LoadFile(m_rServerGameInfo.GetSaveDir() + DIR_SEP + i_FileName);
}

bool Game::Save(const std::string &i_FileName)
{
  return SaveFile(m_rServerGameInfo.GetSaveDir() + DIR_SEP + i_FileName);
}
