
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
     ServerSocket.o RoomOutputPort.o GameBox.o GameProcessProxy.o

CFILES=$(OBJS:%.o=%.cpp)
INCS=-I${GAMECOMMON} -I${COMMON} -I${BOOSTINC} -DBOOST_TEST_DYN_LINK
GCC=g++ -g -Wall -Werror ${INCS} -D${OSTYPE} -MMD -MP
LOCLIBS=${GAMECOMMON}/libGameCommon.a ${COMMON}/libCommon.a

TESTOBJS=cpp_tests/testmain.o cpp_tests/GameProcessProxyTest.o

LIBS=${OSLIBS} ${LOCLIBS}

gameserver: ${OBJS} ${INTOBJS} ${LOCLIBS}
	${GCC} -o $@ ${OBJS} ${INTOBJS} ${LIBS}

servertest: ${TESTOBJS} GameProcessProxy.o ${LOCLIBS}
	${GCC} -o $@ ${TESTOBJS} GameProcessProxy.o ${LIBS} -lboost_unit_test_framework

${COMMON}/libCommon.a:
	- (cd ${COMMON} ; ${MAKE})
${GAMECOMMON}/libGameCommon.a:
	- (cd ${GAMECOMMON} ; ${MAKE})

%.o:%.cpp
	${GCC} -c $<

cpp_tests/%.o:cpp_tests/%.cpp
	${GCC} -I. -c $< -o $@

clean:
	@- (cd ${COMMON} ; ${MAKE} clean)
	@- (cd ${GAMECOMMON} ; ${MAKE} clean)
	@- rm -f ${OBJS} ${TESTOBJS} gameserver servertest $(OBJS:.o=.d) $(TESTOBJS:.o=.d)

-include $(OBJS:.o=.d)
-include $(TESTOBJS:.o=.d)

