#ifndef JSONLINEPROTOCOLHPP
#define JSONLINEPROTOCOLHPP

#include <boost/json.hpp>
#include <boost/optional.hpp>
#include <istream>
#include <ostream>

// One JSON value per line, over any stream -- the shared framing used by
// both sides of the game<->server wire protocol (see
// .claude/server_game_interface_spec.md). Deliberately minimal: no
// message-shape knowledge (request/response/notification), and no error
// handling beyond what boost::json::parse itself throws on malformed input --
// callers decide what a parse failure means.

boost::optional<boost::json::value> ReadMessage(std::istream &i_Stream);
void WriteMessage(std::ostream &o_Stream, const boost::json::value &i_Value);

#endif
