#include "LogManager.hpp"

namespace LogManager
{
	Logger *theLogger;

	void SetLogger(Logger &i_Logger)
	{
		theLogger = &i_Logger;
	}

	Logger &GetLogger()
	{
		return *theLogger;
	}

}

