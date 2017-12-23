#include "CardManager.hpp"
#include "DeckInfo.hpp"
#include <stdexcept>
#include <iostream>

CardManager::CardManager(const DeckInfo &i_DeckInfo) :
  m_NumEpochs(0),
  m_CurEpoch(1),
  m_DeckInfo(i_DeckInfo)
{
  std::cout << "Filling Card Vector:" << std::endl;
  m_DeckInfo.FillCardNameVector(m_AllCards);
  std::cout << "AllCards now has: " << m_AllCards.size() << std::endl;

  AddEpochToDeck(1);

  std::cout << "Deck now has: " << m_CurDeck.size() << std::endl;
  std::cout << "AllCards now has: " << m_AllCards.size() << std::endl;

  m_CurDeck.Shuffle();
}

CardManager::~CardManager()
{
}

void CardManager::SetNumEpochs(int i_NumEpochs) 
{
  m_NumEpochs = i_NumEpochs;
}

int CardManager::GetNumEpochs() const
{
  return m_NumEpochs;
}

int CardManager::GetCurEpoch() const
{
  return m_CurEpoch;
}

void CardManager::AddEpochToDeck(int i_Epoch)
{
  CardPile tempPile;

  while(!m_AllCards.IsEmpty())
  {
    std::string curCard= m_AllCards.DrawCard();
    if (m_DeckInfo.GetCardInfo(curCard)->GetEpoch() == i_Epoch)
    {
      m_CurDeck.AddCard(curCard);
    }
    else
    {
      tempPile.AddCard(curCard);
    }
  }
  m_AllCards = tempPile;
}

bool CardManager::CanDrawCard() const
{
  return !m_CurDeck.IsEmpty() || m_CurEpoch < m_NumEpochs;
}

std::string CardManager::DrawCard()
{
  if (!m_CurDeck.IsEmpty())
  {
    return m_CurDeck.DrawCard();
  }

  if (m_CurEpoch == m_NumEpochs)
  {
    throw std::out_of_range("end-of-game CardManager was asked for card!");
  }

  m_CurEpoch++;

  m_CurDeck = m_ReplayPile;
  AddEpochToDeck(m_CurEpoch);
  m_CurDeck.Shuffle();
  
  return m_CurDeck.DrawCard();
}

int CardManager::GetNumCards() const
{
  return m_CurDeck.size();
}

