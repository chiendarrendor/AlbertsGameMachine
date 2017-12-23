#include "GameCloset.hpp"
#include <fstream>
#include <iostream>

#include "GameBox.hpp"
#include "StringUtilities.hpp"
#include "FileUtilities.hpp"

GameCloset::GameCloset(const std::string &i_rDataFile,const std::string &i_rDataDir)
{
  std::ifstream data(i_rDataFile.c_str());
  if (!data)
  {
    std::cout << "Can't read Game Closet Init File " << i_rDataFile << std::endl;
    exit(1);
  }
  std::string buf;

  while(std::getline(data,buf))
  {
    std::vector<std::string> tokens;
    Tokenize(tokens,buf,",");
    if (tokens.size() != 4)
    {
      std::cout << "Illegal line in GameCloset file " << i_rDataFile << ": " << buf << std::endl;
      exit(1);
    }

    std::string name = tokens[0];
    std::string dir = i_rDataDir + DIR_SEP + name;
    std::string XMLloc = tokens[1];
    std::string XMLfile = tokens[2];
    std::string DLLfile = tokens[3];

    GameBox *pNewGameBox = new GameBox(name,dir,XMLloc,XMLfile,DLLfile); 
    if (!pNewGameBox->IsValid())
    {
      std::cout << "Couldn't load External Library " << DLLfile << " for GameBox " << name << ": " << pNewGameBox->GetErrorString() << std::endl;
      delete pNewGameBox;
      continue;
    }
    m_GameBoxNames.insert(name);
    m_GameBoxes[name] = pNewGameBox;
  }

  if (m_GameBoxes.size() == 0)
  {
    //    std::cout << "No Games loaded! exiting..." << std::endl;
    //    exit(1);
  }
}

const std::set<std::string> &GameCloset::GetGameBoxNames() const
{
  return m_GameBoxNames;
}

const GameBox *GameCloset::GetGameBoxByName(const std::string &i_Name) const
{
  std::map<std::string,GameBox *>::const_iterator findit = m_GameBoxes.find(i_Name);

  if (findit == m_GameBoxes.end())
  {
    return NULL;
  }

  return findit->second;
}

  
