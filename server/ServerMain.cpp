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
	if (argc != 2)
	{
		std::cout << "Bad Command Line" << std::endl;
		exit(1);
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
	ServerSocket ssocket(4356,gschf);

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


			

