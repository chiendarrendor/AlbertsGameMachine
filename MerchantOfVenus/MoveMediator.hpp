#include "Serialize.hpp"
#include "Dest.hpp"

class Players;
class MapOverlay;

class MoveMediator
{
public:
  MoveMediator(Players& i_players,MapOverlay& i_mapoverlay);

  // determines whether a player can mulligan
  // rolls the proper dice for the player (including Auto Pilot)
  // sets initial movement points
  // sets list of possible pilot numbers
  void StartMove();

  // the cost of not starting to move from a penalty, and also
  // the cost of moving through a penalty.
  int GetPenaltyCost() const;

  // clears list of Dests, and adds all adjacent spaces
  // to list of Dests (only allowing flyable adjacencies if Air Foil)
  // invariant: either no Dests have pilot numbers, or they all do,
  //   and no pilot number is specified more than once.
  void AddAdjacents();

  // adds active telegates to list of Dests if either:
  // * current space is telegate and ! Gate Lock
  // * JumpStart and first
  // for any non-jump adjacency with pilot #'s, remove all pilot numbers
  //   for the telegate Dests, ad remove the non-jump dest if it has no
  //   pilot #'s
  // for any non-jump Dest with no pilot #'s, set its pilot #'s
  //   to the set of pilot #'s that are not telegate pilot #'s
  //  (and remove the Dest if all telegates are in play)
  void AddTelegates();
  
  // remove any non-jump Dests that violate backtrcking
  void CullBacktracks();

  // do nothing if first
  // do nothing to dests with no pilot #'s
  // (either no Dest has pilot #'s or they all do)
  // if pilot # is unset, or Switch Switch, remove
  //   all Dest pilot #'s that are not on the dice
  // else remove all Dest pilot #'s except pilot #
  // remove all Dests with no pilot #'s
  void CullByPilotNumber();

  // if mp left = 0, remove all dests
  // for each Dest
  //   mp = 1
  //   mp = 0 if color of dest is yellow and SkipYellow
  //   mp = 0 if color of dest is red and Skip Red
  //   mp = 2 if !AirFoil and source or dest is city
  //      (remove this dest if player has only 1 mp)
  void ApplyMovementPoints();

  // syntatic sugar for the above 5 methods, applied in order
  void PrepareForStep(); 

  // '/' separated list of dests
  // a dest is <name>!<jump/walk>!<cost>![<pilot numbers>]
  std::string GetCurrentDests() const;

  bool IsFirst() const;

  // true if result of PrepareForStep produces no dests
  bool AutoStop() const;
  // true if either first is true, or
  // if current space is asteroid, penalty, city, or station
  // (not dot or telegate) (qboxes don't exist at this point
  bool ManualStop() const;

  // provide this method with the names of the source and destination
  // spaces (in the way the player would move)
  void AddToBacks(const std::string& i_sourcename,const std::string i_destname);

  void ClearFirst();
  void MakeDiceVisible();
  bool AreDiceVisible() const;
  const std::vector<int>& GetDice() const;
  std::string GetDiceString() const;
  int GetMP() const;
  void SpendMP(int i_decrement);
  void SetPilotNumber(int i_pnum);
  int GetPilotNumber() const;

  // returns true if pilot number has been selected and
  // player does not have switch switch
  bool IsPilotNumberPermanent() const;
  bool CanMulligan() const;
  void ClearMulligan();

  void SetDestination(const std::string& i_dest,bool i_isjump);
  bool ValidateDestination() const;
  const Dest& GetChosenDest() const;

  bool DoMulligan(int i_rolltoreplace);

  const std::vector<Dest>& GetDests() const;


private:
  Players& m_players;
  MapOverlay& m_mapoverlay;

    
  std::vector<Dest> m_dests;
  // this will be true if a dest with valid pnums was culled due to a backtrack.
  // in this case, it is actually legal to choose a pnum that forces the backtrack,
  // thereby stopping.  This will be represented by a Manual Stop being allowed
  // if this case is true.
  bool m_backtrackingpnum; 

  bool m_canmulligan;
  std::vector<int> m_dicerolled;
  std::set<size_t> m_pnumbers;
  int m_pnumber;
  bool m_first;
  int m_mp;
  std::set<std::string> m_backs;
  bool m_dicevisible;
  std::string m_destination;
  bool m_destisjump;

  // and this will tell you if the potential move is a backtrack.
  bool IsBackTracking(const std::string& i_sourcename,
                      const std::string i_destname) const;


  SERIALIZE_FUNC
  {
    SERIALIZE(m_dests);
    SERIALIZE(m_backtrackingpnum);
    SERIALIZE(m_canmulligan);
    SERIALIZE(m_dicerolled);
    SERIALIZE(m_pnumbers);
    SERIALIZE(m_pnumber);
    SERIALIZE(m_first);
    SERIALIZE(m_mp);
    SERIALIZE(m_backs);
    SERIALIZE(m_dicevisible);
    SERIALIZE(m_destination);
    SERIALIZE(m_destisjump);
  }
};
