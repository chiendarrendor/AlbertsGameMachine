#include "Serialize.hpp"
#include "Advances.hpp"

#define NOPILOTNUMBER 0;

class Player
{
public:
  Player();
  Player(const std::string &i_Name,int i_Id);
  Player(const Player &i_Player);
  Player &operator=(const Player &i_Player);

  void SetId(int i_Id);
  int GetId() const;

  const std::string &GetName() const;
 
  void SetOkStartup(bool oks);
  bool OkStartup() const;

  void AddToken(Token t);
  void RemoveToken(size_t i_tokidx);
  void AgeTokens();
  // special:  the ship is the first item in the token list,
  // so we need a special method to replace it.
  void ExchangeShip(Token t);
  const Token& GetShip() const;
  const std::vector<Token>& GetTokens() const;

  // without an argument, should calculate the hold usage
  // of all items owned by the player (hence the default being
  // an item that adds nothing to ship size)
  // with an argument, will calculate the hold usage with that item.
  int GetStuffSize(Token t=Token::Relic("Dummy",0)) const;

  void SetRace(const std::string &i_race);
  const std::string &GetRace() const;

  void SetMoney(int i_money);
  void AddMoney(int i_increment);
  int GetMoney() const;

  void SetBarter(int i_barter);
  void AddBarter(int i_increment);
  int GetBarter() const;

  // gets the total amount that a player can spend.
  // (barter + money)
  int GetTotal() const;
  
  // removes from barter first, then from money
  // (assumes that GetTotal was checked first to 
  // ensure enough)
  void Spend(int i_amount);

  // gets the players net worth (money + face value of deeds)
  int GetNetWorth() const;


  // contains board space identifier for current location
  void SetLocation(const std::string& i_location);
  const std::string& GetLocation() const;

  const Advances& GetAdvances() const;
  Advances& GetAdvances();

  void ResetExchangeCounts();
  void IncrementBuyCount();
  void IncrementSellCount();
  int GetBuyCount() const;
  int GetSellCount() const;

private:

  SERIALIZE_FUNC
  {
    SERIALIZE(m_Name);
    SERIALIZE(m_Id);
    SERIALIZE(m_okstartup);
    SERIALIZE(m_raceid);
    SERIALIZE(m_money);
    SERIALIZE(m_barter);
    SERIALIZE(m_tokens);
    SERIALIZE(m_advances);
    SERIALIZE(m_location);
    SERIALIZE(m_sellcount);
    SERIALIZE(m_buycount);
  }

  std::string m_Name;
  int m_Id;
  bool m_okstartup;
  std::string m_raceid;
  int m_money;
  int m_barter;
  std::vector<Token> m_tokens;
  Advances m_advances;
  std::string m_location;
  int m_sellcount;
  int m_buycount;
};

