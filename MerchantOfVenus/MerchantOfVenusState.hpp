#include "Serialize.hpp"
#include "Players.hpp"
#include "Utilities.hpp"
#include "Cup.hpp"
#include "MapOverlay.hpp"
#include "TradeBases.hpp"
#include "MoveMediator.hpp"
#include "Options.hpp"


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

  const Options &GetOptions() const;
  Options &GetOptions();

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
  // related to above two functions, but more useful to user
  // returns -1 if the transaction type is unlimited
  // returns 1 if the transaction is limited to 1 and hasn't been used this turn
  // returns 0 if the tranaction is limited and _has_ been used this turn.
  int GetRemainingBuys() const;
  int GetRemainingSells() const;


private:
  const MerchantOfVenusSet &m_Set;

  Players m_Players;
  Cup m_cup;
  Options m_Options;
  MapOverlay m_mapoverlay;
  TradeBases m_tradebases;
  MoveMediator m_movemediator;

  SERIALIZE_FUNC
  {
    SERIALIZE(m_Players);
    SERIALIZE(m_cup);
    SERIALIZE(m_Options);
    SERIALIZE(m_mapoverlay);
    SERIALIZE(m_tradebases);
    SERIALIZE(m_movemediator);
  }


};



