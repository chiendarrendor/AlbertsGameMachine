#include "Ship.hpp"

Token Ship::GetShipOfClass(ShipClass i_class)
{
  switch(i_class)
  {
  case SCOUT: return Token::Ship("Scout",60,30,3,4);
  case TRANSPORT: return Token::Ship("Transport",240,120,3,6);
  case CLIPPER: return Token::Ship("Clipper",120,60,4,4);
  case FREIGHTER: return Token::Ship("Freighter",300,150,2,10);
  default:
    THROW(std::runtime_error,"Unknown ship class! " << i_class);
  }
}

