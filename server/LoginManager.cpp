#include "LoginManager.hpp"

#include "LogManager.hpp"
#include <fstream>
#include <iostream>

LoginManager::LoginManager(const std::string &i_Passfile) : 
m_Passfile(i_Passfile),
m_IsValid(true)
{
	std::ifstream readfile(i_Passfile.c_str());

	if (!readfile)
	{
		LOGSTREAM("Can't open file " << i_Passfile);

		std::ofstream makefile(i_Passfile.c_str());
	
		if (!makefile)
		{
			LOGSTREAM("Couldn't create new password file " << i_Passfile);
			m_IsValid = false;
			return;
		}

		LOGSTREAM("Created New Password file" << i_Passfile);
		return;
	}

	while(1)
	{
		std::string uname;
		if (!std::getline(readfile,uname))
		{
			break;
		}

		std::string pword;
		if (!std::getline(readfile,pword))
		{
			LOGSTREAM("Bad Password file " << i_Passfile);
			m_IsValid = false;
			return;
		}

		m_Logins[uname] = pword;
	}
}

		
bool LoginManager::IsValid()
{
	return m_IsValid;
}

bool LoginManager::ValidateLogin(const std::string &i_Name,const std::string &i_Password)
{
	if (!m_IsValid)
	{
		return false;
	}

	std::map<std::string,std::string>::iterator findit = m_Logins.find(i_Name);

	if (findit != m_Logins.end())
	{
		if (findit->second == i_Password)
		{
			return true;
		}
		return false;
	}

	std::fstream outfile(m_Passfile.c_str(),std::ios::app);

	if (!outfile)
	{
		LOGSTREAM("Can't open " << m_Passfile << " for append.");
		m_IsValid = false;
		return false;
	}

	m_Logins[i_Name] = i_Password;

	outfile << i_Name << std::endl;
	outfile << i_Password << std::endl;

	return true;
}
