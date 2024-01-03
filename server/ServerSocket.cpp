#include "ServerSocket.hpp"

#include <iostream>

#include "LogManager.hpp"

ConnectionHandler::ConnectionHandler(int i_MySocket,ServerSocket &i_SSock) : 
	m_MySocket(i_MySocket),
	m_SSock(i_SSock)
{
}

ConnectionHandler::~ConnectionHandler()
{
}


int ConnectionHandler::GetSocket()
{
	return m_MySocket;
}

void ConnectionHandler::SendLine(const std::string &i_string)
{
	std::string tosend = i_string;
	tosend += "\n";

  //	std::cout << "Sending: (" << tosend << ")" << std::endl;

	if (!send(m_MySocket,tosend.c_str(),tosend.size(),0))
	{
		std::cerr << "Can't send on socket: " << ERRSTRING << std::endl;
		Shutdown();
		return;
	}
}

void ConnectionHandler::ReadOneBuffer()
{
	char buf[256];
  int tcount = 0;

  while(1)
  {
    int numbytes = recv(m_MySocket,buf,1,0);

    if (numbytes < 0)
    {
      std::cerr << "Error Receiving on socket: " << ERRSTRING << std::endl;
      Shutdown();
      return;
    }

    if (numbytes == 0 && tcount == 0)
    {
      std::cerr << "Socket Closing normally" << std::endl;
      Shutdown();
      return;
    }
    
    // if we get here, we processed some bytes, but not enough to 
    // get a whole packet...that's ok.
    if (numbytes == 0)
    {
      return;
    }

    tcount++;
    
    if (buf[0] == '\n')
    {
      HandleLine(m_stringbuf);
      m_stringbuf.erase();
      break;
    }
    else if (buf[0] < ' ') // all characters below ASCII space are non-printing characters... this should eat linefeeds
    {
        // do nothing here.
    }
    else
    {
      m_stringbuf += buf[0];
    }
	}
}

void ConnectionHandler::Shutdown()
{
	CLOSESOCKET(m_MySocket);
	m_SSock.UnhandleSocket(*this);
	CleanUp();
}

bool ServerSocket::s_Initialized(false);

ServerSocket::ServerSocket(int i_Port,ConnectionHandlerFactory &i_ConnFactory) :
  m_Listening(false),
	m_ConnFactory(i_ConnFactory)
{
	Initialize();

	m_ListenSocket = socket(AF_INET,SOCK_STREAM,0);
	if (m_ListenSocket == INVALID_SOCKET)
	{
		std::cerr << "Can't open listen socket: " << ERRSTRING << std::endl;
		exit(1);
	}
	
	m_LargestSocket = m_ListenSocket;

	BOOL bOptVal = TRUE;

	// lose the pesky "address already in use" error message 
	if (setsockopt(m_ListenSocket, SOL_SOCKET, SO_REUSEADDR, 
		(const char *)&bOptVal,sizeof(BOOL)) == SOCKET_ERROR ) { 
		std::cerr << "Can't option listen socket: " << ERRSTRING << std::endl;
		exit(1); 
	} 



	struct sockaddr_in my_addr;
	
	my_addr.sin_family = AF_INET;         // host byte order
	my_addr.sin_port = htons(i_Port);     // short, network byte order
	my_addr.sin_addr.s_addr = INADDR_ANY; 
	memset(&(my_addr.sin_zero), '\0', 8); // zero the rest of the struct 

	if (bind(m_ListenSocket, (struct sockaddr *)&my_addr, sizeof(struct sockaddr))==SOCKET_ERROR)
	{
		std::cerr << "Can't bind listener: " << ERRSTRING << std::endl;
		return;
	}

	if (listen(m_ListenSocket,SOMAXCONN) == SOCKET_ERROR)
	{
		std::cerr << "Can't listen listener: " << ERRSTRING << std::endl;
		return;
	}
	m_Listening = true;
}

void ServerSocket::Initialize()
{
	if (s_Initialized) return;
	s_Initialized = true;

	IPC_INITIALIZE;
}

bool ServerSocket::IsListening()
{
	return m_Listening;
}

void ServerSocket::CloseListener()
{
	CLOSESOCKET(m_ListenSocket);
	m_Listening = false;
}

int ServerSocket::ConnectOne()
{
	if (!IsListening())
	{
		return INVALID_SOCKET;
	}

	struct sockaddr_in their_addr; // connector's address information
	size_t sin_size = sizeof(struct sockaddr_in); 
	
	SOCKET new_fd = ACCEPT(m_ListenSocket, (struct sockaddr *)&their_addr,&sin_size); 
 
	if (new_fd == INVALID_SOCKET)
	{
		CloseListener();
		std::cerr << "Can't accept socket: " << ERRSTRING << std::endl;
	}

	return new_fd;
}

void ServerSocket::RunConnections()
{
	while(1)
	{
		size_t i;
		for (i = 0 ; i < m_DoomedHandlers.size() ; i++)
		{
			ConnectionHandler *pDoomed = m_DoomedHandlers[i];
			std::map<int,ConnectionHandler *>::iterator findit;
			findit = m_ConnectionHandlers.find(pDoomed->GetSocket());
			if (findit == m_ConnectionHandlers.end())
			{
				std::cerr << "This shouldn't happen!" << std::endl;
			}
			if (findit->second == pDoomed)
			{
				m_ConnectionHandlers.erase(pDoomed->GetSocket());
			}
		}
		m_DoomedHandlers.clear();
    m_ConnFactory.Sweep();

		bool anySockets = false;
		fd_set readfds;
		struct timeval tv;
		FD_ZERO(&readfds);

		if (IsListening())
		{
      //			std::cout << "Adding Listen Socket to select" << std::endl;
			FD_SET(m_ListenSocket,&readfds);
			anySockets = true;
		}

		std::map<int,ConnectionHandler *>::iterator conit;
		for (conit = m_ConnectionHandlers.begin() ; conit != m_ConnectionHandlers.end() ; ++conit)
		{
      //			std::cout << "Adding Socket " << conit->first << " to select" << std::endl;
			FD_SET(conit->first,&readfds);
			anySockets = true;
		}

		if (anySockets == false) break;

		tv.tv_sec = 5;
		tv.tv_usec = 0;

		int result = select(m_LargestSocket + 1,&readfds,NULL,NULL,&tv);
		if (result < 0) 
		{
			std::cerr << "Error in Select: " << ERRSTRING << std::endl;
			break;
		}
		if (result == 0)
		{
      //			std::cout << "Select Timed out." << std::endl;
			continue;
		}

    SOCKET sidx;
    for (sidx = 0 ; sidx < m_LargestSocket + 1 ; ++sidx)
    {
      if (!FD_ISSET(sidx,&readfds)) continue;

      if (m_ListenSocket == sidx)
      {
        std::cout << "Listen Heard Something!" << std::endl;
        SOCKET newSock = ConnectOne();
        if (newSock != INVALID_SOCKET) 
        {
          if (newSock > m_LargestSocket) m_LargestSocket = newSock;
          ConnectionHandler &rCH = m_ConnFactory.GetConnectionHandler(newSock,*this);
          m_ConnectionHandlers[newSock] = &rCH;
        }
      }
      else
      {
        std::map<int,ConnectionHandler*>::iterator findit = m_ConnectionHandlers.find(sidx);
        if (findit == m_ConnectionHandlers.end()) continue;

        findit->second->ReadOneBuffer();
      }
      break;
    }
	}
}

void ServerSocket::UnhandleSocket(ConnectionHandler &i_pHandler)
{
	m_DoomedHandlers.push_back(&i_pHandler);
}

ServerSocket::~ServerSocket()
{
  CLOSESOCKET(m_ListenSocket);

	std::map<int,ConnectionHandler *>::iterator conit;
	for (conit = m_ConnectionHandlers.begin() ; conit != m_ConnectionHandlers.end() ; ++conit)
	{
		CLOSESOCKET(conit->first);
	}
}
	
