#include "GameProcessProxy.hpp"
#include "JsonLineProtocol.hpp"
#include "OutputPort.hpp"
#include "StringUtilities.hpp"
#include "ActionParser.hpp"
#include "ServerGameInfo.hpp"

#include <unistd.h>
#include <sys/wait.h>

GameProcessProxy::GameProcessProxy(const ServerGameInfo &i_rServerGameInfo,
                                    OutputPort &i_rOutputPort,
                                    const std::string &i_LaunchCommand,
                                    const std::string &i_DataDir) :
  Game(i_rServerGameInfo, i_rOutputPort),
  m_ChildPid(-1),
  m_NextId(1)
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

    execlp(i_LaunchCommand.c_str(), i_LaunchCommand.c_str(), i_DataDir.c_str(), (char *)NULL);
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

void GameProcessProxy::DispatchEvent(const boost::json::object &i_Params) const
{
  std::string message(i_Params.at("message").as_string().c_str());

  boost::json::object::const_iterator targetIt = i_Params.find("target");
  bool isBroadcast = (targetIt == i_Params.end()) || (targetIt->value().as_string() == "*");

  if (isBroadcast)
  {
    GetOutputPort().BroadCast(message);
  }
  else
  {
    std::string target(targetIt->value().as_string().c_str());
    GetOutputPort().UniCast(target, message);
  }
  // Note: a "SPECTATOR" target is phase-3-only (see the interface spec) --
  // not reachable from any phase-1 game, so not handled here yet.
}

void GameProcessProxy::HandleAction(const std::string &i_Name,const ActionParser &i_ap)
{
  boost::json::object params;
  params["player"] = i_Name;
  params["action"] = i_ap.GetRawLine();

  boost::json::value response = SendRequest("handleAction", params);
  const boost::json::object &obj = response.as_object();

  if (obj.contains("error"))
  {
    std::string errorText(obj.at("error").as_string().c_str());
    GetOutputPort().UniCast(i_Name, "ERROR," + UnComma(errorText));
  }
}

bool GameProcessProxy::LoadFile(const std::string &i_FileName)
{
  boost::json::object params;
  params["filename"] = i_FileName;
  boost::json::value response = SendRequest("load", params);
  return response.at("result").at("success").as_bool();
}

bool GameProcessProxy::SaveFile(const std::string &i_FileName) const
{
  boost::json::object params;
  params["filename"] = i_FileName;
  boost::json::value response = SendRequest("save", params);
  return response.at("result").at("success").as_bool();
}

std::string GameProcessProxy::GetStatusString() const
{
  boost::json::value response = SendRequest("getStatusString", boost::json::object());
  return std::string(response.at("result").at("status").as_string().c_str());
}

std::string GameProcessProxy::GetName() const
{
  return GetServerGameInfo().GetName();
}

bool GameProcessProxy::IsDone() const
{
  boost::json::value response = SendRequest("isDone", boost::json::object());
  return response.at("result").at("done").as_bool();
}

void GameProcessProxy::SendFullState(const std::string &i_Name) const
{
  boost::json::object params;
  params["player"] = i_Name;
  SendRequest("sendFullState", params);
}
