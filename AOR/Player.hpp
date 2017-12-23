#include "Serialize.hpp"
#include "CardPile.hpp"
#include "TurnLogEntry.hpp"

const int SECRET_BID = -2;
const int NO_BID = -1;

enum ShipType { GALLEYS,SEAWORTHYVESSEL,OCEANNAVIGATION };

const int CHAOS_MISERY = 26;
const int STARTING_NUM_TOKENS = 36;
const int STARTING_NUM_DOMINATIONS = 25;

class Player
{
public:
  Player();
  Player(const std::string &i_Name);
  Player(const Player &i_Player);
  Player &operator=(const Player &i_Player);


  const std::string &GetName() const;

  size_t GetNumCards() const;
  void AddCard(std::string i_CardName);

  void SetCash(int i_CurCash);
  int GetCash() const;
    
  void SetCapitalBid(int i_BidValue);
  int GetCapitalBid() const;

  const CardPile &GetHand() const;

  const std::string &GetCapital() const;
  void SetCapital(const std::string &i_Capital);

  void SetShipType(ShipType i_ShipType);
  ShipType GetShipType() const;

  void SetShipGrade(int i_ShipGrade);
  int GetShipGrade() const;

  void SetMiseryIndex(int i_MiseryIndex);
  int GetMiseryIndex() const;

  void IncrementDominations(int i_Num = 1);
  void DecrementDominations(int i_Num = 1);
  int GetNumDominations() const;

  void IncrementStockTokens(int i_Num = 1);
  void DecrementStockTokens(int i_Num = 1);
  int GetNumStockTokens() const;

  void IncrementExpansionTokens(int i_Num = 1);
  void DecrementExpansionTokens(int i_Num = 1);
  int GetNumExpansionTokens() const;

  int GetNumBoardDominations() const;
  int GetNumBoardTokens() const;

  
  // This will do the right thing for a players
  // oop bid (i.e. < 0 means no tokens, 
  // expansion tokens set to i_Num or # of stock tokens, 
  // whichever is less), and alter Stock Tokens appropriately.
  void SetExpansionTokens(int i_Num);
  // this will return Expansion Tokens to 0, returning them to stock.
  void ClearExpansionTokens();



  void ExtendTurnLog(int i_TurnToExtend);
  const std::vector<TurnLogEntry> &GetTurnLog() const;
  TurnLogEntry &GetLastTurnLogEntry();

private:

  SERIALIZE_FUNC
  {
    SERIALIZE(m_Name);
    SERIALIZE(m_CurCash);
    SERIALIZE(m_CapitalBid);
    SERIALIZE(m_Hand);
    SERIALIZE(m_Capital);
    SERIALIZE(m_ShipType);
    SERIALIZE(m_ShipGrade);
    SERIALIZE(m_MiseryIndex);
    SERIALIZE(m_NumDominations);
    SERIALIZE(m_NumStockTokens);
    SERIALIZE(m_NumExpansionTokens);
    SERIALIZE(m_TurnLog);
  }

  std::string m_Name;
  int m_CurCash;
  int m_CapitalBid;
  CardPile m_Hand;
  std::string m_Capital;
  ShipType m_ShipType;
  int m_ShipGrade;
  int m_MiseryIndex;
  int m_NumDominations;
  int m_NumStockTokens;
  int m_NumExpansionTokens;
  std::vector<TurnLogEntry> m_TurnLog;
};

