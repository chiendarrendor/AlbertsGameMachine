#include "Serialize.hpp"
#include "Player.hpp"
#include "Outpost.hpp"

const size_t NO_PLAYER = 100;

class Players
{
public:
  Players();
  virtual ~Players();


  size_t size() const;
  void add(const std::string &i_Name);
  void remove(const std::string &i_Name);
  bool IsPlayer(const std::string &i_Name) const;

  const Player &operator[](size_t i_Num) const;
  Player &operator[](size_t i_Num) ;

  // will throw an exception if the name is not a player's name
  Player &GetPlayerByName(const std::string &i_Name);
  const Player &GetPlayerByName(const std::string &i_Name) const;

  size_t GetPlayerId(const std::string &i_Name) const;

  int GetPhase() const;
  bool AllOk() const;

  void DetermineTurnOrder();
  const std::vector<int> &GetTurnOrder() const;


  // these methods allow us to manage walking through the turn order created in 
  // DetermineTurnOrder
  void StartTurnOrder();
  void IncrementTurnOrder();
  const Player &GetCurTurnPlayer() const;
  Player &GetCurTurnPlayer();
  bool AllPlayersDone() const;

  // additional methods for Purchase Phase
  void StartPurchasePhase(); 
  void PurchaseIncrementTurnOrder(PurchaseState i_state = DONE); 
  bool PurchaseAllPlayersDone() const; 

  // methods for the Bidding Phase.
  void StartBidPhase(bool i_MustBeActive,bool i_OnePurchase,int i_InitialBid);
  const Player &GetCurBidPlayer() const;
  Player &GetCurBidPlayer();
  void IncrementBid(BidState i_BidState,int i_HighBid = NOT_HIGH_BID);

  const Player &GetHighBidPlayer() const;
  Player &GetHighBidPlayer();
  int GetCurrentHighBid() const;

  // methods for manning phase
  void StartManningTurnOrder();
  void IncrementManningTurnOrder();

  // more helper functions
  bool IsPlayerInternalizing(const std::string &i_PlayerName) const;
  bool CanPlayerPurchase(const std::string &i_PlayerName,
                         PurchasableType i_Purchasable,
                         RobotMechanism i_mechanism) const;
  bool CanPlayerMulligan(const std::string& i_PlayerName) const;
  std::set<size_t> GetWinningPlayerIndices() const;


private:
  // causes m_CurBidIndex to point to the next player in turn order,
  // wrapping around, whose bid state is not PASS_OUT
  void IncrementBidIndex();

  SERIALIZE_FUNC
  {
    SERIALIZE(m_Players);
    SERIALIZE(m_TurnOrder);
    SERIALIZE(m_CurTurnIndex);
    SERIALIZE(m_CurBidIndex);
    SERIALIZE(m_HighBidIndex);
  }
  std::vector<Player> m_Players;
  std::vector<int> m_TurnOrder;
  size_t m_CurTurnIndex;
  size_t m_HighBidIndex;
  size_t m_CurBidIndex;
};
