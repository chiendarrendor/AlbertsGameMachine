BOOSTINC=/c/Boost/include/boost-1_33_1
GAMECOMMON=../gamecommon
COMMON=../common
SENGINE=../stateengine

INSTALLTARGET=../../data/Age_Of_Renaissance
TCOMDIR=../transitioncompiler
TCOM=perl -I${TCOMDIR} ${TCOMDIR}/tcom.pl

OBJS=AORDLL.o AORState.o AORSet.o Players.o Player.o CardManager.o \
     DeckInfo.o CardInfo.o CommodityInfo.o CardPile.o \
     SpaceInfo.o SpaceInfoMap.o GameBoard.o BoardCounter.o\
     SpacePather.o SpaceState.o BoardState.o TurnLogEntry.o

CFILES=$(OBJS:%.o=%.cpp) 

INCS=-I${GAMECOMMON} -I${COMMON} -I${SENGINE} -I${BOOSTINC}
LOCLIBS=${GAMECOMMON}/libGameCommon.a ${COMMON}/libCommon.a 
GCC=/home/mingw/bin/g++ -g -Wall -Werror ${INCS}
CPP=/home/mingw/bin/cpp
BOOSTLIB=/c/Boost/lib/libboost_serialization-mgw-s-1_33_1.lib


AOR.dll: ${OBJS} ${LOCLIBS}
	${GCC} -shared -o $@ ${OBJS} ${LOCLIBS} ${BOOSTLIB}

test: ${OBJS} PluginTest.o 
	${GCC} -o $@ ${OBJS} PluginTest.o ${LOCLIBS} ${BOOSTLIB}

boardtest: ${OBJS} BoardTest.o
	${GCC} -o $@ ${OBJS} BoardTest.o ${LOCLIBS} ${BOOSTLIB}

${COMMON}/libCommon.a:
	- (cd ${COMMON} ; make)
${GAMECOMMON}/libGameCommon.a:
	- (cd ${GAMECOMMON} ; make)

AORDLL.cpp: AORServer.xml
	${TCOM} $<

install: AOR.dll AORClient.xml
	cp AOR.dll AORClient.xml ${INSTALLTARGET}

AORClient.xml: client/AORClient.xml
	${CPP} -Iclient -P client/AORClient.xml > AORClient.xml

%.o:%.cpp
	${GCC} -c $<

depend: AORDLL.cpp
	@ awk 'NR==1,/^# <MAKE DEPEND TARGETS BELOW>/' Makefile  > md.tmp
	@ ${GCC} -MM ${CFILES} | perl filterboost.pl | tr -d '\015' >> md.tmp
	@ ${CPP} -MM client/AORClient.xml | tr -d '\015' | sed 's/AORClient\.o/AORClient\.xml/'>> md.tmp
	@ mv md.tmp Makefile

clean:
	@- (cd ${COMMON} ; make clean)
	@- (cd ${GAMECOMMON} ; make clean)
	@- rm -f ${OBJS} AOR.dll AORDLL.cpp

# <MAKE DEPEND TARGETS BELOW>
AORDLL.o: AORDLL.cpp ../common/StringUtilities.hpp ../gamecommon/GameBoxDLL.hpp \
          ../stateengine/GameInfo.hpp ../stateengine/StateMachine.hpp \
          ../stateengine/Transition.hpp ../stateengine/State.hpp \
          ../gamecommon/DLLGame.hpp ../gamecommon/Game.hpp \
          ../stateengine/StateWalker.hpp ../common/Serialize.hpp \
          ../gamecommon/ServerGameInfo.hpp ../gamecommon/OutputPort.hpp \
          ../gamecommon/ActionParser.hpp AORSet.hpp DeckInfo.hpp \
          CommodityInfo.hpp GameBoard.hpp SpaceInfoMap.hpp AORState.hpp \
          Players.hpp Player.hpp CardPile.hpp TurnLogEntry.hpp CardManager.hpp \
          CardInfo.hpp SpaceState.hpp BoardState.hpp AOREventGenerator.hpp
AORState.o: AORState.cpp AORState.hpp ../common/Serialize.hpp Players.hpp \
            Player.hpp CardPile.hpp TurnLogEntry.hpp CardManager.hpp \
            CardInfo.hpp SpaceState.hpp BoardState.hpp GameBoard.hpp \
            SpaceInfoMap.hpp AORSet.hpp DeckInfo.hpp CommodityInfo.hpp
AORSet.o: AORSet.cpp AORSet.hpp DeckInfo.hpp CommodityInfo.hpp GameBoard.hpp \
          SpaceInfoMap.hpp
Players.o: Players.cpp Players.hpp ../common/Serialize.hpp Player.hpp \
           CardPile.hpp TurnLogEntry.hpp ../gamecommon/Utilities.hpp
Player.o: Player.cpp Player.hpp ../common/Serialize.hpp CardPile.hpp \
          TurnLogEntry.hpp
CardManager.o: CardManager.cpp CardManager.hpp ../common/Serialize.hpp \
               CardPile.hpp CardInfo.hpp DeckInfo.hpp
DeckInfo.o: DeckInfo.cpp DeckInfo.hpp CommodityInfo.hpp CardInfo.hpp
CardInfo.o: CardInfo.cpp CardInfo.hpp
CommodityInfo.o: CommodityInfo.cpp CommodityInfo.hpp
CardPile.o: CardPile.cpp CardPile.hpp ../common/Serialize.hpp \
            ../gamecommon/Utilities.hpp
SpaceInfo.o: SpaceInfo.cpp SpaceInfo.hpp SpaceInfoMap.hpp CommodityInfo.hpp
SpaceInfoMap.o: SpaceInfoMap.cpp SpaceInfoMap.hpp SpaceInfo.hpp \
                CommodityInfo.hpp
GameBoard.o: GameBoard.cpp GameBoard.hpp SpaceInfoMap.hpp CommodityInfo.hpp \
             ../common/StringUtilities.hpp BoardParsingUtilities.hpp \
             SpaceInfo.hpp
BoardCounter.o: BoardCounter.cpp BoardCounter.hpp BoardState.hpp SpaceState.hpp \
                ../common/Serialize.hpp GameBoard.hpp SpaceInfoMap.hpp \
                Players.hpp Player.hpp CardPile.hpp TurnLogEntry.hpp \
                CommodityInfo.hpp SpaceInfo.hpp
SpacePather.o: SpacePather.cpp SpacePather.hpp SpaceInfo.hpp SpaceInfoMap.hpp \
               CommodityInfo.hpp
SpaceState.o: SpaceState.cpp SpaceState.hpp ../common/Serialize.hpp
BoardState.o: BoardState.cpp BoardState.hpp SpaceState.hpp \
              ../common/Serialize.hpp GameBoard.hpp SpaceInfoMap.hpp \
              Players.hpp Player.hpp CardPile.hpp TurnLogEntry.hpp \
              AOREventGenerator.hpp ../gamecommon/OutputPort.hpp \
              ../gamecommon/ServerGameInfo.hpp BoardCounter.hpp \
              CommodityInfo.hpp SpaceInfo.hpp
TurnLogEntry.o: TurnLogEntry.cpp TurnLogEntry.hpp ../common/Serialize.hpp
AORClient.xml: client/AORClient.xml client/actions.xml client/events.xml \
  client/gameboard.xml client/commoditydisplay.xml client/misery.xml \
  client/playerdisplay.xml client/cards.xml client/advancelog.xml
