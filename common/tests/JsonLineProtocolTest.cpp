#include "JsonLineProtocol.hpp"
#include <boost/test/unit_test.hpp>
#include <sstream>

BOOST_AUTO_TEST_CASE( WriteMessageAppendsNewlineAndFlushes )
{
  std::ostringstream oss;
  boost::json::object obj;
  obj["method"] = "handleAction";
  WriteMessage(oss, obj);

  BOOST_CHECK_EQUAL(oss.str(), "{\"method\":\"handleAction\"}\n");
}

BOOST_AUTO_TEST_CASE( ReadMessageParsesOneLine )
{
  std::istringstream iss("{\"id\":1,\"result\":{}}\n");
  boost::optional<boost::json::value> msg = ReadMessage(iss);

  BOOST_REQUIRE(msg);
  BOOST_CHECK_EQUAL(msg->at("id").as_int64(), 1);
  BOOST_CHECK(msg->at("result").as_object().empty());
}

BOOST_AUTO_TEST_CASE( ReadMessageReturnsNoneAtEof )
{
  std::istringstream iss("");
  boost::optional<boost::json::value> msg = ReadMessage(iss);

  BOOST_CHECK(!msg);
}

BOOST_AUTO_TEST_CASE( ReadMessageThrowsOnMalformedJson )
{
  std::istringstream iss("not json at all {{{\n");
  BOOST_CHECK_THROW(ReadMessage(iss), boost::system::system_error);
}

BOOST_AUTO_TEST_CASE( RoundTripPreservesValue )
{
  std::ostringstream oss;
  boost::json::object obj;
  obj["target"] = "alice";
  obj["message"] = "NEWSTATE,InitialState,Initial Game State";
  WriteMessage(oss, obj);

  std::istringstream iss(oss.str());
  boost::optional<boost::json::value> msg = ReadMessage(iss);

  BOOST_REQUIRE(msg);
  BOOST_CHECK(*msg == boost::json::value(obj));
}

BOOST_AUTO_TEST_CASE( ReadMessageReadsOneAtATimeFromMultipleLines )
{
  std::istringstream iss("{\"n\":1}\n{\"n\":2}\n");

  boost::optional<boost::json::value> first = ReadMessage(iss);
  BOOST_REQUIRE(first);
  BOOST_CHECK_EQUAL(first->at("n").as_int64(), 1);

  boost::optional<boost::json::value> second = ReadMessage(iss);
  BOOST_REQUIRE(second);
  BOOST_CHECK_EQUAL(second->at("n").as_int64(), 2);

  boost::optional<boost::json::value> third = ReadMessage(iss);
  BOOST_CHECK(!third);
}
