
ifeq (${OSTYPE},msys)
OSLIBS=-lwsock32
BOOSTINC=/c/Boost/include/boost-1_46
endif

ifeq (${OSTYPE},linux)
BOOSTINC=/usr/include
OSLIBS=-ldl
endif


COMMON=../common
GAMECOMMON=../gamecommon

OBJS=ServerMain.o GameServerConnectionHandler.o \
     LogManager.o LoginManager.o RoomManager.o GameCloset.o \
     ServerSocket.o RoomOutputPort.o GameBox.o

CFILES=$(OBJS:%.o=%.cpp) 
INCS=-I${GAMECOMMON} -I${COMMON} -I${BOOSTINC}
GCC=g++ -g -Wall -Werror ${INCS} -D${OSTYPE}
LOCLIBS=${GAMECOMMON}/libGameCommon.a ${COMMON}/libCommon.a



LIBS=${OSLIBS} ${LOCLIBS}

gameserver: ${OBJS} ${INTOBJS} ${LOCLIBS}
	${GCC} -o $@ ${OBJS} ${INTOBJS} ${LIBS} 

${COMMON}/libCommon.a:
	- (cd ${COMMON} ; ${MAKE})
${GAMECOMMON}/libGameCommon.a:
	- (cd ${GAMECOMMON} ; ${MAKE})

%.o:%.cpp
	${GCC} -c $<

depend:
	@ ${GCC} -MM ${CFILES} | tr -d '\015' >> Makefile.depend

clean:
	@- (cd ${COMMON} ; ${MAKE} clean)
	@- (cd ${GAMECOMMON} ; ${MAKE} clean)
	@- rm -f ${OBJS} gameserver


# <MAKE DEPEND TARGETS BELOW>
-include "Makefile.depend"

