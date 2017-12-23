#ifndef TRANSITIONHPP
#define TRANSITIONHPP

#include "GameInfo.hpp"

#include <string>

class ActionParser;

typedef std::string StateName;

template <class T_GameInfo>
class Transition
{
public:
	virtual bool IsLegal(const std::string &i_PlayerName,
                       const StateName &i_StateName,
                       const T_GameInfo &) const = 0;
	virtual bool IsAuto(const StateName &i_StateName,
                      const T_GameInfo &) const = 0;
	virtual bool ExecuteAction(const StateName &i_StateName,
                             T_GameInfo &,
                             const std::string &i_PlayerName,
                             const ActionParser &) const = 0;
	virtual std::string GetName() const = 0;

};

#endif
