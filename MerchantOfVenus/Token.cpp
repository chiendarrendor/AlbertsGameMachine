#include "Token.hpp"
#include <boost/algorithm/string.hpp>
#include "MerchantOfVenus.hpp"

std::string SellTypeString(SellType st) {
  switch(st)
  {
  case CASH: return "cash" ; 
  case BARTER: return "barter" ; 
  case NONE: return "none" ; 
  default: return "????";
  }
}

std::string SellDestinationString(SellDestination sd) {
 switch(sd)
  {
  case CUP: return "cup";
  case CULTURE: return "culture"; 
  case NOWHERE: return "nowhere"; 
  default: return "????";
  }
}

std::string DeedTypeString(DeedType dt) {
  switch(dt) {
    case NOT_DEED: return "notdeed";
    case FACTORY_DEED: return "factory";
    case SPACEPORT_DEED: return "spaceport";
    default: return "????";
  }
}

std::string SizeClassString(SizeClass sc) {
  switch(sc) {
    case SHIP_SIZE: return "ship";
    case GOOD_SIZE: return "good";
    case EQUIPMENT_SIZE: return "equip";
    case DRIVE_SIZE: return "drive";
    case FARE_SIZE: return "fare";
    case RELIC_SIZE: return "relic";
    case DEED_SIZE: return "deed";
    case IOU_SIZE: return "iou";
    case DEMAND_SIZE: return "demand";
    default: return "????";
  }
}


std::vector<std::string> Token::s_allbuyers;

Token::Token() :
  m_name(""),
  m_sizeclass(GOOD_SIZE),
  m_buyprice(0),
  m_sellprice(0),
  m_selltype(NONE),
  m_selldestination(NOWHERE),
  m_isfactorygood(false),
  m_isunlimited(false),
  m_buyers(),
  m_source(""),
  m_deedtype(NOT_DEED),
  m_defensevalue(0),
  m_attackvalue(0),
  m_isdisposable(false),
  m_skipcolors(0),
  m_speed(0),
  m_holdcount(0),
  m_isnew(false)
{
  if (s_allbuyers.size() == 0)
  {
    boost::split(s_allbuyers,
                 "1a,1b,2,3,4a,4b,5,6,7a,7b,8,9a,9b,10,base",
                 boost::is_any_of(","));
  }
}

Token::Token(const std::string& i_name,SizeClass i_class) :
  m_name(i_name),
  m_sizeclass(i_class),
  m_buyprice(0),
  m_sellprice(0),
  m_selltype(NONE),
  m_selldestination(NOWHERE),
  m_isfactorygood(false),
  m_isunlimited(false),
  m_buyers(),
  m_source(""),
  m_deedtype(NOT_DEED),
  m_defensevalue(0),
  m_attackvalue(0),
  m_isdisposable(false),
  m_skipcolors(0),
  m_speed(0),
  m_holdcount(0),
  m_isnew(false)
{
  if (s_allbuyers.size() == 0)
  {
    boost::split(s_allbuyers,
                 "1a,1b,2,3,4a,4b,5,6,7a,7b,8,9a,9b,10,base",
                 boost::is_any_of(","));
  }
}

Token::Token(const Token& i_right)
{
  *this = i_right;
}

Token& Token::operator=(const Token& i_right)
{
  if (this != &i_right) 
  {
    m_name = i_right.m_name;
    m_sizeclass = i_right.m_sizeclass;
    m_buyprice = i_right.m_buyprice;
    m_sellprice = i_right.m_sellprice;
    m_selltype = i_right.m_selltype;
    m_selldestination = i_right.m_selldestination;
    m_isfactorygood = i_right.m_isfactorygood;
    m_isunlimited = i_right.m_isunlimited;
    m_buyers = i_right.m_buyers;
    m_source = i_right.m_source;
    m_deedtype = i_right.m_deedtype;
    m_defensevalue = i_right.m_defensevalue;
    m_attackvalue = i_right.m_attackvalue;
    m_isdisposable = i_right.m_isdisposable;
    m_skipcolors = i_right.m_skipcolors;
    m_speed = i_right.m_speed;
    m_holdcount = i_right.m_holdcount;
    m_isnew = i_right.m_isnew;
  }

  return *this;
}

Token Token::Good(const std::string& i_name,const std::string& i_owner,
                  const std::string& i_buyers,
                  int i_buyprice,int i_sellprice,bool i_isFactory)
{
  Token result(i_name,GOOD_SIZE);
  result.m_buyprice = i_buyprice;
  result.m_sellprice = i_sellprice;
  result.m_isfactorygood = i_isFactory;
  result.m_selltype = CASH;
  result.m_selldestination = CUP;
  result.m_source = i_owner;
  boost::split(result.m_buyers,i_buyers,boost::is_any_of(","));
  return result;
}

Token Token::IOU(const std::string& i_owner,int i_sellprice)
{
  Token result(std::string("IOU ") + i_owner,IOU_SIZE);
  result.m_sellprice = i_sellprice;
  result.m_selltype = BARTER;
  result.m_selldestination = NOWHERE;
  result.m_buyers.push_back(i_owner);
  return result;
}


Token Token::Relic(const std::string& i_name,int i_buyprice)
{
  Token result(i_name,RELIC_SIZE);
  result.m_buyprice = i_buyprice;
  result.m_sellprice = i_buyprice/2;
  result.m_selltype = BARTER;
  result.m_selldestination = CULTURE;
  result.m_buyers = s_allbuyers;
  return result;
}
  
Token Token::Drive(bool i_skipRed,bool i_skipYellow,bool i_isRelic,int i_buyprice)
{
  std::string name;
  if (i_skipRed && i_skipYellow) name += "Combined";
  else if (i_skipRed) name += "Red";
  else if (i_skipYellow) name += "Yellow";
  else THROW(std::runtime_error,"driveless drive!?");
  Token result(name + " Drive",i_isRelic ? RELIC_SIZE : DRIVE_SIZE);

  result.m_buyprice = i_buyprice;
  result.m_sellprice = i_buyprice/2;
  result.m_selltype = BARTER;
  result.m_selldestination = i_isRelic ? CULTURE : NOWHERE;
  result.m_isunlimited = !i_isRelic ;
  result.m_buyers = s_allbuyers;;
  result.m_skipcolors = (i_skipRed ? 2 : 0) + (i_skipYellow ? 1 : 0);

  return result;
}


Token Token::Shield(bool i_isRelic,int i_buyprice)
{
  Token result("Shield -20",i_isRelic ? RELIC_SIZE : EQUIPMENT_SIZE);
  result.m_buyprice = i_buyprice;
  result.m_sellprice = i_buyprice/2;
  result.m_selltype = BARTER;
  result.m_selldestination = i_isRelic ? CULTURE : NOWHERE;
  result.m_isunlimited = !i_isRelic ;
  result.m_buyers = s_allbuyers;;
  result.m_defensevalue = 20;
  return result;

}

Token Token::Laser(bool i_isRelic,int i_buyprice)
{
  Token result("Laser 20",i_isRelic ? RELIC_SIZE : EQUIPMENT_SIZE);
  result.m_buyprice = i_buyprice;
  result.m_sellprice = i_buyprice / 2;
  result.m_selltype = BARTER;
  result.m_selldestination = i_isRelic ? CULTURE : NOWHERE;
  result.m_isunlimited = !i_isRelic ;
  result.m_buyers = s_allbuyers;;
  result.m_attackvalue = 20;
  return result;
}

Token Token::NovaBall()
{
  Token result("Novaball 20",EQUIPMENT_SIZE);
  result.m_buyprice = 20;
  result.m_sellprice = 10;
  result.m_selltype = BARTER;
  result.m_selldestination = NOWHERE;
  result.m_isunlimited = true ;
  result.m_buyers = s_allbuyers;
  result.m_attackvalue = 20;
  result.m_isdisposable = true;
  return result;
}

Token Token::Deed(const std::string& i_name,DeedType i_type,int i_buyprice,const std::string& i_owner)
{
  Token result(i_name,DEED_SIZE);
  result.m_buyprice = i_buyprice;
  result.m_selltype = NONE;
  result.m_deedtype = i_type;
  result.m_source = i_owner;
  result.m_selldestination = NOWHERE;
  return result;
}
  
Token Token::Ship(const std::string& i_name,int i_buyprice,int i_tradein,
                  int i_speed,int i_holdcount)
{
  Token result(i_name,SHIP_SIZE);
  result.m_buyprice = i_buyprice;
  result.m_sellprice = i_tradein;
  result.m_selltype = BARTER;
  result.m_selldestination = NOWHERE;
  result.m_isunlimited = true;
  result.m_speed = i_speed;
  result.m_holdcount = i_holdcount;

  return result;
}


Token Token::Fare(const std::string& i_source,const std::string& i_destination,
                         int i_payoff)
{
  Token result(std::string("Fare to ") + i_destination,FARE_SIZE);
  result.m_buyprice = 0;
  result.m_sellprice = i_payoff;
  result.m_selltype = CASH;
  result.m_selldestination = CUP;
  result.m_buyers.push_back(i_destination);
  result.m_source = i_source;
  return result;
}

Token Token::Demand(const std::string& i_good,const std::string& i_source,
                           int i_bonus)
{
  Token result(std::string(i_good),DEMAND_SIZE);
  result.m_sellprice = i_bonus;
  result.m_selldestination = CUP;
  result.m_source = i_source;
  return result;
}

const std::string& Token::GetName() const { return m_name; }
SizeClass Token::GetSizeClass() const { return m_sizeclass; }
int Token::GetBuyPrice() const { return m_buyprice; }
int Token::GetSellPrice() const { return m_sellprice; }
SellType Token::GetSellType() const { return m_selltype; }
SellDestination Token::GetSellDestination() { return m_selldestination; }
bool Token::IsFactoryGood() const { return m_isfactorygood; }
bool Token::IsUnlimited() const { return m_isunlimited; }
const std::vector<std::string>& Token::GetBuyers() const { return m_buyers; }
const std::string& Token::GetSource() const { return m_source; }

int Token::GetDefenseValue() const { return m_defensevalue; }
int Token::GetAttackValue() const { return m_attackvalue; }
bool Token::IsDisposable() const { return m_isdisposable; }
int Token::GetSkipColors() const { return m_skipcolors; }

int Token::GetSpeed() const { return m_speed; }
int Token::GetNumHolds() const { return m_holdcount; }
DeedType Token::GetDeedType() const { return m_deedtype; }



std::string Token::Encoding() const
{
  std::ostringstream result;

  result << m_name << '|';

  result << SizeClassString(m_sizeclass);

  result << '|' << m_buyprice << '|' << m_sellprice << '|';

  result << SellTypeString(m_selltype);

  result << '|';

  result << SellDestinationString(m_selldestination);

  result << '|' << (m_isfactorygood ? "fg" : "notfg") << '|';
  result << (m_isunlimited ? "unlimited" : "limited") << '|';

  for (size_t i = 0 ; i < m_buyers.size() ; ++i)
  {
    if (i != 0) result << '?';
    result << m_buyers[i];
  }
  result << '|' << m_source << '|';

  result << DeedTypeString(m_deedtype);
  
  result << '|' << m_defensevalue << '|' << m_attackvalue << '|';
  result << (m_isdisposable ? "disposable" : "reusable") << '|';

  switch(m_skipcolors)
  {
  case 1: result << 'Y'; break;
  case 2: result << 'R'; break;
  case 3: result << "YR"; break;
  }

  result << '|' << m_speed << '|' << m_holdcount;

  result << '|' << (m_isnew ? "new" : "old");

  return result.str();
}

bool Token::IsNew() const
{
  return m_isnew;
}

void Token::MakeNew() { m_isnew = true; }
void Token::MakeOld() { m_isnew = false; }

boost::json::object Token::ToJSON() const {
  boost::json::object result;
  result["name"] = m_name;
  result["sizeClass"] = SizeClassString(m_sizeclass);
  result["buyPrice"] = m_buyprice;
  result["sellPrice"] = m_sellprice;
  result["sellType"] = SellTypeString(m_selltype);
  result["sellDestination"] = SellDestinationString(m_selldestination);
  result["isFactoryGood"] = m_isfactorygood;
  result["isUnlimited"] = m_isunlimited;
  boost::json::array buyers;
  for (std::string buyer : m_buyers) {
    // arrays must take json objects, so you have to wrap string in json value
    buyers.push_back(boost::json::value(buyer));
  }
  // don't forget, this is a copy, not a reference, so you have to fill it first then give it to the parent.
  result["buyers"] = buyers;
  result["source"] = m_source;
  result["deedType"] = DeedTypeString(m_deedtype);
  result["defenseValue"] = m_defensevalue;
  result["attackValue"] = m_attackvalue;
  result["isDisposable"] = m_isdisposable;
  result["skipsYellow"] = m_skipcolors == 1 || m_skipcolors == 3;
  result["skipsRed"] = m_skipcolors == 2 || m_skipcolors == 3;
  result["speed"] = m_speed;
  result["holdCount"] = m_holdcount;

  return result;
}
    






  
