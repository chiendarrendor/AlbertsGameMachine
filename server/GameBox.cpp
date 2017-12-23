#include "GameBox.hpp"
#include <iostream>
#include "SystemSpecificDynamicLoading.hpp"

typedef bool (*InitializePointer)(std::string);


GameBox::GameBox(const std::string &i_Name,
                 const std::string &i_DataDir,
                 const std::string &i_XMLLoc,
                 const std::string &i_XMLFile,
                 const std::string &i_DLLFile) :
  ServerGameInfo(i_Name,i_DataDir,i_XMLLoc,i_XMLFile),
  m_DLLFile(i_DLLFile),
  m_IsValid(false),
  m_ploader(NULL),
  m_pCreateGameFunc(NULL)
{
  m_ploader = new GenericLibraryLoader(i_DLLFile);

  if (!m_ploader->IsOk())
  {
    std::cout << "Invalid Library " << i_Name << std::endl;
    return;
  }

  InitializePointer pInitFunc = (InitializePointer)m_ploader->GetFunctionAddress("Initialize");
  if (!pInitFunc)
  {
    std::cout << "No Initialize Function for " << i_Name << std::endl;
    return;
  }
    
  if (!pInitFunc(i_DataDir))
  {
    std::cout << "Initialize Function Failed for " << i_Name << std::endl;
    return;
  }

  m_pCreateGameFunc = (CreateGamePointer)m_ploader->GetFunctionAddress("CreateGame");
  
  if (!m_pCreateGameFunc)
  {
    std::cout << "No Create Game Function for " << i_Name << std::endl;
    return;
  }

  m_IsValid = true;
}

GameBox::~GameBox()
{
}

Game *GameBox::CreateGame(OutputPort &i_rConnections) const
{
  return m_pCreateGameFunc(*this,i_rConnections);
}

const std::string &GameBox::GetDLLFile() const
{
  return m_DLLFile;
}


bool GameBox::IsValid() const
{
  return m_IsValid;
}

std::string GameBox::GetErrorString() const
{
  if (m_ploader == NULL) return ("no loader");
  return m_ploader->GetErrorString();
}

