
#ifndef GAMEINFOHPP
#define GAMEINFOHPP

class OutputPort;
class ServerGameInfo;

template <class T_GameData,class T_GameState>
class GameInfo : public T_GameState
{
public:
  GameInfo(const T_GameData &i_rGameData,
           const ServerGameInfo &i_ginfo,
           OutputPort &i_rOutputPort);
protected:
  const ServerGameInfo &m_ServerGameInfo;
  OutputPort &m_OutputPort;
};




template<class T_GameData,class T_GameState>
GameInfo<T_GameData,T_GameState>::GameInfo(const T_GameData &i_rGameData,
                                           const ServerGameInfo &i_ginfo,
                                           OutputPort &i_rOutputPort) :
  T_GameState(i_rGameData),m_ServerGameInfo(i_ginfo),m_OutputPort(i_rOutputPort)
{
}


#endif
