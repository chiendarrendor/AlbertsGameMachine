#ifndef STATEMACHINEHPP
#define STATEMACHINEHPP

#include "Transition.hpp"
#include "State.hpp"

#define INITIALSTATENAME "InitialState"
#define TERMINALSTATENAME "TerminalState"

template <class T_GameInfo>
class StateMachine
{
public:
  typedef State<T_GameInfo> StateType;
  typedef Transition<T_GameInfo> TransitionType;

  StateMachine();
  virtual ~StateMachine();

  const StateType *GetState(const std::string &i_Name) const;
  const TransitionType *GetTransition(const std::string &i_Name) const;

protected:
  void InsertState(StateType *i_pState);
  void InsertTransition(TransitionType *i_pTransition);

private:
  std::map<std::string,StateType *>m_States;
  std::map<std::string,TransitionType *>m_Transitions;
};

template <class T_GameInfo>
StateMachine<T_GameInfo>::StateMachine()
{
}


template <class T_GameInfo>
StateMachine<T_GameInfo>::~StateMachine()
{
  typename std::map<std::string,typename StateMachine<T_GameInfo>::StateType *>::iterator stateit;
  for (stateit = m_States.begin() ; stateit != m_States.end(); ++stateit)
  {
    delete stateit->second;
  }

  typename std::map<std::string,typename StateMachine<T_GameInfo>::TransitionType *>::iterator transit;
  for (transit = m_Transitions.begin() ; transit != m_Transitions.end(); ++transit)
  {
    delete transit->second;
  }
}

template <class T_GameInfo>
const typename StateMachine<T_GameInfo>::StateType *StateMachine<T_GameInfo>::GetState(const std::string &i_Name) const
{
  typename std::map<std::string,typename StateMachine<T_GameInfo>::StateType *>::const_iterator findit = 
    m_States.find(i_Name);
  if (findit == m_States.end())
  {
    return NULL;
  }
  return findit->second;
}


template <class T_GameInfo>
const typename StateMachine<T_GameInfo>::TransitionType *StateMachine<T_GameInfo>::GetTransition(const std::string &i_Name) const
{
  typename std::map<std::string,TransitionType *>::const_iterator findit = m_Transitions.find(i_Name);
  if (findit == m_Transitions.end())
  {
    return NULL;
  }
  return findit->second;
}


template <class T_GameInfo>
void StateMachine<T_GameInfo>::InsertState(StateType *i_pState)
{
  m_States[i_pState->GetName()] = i_pState;
}

template <class T_GameInfo>
void StateMachine<T_GameInfo>::InsertTransition(TransitionType *i_pTransition)
{
  m_Transitions[i_pTransition->GetName()] = i_pTransition;
}


#endif
