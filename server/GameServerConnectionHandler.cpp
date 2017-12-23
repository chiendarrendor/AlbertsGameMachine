#include "GameServerConnectionHandler.hpp"

#include <iostream>
#include <boost/lexical_cast.hpp>

const char *ALREADYLOGGEDIN = "This User Name is already Logged in!";


GameServerConnectionHandler::GameServerConnectionHandler(int i_Socket,ServerSocket &i_SSock,GameServerConnectionHandlerFactory &i_fac) : 
	ConnectionHandler(i_Socket,i_SSock),
	m_fac(i_fac),
	m_Named(false)
{
  SendLine("CONNECTSTRING,Welcome to Albert's Game Server, V1.0!");
}

GameServerConnectionHandler::~GameServerConnectionHandler()
{
}

bool GameServerConnectionHandler::IsNamed()
{
	return m_Named;
}

const std::string &GameServerConnectionHandler::GetName()
{
  return m_Name;
}

void GameServerConnectionHandler::HandleLine(const std::string &i_Line)
{
	ActionParser ap(i_Line);

  std::cout << "In HandleLine: " << std::endl;
  std::cout << ap << std::endl;

	
	if (m_Named)
	{
		m_fac.HandleAction(m_Name,ap);
	}
	else if (ap.GetActionName() == "LOGIN")
  {
    if (ap.GetNumArguments() != 2)
    {
			SendLine("ERROR,Illegal LOGIN Packet");
			return;
		}

    if (!m_fac.ValidateLogin(ap[0],ap[1]))
    {
      SendLine("ERROR,Can't login with name " + ap[0] + ": User Name/Password not validated");
      return;
    }

    if (m_fac.GetNamedHandler(ap[0]) != NULL)
    {
      SendLine("ALREADYLOGGEDIN");
      return;
    }

    m_fac.NameMe(*this,ap[0]);
    SendLine("WELCOME");
    m_Named = true;
    m_Name = ap[0];
    m_fac.InitialConnection(m_Name);
  }
  else if (ap.GetActionName() == "LOGOUTOTHER")
  {
    if (ap.GetNumArguments() != 2)
    {
			SendLine("ERROR,Illegal LOGOUTOTHER Packet");
			return;
		}

    if (!m_fac.ValidateLogin(ap[0],ap[1]))
    {
      SendLine("ERROR,Can't logout name " + ap[0] + ": User Name/Password not validated");
      return;
    }

    GameServerConnectionHandler *pgsch = m_fac.GetNamedHandler(ap[0]);
    if (pgsch == NULL)
    {
      return;
    }
    pgsch->Shutdown();
  }
  else
  {
    SendLine("ERROR,Unknown Initialization Packet " + ap.GetActionName());
  }
}

void GameServerConnectionHandler::CleanUp()
{
	std::cout << "Cleaning up Socket" << std::endl;
	m_fac.CleanUpConnection(*this);
}

GameServerConnectionHandlerFactory::GameServerConnectionHandlerFactory(LoginManager &i_LoginManager,ActionManager &i_rActionManager) :
m_LoginManager(i_LoginManager),m_rActionManager(i_rActionManager)
{
}

GameServerConnectionHandlerFactory::~GameServerConnectionHandlerFactory()
{
}


void GameServerConnectionHandlerFactory::CleanUpConnection(GameServerConnectionHandler &i_Handler)
{
	m_Doomed.push_back(&i_Handler);
  if (i_Handler.IsNamed())
  {
    m_rActionManager.HandleDisconnect(i_Handler.GetName(),*this);
  }
}

void GameServerConnectionHandlerFactory::Sweep()
{
	size_t i;
	for (i = 0 ; i < m_Doomed.size() ; ++i)
	{
		GameServerConnectionHandler *pHandler = m_Doomed[i];
		std::map<int,GameServerConnectionHandler *>::iterator findit;
		findit = m_Handlers.find(pHandler->GetSocket());

		if (findit == m_Handlers.end())
		{
			std::cerr << "This shouldn't happen! (In Sweep())" << std::endl;
			delete pHandler;
			continue;
		}
		if (findit->second == pHandler)
		{
			m_Handlers.erase(pHandler->GetSocket());
		}

		if (pHandler->IsNamed())
		{
			std::map<std::string,GameServerConnectionHandler *>::iterator sfindit;
			sfindit = m_NamedHandlers.find(pHandler->GetName());
			if (sfindit == m_NamedHandlers.end())
			{
				std::cerr << "This shouldn't happen either! (In Sweep())" << std::endl;
				delete pHandler;
				continue;
			}
			if (sfindit->second == pHandler)
			{
				m_NamedHandlers.erase(pHandler->GetName());
			}
		}

		delete pHandler;
	}

	m_Doomed.clear();

}

ConnectionHandler &GameServerConnectionHandlerFactory::GetConnectionHandler(int i_MySocket,ServerSocket &i_SSock)
{
	GameServerConnectionHandler *pHandler = 
		new GameServerConnectionHandler(i_MySocket,i_SSock,*this);

	m_Handlers[i_MySocket] = pHandler;

	return *pHandler;
}


void GameServerConnectionHandlerFactory::NameMe(GameServerConnectionHandler &i_handler,const std::string &i_Name)
{
	m_NamedHandlers[i_Name] = &i_handler;
}

void GameServerConnectionHandlerFactory::HandleAction(const std::string &i_Name,const ActionParser &i_ap)
{
  //	std::cout << "Factory Handling action from connection " << i_Name << std::endl;

	m_rActionManager.HandleAction(i_Name,i_ap,*this);
}

bool GameServerConnectionHandlerFactory::ValidateLogin(const std::string &i_Name,const std::string &i_Password)
{
  return m_LoginManager.ValidateLogin(i_Name,i_Password);
}

GameServerConnectionHandler *GameServerConnectionHandlerFactory::GetNamedHandler(const std::string &i_Name)
{
  std::map<std::string,GameServerConnectionHandler *>::const_iterator findit = m_NamedHandlers.find(i_Name);
  if (findit == m_NamedHandlers.end())
  {
    return NULL;
  }
  return findit->second;
}

void GameServerConnectionHandlerFactory::SendLineToName(const std::string &i_Name,const std::string &i_String)
{
	std::map<std::string,GameServerConnectionHandler *>::iterator findit = m_NamedHandlers.find(i_Name);

	if (findit == m_NamedHandlers.end())
	{	
		return;
	}
  std::cout << "Sending to " << i_Name << ": " << i_String << std::endl;
	findit->second->SendLine(i_String);
}

void GameServerConnectionHandlerFactory::InitialConnection(const std::string &i_Name)
{
  m_rActionManager.HandleInitialConnection(i_Name,*this);
}
