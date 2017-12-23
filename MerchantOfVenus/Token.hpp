#ifndef TOKENHPP
#define TOKENHPP

#include "Serialize.hpp"

enum SellType{ CASH,BARTER,NONE};
enum SellDestination { CUP, CULTURE, NOWHERE };
enum DeedType { NOT_DEED,FACTORY_DEED,SPACEPORT_DEED };

// SHIP -- special
// GOOD = always 2 units
// EQUIPMENT = first one of each name is free, subsequent is 1 each
// DRIVE = always 1 unit
// FARE = always 1 unit
// RELIC = always 0 units
// DEED = always 0 units
// IOU = always 0 units
// DEMAND -- special (should never be owned by a player)
enum SizeClass { SHIP_SIZE,GOOD_SIZE, EQUIPMENT_SIZE, DRIVE_SIZE, 
                 FARE_SIZE, RELIC_SIZE, DEED_SIZE,IOU_SIZE,DEMAND_SIZE };

class Token
{
public:
  
  const std::string& GetName() const;
  SizeClass GetSizeClass() const;
  int GetBuyPrice() const;
  int GetSellPrice() const;
  SellType GetSellType() const; // can it be sold?  if so, does a player get
                                // cash or barter?
  SellDestination GetSellDestination(); // where does item go when a player
                                        // sells it?
  bool IsFactoryGood() const; // true if this good is a factory good
  bool IsUnlimited() const; // true if this good can be sold over and over
                            // without being expended from selling culture
  const std::vector<std::string>& GetBuyers() const;
  const std::string& GetSource() const; // only used for cup-able items.

  // deed property
  DeedType GetDeedType() const;

  // equipment properties
  int GetDefenseValue() const;
  int GetAttackValue() const;
  bool IsDisposable() const; // true if using it spends it (nova ball)
  int GetSkipColors() const; // 1 - skips yellow, 2 - skips red, 3 - skips both

  // properties only of ships
  int GetSpeed() const;
  int GetNumHolds() const;

  bool IsNew() const;
  void MakeNew();
  void MakeOld();


  Token();
  Token(const Token& i_right);
  Token& operator=(const Token& i_right);

  // things to make:
  // ious
  // fares
  // demands
  // goods
  // factory goods
  // relics:
  //   Jump Start, Gate Lock, Switch Switch, Auto Pilot 4, Spy Eye
  //   Air Foil, Mulligan Gear
  //   Yellow Drive
  //   Shield -20
  //   Laser 20

  // equipment
  //   Laser 20
  //   Yellow Drive
  //   Nova Ball 20
  //   Red Drive
  //   Shield -20
  //   Combined Drive
  // factory deeds
  // spaceport deeds
  // ships

  static Token Good(const std::string& i_name,const std::string& i_owner,
                    const std::string& i_buyers,
                    int i_buyprice,int i_sellprice,bool i_isFactory);
  static Token Fare(const std::string& i_source,const std::string& i_destination,
                    int i_payoff);
  static Token Demand(const std::string& i_good,const std::string& i_source,
                      int i_bonus);
  static Token IOU(const std::string& i_owner,int i_sellprice);
  static Token Relic(const std::string& i_name,int i_buyprice);
  static Token Drive(bool i_skipRed,bool i_skipYellow,bool i_isRelic,int i_buyprice);
  static Token Shield(bool i_isRelic,int i_buyprice);
  static Token Laser(bool i_isRelic,int i_buyprice);
  static Token NovaBall();
  static Token Deed(const std::string& i_name,DeedType i_type,int i_buyprice,const std::string& i_source);
  static Token Ship(const std::string& i_name,int i_buyprice,int i_tradein,
                    int i_speed,int i_holdcount);


  // packet-transferable info about this token.
  // list of items separated by '|' characters, thus:
  // name
  // size class: ship,good,equip,drive,fare,relic,deed,iou,demand
  // buy price (how much it costs player to buy it
  // sell price (how much player will get for selling it
  //             (trade-in value for a ship)
  //             (jettison value for a fare)
  //             (bonus add-on for a demand)
  // sell type:cash,barter,none
  //           --cash/barter is what kind of fiduciary benefit player gets
  //           --none means player cannot sell it.
  // selldestination:cup,culture,nowhere
  //           --where does item go when it is sold? to the cup, 
  //             to the purchase list of the buying culture, or
  //             into thin air?
  // isfactorygood:fg,notfg
  //           -- for a good, true if it is a factory good.
  // isunlimited:unlimited,limited
  //           -- true if a player buying it from a culture does not
  //              cause the item to be removed from the culture's purchase list
  // buyers:c1?c2?c3
  //           -- identifiers (1a,base, etc) of the cultures that will
  //              buy this item.
  // source:
  //           -- identifier (like buyers) of the culture that this 
  //              token will be placed with when drawn from the cup
  //              (this is only meaningful for goods, fares, and demands)
  // deedtype:notdeed,factory,spaceport
  //           -- meaningful only for deeds, specifys which type it is
  // defensevalue
  // attackvalue
  // isdisposable:disposable,reusable
  //  -- true for a weapon if using it makes it go away
  //                 (i.e. nova balls)
  // skipcolors:Y and/or R (Y,R,YR) drive specification
  // speed -- number of dice to roll (only for a ship)
  // holdcount -- size of holds (only for a ship)
  //              (note, holds are doubled from real game rules
  //              so that we don't have to use floating point
  //              for ship lading (i.e. goods are now size 2, and
  //              equipment and fares are size 1, so it comes out the
  //              same)
  // age:new,old -- new if this token was bought this turn, false otherwise
  std::string Encoding() const;

private:
  Token(const std::string& i_name,SizeClass i_class);
    
  std::string m_name;
  SizeClass m_sizeclass;
  int m_buyprice;
  int m_sellprice;
  SellType m_selltype;
  SellDestination m_selldestination;
  bool m_isfactorygood;
  bool m_isunlimited;
  std::vector<std::string> m_buyers;
  std::string m_source;

  DeedType m_deedtype;

  int m_defensevalue;
  int m_attackvalue;
  bool m_isdisposable;
  int m_skipcolors;

  int m_speed;
  int m_holdcount;
  bool m_isnew;

  static std::vector<std::string> s_allbuyers;

  SERIALIZE_FUNC
  {
    SERIALIZE(m_name);
    SERIALIZE(m_sizeclass);
    SERIALIZE(m_buyprice);
    SERIALIZE(m_sellprice);
    SERIALIZE(m_selltype);
    SERIALIZE(m_selldestination);
    SERIALIZE(m_isfactorygood);
    SERIALIZE(m_isunlimited);
    SERIALIZE(m_buyers);
    SERIALIZE(m_source);
    SERIALIZE(m_deedtype);
    SERIALIZE(m_defensevalue);
    SERIALIZE(m_attackvalue);
    SERIALIZE(m_isdisposable);
    SERIALIZE(m_skipcolors);
    SERIALIZE(m_speed);
    SERIALIZE(m_holdcount);
    SERIALIZE(m_isnew);
  }
};

#endif
  

