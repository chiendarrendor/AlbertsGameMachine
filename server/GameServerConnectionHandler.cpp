#include "GameServerConnectionHandler.hpp"
#include "JsonParamExtraction.hpp"

#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/json.hpp>

GameServerConnectionHandler::GameServerConnectionHandler(int i_Socket,ServerSocket &i_SSock,GameServerConnectionHandlerFactory &i_fac) :
	ConnectionHandler(i_Socket,i_SSock),
	m_fac(i_fac),
	m_Named(false),
	m_Authenticated(false)
{
  SendCurrentLoginState("");
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
	try
	{
		ActionParser ap(i_Line);

		std::cout << "from " << m_Name << ": " << ap;

		if (m_Named)
		{
			m_fac.HandleAction(m_Name,ap);
			return;
		}

		// Pre-login: AUTHENTICATE/LOGOUTOTHER live in their own reserved
		// "login" namespace -- still the same envelope/parser as every other
		// Action, per Albert 2026-09-29 (see
		// .claude/server_game_interface_spec.md's Action-namespacing
		// section), but drive the hand-written LOGIN-gui mini-FSM above
		// instead of a real game's state machine.
		if (ap.GetNamespace() != "login")
		{
			SendCurrentLoginState("Unknown Initialization Packet " + ap.GetActionName());
			return;
		}

		if (ap.GetActionName() == "AUTHENTICATE")
		{
			if (m_Authenticated)
			{
				SendCurrentLoginState("Already authenticated; waiting on LOGOUTOTHER for the existing session.");
				return;
			}

			std::string username = ExtractJsonParam<std::string>(ap.GetParams(),"username");
			std::string password = ExtractJsonParam<std::string>(ap.GetParams(),"password");

			if (!m_fac.ValidateLogin(username,password))
			{
				SendCurrentLoginState("User Name/Password not validated for " + username);
				return;
			}

			m_PendingName = username;
			m_Authenticated = true;

			if (m_fac.GetNamedHandler(m_PendingName) == NULL)
			{
				CompleteLogin();
			}
			else
			{
				SendCurrentLoginState("");
			}
		}
		else if (ap.GetActionName() == "LOGOUTOTHER")
		{
			// Only legal once this connection has proven its own identity --
			// otherwise there's no way to know which session to kick, and
			// allowing it pre-authentication would mean anyone could kick
			// anyone's session for free (Albert, 2026-10-07).
			if (!m_Authenticated)
			{
				SendCurrentLoginState("Must authenticate before logging out another session.");
				return;
			}

			GameServerConnectionHandler *pgsch = m_fac.GetNamedHandler(m_PendingName);
			if (pgsch != NULL)
			{
				pgsch->Shutdown();
			}

			CompleteLogin();
		}
		else
		{
			SendCurrentLoginState("Unknown Initialization Packet " + ap.GetActionName());
		}
	}
	catch (const std::exception &e)
	{
		SendCurrentLoginState(std::string("Malformed action: ") + e.what());
	}
}

void GameServerConnectionHandler::SendLoginState(bool i_Authenticated,int i_OtherConnections,const std::string &i_Error)
{
	boost::json::object state;
	state["connectstring"] = "Welcome to Albert's Game Server, V3.0!";
	state["gui"] = "LOGIN";
	state["authenticated"] = i_Authenticated;
	state["otherconnections"] = i_OtherConnections;
	state["error"] = i_Error.empty() ? boost::json::value() : boost::json::value(i_Error);
	std::string serialized = boost::json::serialize(state);
	// Same spirit as SendLineToName's "Sending to <name>: ..." print below,
	// for messages sent before a connection has a confirmed name at all --
	// the socket is the only identifier available this early.
	std::cout << "Sending to socket " << GetSocket() << ": " << serialized << std::endl;
	SendLine(serialized);
}

void GameServerConnectionHandler::SendCurrentLoginState(const std::string &i_Error)
{
	// Invariant: while still inside this FSM (m_Named == false), being
	// authenticated always means a conflicting session is still pending --
	// resolving that (otherconnections hitting 0) immediately calls
	// CompleteLogin() and leaves the FSM for good, so there's no "already
	// authenticated, no conflict" state left to redisplay here.
	SendLoginState(m_Authenticated,m_Authenticated ? 1 : -1,i_Error);
}

void GameServerConnectionHandler::CompleteLogin()
{
	SendLoginState(true,0,"");

	m_fac.NameMe(*this,m_PendingName);
	m_Named = true;
	m_Name = m_PendingName;
	m_fac.InitialConnection(m_Name);
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
