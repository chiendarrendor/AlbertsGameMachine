#ifndef GAMESERVERMAINHPP
#define GAMESERVERMAINHPP

// Generic driver that turns an existing DLLGame-based game into a
// standalone executable speaking the stdio + newline-delimited
// JSON-RPC-ish protocol described in .claude/server_game_interface_spec.md
// -- what server/GameProcessProxy.cpp expects on the other end of the pipe.
//
// Usage (Albert, 2026-09-27), mirroring Boost.Test's "#define, then
// #include one header, get a real main() for free" convention -- the
// entire content of a small, hand-written <Game>ServerMain.cpp living
// alongside <Game>Set.cpp (NOT in tca/, which holds only the Makefile and
// compiled/generated artifacts -- see Outpost/OutpostServerMain.cpp):
//
//   #define GAME_SERVER_NAME() Outpost
//   #define GAME_SERVER_MAIN
//   #include "GameServerMain.hpp"
//
// GAME_SERVER_NAME must be a zero-argument *function-like* macro, not a
// plain object-like one -- its trailing () is what lets the preprocessor
// tell where the name ends and the literal "GameInfo.hpp"/"Set"/
// "StateMachine" suffixes below begin; an object-like macro's expansion
// can't be decomposed that way, and plain token-pasting (##) can't cross a
// literal '.' into a computed #include. <Game>Set/<Game>GameInfo/<Game>
// StateMachine (transitioncompiler's own developer-facing/generated naming
// convention -- see [[component-perl-compiler]]) are derived from
// GAME_SERVER_NAME by pasting below, never separately named.
// GAME_SERVER_AUTORECURSION_DEPTH defaults to 60 (today's common case, per
// Albert) -- #define it yourself, before this #include, only if a game's
// own auto-transition-cascade genuinely needs a deeper bound (MoV needs
// 100 -- see MerchantOfVenus/MerchantOfVenusServerMain.cpp). Only #define
// GAME_SERVER_MAIN in the one translation unit that should get the
// generated main(), same reason BOOST_TEST_MAIN is only ever defined in
// one .cpp.

#ifndef GAME_SERVER_NAME
#error "Define GAME_SERVER_NAME() (e.g. #define GAME_SERVER_NAME() Outpost) before #include-ing GameServerMain.hpp"
#endif

#ifndef GAME_SERVER_AUTORECURSION_DEPTH
#define GAME_SERVER_AUTORECURSION_DEPTH 60
#endif

// Two levels of indirection, in both helpers below, are required: a
// macro's argument is only fully macro-expanded before substitution when
// that macro's own body doesn't apply # or ## directly to it -- so
// GAME_SERVER_STR/GAME_SERVER_PASTE (no # or ## of their own) force
// GAME_SERVER_NAME() to expand to "Outpost" first, before the _2 variants
// (which do apply # / ##) ever see it.
#define GAME_SERVER_STR2(x) #x
#define GAME_SERVER_STR(x) GAME_SERVER_STR2(x)
#define GAME_SERVER_PASTE2(a,b) a##b
#define GAME_SERVER_PASTE(a,b) GAME_SERVER_PASTE2(a,b)

// StringUtilities.hpp must be visible before <Game>GameInfo.hpp is parsed:
// its generated UnCommaStringify() template method calls the non-template
// UnComma(), and GCC's two-phase lookup needs that declaration visible at
// first parse, not just at instantiation -- a pre-existing issue in every
// transitioncompiler-generated GameInfo header (tracked in TODO.md),
// not specific to this file.
#include "StringUtilities.hpp"
#include GAME_SERVER_STR(GAME_SERVER_NAME()GameInfo.hpp)

#define GAME_SERVER_GAMESET GAME_SERVER_PASTE(GAME_SERVER_NAME(),Set)
#define GAME_SERVER_GAMEINFO GAME_SERVER_PASTE(GAME_SERVER_NAME(),GameInfo)
#define GAME_SERVER_STATEMACHINE GAME_SERVER_PASTE(GAME_SERVER_NAME(),StateMachine)

#include "ActionParser.hpp"
#include "DLLGame.hpp"
#include "JsonLineProtocol.hpp"
#include "OutputPort.hpp"
#include "ServerGameInfo.hpp"

#include <boost/json.hpp>
#include <iostream>
#include <set>
#include <string>

// The game-side counterpart to GameProcessProxy: UniCast/BroadCast each
// become one {"method":"event","params":{...}} line written to stdout.
class GameServerMainOutputPort : public OutputPort
{
public:
  explicit GameServerMainOutputPort(std::ostream &o_rStream) : m_rStream(o_rStream) {}

  virtual void UniCast(const std::string &i_Name,const std::string &i_Message) const
  {
    EmitEvent(i_Message,&i_Name);
  }

  virtual void BroadCast(const std::string &i_Message) const
  {
    EmitEvent(i_Message,NULL);
  }

private:
  void EmitEvent(const std::string &i_Message,const std::string *i_pTarget) const
  {
    boost::json::object params;
    params["message"] = i_Message;
    if (i_pTarget)
    {
      params["target"] = *i_pTarget;
    }
    boost::json::object msg;
    msg["method"] = "event";
    msg["params"] = std::move(params);
    WriteMessage(m_rStream,msg);
  }

  std::ostream &m_rStream;
};

template <class T_GameSet,class T_StateMachine,class T_DLLGameInfo>
int RunGameServerMain(int argc,char **argv,int i_AutoRecursionDepth,
                       std::istream &i_rIn,std::ostream &o_rOut)
{
  if (argc != 5)
  {
    std::cerr << "usage: " << argv[0] << " <dataDir> <name> <xmlLoc> <xmlFile>" << std::endl;
    return 1;
  }

  std::string dataDir = argv[1];
  // Name is the front-end/GUI identity from gameconfig.txt (what the client
  // sees via NEWGUI/GAMES) -- distinct from GAME_SERVER_NAME, which is the
  // back-end source-code identity driving which types this executable was
  // compiled from. They're allowed to differ by design (Albert, 2026-09-27)
  // even though they're the same string for Outpost/MoV today, so Name has
  // to come from the server (argv), never be derived from GAME_SERVER_NAME.
  // It's load-bearing, not cosmetic: some generated per-game events (e.g.
  // RESET) embed it directly in their wire payload -- an empty Name here
  // produced a literal "RESET," with nothing after the comma, found live.
  std::string name = argv[2];
  // XMLLoc/XMLFile: nothing game-side is known to consume these (they're
  // resolved entirely at the Room level, server-side) -- passed through
  // anyway so this ServerGameInfo is a faithful reconstruction of the one
  // object server and game used to share before process decoupling split
  // it into two copies, not a partial one built only from what's been
  // discovered necessary so far.
  std::string xmlLoc = argv[3];
  std::string xmlFile = argv[4];
  ServerGameInfo sgi(name,dataDir,xmlLoc,xmlFile);
  GameServerMainOutputPort outputPort(o_rOut);
  T_GameSet gameSet(dataDir);
  T_StateMachine stateMachine;
  DLLGame<T_GameSet,T_DLLGameInfo> game(sgi,outputPort,gameSet,stateMachine,i_AutoRecursionDepth);

  while (true)
  {
    boost::optional<boost::json::value> request = ReadMessage(i_rIn);
    if (!request)
    {
      return 0; // stdin closed -- the server's clean-shutdown signal
    }
    if (!request->is_object())
    {
      std::cerr << "GameServerMain: received a non-object message" << std::endl;
      return 1;
    }

    const boost::json::object &req = request->as_object();
    int id = req.at("id").to_number<int>();
    std::string method(req.at("method").as_string().c_str());
    const boost::json::object &params = req.at("params").as_object();

    boost::json::object result;

    if (method == "handleAction")
    {
      std::string player(params.at("player").as_string().c_str());
      std::string action(params.at("action").as_string().c_str());

      std::set<std::string> roster;
      const boost::json::array &rosterArray = params.at("roster").as_array();
      boost::json::array::const_iterator rosterit;
      for (rosterit = rosterArray.begin() ; rosterit != rosterArray.end() ; ++rosterit)
      {
        roster.insert(std::string(rosterit->as_string().c_str()));
      }

      ActionParser ap(action);
      game.HandleAction(player,ap,roster);
      // DLLGame::HandleAction reports a rejected action as a plain
      // UniCast("ERROR,...") to the sender, not a distinguished failure --
      // it always completes normally from the RPC's point of view. See the
      // "one open question" note in the spec/GameServerMain design
      // discussion: this doesn't match "ERROR is a special case"'s native
      // RPC-error framing literally, but produces identical client-visible
      // behavior, since GameProcessProxy dispatches any interleaved event
      // (error or not) before ever inspecting this response.
    }
    else if (method == "load")
    {
      std::string filename(params.at("filename").as_string().c_str());
      // The wire "filename" already has the game's SaveDir prepended by
      // the server (Game::Load, the non-virtual base wrapper, does that
      // before ever reaching GameProcessProxy::LoadFile) -- call the raw
      // LoadFile override directly, not the base Load(), or SaveDir would
      // be applied twice.
      result["success"] = game.LoadFile(filename);
    }
    else if (method == "save")
    {
      std::string filename(params.at("filename").as_string().c_str());
      result["success"] = game.SaveFile(filename);
    }
    else if (method == "getStatusString")
    {
      result["status"] = game.GetStatusString();
    }
    else if (method == "isDone")
    {
      result["done"] = game.IsDone();
    }
    else if (method == "sendFullState")
    {
      std::string player(params.at("player").as_string().c_str());
      game.SendFullState(player);
    }
    else
    {
      boost::json::object response;
      response["id"] = id;
      response["error"] = "Unknown method: " + method;
      WriteMessage(o_rOut,response);
      continue;
    }

    boost::json::object response;
    response["id"] = id;
    response["result"] = std::move(result);
    WriteMessage(o_rOut,response);
  }
}

#ifdef GAME_SERVER_MAIN
int main(int argc,char **argv)
{
  return RunGameServerMain<GAME_SERVER_GAMESET,GAME_SERVER_STATEMACHINE,GAME_SERVER_GAMEINFO>
    (argc,argv,GAME_SERVER_AUTORECURSION_DEPTH,std::cin,std::cout);
}
#endif

#endif
