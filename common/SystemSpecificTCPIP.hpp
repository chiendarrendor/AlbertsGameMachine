/* msys (windows, MinGW, gnu) */
#ifdef msys
#include <winsock.h>
#define ERRSTRING WSAGetLastError()
#define CLOSESOCKET(ARG) closesocket(ARG)
#define IPC_INITIALIZE \
{\
  WSADATA wsaData;\
  if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0)\
  {\
    std::cerr <<"Can't initialize WinSock" << std::endl;\
    exit(1);\
  }\
}
#define ACCEPT(SOCK,ADDR,SIZE) accept(SOCK,ADDR,(int*)SIZE)

/* FreeBSD (Rasperry PI?) */
#elif defined FreeBSD
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <sys/time.h>
#define SOCKET int
#define INVALID_SOCKET -1
#define ERRSTRING strerror(errno)
#define CLOSESOCKET(ARG) close(ARG)
typedef int BOOL;
#define TRUE 1
#define SOCKET_ERROR -1
#define IPC_INITIALIZE
#define ACCEPT(SOCK,ADDR,SIZE) accept(SOCK,ADDR,SIZE)

/* AWS EC2 instance */
#elif defined linux
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#define SOCKET int
#define INVALID_SOCKET -1
typedef int BOOL;
#define ERRSTRING strerror(errno)
#define CLOSESOCKET(ARG) close(ARG)
#define TRUE 1
#define SOCKET_ERROR -1
#define ACCEPT(SOCK,ADDR,SIZE) accept(SOCK,ADDR,(unsigned int*)SIZE)
#define IPC_INITIALIZE


#endif





