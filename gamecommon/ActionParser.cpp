#include "ActionParser.hpp"
#include "StringUtilities.hpp"
#include <stdexcept>
#include <iostream>

ActionParser::ActionParser(const std::string &i_InString)
{
	Tokenize(m_Arguments,i_InString,",",true);

	if (m_Arguments.size() == 0) return;

	for (size_t i = 0 ; i < m_Arguments.size() ; i++)
	{
		m_Arguments[i] = ReComma(m_Arguments[i]);
	}

	m_ActionName = m_Arguments[0];

	m_Arguments.erase(m_Arguments.begin());
}

const std::string &ActionParser::GetActionName() const 
{
	return m_ActionName;
}

int ActionParser::GetNumArguments() const
{
	return m_Arguments.size();
}

const std::string &ActionParser::operator[] (size_t i_Index) const
{
	if (i_Index < 0 || i_Index >= m_Arguments.size())
	{
		throw std::out_of_range("Out of Range in ActionParser");
	}
	return m_Arguments[i_Index];
}

std::ostream &operator<<(std::ostream &o, const ActionParser &i_ap)
{
	o << "Name: " << i_ap.GetActionName() << std::endl;
	for (int i = 0 ; i < i_ap.GetNumArguments() ; ++i)
	{
		o << "   " << i_ap[i] << std::endl;
	}
	return o;
}

