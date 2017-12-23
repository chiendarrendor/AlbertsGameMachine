#ifndef CARDPILEHPP
#define CARDPILEHPP

#include "Serialize.hpp"

typedef std::vector<std::string> StringVector;


class CardPile : public StringVector
{
public:
  void Shuffle();
  bool IsEmpty() const;
  std::string DrawCard();
  void AddCard(const std::string &i_Card);
private:
  SERIALIZE_FUNC
  {
    SERIALIZE_PARENT(StringVector);
  }
};

#endif
