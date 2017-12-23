#include "CardPile.hpp"
#include "Utilities.hpp"
#include <stdexcept>
#include <iostream>

void CardPile::Shuffle()
{
  myshuffle(this->begin(),this->end());
}

bool CardPile::IsEmpty() const
{
  return this->size() == 0;
}

std::string CardPile::DrawCard()
{
  if (IsEmpty())
  {
    throw std::out_of_range("CardPile has no cards!");
  }

  std::string result = (*this)[0];

  this->erase(this->begin());

  return result;
}

void CardPile::AddCard(const std::string &i_Card)
{
  this->push_back(i_Card);
}

