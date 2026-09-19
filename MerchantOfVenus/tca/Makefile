GAMECOMMON=../../gamecommon
COMMON=../../common
SENGINE=../../stateengine

ifeq (${OSTYPE},msys)
BOOSTINC=/c/local/boost_1_92_0
BOOSTLIBDIR=${BOOSTINC}/lib
BOOSTSERIALLIB=${BOOSTLIBDIR}/libboost_serialization-mgw45-mt-1_92_0.a
BOOSTTESTLIB=${BOOSTLIBDIR}/libboost_unit_test_framework-mgw45-mt-1_92_0.a
BOOSTREGEXLIB=${BOOSTLIBDIR}/libboost_regex-mgw45-mt-1_92_0.a
BOOSTFSLIB=${BOOSTLIBDIR}/libboost_filesystem-mgw45-mt-1_92_0.a
BOOSTSYSLIB=${BOOSTLIBDIR}/libboost_system-mgw45-mt-1_92_0.a
EXTLIBS=${BOOSTSERIALLIB} ${BOOSTTESTLIB} ${BOOSTREGEXLIB} ${BOOSTFSLIB} ${BOOSTSYSLIB}
BOOSTDEPS=${EXTLIBS}
SHLIBEXT=dll
endif

ifeq (${OSTYPE},linux)
BOOSTINC=/usr/include
BOOSTLIBDIR=/usr/lib64
EXTLIBS=-L${BOOSTLIBDIR} -lboost_serialization -lboost_unit_test_framework -lboost_regex -lboost_filesystem -lboost_system
BOOSTDEPS=
PICFLAG=-fPIC
SHLIBEXT=so
endif

#DEFINES=-DSTATEDEBUG

JAVADIR=/c/Program\ Files/Java/jdk-19
JAVAC=${JAVADIR}/bin/javac

CLIENTJAVADIR=../client
CLIENTJAVALIB=../../client
CLIENTINSTALL=C:/apache/htdocs/client/MerchantOfVenus
SERVERINSTALL=../../data/MerchantOfVenus
TCOMDIR=../../transitioncompiler
TCOM=perl -I${TCOMDIR} ${TCOMDIR}/tcom.pl

OBJS= MerchantOfVenusState.o MerchantOfVenusSet.o Players.o Player.o \
	MapData.o MapSpace.o Token.o Ship.o MapOverlay.o \
	Cup.o TradeBases.o TradeBase.o Advances.o Secret.o \
	MoveMediator.o Options.o

TESTOBJS=MoveMediatorTest.o MapDataTest.o AppendShuffleDrawsTest.o TradeBasesTest.o

MAINTESTOBJ=testmain.o

ARTIFACTOBJS=MerchantOfVenusDLL.o 

CLIENTJAVA=PlayerData.java MerchantOfVenus.java
VJAVA=VoronoiTest.java VoronoiStep.java

JFILES=$(CLIENTJAVA:%=${CLIENTJAVADIR}/MerchantOfVenus/%)
VFILES=$(VJAVA:%=${CLIENTJAVADIR}/MerchantOfVenus/%)

ALLOBJS=${OBJS} ${ARTIFACTOBJS} ${TESTOBJS} ${MAINTESTOBJ} TestTokens.o TestStartMove.o

INCS=-I${GAMECOMMON} -I${COMMON} -I${SENGINE} -I${BOOSTINC} -I.. -I.
LOCLIBS=${GAMECOMMON}/libGameCommon.a ${COMMON}/libCommon.a
GCC=g++ -g -Wall -Werror ${PICFLAG} ${INCS} ${DEFINES} -MMD -MP
GCCL=g++ -g -Wall -Werror
CPP=cpp

MerchantOfVenus.${SHLIBEXT}: ${OBJS} ${ARTIFACTOBJS} ${LOCLIBS} ${BOOSTDEPS}
	${GCCL} -shared -o $@ ${OBJS} ${ARTIFACTOBJS} ${LOCLIBS} ${EXTLIBS}

tests: MoVunittests.exe

tt: TestTokens.o ${OBJS}
	${GCCL} -o $@ ${OBJS} TestTokens.o ${EXTLIBS} ${LOCLIBS}

tsm: TestStartMove.o ${OBJS}
	${GCCL} -o $@ ${OBJS} TestStartMove.o ${EXTLIBS} ${LOCLIBS}


MoVunittests.exe: ${MAINTESTOBJ} ${TESTOBJS} ${ARTIFACTOBJS} ${OBJS} ${LOCLIBS}
	${GCCL} -o $@ ${MAINTESTOBJ} ${TESTOBJS} ${OBJS} ${ARTIFACTOBJS} ${LOCLIBS} ${EXTLIBS} 



${COMMON}/libCommon.a:
	- (cd ${COMMON} ; make)
${GAMECOMMON}/libGameCommon.a:
	- (cd ${GAMECOMMON} ; make)

MerchantOfVenusDLL.cpp MerchantOfVenusEventGenerator.hpp MerchantOfVenusGameInfo.hpp: ../MerchantOfVenusServer.xml
	${TCOM} $<

install: serverinstall clientinstall

serverinstall: MerchantOfVenus.${SHLIBEXT} ../MerchantOfVenusMap.xml
	cp -f MerchantOfVenus.${SHLIBEXT} ${SERVERINSTALL}
	cp -f ../MerchantOfVenusMap.xml ${SERVERINSTALL}

clientinstall: ${CLIENTJAVADIR}/MerchantOfVenusClient.xml clientjava
	cp -f ${CLIENTJAVADIR}/MerchantOfVenusClient.xml ${CLIENTINSTALL}
	cp -f ${CLIENTJAVADIR}/MerchantOfVenus/*.class ${CLIENTINSTALL}
	cp -f ../MerchantOfVenusMap.xml ${CLIENTINSTALL}
	cp -f ${CLIENTJAVADIR}/*.JPG ${CLIENTINSTALL}
	cp -f ${CLIENTJAVADIR}/*.png ${CLIENTINSTALL}

MerchantOfVenusClient.xml: ../client/MerchantOfVenusClient.xml 
	${CPP} -I../client -P ../client/MerchantOfVenusClient.xml > MerchantOfVenusClient.xml

clientjava:
	@- rm ${CLIENTJAVADIR}/MerchantOfVenus/*.class
	${JAVAC} -d ${CLIENTJAVADIR} -sourcepath ${CLIENTJAVADIR} -classpath ${CLIENTJAVALIB} ${JFILES}

voronoi:
	@- rm ${CLIENTJAVADIR}/MerchantOfVenus/*.class
	${JAVAC} -d ${CLIENTJAVADIR} -sourcepath ${CLIENTJAVADIR} -classpath ${CLIENTJAVALIB} ${VFILES}

%.o:%.cpp
	${GCC} -c $<

%.o:../%.cpp
	${GCC} -c $<

%.o:../tests/%.cpp
	${GCC} -c $<

clean:
	@- rm -f ${OBJS} ${TESTOBJS} MerchantOfVenus.dll MerchantOfVenus.so MerchantOfVenusDLL.cpp ${MAINTESTOBJ}
	@- rm -f outposttest.exe MerchantOfVenusClient.xml MerchantOfVenusEventGenerator.hpp MerchantOfVenusGameInfo.hpp
	@- rm -f ${ALLOBJS:.o=.d}

-include $(ALLOBJS:.o=.d)

