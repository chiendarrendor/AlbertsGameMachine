#include "TradeBase.hpp"

TradeBase::TradeBase() :
  m_id(""),
  m_name(""),
  m_sellabletokens(),
  m_demandtokens(),
  m_spaceportdeeds(),
  m_factorygood(),
  m_secret()
{
}

TradeBase::TradeBase(const std::string& i_id,const std::string& i_name) :
  m_id(i_id),
  m_name(i_name),
  m_sellabletokens(),
  m_demandtokens(),
  m_spaceportdeeds(),
  m_factorygood(),
  m_secret()
{
}

TradeBase::TradeBase(const TradeBase& i_right)
{
  *this = i_right;
}

TradeBase& TradeBase::operator=(const TradeBase& i_right)
{
  if (this == &i_right) return *this;
  
  m_id = i_right.m_id;
  m_name = i_right.m_name;
  m_sellabletokens = i_right.m_sellabletokens;
  m_demandtokens = i_right.m_demandtokens;
  m_spaceportdeeds = i_right.m_spaceportdeeds;
  m_factorygood = i_right.m_factorygood;
  m_secret = i_right.m_secret;

  return *this;
}

const std::string& TradeBase::GetId() const { return m_id; }
const std::string& TradeBase::GetName() const { return m_name; }
Token TradeBase::GetFactoryGood() const { return m_factorygood; }

const std::vector<Token>& TradeBase::GetSellableTokens() const 
{ 
  return m_sellabletokens;
}

const std::vector<Token>& TradeBase::GetDemandTokens() const 
{ 
  return m_demandtokens; 
}

const std::vector<Token>& TradeBase::GetSpaceportDeeds() const 
{ 
  return m_spaceportdeeds; 
}


void TradeBase::SetFactoryGood(const Token& i_token)
{
  m_factorygood = i_token;
}

void TradeBase::AddSpaceportDeed(const Token& i_token)
{
  m_spaceportdeeds.push_back(i_token);
}

void TradeBase::TransferDeeds() 
{
  std::vector<Token>::iterator deedit;
  
  for (deedit = m_spaceportdeeds.begin() ; 
       deedit != m_spaceportdeeds.end() ; ++deedit)
  {
    m_sellabletokens.push_back(*deedit);
  }
  m_spaceportdeeds.clear();
}

void TradeBase::AddToken(const Token& i_token)
{
  if (i_token.GetSizeClass() == DEMAND_SIZE)
  {
    m_demandtokens.push_back(i_token);
  }
  else
  {
    m_sellabletokens.push_back(i_token);
  }
}

void TradeBase::RemoveSellableToken(size_t i_index)
{
  std::vector<Token>::iterator it = m_sellabletokens.begin();
  std::advance(it,i_index);
  m_sellabletokens.erase(it);
}

void TradeBase::RemoveDemandToken(size_t i_index)
{
  std::vector<Token>::iterator it = m_demandtokens.begin();
  std::advance(it,i_index);
  m_demandtokens.erase(it);
}

const Secret& TradeBase::GetSecret() const { return m_secret; }
Secret& TradeBase::GetSecret() { return m_secret; }
  

  
