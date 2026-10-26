#ifndef ACTIONPARSERHPP
#define ACTIONPARSERHPP

#include <string>
#include <iosfwd>
#include <boost/json.hpp>

class ActionParser
{
public:
	// Client-facing: parses a raw JSON line of the form
	// {"namespace":"...", "action":"...", "type":"...", "params":{...}}.
	// Throws boost::system::system_error / boost::json exceptions on
	// malformed input -- callers parsing untrusted socket input must catch
	// around this.
	explicit ActionParser(const std::string &i_RawJsonLine);

	// Game-process-facing: already-decomposed action name + params object, as
	// received over the game<->server wire. No namespace or type at this
	// layer -- the server has already resolved/validated them (type
	// specifically determines which RPC method, e.g. handleAction vs. a
	// sequence request, GameServerMain.hpp dispatches to, so by the time an
	// ActionParser is built from that RPC's payload the type is already
	// known) before this ever gets built.
	ActionParser(const std::string &i_ActionName,const boost::json::object &i_Params);

	const std::string &GetActionName() const;
	const std::string &GetNamespace() const;
	const std::string &GetType() const;
	const boost::json::object &GetParams() const;

private:
	std::string m_Namespace;
	std::string m_ActionName;
	std::string m_Type;
	boost::json::object m_Params;
};

std::ostream &operator<<(std::ostream &o, const ActionParser &i_ap);

#endif
