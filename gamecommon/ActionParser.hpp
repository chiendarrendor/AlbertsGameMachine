#ifndef ACTIONPARSERHPP
#define ACTIONPARSERHPP

#include <string>
#include <iosfwd>
#include <boost/json.hpp>

class ActionParser
{
public:
	// Client-facing: parses a raw JSON line of the form
	// {"namespace":"...", "action":"...", "params":{...}}. Throws
	// boost::system::system_error / boost::json exceptions on malformed input
	// -- callers parsing untrusted socket input must catch around this.
	explicit ActionParser(const std::string &i_RawJsonLine);

	// Game-process-facing: already-decomposed action name + params object, as
	// received over the game<->server wire. No namespace at this layer -- the
	// server has already resolved/validated it before this ever gets built.
	ActionParser(const std::string &i_ActionName,const boost::json::object &i_Params);

	const std::string &GetActionName() const;
	const std::string &GetNamespace() const;
	const boost::json::object &GetParams() const;

private:
	std::string m_Namespace;
	std::string m_ActionName;
	boost::json::object m_Params;
};

std::ostream &operator<<(std::ostream &o, const ActionParser &i_ap);

#endif
