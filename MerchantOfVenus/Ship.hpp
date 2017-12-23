#include "Token.hpp"
#include "MerchantOfVenus.hpp"
enum ShipClass { SCOUT,TRANSPORT,CLIPPER,FREIGHTER };

class Ship
{
public:
  static Token GetShipOfClass(ShipClass i_class);
};
