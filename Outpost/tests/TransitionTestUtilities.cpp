#include "TransitionTestUtilities.hpp"
#include <boost/test/auto_unit_test.hpp>

const TransitionType *GetTransition(const OutpostStateMachine &i_osm,
                                    const std::string &i_sourceState,
                                    const std::string &i_transName,
                                    const std::string &i_destState,
                                    size_t i_peercount)
{
  const OutpostStateMachine::StateType *pstate = i_osm.GetState(i_sourceState);
  BOOST_REQUIRE(pstate != NULL);
  const OutpostStateMachine::StateType::TransMap &tmap = pstate->GetTransitionMap();
  BOOST_CHECK_EQUAL(tmap.size() , i_peercount);
  OutpostStateMachine::StateType::TransMap::const_iterator cit;
  cit = tmap.find(i_transName);
  BOOST_REQUIRE(cit != tmap.end());

  const std::string &nname = cit->first;
  const OutpostStateMachine::StateType::TransPair & pair = cit->second;
  const TransitionType *ptrans = pair.first;
  const OutpostStateMachine::StateType::StateType *pnext = pair.second;

  BOOST_CHECK_EQUAL(nname , i_transName);
  BOOST_CHECK_EQUAL(ptrans->GetName(),i_transName);
  BOOST_CHECK_EQUAL(pnext->GetName() , i_destState);
  
  return ptrans;
}

