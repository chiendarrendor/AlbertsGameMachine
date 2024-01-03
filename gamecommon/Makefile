
COMMON=../common
OBJS=ActionParser.o Game.o ServerGameInfo.o

CFILES=$(OBJS:%.o=%.cpp)
INCS=-I${COMMON}

AR=ar
GCC=g++ -g -Wall -Werror -fPIC ${INCS}

libGameCommon.a: ${OBJS}
	${AR} rcs $@ ${OBJS}

%.o:%.cpp
	${GCC} -c $<

depend:
	@ ${GCC} -MM ${CFILES} | tr -d '\015' >> Makefile.depend

clean:
	@- rm -f ${OBJS} libGameCommon.a


# <MAKE DEPEND TARGETS BELOW>
-include "Makefile.depend"