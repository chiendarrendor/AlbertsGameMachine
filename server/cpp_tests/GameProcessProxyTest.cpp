#include "GameProcessProxy.hpp"
#include "ServerGameInfo.hpp"
#include "ActionParser.hpp"
#include "RecordingOutputPort.hpp"

#include <boost/test/auto_unit_test.hpp>
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
  std::string dir = MakeGameDir(
    "[{\"expect\":{\"method\":\"handleAction\",\"params\":{\"action\":\"JOIN\"}},"
    "\"emit\":[{\"target\":\"alice\",\"message\":\"NEWSTATE,Playing,desc\"},"
    "{\"message\":\"TURNORDER,alice\"}],"
    "\"then\":{\"result\":{}}}]");
  ServerGameInfo sgi("TestGame", dir, "https://example.invalid/", "TestGameClient.xml");
  RecordingOutputPort port;
  GameProcessProxy proxy(sgi, port, MOCK_GAME, dir);

  ActionParser join("JOIN");
  std::set<std::string> roster;
  roster.insert("alice");
  proxy.HandleAction("alice", join, roster);

  BOOST_REQUIRE_EQUAL(port.m_UniCasts.size(), 1u);
  BOOST_CHECK_EQUAL(port.m_UniCasts[0].first, "alice");
  BOOST_CHECK_EQUAL(port.m_UniCasts[0].second, "NEWSTATE,Playing,desc");
  BOOST_REQUIRE_EQUAL(port.m_BroadCasts.size(), 1u);
  BOOST_CHECK_EQUAL(port.m_BroadCasts[0], "TURNORDER,alice");
  BOOST_CHECK(!port.m_VariCastCalled);
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
    "\"params\":{\"action\":\"JOIN\",\"roster\":[\"alice\",\"bob\"]}},"
    "\"then\":{\"result\":{}}}]");
  ServerGameInfo sgi("TestGame", dir, "https://example.invalid/", "TestGameClient.xml");
  RecordingOutputPort port;
  GameProcessProxy proxy(sgi, port, MOCK_GAME, dir);

  ActionParser join("JOIN");
  std::set<std::string> roster;
  roster.insert("bob");
  roster.insert("alice");
  BOOST_CHECK_NO_THROW(proxy.HandleAction("alice", join, roster));
}

BOOST_AUTO_TEST_CASE( HandleActionErrorBecomesSenderOnlyUnicast )
{
  std::string dir = MakeGameDir(
    "[{\"expect\":{\"method\":\"handleAction\",\"params\":{\"action\":\"MOVE,99\"}},"
    "\"then\":{\"error\":\"That is not a legal move.\"}}]");
  ServerGameInfo sgi("TestGame", dir, "https://example.invalid/", "TestGameClient.xml");
  RecordingOutputPort port;
  GameProcessProxy proxy(sgi, port, MOCK_GAME, dir);

  ActionParser move("MOVE,99");
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
    "\"emit\":[{\"target\":\"bob\",\"message\":\"NEWSTATE,Playing,desc\"}],"
    "\"then\":{\"result\":{}}}]");
  ServerGameInfo sgi("TestGame", dir, "https://example.invalid/", "TestGameClient.xml");
  RecordingOutputPort port;
  GameProcessProxy proxy(sgi, port, MOCK_GAME, dir);

  proxy.SendFullState("bob");

  BOOST_REQUIRE_EQUAL(port.m_UniCasts.size(), 1u);
  BOOST_CHECK_EQUAL(port.m_UniCasts[0].first, "bob");
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
    "[{\"expect\":{\"method\":\"handleAction\",\"params\":{\"action\":\"JOIN\"}},"
    "\"then\":{\"crash\":true}}]");
  ServerGameInfo sgi("TestGame", dir, "https://example.invalid/", "TestGameClient.xml");
  RecordingOutputPort port;
  GameProcessProxy proxy(sgi, port, MOCK_GAME, dir);

  ActionParser join("JOIN");
  std::set<std::string> roster;
  roster.insert("alice");
  BOOST_CHECK_NO_THROW(proxy.HandleAction("alice", join, roster));

  BOOST_REQUIRE_EQUAL(port.m_BroadCasts.size(), 1u);
  BOOST_CHECK_EQUAL(port.m_BroadCasts[0].find("ERROR,"), 0u);
  BOOST_CHECK_EQUAL(proxy.IsDone(), true);
}
