
#include <string>
#include <vector>

enum CardType
{
  COMMODITYCARD,
  EVENTCARD,
  LEADERCARD
};

class CardInfo
{
public:
  // creating Commodity cards
  CardInfo(const std::string &i_Commodity,int i_Epoch);
  CardInfo(const std::string &i_Commodity1,
           const std::string &i_Commodity2,int i_Epoch);
  // creating Leader cards
  CardInfo(const std::string &i_Name,
           int i_Epoch,
           int i_Discount,
           const char *i_Advances);
  // creating Event card
  CardInfo(const std::string &i_Name,
           int i_Epoch,
           bool i_Recycles);
  
  const std::string &GetName() const;
  CardType GetType() const;
  int GetEpoch() const;
  bool Recycles() const;

  const std::vector<std::string> &GetCommodityVector() const;
  const std::vector<char> &GetAdvanceVector() const;
  int GetDiscount() const;

private:
  CardType m_Type;
  std::string m_Name;
  bool m_Recycles;
  int m_Epoch;

  int m_Discount;
  std::vector<std::string> m_Commodities;
  std::vector<char> m_Advances;

};

    
