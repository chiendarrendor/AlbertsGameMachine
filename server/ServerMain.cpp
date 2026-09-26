#include <iostream>
#include <string>
#include <vector>

#include "LogManager.hpp"

#include "GameServerConnectionHandler.hpp"
#include "RoomManager.hpp"
#include "GameCloset.hpp"
#include "FileUtilities.hpp"

#include <fstream>
#include <stdio.h>
#include <boost/lexical_cast.hpp>

const int DEFAULT_PORT = 4356;

class ServerLogger : public Logger
{
public:
	ServerLogger(std::ostream &o) : m_o(o)
	{
	}
	virtual void AddToLog(const std::string &i_LogLine)
	{
		m_o << "Log: " << i_LogLine << std::endl;
	}
private:
	std::ostream &m_o;
};


int main(int argc,char **argv)
{
	if (argc != 2 && argc != 3)
	{
		std::cout << "Usage: gameserver <dataDir> [port]" << std::endl;
		exit(1);
	}

	int port = DEFAULT_PORT;
	if (argc == 3)
	{
		try
		{
			port = boost::lexical_cast<int>(argv[2]);
		}
		catch (boost::bad_lexical_cast &)
		{
			std::cout << "Bad port argument: " << argv[2] << std::endl;
			exit(1);
		}
	}

	std::string logname = argv[1];
	logname += DIR_SEP;
	logname += "ServerLog.txt";

	std::ofstream logstream(logname.c_str());
	if (!logstream)
	{
		std::cerr << "Can't open log...exiting." << std::endl;
		exit(1);
	}

	ServerLogger aorlog(logstream);
	LogManager::SetLogger(aorlog);

	std::string passwd = argv[1];
	passwd += DIR_SEP;
	passwd += "passwd";
	
	LoginManager lmgr(passwd);

	if (!lmgr.IsValid())
	{
		LOGSTREAM("Can't get Password file!");
		exit(1);
	}

  std::string gameFile = argv[1];
  gameFile += DIR_SEP;
  gameFile += "gameconfig.txt";

  GameCloset gamecloset(gameFile,argv[1]);

  std::string mainXMLFile = argv[1];
  mainXMLFile += DIR_SEP;
  mainXMLFile += "mainloc.txt";
  
  std::string loc;
  std::string fname;
  
  std::ifstream xmlloc(mainXMLFile.c_str());
  if (!xmlloc)
  {
    LOGSTREAM("Can't open base XML locator file " << mainXMLFile);
    exit(1);
  }
  std::getline(xmlloc,loc);
  std::getline(xmlloc,fname);
  
	RoomManager rmanager(loc,fname,gamecloset);

	GameServerConnectionHandlerFactory gschf(lmgr,rmanager);
	ServerSocket ssocket(port,gschf);

	try
	{
		ssocket.RunConnections();
	}
	catch (std::exception &stdex)
	{
		std::cout << "Caught STL exception " << stdex.what() << std::endl;
	}

	return 0;
}


			

