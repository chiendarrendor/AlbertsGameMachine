GAMECOMMON=../../gamecommon
COMMON=../../common
SENGINE=../../stateengine

JAVADIR=/c/Program\ Files/Java/jdk1.6.0_25

JAVAC=${JAVADIR}/bin/javac

CLIENTJAVADIR=../client
CLIENTJAVALIB=../../client
CLIENTINSTALL=C:/apache/htdocs/client/Outpost
SERVERINSTALL=../../data/Outpost
TCOMDIR=../../transitioncompiler
TCOM=perl -I${TCOMDIR} ${TCOMDIR}/tcom.pl

MAINTESTOBJ=testmain.o

STOCHASTICTESTOBJS= \
	CommodityDeckTest.o StochasticUnownedItemsTest.o StochasticPlayersTest.o
TESTOBJS = \
	FactoriesTest.o CommodityTest.o CommodityManagerTest.o \
	OwnedItemsTest.o PlayerTest.o PlayersTest.o \
	OutpostSetTest.o UnownedItemsTest.o CommodityHandTest.o \
	OutpostStateTest.o ProductionManagerTest.o \
	TestOutputPort.o ItemsTest.o OptionsTest.o \
	StateMachineBidTest.o StateMachinePurchaseTest.o \
	TransitionTestUtilities.o StateMachineTest.o 

TESTCFILES= \
	$(TESTOBJS:%.o=../tests/%.cpp) \
	$(MAINTESTOBJ:%.o=../tests/%.cpp) \
	$(STOCHASTICTESTOBJS:%.o=../tests/%.cpp) 

OBJS= Commodity.o \
	CommodityDeck.o CommodityManager.o \
	Factories.o OwnedItems.o Player.o Players.o OutpostSet.o \
	Options.o UnownedItems.o CommodityHand.o OutpostState.o \
	ProductionManager.o Items.o Spend.o

ARTIFACTOBJS=OutpostDLL.o 

CLIENTJAVA=ItemCard.java CommodityCard.java CommodityHand.java Factories.java \
	ItemList.java ShipList.java TurnOrder.java StateSensitiveTextBox.java \
	OptionSelector.java BidItemWatcher.java Spends.java ResourceDecks.java \
	PlayerDisplay.java PlayerWindow.java

JFILES=$(CLIENTJAVA:%=${CLIENTJAVADIR}/Outpost/%)

CFILES=$(OBJS:%.o=../%.cpp) $(ARTIFACTOBJS:%.o=%.cpp)

INCS=-I${GAMECOMMON} -I${COMMON} -I${SENGINE} -I${BOOSTINC} -I.. -I.
LOCLIBS=${GAMECOMMON}/libGameCommon.a ${COMMON}/libCommon.a 
GCC=g++ -g -Wall -Werror ${INCS}
GCCL=g++ -g -Wall -Werror
CPP=cpp
BOOSTSERIALLIB=${BOOSTINC}/lib/libboost_serialization-mgw45-mt-1_46.a
BOOSTTESTLIB=${BOOSTINC}/lib/libboost_unit_test_framework-mgw45-mt-1_46.a
EXTLIBS=${BOOSTSERIALLIB} ${BOOSTTESTLIB} 

Outpost.dll: ${OBJS} ${ARTIFACTOBJS} ${LOCLIBS}
	${GCCL} -shared -o $@ ${OBJS} ${ARTIFACTOBJS} ${LOCLIBS} ${BOOSTSERIALLIB} 

tests: stochastictest.exe outposttest.exe

outposttest.exe: ${MAINTESTOBJ} ${TESTOBJS} ${ARTIFACTOBJS} ${OBJS} ${LOCLIBS}
	${GCCL} -o outposttest.exe ${MAINTESTOBJ} ${TESTOBJS} ${OBJS} ${ARTIFACTOBJS} ${LOCLIBS} ${EXTLIBS} 


stochastictest.exe: ${MAINTESTOBJ} ${STOCHASTICTESTOBJS} ${ARTIFACTOBJS} ${OBJS} ${LOCLIBS}
	${GCCL} -o stochastictest.exe ${MAINTESTOBJ} ${STOCHASTICTESTOBJS} ${OBJS} ${ARTIFACTOBJS} ${LOCLIBS} ${EXTLIBS}

${COMMON}/libCommon.a:
	- (cd ${COMMON} ; make)
${GAMECOMMON}/libGameCommon.a:
	- (cd ${GAMECOMMON} ; make)

OutpostDLL.cpp OutpostEventGenerator.hpp OutpostGameInfo.hpp: ../OutpostServer.xml
	${TCOM} $<

install: serverinstall clientinstall

serverinstall: Outpost.dll
	cp -f Outpost.dll ${SERVERINSTALL}

clientinstall: OutpostClient.xml clientjava
	cp -f OutpostClient.xml ${CLIENTINSTALL}
	cp -f ../client/*.JPG ${CLIENTINSTALL}
	cp -f ../client/*.PNG ${CLIENTINSTALL}
	cp -f ../client/Outpost/*.class ${CLIENTINSTALL}


stageclient: OutpostClient.xml clientjava
	cp ../client/*.PNG .
	cp ../client/*.JPG .


OutpostClient.xml: ../client/OutpostClient.xml 
	${CPP} -I../client -P ../client/OutpostClient.xml > OutpostClient.xml

clientjava:
	- rm ${CLIENTJAVADIR}/Outpost/*.class
	${JAVAC} -d ${CLIENTJAVADIR} -sourcepath ${CLIENTJAVADIR} -classpath ${CLIENTJAVALIB} ${JFILES}


%.o:%.cpp
	${GCC} -c $<

%.o:../%.cpp
	${GCC} -c $<

%.o:../tests/%.cpp
	${GCC} -c $<

depend: OutpostDLL.cpp
	 ${GCC} -MM ${CFILES} ${TESTCFILES} | tr -d '\015' | perl filterboost.pl > makefile.depend
	 ${CPP} -MM ../client/OutpostClient.xml | tr -d '\015' | sed 's/OutpostClient\.o/OutpostClient\.xml/' >> makefile.depend

clean:
	@- rm -f ${OBJS} ${TESTOBJS} Outpost.dll OutpostDLL.cpp ${MAINTESTOBJ}
	@- rm -f outposttest.exe OutpostClient.xml OutpostEventGenerator.hpp OutpostGameInfo.hpp

-include Makefile.depend

