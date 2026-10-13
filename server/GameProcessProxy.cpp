#include "GameProcessProxy.hpp"
#include "JsonLineProtocol.hpp"
#include "OutputPort.hpp"
#include "ActionParser.hpp"
#include "ServerGameInfo.hpp"

#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

GameProcessProxy::GameProcessProxy(const ServerGameInfo &i_rServerGameInfo,
                                    OutputPort &i_rOutputPort,
                                    const std::string &i_LaunchCommand,
                                    const std::string &i_DataDir) :
  Game(i_rServerGameInfo, i_rOutputPort),
  m_ChildPid(-1),
  m_NextId(1),
  m_Terminated(false)
{
  int toChild[2];
  int fromChild[2];

  if (pipe(toChild) != 0 || pipe(fromChild) != 0)
  {
    throw ProtocolError("GameProcessProxy: pipe() failed");
  }

  pid_t pid = fork();
  if (pid < 0)
  {
    throw ProtocolError("GameProcessProxy: fork() failed");
  }

  if (pid == 0)
  {
    // child
    dup2(toChild[0], STDIN_FILENO);
    dup2(fromChild[1], STDOUT_FILENO);
    close(toChild[0]);
    close(toChild[1]);
    close(fromChild[0]);
    close(fromChild[1]);

    // DataDir is the one argument every game genuinely uses; Name/XMLLoc/
    // XMLFile are appended after it so the game process can reconstruct a
    // real, faithful ServerGameInfo rather than a partial one -- matching
    // what used to be one shared object in-process, before decoupling
    // split it into a server-side and a game-side copy. Name specifically
    // is load-bearing: some generated per-game events (e.g. RESET) embed
    // it directly in their wire payload (found live, 2026-09-27).
    execlp(i_LaunchCommand.c_str(), i_LaunchCommand.c_str(),
           i_DataDir.c_str(),
           i_rServerGameInfo.GetName().c_str(),
           i_rServerGameInfo.GetXMLLoc().c_str(),
           i_rServerGameInfo.GetXMLFile().c_str(),
           (char *)NULL);
    // execlp only returns on failure. There's no parent left to tell
    // synchronously; the parent finds out via EOF on its first read.
    _exit(127);
  }

  // parent
  m_ChildPid = pid;
  close(toChild[0]);
  close(fromChild[1]);

  m_pToChildBuf.reset(new __gnu_cxx::stdio_filebuf<char>(toChild[1], std::ios::out));
  m_pFromChildBuf.reset(new __gnu_cxx::stdio_filebuf<char>(fromChild[0], std::ios::in));
  m_pToChild.reset(new std::ostream(m_pToChildBuf.get()));
  m_pFromChild.reset(new std::istream(m_pFromChildBuf.get()));
}

GameProcessProxy::~GameProcessProxy()
{
  if (m_pToChildBuf)
  {
    m_pToChildBuf->close(); // closes the child's stdin -> clean shutdown signal
  }
  if (m_ChildPid > 0)
  {
    int status = 0;
    waitpid(m_ChildPid, &status, 0);
  }
}

boost::json::value GameProcessProxy::SendRequest(const std::string &i_Method,
                                                  boost::json::object i_Params) const
{
  int id = m_NextId++;

  boost::json::object req;
  req["id"] = id;
  req["method"] = i_Method;
  req["params"] = std::move(i_Params);
  WriteMessage(*m_pToChild, req);

  while (true)
  {
    boost::optional<boost::json::value> msg = ReadMessage(*m_pFromChild);
    if (!msg)
    {
      throw ProtocolError("GameProcessProxy: game process closed its output unexpectedly");
    }
    if (!msg->is_object())
    {
      throw ProtocolError("GameProcessProxy: received a non-object message");
    }
    const boost::json::object &obj = msg->as_object();

    if (obj.contains("method"))
    {
      if (obj.at("method").as_string() != "event")
      {
        throw ProtocolError("GameProcessProxy: unknown notification method");
      }
      DispatchEvent(obj.at("params").as_object());
      continue;
    }

    if (!obj.contains("id") || obj.at("id").to_number<int>() != id)
    {
      throw ProtocolError("GameProcessProxy: response id mismatch");
    }
    return *msg;
  }
}

boost::optional<boost::json::value> GameProcessProxy::SafeSendRequest(const std::string &i_Method,
                                                                        boost::json::object i_Params) const
{
  if (m_Terminated)
  {
    return boost::none;
  }

  try
  {
    return SendRequest(i_Method, std::move(i_Params));
  }
  catch (ProtocolError &e)
  {
    Terminate(e.what());
    return boost::none;
  }
}

void GameProcessProxy::Terminate(const std::string &i_Reason) const
{
  if (m_Terminated)
  {
    return;
  }
  m_Terminated = true;

  // Synthetic, not a real StateWalker envelope -- the game is gone, there's
  // no state/legalActions left to report. Same shape as a real rejection
  // (state/stateDesc/legalActions/error) so this still unwraps identically
  // on the client, just with legalActions empty (nothing is legal anymore).
  boost::json::object envelope;
  envelope["state"] = "TERMINATED";
  envelope["stateDesc"] = "Game process terminated unexpectedly.";
  envelope["legalActions"] = boost::json::array();
  envelope["error"] = i_Reason;

  GetOutputPort().BroadCastFullState(GetServerGameInfo().GetName(),
                                     GetServerGameInfo().GetXMLLoc(),GetServerGameInfo().GetXMLFile(),
                                     envelope);

  if (m_ChildPid > 0)
  {
    kill(m_ChildPid, SIGKILL);
    int status = 0;
    waitpid(m_ChildPid, &status, 0);
    m_ChildPid = -1;
  }
}

void GameProcessProxy::DispatchEvent(const boost::json::object &i_Params) const
{
  // Untargeted/broadcast events (the old legacy-plain-text-BroadcastERROR
  // mechanism) are no longer supported at all -- nothing a real game
  // process can legitimately emit is untargeted anymore (every real event
  // is a per-player full-state envelope, every real game's custom <event>
  // declarations are gone, see the Generator-cleanup section of
  // .claude/server_game_interface_spec.md). A missing/"*" target now means
  // the game process is violating the protocol, same as any other
  // malformed message -- GameProcessProxy is the final arbiter of protocol
  // correctness (see this class's own header comment).
  boost::json::object::const_iterator targetIt = i_Params.find("target");
  if (targetIt == i_Params.end() || !targetIt->value().is_string() ||
      targetIt->value().as_string() == "*")
  {
    throw ProtocolError("GameProcessProxy: untargeted/broadcast events are no longer supported; "
                         "every event must be addressed to a specific recipient");
  }

  std::string target(targetIt->value().as_string().c_str());

  // Every event is a full-state envelope now: a native JSON object (the
  // game process's pure layers-1+2 state/legalActions/body-or-error), not
  // a legacy comma-line string. The server already has Name/XMLLoc/XMLFile
  // authoritatively via its own GetServerGameInfo() -- it's literally what
  // spawned this game process with that exact data -- so it adds layers
  // 3+4 here in one place via UniCastFullState, rather than having the
  // game process redundantly echo static data back on every message. See
  // .claude/server_game_interface_spec.md's Phase 3 envelope design.
  GetOutputPort().UniCastFullState(target,GetServerGameInfo().GetName(),
                                   GetServerGameInfo().GetXMLLoc(),GetServerGameInfo().GetXMLFile(),
                                   i_Params.at("message").as_object());
  // Note: a "SPECTATOR" target is phase-3-only (see the interface spec) --
  // not reachable from any phase-1 game, so not handled here yet.
}

void GameProcessProxy::HandleAction(const std::string &i_Name,const ActionParser &i_ap,
                                     const std::set<std::string> &i_Roster)
{
  boost::json::object params;
  params["player"] = i_Name;
  params["action"] = i_ap.GetActionName();
  params["params"] = i_ap.GetParams();

  boost::json::array roster;
  for (std::set<std::string>::const_iterator it = i_Roster.begin(); it != i_Roster.end(); ++it)
  {
    roster.push_back(boost::json::value(*it));
  }
  params["roster"] = std::move(roster);

  boost::optional<boost::json::value> response = SafeSendRequest("handleAction", params);
  if (!response)
  {
    return; // Terminate() already told the room; nothing left to do.
  }
  const boost::json::object &obj = response->as_object();

  if (obj.contains("error"))
  {
    // Unreachable for any real game process today (GameServerMain.hpp's
    // dispatch never attaches "error" to a handleAction response
    // specifically -- that only happens for an unrecognized method, and
    // "handleAction" always is one), but kept as a defensive, correctly-
    // JSON-shaped fallback rather than assuming it can never happen. A
    // real in-game rejection takes a completely different path: it's
    // reported by the game process itself, as its own targeted
    // UniCastJson envelope (an "event" notification, dispatched above via
    // DispatchEvent before this response is ever inspected), not via this
    // RPC-level "error" field at all.
    std::string errorText(obj.at("error").as_string().c_str());

    boost::json::object envelope;
    envelope["state"] = "PROTOCOLERROR";
    envelope["stateDesc"] = "Unexpected response from game process.";
    envelope["legalActions"] = boost::json::array();
    envelope["error"] = errorText;

    GetOutputPort().UniCastFullState(i_Name,GetServerGameInfo().GetName(),
                                     GetServerGameInfo().GetXMLLoc(),GetServerGameInfo().GetXMLFile(),
                                     envelope);
  }
}

bool GameProcessProxy::LoadFile(const std::string &i_FileName)
{
  boost::json::object params;
  params["filename"] = i_FileName;
  boost::optional<boost::json::value> response = SafeSendRequest("load", params);
  if (!response) return false;
  return response->at("result").at("success").as_bool();
}

bool GameProcessProxy::SaveFile(const std::string &i_FileName) const
{
  boost::json::object params;
  params["filename"] = i_FileName;
  boost::optional<boost::json::value> response = SafeSendRequest("save", params);
  if (!response) return false;
  return response->at("result").at("success").as_bool();
}

std::string GameProcessProxy::GetStatusString() const
{
  boost::optional<boost::json::value> response = SafeSendRequest("getStatusString", boost::json::object());
  if (!response) return "Game process terminated.";
  return std::string(response->at("result").at("status").as_string().c_str());
}

std::string GameProcessProxy::GetName() const
{
  return GetServerGameInfo().GetName();
}

bool GameProcessProxy::IsDone() const
{
  boost::optional<boost::json::value> response = SafeSendRequest("isDone", boost::json::object());
  if (!response) return true; // terminated (or just now detected broken) counts as done
  return response->at("result").at("done").as_bool();
}

void GameProcessProxy::SendFullState(const std::string &i_Name) const
{
  boost::json::object params;
  params["player"] = i_Name;
  SafeSendRequest("sendFullState", params);
}
