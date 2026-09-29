#include "OutpostGameInfo.hpp"
#include "ActionParser.hpp"
#include <boost/json.hpp>
#include <initializer_list>
#include <utility>

typedef OutpostStateMachine::StateType::TransitionType TransitionType;

const TransitionType *GetTransition(const OutpostStateMachine &i_osm,
                                    const std::string &i_sourceState,
                                    const std::string &i_transName,
                                    const std::string &i_destState,
                                    size_t i_peercount);

// Test-only helper: builds an ActionParser the way the game process actually
// receives one post-JSON-redesign (name + a typed, named params object),
// instead of the old comma-line format these tests used to hand-construct
// directly. See .claude/server_game_interface_spec.md's Action-namespacing
// section.
ActionParser MakeAction(const std::string &i_ActionName,
                        std::initializer_list<std::pair<std::string,boost::json::value> > i_Params = {});

