#include "OutpostGameInfo.hpp"

typedef OutpostStateMachine::StateType::TransitionType TransitionType;

const TransitionType *GetTransition(const OutpostStateMachine &i_osm,
                                    const std::string &i_sourceState,
                                    const std::string &i_transName,
                                    const std::string &i_destState,
                                    size_t i_peercount);

