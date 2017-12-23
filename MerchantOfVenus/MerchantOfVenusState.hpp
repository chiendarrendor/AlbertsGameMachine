#include "Serialize.hpp"
#include "Players.hpp"
#include "Utilities.hpp"
#include "Cup.hpp"
#include "MapOverlay.hpp"
#include "TradeBases.hpp"
#include "MoveMediator.hpp"


class MerchantOfVenusSet;

// primary game state interface.

class MerchantOfVenusState
{
public:
  // required external interfaces
  MerchantOfVenusState(const MerchantOfVenusSet &i_Set);
  
  const MerchantOfVenusSet &GetMerchantOfVenusSet() const;

  Players &GetPlayers();
  const Players &GetPlayers() const;

  Cup& GetCup();
  MapOverlay& GetMapOverlay();
  const MapOverlay& GetMapOverlay() const;

  TradeBases& GetTradeBases();
  const TradeBases& GetTradeBases() const;

  int m_winmoney;

  MoveMediator& GetMoveMediator();
  const MoveMediator& GetMoveMediator() const;

  // returns an index into the acting player's item list
  // of the IOU with the same ID as the race owning the 
  // trade base in the solar system the player is in.
  // or -1 if the player has no such IOU
  // the player is guaranteed to be on a city or station,
  // and therefore in an inhabited solar system
  int GetIndexOfAppropriateIOU() const;


  // true if either the current player did not move this turn
  // or they have not bought/sold something this turn already.
  bool HasRemainingBuy() const;
  bool HasRemainingSell() const;



private:
  const MerchantOfVenusSet &m_Set;

  Players m_Players;
  Cup m_cup;
  MapOverlay m_mapoverlay;
  TradeBases m_tradebases;
  MoveMediator m_movemediator;

  SERIALIZE_FUNC
  {
    SERIALIZE(m_winmoney);
    SERIALIZE(m_Players);
    SERIALIZE(m_cup);
    SERIALIZE(m_mapoverlay);
    SERIALIZE(m_tradebases);
    SERIALIZE(m_movemediator);
  }


};



