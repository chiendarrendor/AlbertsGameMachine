#ifndef ACTIONPARSERHPP
#define ACTIONPARSERHPP

#include <string>
#include <vector>

class ActionParser
{
public:
	ActionParser(const std::string &i_InString);
	int GetNumArguments() const;
	const std::string &GetActionName() const;
	const std::string &operator[] (size_t i_Index) const;
private:
	std::string m_ActionName;
	std::vector<std::string>m_Arguments;
};

std::ostream &operator<<(std::ostream &o, const ActionParser &i_ap);

#endif
