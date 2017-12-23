#include "Serialize.hpp"
#include "Token.hpp"
#include "Secret.hpp"

class TradeBase
{
public:
  TradeBase();
  TradeBase(const std::string& i_id,const std::string& i_name);
  TradeBase(const TradeBase& i_base);
  TradeBase& operator=(const TradeBase& i_base);

  const std::string& GetId() const;
  const std::string& GetName() const;
  
  const Secret& GetSecret() const;
  Secret& GetSecret();


  void SetFactoryGood(const Token& i_token);
  void AddSpaceportDeed(const Token& i_token);
  
  // called when race is discovered.
  void TransferDeeds();
  Token GetFactoryGood() const;

  void AddToken(const Token& i_token);
  void RemoveSellableToken(size_t i_index);
  void RemoveDemandToken(size_t i_index);
  
  const std::vector<Token>& GetSellableTokens() const;
  const std::vector<Token>& GetDemandTokens() const;
  const std::vector<Token>& GetSpaceportDeeds() const;
private:
  SERIALIZE_FUNC
  {
    SERIALIZE(m_id);
    SERIALIZE(m_name);
    SERIALIZE(m_sellabletokens);
    SERIALIZE(m_demandtokens);
    SERIALIZE(m_spaceportdeeds);
    SERIALIZE(m_factorygood);
    SERIALIZE(m_secret);
  }

  std::string m_id;
  std::string m_name;

  std::vector<Token> m_sellabletokens;
  std::vector<Token> m_demandtokens;
  std::vector<Token> m_spaceportdeeds;
  Token m_factorygood;
  Secret m_secret;
};

