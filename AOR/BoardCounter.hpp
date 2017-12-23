// this class counts the number of total and new
// Dominance markers on the board for each Player/Commodity pair,
// and the total 

class BoardState;
class Players;

#include<string>
#include<map>
#include<vector>

class BoardCounter
{
public:
  BoardCounter(const BoardState &i_bs,const Players &i_PlayerList);

  int GetNumSpaces(const std::string &i_CommodityName,const std::string &i_CapitalName);
  int GetNumNewSpaces(const std::string &i_CommodityName,const std::string &i_CapitalName);

  int GetNumSpaces(const std::string &i_CapitalName);
  int GetNumNewSpaces(const std::string &i_CapitalName);
private:
  std::vector<std::pair<int,int> > m_Totals;
  std::vector<std::map<std::string,std::pair<int,int> > > m_PerCommodity;
  std::map<std::string,int> m_CapitalToIdMapper;
};

  
