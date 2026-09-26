#include "JsonLineProtocol.hpp"

// Boost.JSON header-only mode: this include must appear in exactly one
// translation unit across any final link that uses boost::json -- same
// discipline as this project's BOOST_TEST_MAIN/GAMESERVERMAIN single-
// definition conventions. Since this is the only place in the tree that
// uses Boost.JSON so far, it lives here.
#include <boost/json/src.hpp>

#include <string>

boost::optional<boost::json::value> ReadMessage(std::istream &i_Stream)
{
  std::string line;
  if (!std::getline(i_Stream, line))
  {
    return boost::none;
  }
  return boost::json::parse(line);
}

void WriteMessage(std::ostream &o_Stream, const boost::json::value &i_Value)
{
  o_Stream << boost::json::serialize(i_Value) << "\n";
  o_Stream.flush();
}
