#include "Game.hpp"
#include "StateWalker.hpp"
#include <fstream>
#include <iostream>
#include "ServerGameInfo.hpp"
#include "OutputPort.hpp"
#include "ActionParser.hpp"
#include "Serialize.hpp"

template<class T_GameSet,class T_DLLGameInfo>
class DLLGame : public Game
{
public:
  DLLGame(const ServerGameInfo &i_rServerGameInfo,
          OutputPort &i_rOutputPort,
          const T_GameSet &i_GameSet,
          const StateMachine<T_DLLGameInfo> &i_StateMachine,
          int i_AutoRecursionDepth);

  virtual std::string GetName() const;
  virtual std::string GetStatusString() const;
  virtual bool IsDone() const;
  virtual void SendFullState(const std::string &i_Name) const;
  virtual void HandleAction(const std::string &i_Name,const ActionParser &i_ap);

  virtual bool LoadFile(const std::string &i_FileName);
  virtual bool SaveFile(const std::string &i_FileName) const;

private:
  T_DLLGameInfo m_GameInfo;
  StateWalker<T_DLLGameInfo> m_StateWalker;

  SERIALIZE_FUNC
  {
    SERIALIZE(m_GameInfo);
    SERIALIZE(m_StateWalker);
  }
};

template<class T_GameSet,class T_DLLGameInfo>
DLLGame<T_GameSet,T_DLLGameInfo>::DLLGame
(const ServerGameInfo &i_rServerGameInfo,
 OutputPort &i_rOutputPort,
 const T_GameSet &i_GameSet,
 const StateMachine<T_DLLGameInfo> &i_StateMachine,
 int i_AutoRecursionDepth) :
  Game(i_rServerGameInfo,i_rOutputPort),
  m_GameInfo(i_GameSet,i_rServerGameInfo,i_rOutputPort),
  m_StateWalker(i_StateMachine,m_GameInfo,i_AutoRecursionDepth)
{
}

template<class T_GameSet,class T_DLLGameInfo>
bool DLLGame<T_GameSet,T_DLLGameInfo>::LoadFile
(const std::string &i_FileName)
{
  std::ifstream indata(i_FileName.c_str());
  if (!indata)
  {
    return false;
  }

  try
  {
    LoadArchive i_ar(indata);
    DLLGame<T_GameSet,T_DLLGameInfo> &thisref = *this;
    SERIALIZE(thisref);
  }
  catch(std::exception &ste)
  {
    std::cout << "Caught exception in load:" << ste.what() << std::endl;
    return false;
  }

  return true;
}

template<class T_GameSet,class T_DLLGameInfo>
bool DLLGame<T_GameSet,T_DLLGameInfo>::SaveFile
(const std::string &i_FileName) const
{
  if (!m_StateWalker.IsValid())
  {
    return false;
  }

  std::ofstream outdata(i_FileName.c_str());
  if (!outdata)
  {
    return false;
  }

  try
  {
    SaveArchive i_ar(outdata);
    const DLLGame<T_GameSet,T_DLLGameInfo> &thisref = *this;
    SERIALIZE(thisref);
  }
  catch(std::exception &ste)
  {
    std::cout << "Caught exception in save:" << ste.what() << std::endl;
    return false;
  }

  return true;
}

template<class T_GameSet,class T_DLLGameInfo>
void DLLGame<T_GameSet,T_DLLGameInfo>::SendFullState
(const std::string &i_PlayerName) const
{
  m_GameInfo.SendFullState(i_PlayerName,m_StateWalker.GetCurStateName());

  m_GameInfo.UnicastNEWSTATE(i_PlayerName,m_StateWalker.GetCurStateName(),m_StateWalker.GetCurStateDesc());
  std::vector<std::string> legaltransitions;
  if (!m_StateWalker.GetLegalTransitionNames(i_PlayerName,legaltransitions))
  {
    return;
  }
  std::vector<std::string>::iterator transit;
  for (transit = legaltransitions.begin() ; transit != legaltransitions.end() ; ++transit)
  {
    m_GameInfo.UnicastLEGALACTION(i_PlayerName,*transit);
  }
}

template<class T_GameSet,class T_DLLGameInfo>
bool DLLGame<T_GameSet,T_DLLGameInfo>::IsDone() const
{
  return m_StateWalker.IsAtEnd();
}

template<class T_GameSet,class T_DLLGameInfo>
std::string DLLGame<T_GameSet,T_DLLGameInfo>::GetName() const
{
  return GetServerGameInfo().GetName();
}

template<class T_GameSet,class T_DLLGameInfo>
std::string DLLGame<T_GameSet,T_DLLGameInfo>::GetStatusString() const
{
  std::string result;
  if (m_StateWalker.GetCurStateName() == INITIALSTATENAME)
  {
    result += "Starting up.";
  }
  else if (m_StateWalker.GetCurStateName() == TERMINALSTATENAME)
  {
    result += "Complete.";
  }
  else
  {
    result += "In Progress.";
  }

  return result;
}

template<class T_GameSet,class T_DLLGameInfo>
class TransitionBoolean : public NameBoolean
{
public:
  typedef Transition< T_DLLGameInfo > TransitionType;
  TransitionBoolean(const TransitionType &i_Transition,
                    const T_DLLGameInfo &i_GameInfo,
                    const StateWalker<T_DLLGameInfo> &i_StateWalker) : 
    m_Transition(i_Transition),
    m_GameInfo(i_GameInfo),
    m_StateWalker(i_StateWalker)
  {
  }
  virtual bool operator()(const std::string &i_PlayerName) const
  {
    return m_Transition.IsLegal(i_PlayerName,m_StateWalker.GetCurStateName(),m_GameInfo);
  }
private:
  const TransitionType &m_Transition;
  const T_DLLGameInfo &m_GameInfo;
  const StateWalker<T_DLLGameInfo> &m_StateWalker;
};


template<class T_GameSet,class T_DLLGameInfo>
void DLLGame<T_GameSet,T_DLLGameInfo>::HandleAction
(const std::string &i_Name,const ActionParser &i_ap)
{

  m_GameInfo.ResetERRORCount();

  ExecuteStatus status = m_StateWalker.Execute(i_Name,i_ap);

  switch(status)
  {
  case NO_STATE:
    m_GameInfo.BroadcastERROR("Game is in an invalid state!");
    return;
  case TRANSITION_NOT_FOUND:
    m_GameInfo.UnicastERROR(i_Name,"Unknown Action sent to server!");
    return;
  case EXECUTE_FAILED:
    if (m_GameInfo.GetERRORCount() == 0)
    {
      m_GameInfo.UnicastERROR(i_Name,"That is not a legal move.");
    }
    return;
  case RECURSION_LIMIT:
    m_GameInfo.BroadcastERROR("WARNING: Potential Auto-Infinite-Loop detected! (game state still valid)");
    break;
  case EXECUTE_OK:
    break;
  default:
    m_GameInfo.BroadcastERROR("StateWalker return unknown status!");
    return;
  };

  // OK...If we got here, the state is sufficiently valid to be worth updating everyone.
  m_GameInfo.BroadcastNEWSTATE(m_StateWalker.GetCurStateName(),m_StateWalker.GetCurStateDesc());

  const typename State<T_DLLGameInfo>::TransMap &transmap = m_StateWalker.GetCurState()->GetTransitionMap();
  typename State<T_DLLGameInfo>::TransMap::const_iterator transit;

  for (transit = transmap.begin() ; transit != transmap.end() ; ++transit)
  {
    TransitionBoolean<T_GameSet,T_DLLGameInfo> tb(*(transit->second.first),m_GameInfo,m_StateWalker);
    m_GameInfo.VaricastLEGALACTION(tb,transit->first);
  }
}

