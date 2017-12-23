
#include <string>
#include <sstream>

class Logger
{
public:
	virtual void AddToLog(const std::string &i_LogLine) = 0;
};

namespace LogManager
{
	void SetLogger(Logger &i_Logger);
	Logger &GetLogger();

};

#define LOG(info) LogManager::GetLogger().AddToLog(info);
#define LOGSTREAM(stream)			\
{									\
	std::ostringstream logoss;		\
	logoss << stream;				\
	LOG(logoss.str());				\
}


