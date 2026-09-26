#ifndef GAMEPROCESSPROXYHPP
#define GAMEPROCESSPROXYHPP

#include "Game.hpp"
#include <boost/json.hpp>
#include <boost/optional.hpp>
#include <ext/stdio_filebuf.h>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>
#include <sys/types.h>

// Replaces DLLGame at the server layer: a Game implementation that proxies a
// real, separate game process instead of running game logic in-process.
// Spawns i_LaunchCommand (a single executable path, invoked as
// "<launchCommand> <dataDir>", relying on the OS/shebang to run it) and
// speaks the stdio + newline-delimited JSON-RPC-ish protocol described in
// .claude/server_game_interface_spec.md over its stdin/stdout -- see that
// spec for the full message catalog this implements. No handshake: the
// child is assumed ready to receive requests immediately after spawn.
class GameProcessProxy : public Game
{
public:
  // Thrown when the child's pipe closes unexpectedly (crash) or it sends
  // something the protocol doesn't recognize. Not caught here -- deciding
  // what to do about a dead/misbehaving game process (respawn + reload an
  // autosave, etc.) is separate, later work.
  class ProtocolError : public std::runtime_error
  {
  public:
    explicit ProtocolError(const std::string &i_What) : std::runtime_error(i_What) {}
  };

  GameProcessProxy(const ServerGameInfo &i_rServerGameInfo,
                    OutputPort &i_rOutputPort,
                    const std::string &i_LaunchCommand,
                    const std::string &i_DataDir);
  virtual ~GameProcessProxy();

  virtual std::string GetStatusString() const;
  virtual std::string GetName() const;
  virtual bool IsDone() const;
  virtual void SendFullState(const std::string &i_Name) const;
  virtual void HandleAction(const std::string &i_Name,const ActionParser &i_ap,
                             const std::set<std::string> &i_Roster);

  virtual bool LoadFile(const std::string &i_FileName);
  virtual bool SaveFile(const std::string &i_FileName) const;

private:
  // Sends {"id":<n>,"method":i_Method,"params":i_Params}, then loops reading
  // messages -- dispatching any "event" notifications immediately via
  // OutputPort -- until the matching id'd response arrives, which it
  // returns whole (caller inspects "result" vs. "error").
  boost::json::value SendRequest(const std::string &i_Method,
                                  boost::json::object i_Params) const;
  void DispatchEvent(const boost::json::object &i_Params) const;

  // Wraps SendRequest: any ProtocolError (dead pipe, malformed message,
  // mismatched id -- i.e. the game process violated the protocol) is
  // treated as fatal for this game instance. GameProcessProxy is the final
  // arbiter of protocol correctness -- the game is killed, everyone in the
  // room is told, and boost::none is returned so the caller can fall back
  // to a safe default instead of letting the exception reach RoomManager
  // (which has no per-room recovery and would otherwise take the whole
  // server down -- see .claude/server_game_interface_spec.md).
  boost::optional<boost::json::value> SafeSendRequest(const std::string &i_Method,
                                                        boost::json::object i_Params) const;

  // Idempotent: broadcasts one ERROR line to the room, kills and reaps the
  // child if still alive, and marks this instance done for good.
  void Terminate(const std::string &i_Reason) const;

  mutable pid_t m_ChildPid;
  mutable int m_NextId;
  mutable bool m_Terminated;

  // unique_ptr's own constness doesn't propagate to the pointed-to stream,
  // so these don't need to be mutable even though several Game methods that
  // use them (SendFullState, IsDone, GetStatusString, SaveFile) are const.
  std::unique_ptr<__gnu_cxx::stdio_filebuf<char> > m_pToChildBuf;
  std::unique_ptr<__gnu_cxx::stdio_filebuf<char> > m_pFromChildBuf;
  std::unique_ptr<std::ostream> m_pToChild;
  std::unique_ptr<std::istream> m_pFromChild;
};

#endif
