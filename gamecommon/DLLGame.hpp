#include "Game.hpp"
#include "StateWalker.hpp"
#include <fstream>
#include <iostream>
#include <set>
#include "ServerGameInfo.hpp"
#include "OutputPort.hpp"
#include "ActionParser.hpp"
#include "Serialize.hpp"
#include "GameActionError.hpp"
#include <boost/json.hpp>

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
  virtual void HandleAction(const std::string &i_Name,const ActionParser &i_ap,
                             const std::set<std::string> &i_Roster);

  virtual bool LoadFile(const std::string &i_FileName);
  virtual bool SaveFile(const std::string &i_FileName) const;

private:
  // The generic, game-agnostic half of the full-state envelope: which state
  // we're in and what actions i_PlayerName may legally take from it. Shared
  // between the success path (SendFullState, which adds "body") and the
  // rejected-action path (which will add "error" instead) so both carry
  // identical state/legalActions -- nothing changed on a rejected action, so
  // there's no reason for the two paths to disagree about either.
  boost::json::object BuildStateWalkerEnvelope(const std::string &i_PlayerName) const;

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
boost::json::object DLLGame<T_GameSet,T_DLLGameInfo>::BuildStateWalkerEnvelope
(const std::string &i_PlayerName) const
{
  boost::json::array legalactions;
  std::vector<std::string> legaltransitions;
  if (m_StateWalker.GetLegalTransitionNames(i_PlayerName,legaltransitions))
  {
    std::vector<std::string>::iterator transit;
    for (transit = legaltransitions.begin() ; transit != legaltransitions.end() ; ++transit)
    {
      legalactions.push_back(boost::json::value(*transit));
    }
  }

  boost::json::object envelope;
  envelope["state"] = m_StateWalker.GetCurStateName();
  envelope["stateDesc"] = m_StateWalker.GetCurStateDesc();
  envelope["legalActions"] = legalactions;
  return envelope;
}

template<class T_GameSet,class T_DLLGameInfo>
void DLLGame<T_GameSet,T_DLLGameInfo>::SendFullState
(const std::string &i_PlayerName) const
{
  boost::json::object envelope = BuildStateWalkerEnvelope(i_PlayerName);
  envelope["body"] = m_GameInfo.GetClientState(i_PlayerName,m_StateWalker.GetCurStateName());

  GetOutputPort().UniCastJson(i_PlayerName,envelope);
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
void DLLGame<T_GameSet,T_DLLGameInfo>::HandleAction
(const std::string &i_Name,const ActionParser &i_ap,
 const std::set<std::string> &i_Roster)
{
  ExecuteStatus status;
  try
  {
    status = m_StateWalker.Execute(i_Name,i_ap);
  }
  catch (const GameActionError &e)
  {
    boost::json::object envelope = BuildStateWalkerEnvelope(i_Name);
    envelope["error"] = e.what();
    GetOutputPort().UniCastJson(i_Name,envelope);
    return;
  }

  switch(status)
  {
  case NO_STATE:
    {
      boost::json::object envelope = BuildStateWalkerEnvelope(i_Name);
      envelope["error"] = "Game is in an invalid state!";
      GetOutputPort().UniCastJson(i_Name,envelope);
    }
    return;
  case TRANSITION_NOT_FOUND:
    {
      boost::json::object envelope = BuildStateWalkerEnvelope(i_Name);
      envelope["error"] = "Unknown Action sent to server!";
      GetOutputPort().UniCastJson(i_Name,envelope);
    }
    return;
  case EXECUTE_FAILED:
    {
      boost::json::object envelope = BuildStateWalkerEnvelope(i_Name);
      envelope["error"] = "That is not a legal move.";
      GetOutputPort().UniCastJson(i_Name,envelope);
    }
    return;
  case RECURSION_LIMIT:
    // Not a rejection -- the auto-transition chain hit its safety limit but
    // the resulting state is still valid, so this falls through to the
    // normal roster broadcast below like EXECUTE_OK. i_Name gets this
    // warning now and the real post-action full-state moments later.
    {
      boost::json::object envelope = BuildStateWalkerEnvelope(i_Name);
      envelope["error"] = "WARNING: Potential Auto-Infinite-Loop detected! (game state still valid)";
      GetOutputPort().UniCastJson(i_Name,envelope);
    }
    break;
  case EXECUTE_OK:
    break;
  default:
    {
      boost::json::object envelope = BuildStateWalkerEnvelope(i_Name);
      envelope["error"] = "StateWalker return unknown status!";
      GetOutputPort().UniCastJson(i_Name,envelope);
    }
    return;
  };

  // OK...If we got here, the state is sufficiently valid to be worth
  // updating everyone. This is the entire Phase 3 mechanism: every
  // successful action resends full state (state name, legal actions, and
  // the developer's own GetClientState body) to every current room
  // occupant -- players and spectators alike. i_Roster is the only source
  // of candidate names now that the game runs out-of-process and has no
  // room-membership knowledge of its own; the server supplies it fresh on
  // every call. This replaces both the old BroadcastNEWSTATE and the old
  // per-transition/per-roster UnicastLEGALACTION loop (which relied on
  // RoomOutputPort iterating its own broadcast-target set in-process) --
  // both are now fully subsumed by SendFullState's envelope. See
  // .claude/server_game_interface_spec.md's Phase 3 design.
  std::set<std::string>::const_iterator playerit;
  for (playerit = i_Roster.begin() ; playerit != i_Roster.end() ; ++playerit)
  {
    SendFullState(*playerit);
  }
}

