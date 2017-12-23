#include <string>
#include <map>


class LoginManager
{
public:
	LoginManager(const std::string &i_Passfile);

	bool IsValid();

	bool ValidateLogin(const std::string &i_Name,const std::string &i_Password);
private:
	std::map<std::string,std::string> m_Logins;
	std::string m_Passfile;
	bool m_IsValid;
};


