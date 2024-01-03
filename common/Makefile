
OBJS=FileUtilities.o StringUtilities.o SystemSpecificDynamicLoading.o

CFILES=$(OBJS:%.o=%.cpp)
INCS=-I${BOOSTINC}
AR=ar
GCC=g++ -g -Wall -Werror -fPIC ${INCS} -D${OSTYPE}

libCommon.a: ${OBJS}
	${AR} rcs $@ ${OBJS}

%.o:%.cpp
	${GCC} -c $<

depend:
	${GCC} -MM ${CFILES} | tr -d '\015' > Makefile.depend

clean:
	@- rm -f ${OBJS} libCommon.a


# <MAKE DEPEND TARGETS BELOW>
-include "Makefile.depend"
