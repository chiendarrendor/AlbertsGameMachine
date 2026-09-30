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
endif

ifeq (${OSTYPE},linux)
BOOSTINC=/usr/include
BOOSTLIBDIR=/usr/lib64
EXTLIBS=-L${BOOSTLIBDIR} -lboost_serialization -lboost_unit_test_framework -lboost_regex -lboost_filesystem -lboost_system
BOOSTDEPS=
endif

#DEFINES=-DSTATEDEBUG

JAVADIR=/c/Program\ Files/Java/jdk-19
JAVAC=${JAVADIR}/bin/javac

CLIENTJAVADIR=../client
CLIENTJAVALIB=../../client/classes
CLIENTCLASSDIR=classes
CLIENTINSTALL=C:/apache/htdocs/client/MerchantOfVenus
SERVERINSTALL=../../data/MerchantOfVenus
TCOMDIR=../../transitioncompiler
TCOM=perl -I${TCOMDIR} ${TCOMDIR}/tcom.pl
VALIDATOR=perl -I${TCOMDIR} ${TCOMDIR}/validate_action_schema.pl

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

ALLOBJS=${OBJS} ${ARTIFACTOBJS} ${TESTOBJS} ${MAINTESTOBJ} TestTokens.o TestStartMove.o MerchantOfVenusServerMain.o

INCS=-I${GAMECOMMON} -I${COMMON} -I${SENGINE} -I${BOOSTINC} -I.. -I.
LOCLIBS=${GAMECOMMON}/libGameCommon.a ${COMMON}/libCommon.a
GCC=g++ -g -Wall -Werror ${INCS} ${DEFINES} -MMD -MP
GCCL=g++ -g -Wall -Werror
CPP=cpp

MerchantOfVenus: MerchantOfVenusServerMain.o ${OBJS} ${ARTIFACTOBJS} ${LOCLIBS} ${BOOSTDEPS}
	${GCCL} -o $@ MerchantOfVenusServerMain.o ${OBJS} ${ARTIFACTOBJS} ${LOCLIBS} ${EXTLIBS}

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

MerchantOfVenusDLL.cpp MerchantOfVenusEventGenerator.hpp MerchantOfVenusGameInfo.hpp: ../MerchantOfVenusServer.xml .actionschema.validated
	${TCOM} ../MerchantOfVenusServer.xml

# Cross-validates ../MerchantOfVenusServer.xml's transition/action schema
# against MerchantOfVenusClient.xml's independently-authored action schema --
# nothing else in the build catches these two drifting apart, despite the
# game depending on them matching exactly. See .claude/TODO.md's action-
# schema item.
.actionschema.validated: ../MerchantOfVenusServer.xml MerchantOfVenusClient.xml
	${VALIDATOR} ../MerchantOfVenusServer.xml MerchantOfVenusClient.xml
	touch $@

install: serverinstall clientinstall

serverinstall: MerchantOfVenus ../MerchantOfVenusMap.xml
	cp -f MerchantOfVenus ${SERVERINSTALL}
	cp -f ../MerchantOfVenusMap.xml ${SERVERINSTALL}

clientinstall: ${CLIENTJAVADIR}/MerchantOfVenusClient.xml clientjava
	cp -f ${CLIENTJAVADIR}/MerchantOfVenusClient.xml ${CLIENTINSTALL}
	cp -f ${CLIENTCLASSDIR}/MerchantOfVenus/*.class ${CLIENTINSTALL}
	cp -f ../MerchantOfVenusMap.xml ${CLIENTINSTALL}
	cp -f ${CLIENTJAVADIR}/*.JPG ${CLIENTINSTALL}
	cp -f ${CLIENTJAVADIR}/*.png ${CLIENTINSTALL}

MerchantOfVenusClient.xml: ../client/MerchantOfVenusClient.xml 
	${CPP} -I../client -P ../client/MerchantOfVenusClient.xml > MerchantOfVenusClient.xml

clientjava: .actionschema.validated
	@- rm -f ${CLIENTJAVADIR}/MerchantOfVenus/*.class
	@- rm -rf ${CLIENTCLASSDIR}
	@- mkdir -p ${CLIENTCLASSDIR}
	${JAVAC} -d ${CLIENTCLASSDIR} -sourcepath ${CLIENTJAVADIR} -classpath ${CLIENTJAVALIB} ${JFILES}

voronoi:
	@- rm -f ${CLIENTJAVADIR}/MerchantOfVenus/*.class
	@- rm -rf ${CLIENTCLASSDIR}
	@- mkdir -p ${CLIENTCLASSDIR}
	${JAVAC} -d ${CLIENTCLASSDIR} -sourcepath ${CLIENTJAVADIR} -classpath ${CLIENTJAVALIB} ${VFILES}

%.o:%.cpp
	${GCC} -c $<

%.o:../%.cpp
	${GCC} -c $<

%.o:../tests/%.cpp
	${GCC} -c $<

clean:
	@- rm -f ${OBJS} ${TESTOBJS} MerchantOfVenusDLL.cpp ${MAINTESTOBJ}
	@- rm -f outposttest.exe MerchantOfVenusClient.xml MerchantOfVenusEventGenerator.hpp MerchantOfVenusGameInfo.hpp
	@- rm -f MerchantOfVenus
	@- rm -f .actionschema.validated
	@- rm -f ${ALLOBJS:.o=.d}

-include $(ALLOBJS:.o=.d)

