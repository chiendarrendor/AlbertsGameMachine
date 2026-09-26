#ifndef GAMEBOXHPP
#define GAMEBOXHPP

#include <string>
#include "ServerGameInfo.hpp"

class Game;
class OutputPort;

// Per-game-type launch spec + the server's residual static config for that
// type (Name/XMLLoc/XMLFile, inherited from ServerGameInfo) -- see
// .claude/server_game_interface_spec.md's "Server's residual per-game-type
// knowledge" section. CreateGame() spawns a GameProcessProxy instead of
// dlopen-ing a .so; this class no longer loads anything itself.
class GameBox : public ServerGameInfo
{
public:
  GameBox(const std::string &i_Name,
          const std::string &i_DataDir,
          const std::string &i_XMLLoc,
          const std::string &i_XMLFile,
          const std::string &i_LaunchCommand);
  virtual ~GameBox();

  bool IsValid() const;
  std::string GetErrorString() const;

  Game *CreateGame(OutputPort &i_rConnections) const;

private:
  std::string m_LaunchCommand;
  bool m_IsValid;
  std::string m_ErrorString;
};

#endif
