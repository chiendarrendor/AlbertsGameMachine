#include "ServerSocket.hpp"
#include "ActionParser.hpp"
#include "LoginManager.hpp"
#include "ActionManager.hpp"

class GameServerConnectionHandlerFactory;



class GameServerConnectionHandler : public ConnectionHandler
{
public:
	GameServerConnectionHandler(int i_Socket,ServerSocket &i_SSock,
		GameServerConnectionHandlerFactory &i_fac);

  virtual ~GameServerConnectionHandler();
	
	virtual void HandleLine(const std::string &i_Line);

	virtual void CleanUp();

	bool IsNamed();

	const std::string &GetName();

private:
	GameServerConnectionHandlerFactory &m_fac;
	bool m_Named;
	std::string m_Name;
};

class GameServerConnectionHandlerFactory : public ConnectionHandlerFactory
{
public:

	GameServerConnectionHandlerFactory(LoginManager &i_LoginManager,ActionManager &i_rActionManager);

  virtual ~GameServerConnectionHandlerFactory();

	virtual ConnectionHandler &GetConnectionHandler(int i_MySocket,ServerSocket &i_SSock);
	virtual void Sweep();

	void CleanUpConnection(GameServerConnectionHandler &i_Handler);

	void HandleAction(const std::string &i_Name,const ActionParser &i_Action);
	bool ValidateLogin(const std::string &i_Name,const std::string &i_Password);
  GameServerConnectionHandler *GetNamedHandler(const std::string &i_Name);

	void NameMe(GameServerConnectionHandler &i_me,const std::string &i_Name);

	void SendLineToName(const std::string &i_Name,const std::string &i_String);
  
  void InitialConnection(const std::string &i_Name);

private:
	std::map<int,GameServerConnectionHandler *>m_Handlers;
	std::vector<GameServerConnectionHandler *>m_Doomed;
	std::map<std::string,GameServerConnectionHandler *>m_NamedHandlers;
	LoginManager &m_LoginManager;

	ActionManager &m_rActionManager;
};
