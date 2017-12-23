
#include <map>
#include <string>
#include <vector>
#include "SystemSpecificTCPIP.hpp"

class ServerSocket;

class ConnectionHandler
{
public:
	ConnectionHandler(int i_MySocket,ServerSocket &i_SSock);
  virtual ~ConnectionHandler();

	int GetSocket();

	// deals with the processing of one line that has come in.
	virtual void HandleLine(const std::string & i_Line) = 0;

	// This method will be called when a read or write fails.
	// once this method has been called, ServerSocket will no
	// have a reference to this method, so subclasses should 
	// handle destruction of the object.  This method
  // is called by Shutdown, and does only sub-class cleanup.
  // for full parent and subclass cleanup, call Shutdown()
	virtual void CleanUp() = 0;

	// sends one line out. (adds the CR)
	void SendLine(const std::string &i_Line);

	// this probably won't do what you want it to do if you call it directly.
	void ReadOneBuffer();


  // this method does all necessary parent and subclass cleanup of
  // the ConnectionHandler
	void Shutdown();

private:

	std::string m_stringbuf;
	SOCKET m_MySocket;
	ServerSocket &m_SSock;
};

class ConnectionHandlerFactory
{
public:
	virtual ConnectionHandler &GetConnectionHandler(int i_MySocket,ServerSocket &i_SSock) = 0;
  virtual void Sweep() = 0;
};

class ServerSocket
{
public:
	ServerSocket(int i_Port,ConnectionHandlerFactory &i_ConnFactory);
	virtual ~ServerSocket();

	bool IsListening();

	// closes the listener socket.
	void CloseListener();

  // should be private, I think.
	int ConnectOne();
	
	void UnhandleSocket(ConnectionHandler &i_Socket);

	// this method only returns if all sockets close.  it accepts new connections on the 
	// Listen Socket, and requests a Connection Handler from the Factory
	// for each new connection.  It also listens to all open sockets,
	// gathers the data in the Connection Handlers, and calls the 
	// Connection Handler's HandleLine when a carriage return comes
	// in on that socket.  Errors in reading or writing on the Connection
	// Handler's socket causes the CleanUp method of the ConnectionHandler to be called.
	void RunConnections();

private:
	SOCKET m_ListenSocket;
	bool m_Listening;
	SOCKET m_LargestSocket;

	std::vector<ConnectionHandler *>m_DoomedHandlers;

	std::map<int,ConnectionHandler *>m_ConnectionHandlers;
	ConnectionHandlerFactory &m_ConnFactory;

	static bool s_Initialized;
	static void Initialize();
};
