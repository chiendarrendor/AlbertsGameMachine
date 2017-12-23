#ifndef STATEWALKERHPP
#define STATEWALKERHPP
#include "Serialize.hpp"

#include "StateMachine.hpp"
#include "ActionParser.hpp"

#include <vector>

enum ExecuteStatus { NO_STATE,TRANSITION_NOT_FOUND, EXECUTE_FAILED, RECURSION_LIMIT, EXECUTE_OK };

template <class T_GameInfo>
class StateWalker
{
public:
  typedef StateMachine<T_GameInfo> StateMachineType;

  StateWalker(const StateMachineType &i_Machine,T_GameInfo &i_rGameInfo,int i_AutoRecursionDepth);
  bool IsValid() const;
  bool IsAtEnd() const;
  std::string GetCurStateName() const;
  std::string GetCurStateDesc() const;

  const typename StateMachineType::StateType *GetCurState() const;

  bool SetCurStateByName(const std::string &i_Name);
  bool GetLegalTransitionNames(const std::string &i_PlayerName,std::vector<std::string> &o_Names) const;

  ExecuteStatus Execute(const std::string &i_PlayerName,const ActionParser &i_ap);

private:
  ExecuteStatus ExecuteAuto(int i_AutoRecursionDepth);

  int m_AutoRecursionDepth;
  T_GameInfo &m_rGameInfo;
  const StateMachineType &m_rMachine;
  const typename StateMachineType::StateType *m_pCurState;

  SERIALIZE_FRIENDSHIP;

  SERIALIZE_LOAD_FUNC
  {
    std::string curstate;
    SERIALIZE(curstate);
    SetCurStateByName(curstate);
  }

  SERIALIZE_SAVE_FUNC
  {
    std::string curstate;
    if (m_pCurState)
    {
      curstate = GetCurStateName();
    }
    SERIALIZE(curstate);
  }
  BOOST_SERIALIZATION_SPLIT_MEMBER()
};

template <class T_GameInfo>
StateWalker<T_GameInfo>::StateWalker(const StateMachineType &i_Machine,T_GameInfo &i_rGameInfo,int i_AutoRecursionDepth) :
  m_AutoRecursionDepth(i_AutoRecursionDepth),
  m_rGameInfo(i_rGameInfo),
  m_rMachine(i_Machine)
{
  m_pCurState = m_rMachine.GetState(INITIALSTATENAME);
}


template <class T_GameInfo>
bool StateWalker<T_GameInfo>::IsValid() const
{
  return m_pCurState;
}

template <class T_GameInfo>
bool StateWalker<T_GameInfo>::IsAtEnd() const
{
  return IsValid() && m_pCurState->GetName() == TERMINALSTATENAME;
}

template <class T_GameInfo>
std::string StateWalker<T_GameInfo>::GetCurStateName() const
{
  if (!IsValid())
  {
    return "";
  }
  return m_pCurState->GetName();
}

template <class T_GameInfo>
const typename StateWalker<T_GameInfo>::StateMachineType::StateType *StateWalker<T_GameInfo>::GetCurState() const
{
  return m_pCurState;
}

template <class T_GameInfo>
bool StateWalker<T_GameInfo>::SetCurStateByName(const std::string &i_Name)
{
  m_pCurState = m_rMachine.GetState(i_Name);
  return IsValid();
}

template <class T_GameInfo>
std::string StateWalker<T_GameInfo>::GetCurStateDesc() const
{
  if (!IsValid())
  {
    return "Unknown State!";
  }
  return m_pCurState->GetDesc();
}

template <class T_GameInfo>
bool StateWalker<T_GameInfo>::GetLegalTransitionNames(const std::string &i_PlayerName,
                                                      std::vector<std::string> &o_Names) const
{
  if (!IsValid())
  {
    return false;
  }

  o_Names.clear();

  const typename State<T_GameInfo>::TransMap &transmap = m_pCurState->GetTransitionMap();
  typename State<T_GameInfo>::TransMap::const_iterator transparit;

  for (transparit = transmap.begin() ; transparit != transmap.end() ; ++transparit)
  {
    const std::string &tname = transparit->first;
    const Transition<T_GameInfo> &trans = *(transparit->second.first);

    if (trans.IsLegal(i_PlayerName,GetCurStateName(),m_rGameInfo))
    {
      o_Names.push_back(tname);
    }
  }
  return true;
}

template <class T_GameInfo>
ExecuteStatus StateWalker<T_GameInfo>::Execute(const std::string &i_PlayerName,const ActionParser &i_ap)
{
#ifdef STATEDEBUG
  std::cout << "Execute: player name: " << i_PlayerName << " action parser: " << i_ap << std::endl;
  std::cout << "Execute:  current state: " << GetCurStateName() << std::endl;
#endif

  if (!IsValid())
  {
#ifdef STATEDEBUG
    std::cout << "Execute: NO STATE" << std::endl;
#endif
    return NO_STATE;
  }

  const typename State<T_GameInfo>::TransMap &transmap = m_pCurState->GetTransitionMap();
  typename State<T_GameInfo>::TransMap::const_iterator transpairit;

  transpairit = transmap.find(i_ap.GetActionName());
  if (transpairit == transmap.end())
  {
#ifdef STATEDEBUG
    std::cout << "Execute: action not found in transitions from this state" << std::endl;
#endif
    return TRANSITION_NOT_FOUND;
  }
  
  const Transition<T_GameInfo> *pTrans = transpairit->second.first;
  const State<T_GameInfo> *pNextState = transpairit->second.second;

  if (!pTrans || !pNextState)
  {
#ifdef STATEDEBUG
    std::cout << "Execute: Missing transition or next state?" << std::endl;
#endif
    return TRANSITION_NOT_FOUND;
  }

  if (!pTrans->IsLegal(i_PlayerName,GetCurStateName(),m_rGameInfo))
  {
#ifdef STATEDEBUG
    std::cout << "Execute: Action not legal" << std::endl;
#endif
    return EXECUTE_FAILED;
  }

  if (!pTrans->ExecuteAction(GetCurStateName(),m_rGameInfo,i_PlayerName,i_ap))
  {
#ifdef STATEDEBUG
    std::cout << "Execute: execute failed" << std::endl;
#endif
    return EXECUTE_FAILED;
  }

  m_pCurState = pNextState;
#ifdef STATEDEBUG
  std::cout << "Execute: new state" << GetCurStateName() << std::endl;

  std::cout << "Exeucte: executing auto check" << std::endl;
#endif
  ExecuteStatus status = ExecuteAuto(m_AutoRecursionDepth);
#ifdef STATEDEBUG
  std::cout << "Execute: result of auto: " << status << std::endl;
#endif

  return status;
}

template <class T_GameInfo>
ExecuteStatus StateWalker<T_GameInfo>::ExecuteAuto(int i_AutoRecursionLimit)
{
#ifdef STATEDEBUG
  std::cout << "ExecuteAuto start" << std::endl;
#endif
  if (!IsValid())
  {
#ifdef STATEDEBUG
    std::cout << "ExecuteAuto: not valid" << std::endl;
#endif
    return NO_STATE;
  }

  const typename State<T_GameInfo>::TransMap &transmap = m_pCurState->GetTransitionMap();
  typename State<T_GameInfo>::TransMap::const_iterator transparit;
  const Transition<T_GameInfo> *pTrans = NULL;

  const Transition<T_GameInfo> *pAutoTrans = NULL;
  const State<T_GameInfo> *pAutoNextState = NULL;

#ifdef STATEDEBUG
  std::cout << "ExecuteAuto: searching for auto transitions" << std::endl;
#endif
  for (transparit = transmap.begin() ; transparit != transmap.end() ; ++transparit)
  {
    pTrans = transparit->second.first;
    if (!pTrans)
    {
#ifdef STATEDEBUG
      std::cout << "ExecuteAuto: transition not found" << std::endl;
#endif
      return TRANSITION_NOT_FOUND;
    }

    if (pTrans->IsAuto(GetCurStateName(),m_rGameInfo))
    {
#ifdef STATEDEBUG
      std::cout << "ExecuteAuto: found an auto:" << pTrans->GetName() << std::endl;
#endif
      // i.e. if we've already found an auto....
      if (pAutoTrans)
      {
#ifdef STATEDEBUG
        std::cout << "ExecuteAuto: found more than one auto!" << std::endl;
#endif
        return EXECUTE_OK;
      }
      pAutoTrans = pTrans;
      pAutoNextState = transparit->second.second;
      if (!pAutoNextState)
      {
#ifdef STATEDEBUG
        std::cout << "ExecuteAuto: transition not found 2" << std::endl;
#endif
        return TRANSITION_NOT_FOUND;
      }
    }
  }

  // this test is true if we have no autos.
  if (!pAutoTrans)
  {
#ifdef STATEDEBUG
    std::cout << "ExecuteAuto: no autos found" << std::endl;
#endif
    return EXECUTE_OK;
  }

  if (i_AutoRecursionLimit <= 0)
  {
#ifdef STATEDEBUG
    std::cout << "ExecuteAuto: hit recursion limit" << std::endl;
#endif
    return RECURSION_LIMIT;
  }

  ActionParser dummyap("");
#ifdef STATEDEBUG
  std::cout << "ExecuteAuto: executing auto transition " << pAutoTrans->GetName() << std::endl;
#endif
  if (!pAutoTrans->ExecuteAction(GetCurStateName(),m_rGameInfo,"",dummyap))
  {
#ifdef STATEDEBUG
    std::cout << "ExecuteAuto: auto transtion failed!" << std::endl;
#endif
    return EXECUTE_FAILED;
  }

  m_pCurState = pAutoNextState;
#ifdef STATEDEBUG
  std::cout << "ExecuteAuto: New state: " << GetCurStateName() << std::endl;

  std::cout << "ExecuteAuto: Executing ExecuteAuto" << std::endl;
#endif
  ExecuteStatus status = ExecuteAuto(i_AutoRecursionLimit - 1);
#ifdef STATEDEBUG
  std::cout << "ExecuteAuto: returned status: " << status << std::endl;
#endif
  return status;
}
    

#endif
