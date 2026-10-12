#ifndef GAMEACTIONERRORHPP
#define GAMEACTIONERRORHPP

#include <stdexcept>
#include <string>

// Thrown from an ExecuteAction (generated argument validation, or hand-written
// <body> CDATA) to reject an action with a specific message. Caught once, at
// DLLGame::HandleAction, and turned into the full-state envelope's "error"
// field for the acting player -- see .claude/server_game_interface_spec.md.
// A dedicated type rather than std::runtime_error directly, so the catch site
// can't accidentally swallow an unrelated exception.
class GameActionError : public std::runtime_error
{
public:
  explicit GameActionError(const std::string &i_Message) : std::runtime_error(i_Message) {}
};

#endif
