#ifndef GAMEBOXHPP
#define GAMEBOXHPP

#include <string>
#include "ServerGameInfo.hpp"

class Game;
class OutputPort;
class GenericLibraryLoader;

typedef Game *(*CreateGamePointer)(const ServerGameInfo &,OutputPort &);

class GameBox : public ServerGameInfo
{
public:
  GameBox(const std::string &i_Name,
          const std::string &i_DataDir,
          const std::string &i_XMLLoc,
          const std::string &i_XMLFile,
          const std::string &i_DLLFile);
  virtual ~GameBox();
  const std::string &GetDLLFile() const;

  bool IsValid() const;
  std::string GetErrorString() const;

  Game *CreateGame(OutputPort &i_rConnections) const;

private:
  std::string m_DLLFile;
  bool m_IsValid;
  GenericLibraryLoader *m_ploader;
  CreateGamePointer m_pCreateGameFunc;
};

#endif
