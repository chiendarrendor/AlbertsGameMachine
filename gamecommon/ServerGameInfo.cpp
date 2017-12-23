#include "ServerGameInfo.hpp"
#include "FileUtilities.hpp"
#include <iostream>


ServerGameInfo::ServerGameInfo(const std::string &i_Name,
                               const std::string &i_DataDir,
                               const std::string &i_XMLLoc,
                               const std::string &i_XMLFile) :
  m_Name(i_Name),
  m_XMLLoc(i_XMLLoc),
  m_XMLFile(i_XMLFile),
  m_IsValid(false)
{
    if (!FileExists(i_DataDir))
  {
    if (!MakeDirectory(i_DataDir))
    {
      std::cout << "Can't create directory " << i_DataDir << std::endl;
      return;
    }
  }

  m_SaveDir = i_DataDir + DIR_SEP + "save";

  if (!FileExists(m_SaveDir))
  {
    if (!MakeDirectory(m_SaveDir))
    {
      std::cout << "Can't create save directory " << m_SaveDir << std::endl;
      return;
    }
  }
  m_IsValid = true;
}

ServerGameInfo::~ServerGameInfo()
{
}



const std::string &ServerGameInfo::GetName() const
{
  return m_Name;
}

const std::string &ServerGameInfo::GetSaveDir() const
{
  return m_SaveDir;
}

const std::string &ServerGameInfo::GetXMLLoc() const
{
  return m_XMLLoc;
}

const std::string &ServerGameInfo::GetXMLFile() const
{
  return m_XMLFile;
}


bool ServerGameInfo::IsDirectoryValid() const
{
  return m_IsValid;
}
