#include "Player.hpp"
#include <stdexcept>

Player::Player() :
  m_Name(""),
  m_Id(0),
  m_okstartup(false),
  m_raceid(""),
  m_money(0),
  m_barter(0),
  m_tokens(),
  m_advances(m_tokens),
  m_location("Galactic Base"),
  m_sellcount(0),
  m_buycount(0)
{
}

Player::Player(const std::string &i_Name,int i_Id) : 
  m_Name(i_Name),
  m_Id(i_Id),
  m_okstartup(false),
  m_raceid(""),
  m_money(0),
  m_barter(0),
  m_tokens(),
  m_advances(m_tokens),
  m_location("Galactic Base"),
  m_sellcount(0),
  m_buycount(0)
{
}

Player::Player(const Player &i_Player) :
  m_tokens(),
  m_advances(m_tokens)
{
  *this = i_Player;
}

Player &Player::operator=(const Player &i_Player)
{
  if (this != &i_Player)
  {
    m_Name = i_Player.m_Name;
    m_Id = i_Player.m_Id;
    m_okstartup = i_Player.m_okstartup;
    m_raceid = i_Player.m_raceid;
    m_money = i_Player.m_money;
    m_barter = i_Player.m_barter;
    m_tokens = i_Player.m_tokens;
    m_advances = i_Player.m_advances;
    m_location = i_Player.m_location;
    m_buycount = i_Player.m_buycount;
    m_sellcount = i_Player.m_sellcount;
  }
  return *this;
}

int Player::GetId() const
{
  return m_Id;
}

void Player::SetId(int i_Id)
{
  m_Id = i_Id;
}

const std::string &Player::GetName() const
{
  return m_Name;
}

bool Player::OkStartup() const
{
  return m_okstartup;
}

void Player::SetOkStartup(bool oks)
{
  m_okstartup = oks;
}

void Player::SetRace(const std::string &i_race)
{
  m_raceid = i_race;
}

const std::string &Player::GetRace() const
{
  return m_raceid;
}

void Player::SetMoney(int i_money)
{
  m_money = i_money;
}

void Player::AddMoney(int i_increment) 
{
  m_money += i_increment;
}

 
int Player::GetMoney() const
{
  return m_money;
}

void Player::SetBarter(int i_barter)
{
  m_barter = i_barter;
}

void Player::AddBarter(int i_increment)
{
  m_barter += i_increment;
}
 
int Player::GetBarter() const
{
  return m_barter;
}

int Player::GetTotal() const
{
  return GetMoney() + GetBarter();
}

void Player::Spend(int i_amount)
{
  if (i_amount <= GetBarter())
  {
    AddBarter(-i_amount);
    return;
  }
  i_amount -= GetBarter();
  SetBarter(0);
  AddMoney(-i_amount);
}





void Player::AddToken(Token t)
{
  m_tokens.push_back(t);
}

void Player::RemoveToken(size_t i_tokidx)
{
  m_tokens.erase(m_tokens.begin()+i_tokidx);
}


const Token& Player::GetShip() const
{
  return m_tokens[0];
}


void Player::ExchangeShip(Token t)
{
  m_tokens[0] = t;
}

const std::vector<Token>& Player::GetTokens() const
{
  return m_tokens;
}

void Player::AgeTokens()
{
  for (size_t i = 0 ; i < m_tokens.size() ; ++i ) m_tokens[i].MakeOld();
}



void Player::SetLocation(const std::string& i_location)
{
  m_location = i_location;
}

const std::string& Player::GetLocation() const
{
  return m_location;
}

const Advances& Player::GetAdvances() const
{
  return m_advances;
}

Advances& Player::GetAdvances()
{
  return m_advances;
}

void Player::ResetExchangeCounts()
{
  m_sellcount = 0;
  m_buycount = 0;
}

void Player::IncrementBuyCount()
{
  ++m_buycount;
}

void Player::IncrementSellCount()
{
  ++m_sellcount;
}

int Player::GetBuyCount() const
{
  return m_buycount;
}

int Player::GetSellCount() const
{
  return m_sellcount;
}

int Player::GetStuffSize(Token t) const
{
  std::vector<Token> temp = GetTokens();
  temp.push_back(t);

  int result = 0;
  std::vector<Token>::const_iterator tit;
  std::set<std::string> equip;
  for (tit = temp.begin() ; tit != temp.end() ; ++tit)
  {
    switch(tit->GetSizeClass())
    {
    case RELIC_SIZE:
    case DEED_SIZE:
    case IOU_SIZE:
    case SHIP_SIZE:
    case DEMAND_SIZE:
      break;
    case GOOD_SIZE:
      result += 2;
      break;
    case DRIVE_SIZE:
    case FARE_SIZE:
      result += 1;
      break;
    case EQUIPMENT_SIZE:
      result += (equip.find(tit->GetName()) ==  equip.end()) ? 0 : 1;
      equip.insert(tit->GetName());
      break;
    }
  }
  return result;
}

int Player::GetNetWorth() const
{
  int result = GetMoney();
  std::vector<Token>::const_iterator tit;
  for (tit = GetTokens().begin() ; tit != GetTokens().end() ; ++tit)
  {
    if (tit->GetSizeClass() == DEED_SIZE)
    {
      result += tit->GetBuyPrice();
    }
  }
  return result;
}



