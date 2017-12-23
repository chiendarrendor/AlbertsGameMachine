
#include <string>

class OutputPort;
class ServerGameInfo;
class Game;

#ifdef __cplusplus
extern "C" {
#endif

#define DLLFUNC __declspec(dllexport)

  DLLFUNC bool Initialize(const std::string &i_DataDir);
  DLLFUNC Game *CreateGame(const ServerGameInfo &i_rServerGameInfo,OutputPort &i_rOutputPort);


#ifdef __cplusplus
}
#endif
