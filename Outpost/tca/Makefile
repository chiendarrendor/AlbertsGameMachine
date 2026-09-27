
AWSBUCKET=albertsgamemachine
BOOSTINC=/usr/include
GAMECOMMON=../../gamecommon
COMMON=../../common
SENGINE=../../stateengine

JAVAC=javac -Xlint:unchecked

CLIENTJAVADIR=../client
CLIENTJAVALIB=../../client
CLIENTINSTALL=./clientstage
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

ALLOBJS=${OBJS} ${ARTIFACTOBJS} ${TESTOBJS} ${MAINTESTOBJ} ${STOCHASTICTESTOBJS} OutpostServerMain.o

INCS=-I${GAMECOMMON} -I${COMMON} -I${SENGINE} -I${BOOSTINC} -I.. -I. -DBOOST_TEST_DYN_LINK
LOCLIBS=${GAMECOMMON}/libGameCommon.a ${COMMON}/libCommon.a
GCC=g++ -g -Wall -Werror ${INCS} -MMD -MP
GCCL=g++ -g -Wall -Werror
CPP=cpp
BOOSTLIBDIR=/usr/lib64
BOOSTSERIALLIB=-L${BOOSTLIBDIR} -lboost_serialization
BOOSTTESTLIB=-L${BOOSTLIBDIR} -lboost_unit_test_framework
EXTLIBS=${BOOSTSERIALLIB} ${BOOSTTESTLIB}

Outpost: OutpostServerMain.o ${OBJS} ${ARTIFACTOBJS} ${LOCLIBS}
	${GCCL} -o $@ OutpostServerMain.o ${OBJS} ${ARTIFACTOBJS} ${LOCLIBS} ${BOOSTSERIALLIB}

tests: stochastictest outposttest

outposttest: ${MAINTESTOBJ} ${TESTOBJS} ${ARTIFACTOBJS} ${OBJS} ${LOCLIBS}
	${GCCL} -o $@ ${MAINTESTOBJ} ${TESTOBJS} ${OBJS} ${ARTIFACTOBJS} ${LOCLIBS} ${EXTLIBS}


stochastictest: ${MAINTESTOBJ} ${STOCHASTICTESTOBJS} ${ARTIFACTOBJS} ${OBJS} ${LOCLIBS}
	${GCCL} -o $@ ${MAINTESTOBJ} ${STOCHASTICTESTOBJS} ${OBJS} ${ARTIFACTOBJS} ${LOCLIBS} ${EXTLIBS}

${COMMON}/libCommon.a:
	- (cd ${COMMON} ; make)
${GAMECOMMON}/libGameCommon.a:
	- (cd ${GAMECOMMON} ; make)

OutpostDLL.cpp OutpostEventGenerator.hpp OutpostGameInfo.hpp: ../OutpostServer.xml
	${TCOM} $<

install: serverinstall clientinstall

serverinstall: Outpost
	cp -f Outpost ${SERVERINSTALL}

# I'm sure there are cleaner ways to do this, but I'm lazy.
clientinstall: OutpostClient.xml clientjava
	- rm -fr ${CLIENTINSTALL}
	- mkdir ${CLIENTINSTALL}
	- mkdir ${CLIENTINSTALL}/Outpost

	cp -f ../client/*.JPG ${CLIENTINSTALL}
	cp -f ../client/*.PNG ${CLIENTINSTALL}
	cp -f ../client/Outpost/*.class ${CLIENTINSTALL}/Outpost
	cd ${CLIENTINSTALL} ; jar -cf ../Outpost.jar *
	aws s3 cp OutpostClient.xml s3://${AWSBUCKET}/
	aws s3 cp Outpost.jar s3://${AWSBUCKET}/


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

clean:
	@- rm -f ${OBJS} ${TESTOBJS} OutpostDLL.cpp ${MAINTESTOBJ}
	@- rm -f outposttest.exe OutpostClient.xml OutpostEventGenerator.hpp OutpostGameInfo.hpp
	@- rm -f Outpost
	@- rm -f ${ALLOBJS:.o=.d}

-include $(ALLOBJS:.o=.d)

