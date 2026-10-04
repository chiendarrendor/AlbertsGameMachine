#include <string>
#include <map>


class LoginManager
{
public:
	LoginManager(const std::string &i_Passfile);

	bool IsValid();

	bool ValidateLogin(const std::string &i_Name,const std::string &i_Password);
private:
	// Catches anything that shouldn't be allowed in a username -- not just
	// illegal characters, in case other restrictions get added later. Today:
	// '\n'/'\r' would corrupt m_Passfile's strict alternating-line format
	// (LoginManager's own constructor parses it one getline() per field, with
	// no resync logic); ':' is additionally reserved so a sentinel value
	// (e.g. the future SPECTATOR full-state audience, per .claude/TODO.md)
	// can be constructed that's guaranteed unregistrable as a real username.
	bool IsIllegalUsernameFormat(const std::string &i_Str) const;

	// Kept separate from IsIllegalUsernameFormat even though it enforces the
	// same thing today (passwords land in the same alternating-line file, so
	// '\n'/'\r' are just as corrupting) -- passwords have no sentinel-value
	// concern, so this is free to diverge (e.g. if ':' ever needs to be a
	// legal password character) without that decision also affecting usernames.
	bool IsIllegalPasswordFormat(const std::string &i_Str) const;

	std::map<std::string,std::string> m_Logins;
	std::string m_Passfile;
	bool m_IsValid;
};


