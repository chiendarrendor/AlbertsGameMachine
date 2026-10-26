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

  // Not part of the Game interface -- GameServerMain.hpp calls this directly
  // on the concrete DLLGame it owns, never through a Game&. GameProcessProxy
  // (the server-side stand-in for a real game process) never needs its own
  // version either: it forwards the whole handleAction RPC byte-for-byte
  // regardless of "type", so the transition/sequence distinction only ever
  // matters on this side of the process boundary.
  // i_Params is the raw, untrusted client-supplied "start"/"end" -- extracted
  // and validated here, not by the caller, since this is the only place with
  // access to BuildStateWalkerEnvelope/GetOutputPort() to respond gracefully
  // if they're missing, non-numeric, or negative. Letting any of that
  // propagate uncaught would crash the whole game process (GameProcessProxy
  // reads a crashed process as the game dying and tears down the entire
  // room), not just reject this one request.
  void HandleSequenceRequest(const std::string &i_PlayerName,const std::string &i_SequenceName,
                              const boost::json::object &i_Params);

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
  envelope["gameInstanceId"] = m_StateWalker.GetInstanceId();
  // One generic {name: count} object, parallel to legalActions, for every
  // <sequence> this game declares (empty for a game with none) -- a
  // sequence-watching widget's whole mechanism for learning there's more
  // data worth fetching is this count changing between deliveries
  // (docs/Sequences.txt). Present unconditionally here, same as legalActions
  // itself, rather than only on the success path -- a rejected action or a
  // sequence response both still carry the same generic envelope siblings.
  envelope["sequences"] = m_GameInfo.SequenceCounts(i_PlayerName);
  // Mirrors the incoming action's own "type" (see ActionParser/GameGui.
  // SendAction) so the client can tell what kind of request produced this
  // response and interpret "body"/"error" accordingly -- same spirit as
  // "gui" already telling the client whether "body" is ROOMGUI's
  // {rooms,myroom} shape or a real game's GetClientState() shape.
  // Every caller of this helper today is transition-shaped (SendFullState,
  // every HandleAction rejection path); HandleSequenceRequest is the one
  // exception and overrides this itself.
  envelope["type"] = "transition";
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

// A sequence request is a read-only query, not a state-mutating action -- it
// never touches m_StateWalker at all, and the response goes only to the
// requester, never the whole roster, unlike HandleAction above. Reuses the
// same "body"/"error" fields a transition resend uses, distinguished by the
// envelope's own "type" (set to "sequence" here, overriding
// BuildStateWalkerEnvelope's "transition" default) -- same spirit as "gui"
// already telling the client whether "body" is ROOMGUI's shape or a real
// game's. The client checks "type" before deciding how to handle "error":
// quietly, for whatever's managing this pending request, not through
// whatever user-visible channel a rejected transition's "error" goes through
// today (a modal dialog -- ClientManager.java's ShowDebug) -- a sequence
// request is typically a widget fetching data in the background (e.g.
// scrolling a history list), not a direct user action, so it shouldn't pop
// up an intrusive error the user never asked for.
template<class T_GameSet,class T_DLLGameInfo>
void DLLGame<T_GameSet,T_DLLGameInfo>::HandleSequenceRequest
(const std::string &i_PlayerName,const std::string &i_SequenceName,
 const boost::json::object &i_Params)
{
  boost::json::object envelope = BuildStateWalkerEnvelope(i_PlayerName);
  envelope["type"] = "sequence";

  try
  {
    // Extracted and range-checked as plain signed ints first, deliberately,
    // rather than trusting boost::json's own unsigned-conversion behavior to
    // reject a negative value -- our own explicit check here is obvious to a
    // reader and gives a clear message, instead of depending on library
    // internals nobody reading this code would think to go verify. Only
    // cast to size_t once both are known non-negative, right at the point
    // SequenceCount/SequenceBody actually need it.
    int start = i_Params.at("start").to_number<int>();
    int end = i_Params.at("end").to_number<int>();
    if (start < 0 || end < 0)
    {
      throw GameActionError("start/end must not be negative for sequence " + i_SequenceName);
    }

    int count = m_GameInfo.SequenceCount(i_SequenceName,i_PlayerName);
    if (start >= end || end > count)
    {
      throw GameActionError("Illegal range for sequence " + i_SequenceName);
    }

    boost::json::object body;
    body["sequence"] = i_SequenceName;
    body["firstIndex"] = start;
    body["records"] = m_GameInfo.SequenceBody(i_SequenceName,i_PlayerName,
                                               static_cast<size_t>(start),static_cast<size_t>(end));
    envelope["body"] = std::move(body);
  }
  catch (const GameActionError &e)
  {
    envelope["error"] = e.what();
  }
  // Missing/non-numeric "start"/"end" (a malformed or hostile request, since
  // real requests only ever come from our own client code) throws from
  // boost::json itself, not GameActionError -- caught here too so it can't
  // escape uncaught and crash the whole game process over one bad request.
  catch (const std::exception &e)
  {
    envelope["error"] = std::string("Malformed sequence request: ") + e.what();
  }

  GetOutputPort().UniCastJson(i_PlayerName,envelope);
}

