#ifndef STATEHPP
#define STATEHPP

#include "Transition.hpp"
#include <map>

template <class T_GameInfo>
class State
{
public:
  typedef State<T_GameInfo> StateType;
  typedef Transition<T_GameInfo> TransitionType;
  typedef std::pair<const TransitionType *,const StateType*> TransPair;
  typedef std::map<std::string,TransPair> TransMap;

  State(const std::string &i_Name,const std::string &i_Desc);
  const std::string &GetName() const;
  const std::string &GetDesc() const;

  void InsertTransition(const TransitionType &i_Trans,const StateType &i_Destination);  

  const TransMap &GetTransitionMap() const;

private:
  std::string m_Name;
  std::string m_Desc;
  TransMap m_Transitions;
};



template <class T_GameInfo>
State<T_GameInfo>::State(const std::string &i_Name,const std::string &i_Desc) : 
  m_Name(i_Name),
  m_Desc(i_Desc) 
{
}

template <class T_GameInfo>
const std::string &State<T_GameInfo>::GetName() const 
{ 
  return m_Name; 
}

template <class T_GameInfo>
const std::string &State<T_GameInfo>::GetDesc() const 
{ 
  return m_Desc; 
}

template <class T_GameInfo>
void State<T_GameInfo>::InsertTransition(const Transition<T_GameInfo> &i_Trans,
                                               const State<T_GameInfo> &i_Destination)
{
  m_Transitions[i_Trans.GetName()] = TransPair(&i_Trans,&i_Destination);
}

template <class T_GameInfo>
const std::map<std::string,typename State<T_GameInfo>::TransPair> &State<T_GameInfo>::GetTransitionMap() const
{
  return m_Transitions;
}


#endif




