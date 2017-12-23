#include <string>
#include "DeckInfo.hpp"
#include "CommodityInfo.hpp"
#include "GameBoard.hpp"

class AORSet
{
public:
  AORSet(const std::string &i_DataDir);
  bool IsValid() const;

  const DeckInfo &GetDeckInfo() const;
  const CommodityInfo &GetCommodityInfo() const;
  const GameBoard &GetGameBoard() const;

private:
  DeckInfo m_DeckInfo;
  CommodityInfo m_CommodityInfo;
  GameBoard m_GameBoard;
};
