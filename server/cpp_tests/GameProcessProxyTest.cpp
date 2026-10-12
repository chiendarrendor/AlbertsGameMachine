#include "GameProcessProxy.hpp"
#include "ServerGameInfo.hpp"
#include "ActionParser.hpp"
#include "RecordingOutputPort.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/json.hpp>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <set>
#include <string>

namespace
{
  // Creates a fresh scratch directory (ServerGameInfo needs to create/own
  // one anyway) with the given script.json content, and returns its path.
  // The mock reads <returned path>/script.json at spawn.
  std::string MakeGameDir(const std::string &i_ScriptJson)
  {
    char tmpl[] = "/tmp/gppXXXXXX";
    char *dir = mkdtemp(tmpl);
    BOOST_REQUIRE(dir != NULL);

    std::string path(dir);
    std::ofstream script((path + "/script.json").c_str());
    script << i_ScriptJson;
    return path;
  }

  const char *const MOCK_GAME =
    "/mnt/gamemachine/AlbertsGameMachine/server/tests/mock_game.py";
}

BOOST_AUTO_TEST_CASE( GetNameNeverTouchesTheWire )
{
  std::string dir = MakeGameDir("[]"); // empty script -- any wire traffic fails the mock
  ServerGameInfo sgi("TestGame", dir, "https://example.invalid/", "TestGameClient.xml");
  RecordingOutputPort port;
  GameProcessProxy proxy(sgi, port, MOCK_GAME, dir);

  BOOST_CHECK_EQUAL(proxy.GetName(), "TestGame");
}

BOOST_AUTO_TEST_CASE( HandleActionEmitsEventsThenSucceeds )
{
  // The targeted event is a full-state envelope now (a native JSON object,
  // not a legacy comma-line string) -- every targeted event is, post
  // Phase 3. The untargeted one is still legacy plain text (e.g.
  // BroadcastERROR), which stays a string.
  std::string dir = MakeGameDir(
    "[{\"expect\":{\"method\":\"handleAction\",\"params\":{\"action\":\"JOIN\",\"params\":{}}},"
    "\"emit\":[{\"target\":\"alice\",\"message\":{\"state\":\"Playing\",\"legalActions\":[],\"body\":{}}},"
    "{\"message\":\"TURNORDER,alice\"}],"
    "\"then\":{\"result\":{}}}]");
  ServerGameInfo sgi("TestGame", dir, "https://example.invalid/", "TestGameClient.xml");
  RecordingOutputPort port;
  GameProcessProxy proxy(sgi, port, MOCK_GAME, dir);

  ActionParser join("JOIN", boost::json::object());
  std::set<std::string> roster;
  roster.insert("alice");
  proxy.HandleAction("alice", join, roster);

  BOOST_REQUIRE_EQUAL(port.m_UniCasts.size(), 1u);
  BOOST_CHECK_EQUAL(port.m_UniCasts[0].first, "alice");

  // UniCastFullState (layers 3+4, added here at the server's own dispatch
  // point) should have stamped recipient/gui/XMLLoc/XMLFile from this
  // GameProcessProxy's own ServerGameInfo, and nested the game process's
  // pure envelope unchanged under "fullState".
  boost::json::value wrapped = boost::json::parse(port.m_UniCasts[0].second);
  const boost::json::object &outer = wrapped.as_object();
  BOOST_CHECK_EQUAL(outer.at("recipient").as_string(), "alice");
  BOOST_CHECK_EQUAL(outer.at("gui").as_string(), "TestGame");
  BOOST_CHECK_EQUAL(outer.at("XMLLoc").as_string(), "https://example.invalid/");
  BOOST_CHECK_EQUAL(outer.at("XMLFile").as_string(), "TestGameClient.xml");
  BOOST_CHECK_EQUAL(outer.at("fullState").as_object().at("state").as_string(), "Playing");

  BOOST_REQUIRE_EQUAL(port.m_BroadCasts.size(), 1u);
  BOOST_CHECK_EQUAL(port.m_BroadCasts[0], "TURNORDER,alice");
}

BOOST_AUTO_TEST_CASE( HandleActionForwardsRosterOverTheWire )
{
  // The room roster (every current room occupant, not just recognized game
  // players) travels with handleAction so the game process can evaluate a
  // transition's <allowed> condition against names it never learned any
  // other way -- see .claude/server_game_interface_spec.md's VariCast
  // replacement design. Room::m_Inhabitants is a std::set, so the wire
  // array always comes out sorted regardless of insertion order.
  std::string dir = MakeGameDir(
    "[{\"expect\":{\"method\":\"handleAction\","
    "\"params\":{\"action\":\"JOIN\",\"params\":{},\"roster\":[\"alice\",\"bob\"]}},"
    "\"then\":{\"result\":{}}}]");
  ServerGameInfo sgi("TestGame", dir, "https://example.invalid/", "TestGameClient.xml");
  RecordingOutputPort port;
  GameProcessProxy proxy(sgi, port, MOCK_GAME, dir);

  ActionParser join("JOIN", boost::json::object());
  std::set<std::string> roster;
  roster.insert("bob");
  roster.insert("alice");
  BOOST_CHECK_NO_THROW(proxy.HandleAction("alice", join, roster));
}

BOOST_AUTO_TEST_CASE( HandleActionErrorBecomesSenderOnlyUnicast )
{
  std::string dir = MakeGameDir(
    "[{\"expect\":{\"method\":\"handleAction\",\"params\":{\"action\":\"MOVE\",\"params\":{\"space\":99}}},"
    "\"then\":{\"error\":\"That is not a legal move.\"}}]");
  ServerGameInfo sgi("TestGame", dir, "https://example.invalid/", "TestGameClient.xml");
  RecordingOutputPort port;
  GameProcessProxy proxy(sgi, port, MOCK_GAME, dir);

  boost::json::object moveParams;
  moveParams["space"] = 99;
  ActionParser move("MOVE", moveParams);
  std::set<std::string> roster;
  roster.insert("alice");
  proxy.HandleAction("alice", move, roster);

  BOOST_REQUIRE_EQUAL(port.m_UniCasts.size(), 1u);
  BOOST_CHECK_EQUAL(port.m_UniCasts[0].first, "alice");
  BOOST_CHECK_EQUAL(port.m_UniCasts[0].second, "ERROR,That is not a legal move.");
  BOOST_CHECK_EQUAL(port.m_BroadCasts.size(), 0u);
}

BOOST_AUTO_TEST_CASE( SendFullStateEmitsToRequestedPlayer )
{
  std::string dir = MakeGameDir(
    "[{\"expect\":{\"method\":\"sendFullState\",\"params\":{\"player\":\"bob\"}},"
    "\"emit\":[{\"target\":\"bob\",\"message\":{\"state\":\"Playing\",\"legalActions\":[],\"body\":{}}}],"
    "\"then\":{\"result\":{}}}]");
  ServerGameInfo sgi("TestGame", dir, "https://example.invalid/", "TestGameClient.xml");
  RecordingOutputPort port;
  GameProcessProxy proxy(sgi, port, MOCK_GAME, dir);

  proxy.SendFullState("bob");

  BOOST_REQUIRE_EQUAL(port.m_UniCasts.size(), 1u);
  BOOST_CHECK_EQUAL(port.m_UniCasts[0].first, "bob");

  boost::json::value wrapped = boost::json::parse(port.m_UniCasts[0].second);
  const boost::json::object &outer = wrapped.as_object();
  BOOST_CHECK_EQUAL(outer.at("recipient").as_string(), "bob");
  BOOST_CHECK_EQUAL(outer.at("gui").as_string(), "TestGame");
  BOOST_CHECK_EQUAL(outer.at("XMLLoc").as_string(), "https://example.invalid/");
  BOOST_CHECK_EQUAL(outer.at("XMLFile").as_string(), "TestGameClient.xml");
  BOOST_CHECK_EQUAL(outer.at("fullState").as_object().at("state").as_string(), "Playing");
}

BOOST_AUTO_TEST_CASE( LoadAndSaveReturnSuccessFlag )
{
  std::string dir = MakeGameDir(
    "[{\"expect\":{\"method\":\"save\"},\"then\":{\"result\":{\"success\":true}}},"
    "{\"expect\":{\"method\":\"load\"},\"then\":{\"result\":{\"success\":false}}}]");
  ServerGameInfo sgi("TestGame", dir, "https://example.invalid/", "TestGameClient.xml");
  RecordingOutputPort port;
  GameProcessProxy proxy(sgi, port, MOCK_GAME, dir);

  BOOST_CHECK_EQUAL(proxy.Save("savefile.xml"), true);
  BOOST_CHECK_EQUAL(proxy.Load("savefile.xml"), false);
}

BOOST_AUTO_TEST_CASE( StatusStringAndIsDoneReflectMockedValues )
{
  std::string dir = MakeGameDir(
    "[{\"expect\":{\"method\":\"getStatusString\"},"
    "\"then\":{\"result\":{\"status\":\"Complete.\"}}},"
    "{\"expect\":{\"method\":\"isDone\"},\"then\":{\"result\":{\"done\":true}}}]");
  ServerGameInfo sgi("TestGame", dir, "https://example.invalid/", "TestGameClient.xml");
  RecordingOutputPort port;
  GameProcessProxy proxy(sgi, port, MOCK_GAME, dir);

  BOOST_CHECK_EQUAL(proxy.GetStatusString(), "Complete.");
  BOOST_CHECK_EQUAL(proxy.IsDone(), true);
}

BOOST_AUTO_TEST_CASE( CrashMidRoundTripTerminatesGameAndNotifiesRoom )
{
  // GameProcessProxy is the final arbiter of protocol correctness: a dead or
  // misbehaving child must never let a ProtocolError escape to RoomManager
  // (which has no per-room recovery and would take the whole server down).
  // Instead it broadcasts one ERROR line to the room and marks itself done.
  std::string dir = MakeGameDir(
    "[{\"expect\":{\"method\":\"handleAction\",\"params\":{\"action\":\"JOIN\",\"params\":{}}},"
    "\"then\":{\"crash\":true}}]");
  ServerGameInfo sgi("TestGame", dir, "https://example.invalid/", "TestGameClient.xml");
  RecordingOutputPort port;
  GameProcessProxy proxy(sgi, port, MOCK_GAME, dir);

  ActionParser join("JOIN", boost::json::object());
  std::set<std::string> roster;
  roster.insert("alice");
  BOOST_CHECK_NO_THROW(proxy.HandleAction("alice", join, roster));

  BOOST_REQUIRE_EQUAL(port.m_BroadCasts.size(), 1u);
  BOOST_CHECK_EQUAL(port.m_BroadCasts[0].find("ERROR,"), 0u);
  BOOST_CHECK_EQUAL(proxy.IsDone(), true);
}
