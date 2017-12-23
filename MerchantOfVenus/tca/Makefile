GAMECOMMON=../../gamecommon
COMMON=../../common
SENGINE=../../stateengine

#DEFINES=-DSTATEDEBUG

JAVADIR=/c/Program\ Files/Java/jdk1.6.0_25
JAVAC=${JAVADIR}/bin/javac

CLIENTJAVADIR=../client
CLIENTJAVALIB=../../client
CLIENTINSTALL=C:/apache/htdocs/client/MerchantOfVenus
SERVERINSTALL=../../data/MerchantOfVenus
TCOMDIR=../../transitioncompiler
TCOM=perl -I${TCOMDIR} ${TCOMDIR}/tcom.pl

OBJS= MerchantOfVenusState.o MerchantOfVenusSet.o Players.o Player.o \
	MapData.o pugixml.o MapSpace.o Token.o Ship.o MapOverlay.o \
	Cup.o TradeBases.o TradeBase.o Advances.o Secret.o \
	MoveMediator.o

TESTOBJS=MoveMediatorTest.o

MAINTESTOBJ=testmain.o

ARTIFACTOBJS=MerchantOfVenusDLL.o 

CLIENTJAVA=PlayerData.java MerchantOfVenus.java
VJAVA=VoronoiTest.java VoronoiStep.java

JFILES=$(CLIENTJAVA:%=${CLIENTJAVADIR}/MerchantOfVenus/%)
VFILES=$(VJAVA:%=${CLIENTJAVADIR}/MerchantOfVenus/%)

CFILES=$(OBJS:%.o=../%.cpp) $(ARTIFACTOBJS:%.o=%.cpp)

INCS=-I${GAMECOMMON} -I${COMMON} -I${SENGINE} -I${BOOSTINC} -I.. -I.
LOCLIBS=${GAMECOMMON}/libGameCommon.a ${COMMON}/libCommon.a 
GCC=g++ -g -Wall -Werror ${INCS} ${DEFINES}
GCCL=g++ -g -Wall -Werror
CPP=cpp
BOOSTSERIALLIB=${BOOSTINC}/lib/libboost_serialization-mgw45-mt-1_46.a
BOOSTTESTLIB=${BOOSTINC}/lib/libboost_unit_test_framework-mgw45-mt-1_46.a
BOOSTREGEXLIB=${BOOSTINC}/lib/libboost_regex-mgw45-mt-1_46.a
BOOSTFSLIB=${BOOSTINC}/lib/libboost_filesystem-mgw45-mt-1_46.a
BOOSTSYSLIB=${BOOSTINC}/lib/libboost_system-mgw45-mt-1_46.a
EXTLIBS=${BOOSTSERIALLIB} ${BOOSTTESTLIB} ${BOOSTREGEXLIB} ${BOOSTFSLIB} ${BOOSTSYSLIB}

MerchantOfVenus.dll: ${OBJS} ${ARTIFACTOBJS} ${LOCLIBS} ${EXTLIBS}
	${GCCL} -shared -o $@ ${OBJS} ${ARTIFACTOBJS} ${LOCLIBS} ${EXTLIBS}

tests: movtest.exe

tt: TestTokens.o ${OBJS}
	${GCCL} -o $@ ${OBJS} TestTokens.o ${EXTLIBS} ${LOCLIBS}

tsm: TestStartMove.o ${OBJS}
	${GCCL} -o $@ ${OBJS} TestStartMove.o ${EXTLIBS} ${LOCLIBS}


movtest.exe: ${MAINTESTOBJ} ${TESTOBJS} ${ARTIFACTOBJS} ${OBJS} ${LOCLIBS}
	${GCCL} -o $@ ${MAINTESTOBJ} ${TESTOBJS} ${OBJS} ${ARTIFACTOBJS} ${LOCLIBS} ${EXTLIBS} 



${COMMON}/libCommon.a:
	- (cd ${COMMON} ; make)
${GAMECOMMON}/libGameCommon.a:
	- (cd ${GAMECOMMON} ; make)

MerchantOfVenusDLL.cpp MerchantOfVenusEventGenerator.hpp MerchantOfVenusGameInfo.hpp: ../MerchantOfVenusServer.xml
	${TCOM} $<

install: serverinstall clientinstall

serverinstall: MerchantOfVenus.dll ../MerchantOfVenusMap.xml
	cp -f MerchantOfVenus.dll ${SERVERINSTALL}
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

depend: MerchantOfVenusDLL.cpp
	${GCC} -MM ${CFILES} ${TESTCFILES} | tr -d '\015' | perl filterboost.pl  > makefile.depend
	${CPP} -MM ../client/MerchantOfVenusClient.xml | tr -d '\015' | sed 's/MerchantOfVenusClient\.o/MerchantOfVenusClient\.xml/' >> makefile.depend

clean:
	@- rm -f ${OBJS} ${TESTOBJS} MerchantOfVenus.dll MerchantOfVenusDLL.cpp ${MAINTESTOBJ}
	@- rm -f outposttest.exe MerchantOfVenusClient.xml MerchantOfVenusEventGenerator.hpp MerchantOfVenusGameInfo.hpp

-include Makefile.depend

