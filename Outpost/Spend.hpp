#include "Serialize.hpp"
#include "CommodityHand.hpp"

enum SpendType
{
  ITEMPURCHASE = 0,
  BIDPURCHASE = 1,
  DISCARD = 2,
  WATERMULLIGAN = 3
};

class Spend
{
public:
  Spend();

  Spend(const Spend &i_right);
  Spend& operator=(const Spend& i_right);

  Spend(int i_TurnNum,size_t i_PlayerNum,SpendType i_SpendType,int i_SpendItem);
  void AddCommodity(Commodity i_com);

  void SetSpendCost(int i_Cost);
  
  const int GetTurnNum() const;
  const size_t GetPlayerNum() const;
  const SpendType GetSpendType() const;
  const int GetSpendItem() const;
  const int GetSpendCost() const;
  const CommodityHand& GetCommodityHand() const;

private:
  int m_TurnNum;
  size_t m_PlayerNum;
  SpendType m_SpendType;
  int m_SpendItem; // either PurchasableType or ItemType, depending on m_SpendType
  int m_SpendCost;
  CommodityHand m_Spends;

  SERIALIZE_FUNC
  {
    SERIALIZE(m_TurnNum);
    SERIALIZE(m_PlayerNum);
    SERIALIZE(m_SpendType);
    SERIALIZE(m_SpendItem);
    SERIALIZE(m_SpendCost);
    SERIALIZE(m_Spends);
  }
};

  
