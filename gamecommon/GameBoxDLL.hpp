
#include <string>

class OutputPort;
class ServerGameInfo;
class Game;

#ifdef msys
#define DLLFUNC __declspec(dllexport)
#elif defined(FreeBSD) || defined(linux)
#define DLLFUNC
#endif


#ifdef __cplusplus
extern "C" {
#endif

  DLLFUNC bool Initialize(const std::string &i_DataDir);
  DLLFUNC Game *CreateGame(const ServerGameInfo &i_rServerGameInfo,OutputPort &i_rOutputPort);


#ifdef __cplusplus
}
#endif
