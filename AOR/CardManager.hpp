#include "Serialize.hpp"
#include "CardPile.hpp"
#include "CardInfo.hpp"

class DeckInfo;

class CardManager
{
public:
  CardManager(const DeckInfo &i_DeckInfo);
  virtual ~CardManager();

  void SetNumEpochs(int i_NumEpochs);
  int GetNumEpochs() const;
  int GetCurEpoch() const;

  int GetNumCards() const;

  bool CanDrawCard() const;

  // note: might change cur epoch
  std::string DrawCard();
private:
  void AddEpochToDeck(int i_Epoch);
  SERIALIZE_FUNC
  {
    SERIALIZE(m_NumEpochs);
    SERIALIZE(m_CurEpoch);
    SERIALIZE(m_AllCards);
    SERIALIZE(m_CurDeck);
    SERIALIZE(m_DiscardPile);
    SERIALIZE(m_ReplayPile);
    SERIALIZE(m_MasterArtDiscardPile);
  }
  int m_NumEpochs;
  int m_CurEpoch;

  CardPile m_AllCards;
  CardPile m_CurDeck;
  CardPile m_DiscardPile;
  CardPile m_ReplayPile;
  CardPile m_MasterArtDiscardPile;

  // not serialized
  const DeckInfo &m_DeckInfo;
};
