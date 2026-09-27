
BOOSTINC?=/usr/include

OBJS=FileUtilities.o StringUtilities.o JsonLineProtocol.o

CFILES=$(OBJS:%.o=%.cpp)
INCS=-DBOOST_TEST_DYN_LINK -I${BOOSTINC}
AR=ar
GCC=g++ -g -Wall -Werror -fPIC ${INCS} -D${OSTYPE}

TESTOBJS=tests/testmain.o tests/JsonLineProtocolTest.o

libCommon.a: ${OBJS}
	${AR} rcs $@ ${OBJS}

commontest: ${TESTOBJS} ${OBJS}
	${GCC} -o $@ ${TESTOBJS} ${OBJS} -lboost_unit_test_framework

%.o:%.cpp
	${GCC} -c $<

tests/%.o:tests/%.cpp
	${GCC} -I. -c $< -o $@

depend:
	${GCC} -MM ${CFILES} | tr -d '\015' > Makefile.depend

clean:
	@- rm -f ${OBJS} ${TESTOBJS} libCommon.a commontest


# <MAKE DEPEND TARGETS BELOW>
-include "Makefile.depend"
