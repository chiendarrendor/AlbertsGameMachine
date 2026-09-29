#include "ActionParser.hpp"
#include <ostream>

ActionParser::ActionParser(const std::string &i_RawJsonLine)
{
	boost::json::value parsed = boost::json::parse(i_RawJsonLine);
	const boost::json::object &obj = parsed.as_object();

	m_Namespace = std::string(obj.at("namespace").as_string().c_str());
	m_ActionName = std::string(obj.at("action").as_string().c_str());
	m_Params = obj.at("params").as_object();
}

ActionParser::ActionParser(const std::string &i_ActionName,const boost::json::object &i_Params) :
	m_ActionName(i_ActionName),
	m_Params(i_Params)
{
}

const std::string &ActionParser::GetActionName() const
{
	return m_ActionName;
}

const std::string &ActionParser::GetNamespace() const
{
	return m_Namespace;
}

const boost::json::object &ActionParser::GetParams() const
{
	return m_Params;
}

std::ostream &operator<<(std::ostream &o, const ActionParser &i_ap)
{
	o << i_ap.GetNamespace() << "." << i_ap.GetActionName() << " " << boost::json::serialize(i_ap.GetParams()) << std::endl;
	return o;
}
