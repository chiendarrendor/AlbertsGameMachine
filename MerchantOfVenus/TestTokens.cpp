#include "MapData.hpp"
#include "Cup.hpp"
#include "TradeBases.hpp"
#include "MapOverlay.hpp"

int main(int argc,char **argv)
{

  srand(time(NULL));

  try
  {
    MapData md("../MerchantOfVenusMap.xml");
    Cup cup;
    TradeBases tb(md);
    MapOverlay mo(md);

    std::cout << "Cup: " << std::endl;
    const std::vector<Token>& cuptokens = cup.GetContents();
    for (size_t i = 0 ; i < cuptokens.size() ; ++i)
    {
      std::cout << cuptokens[i].Encoding() << std::endl;
    }

    std::cout << "Bases:" << std::endl;
    const std::map<std::string,TradeBase>& basemap = tb.GetBaseMap();
    std::map<std::string,TradeBase>::const_iterator baseit;
    for (baseit = basemap.begin() ; baseit != basemap.end() ; ++baseit)
    {
      const std::string& bname = baseit->first;
      const TradeBase& base = baseit->second;
      std::cout << "\tBase " << bname;
      std::cout << "(" << base.GetId() << "," << 
        base.GetName() << ")" << std::endl;
      std::cout << "\t\tFactory Good:" << std::endl;
      std::cout << "\t\t" << base.GetFactoryGood().Encoding() << std::endl;
      std::cout << "\t\tSellable Tokens:" << std::endl;
      const std::vector<Token>& bsell = base.GetSellableTokens();
      for (size_t i = 0 ; i < bsell.size() ; ++i)
      {
        std::cout << "\t\t" << bsell[i].Encoding() << std::endl;
      }
      std::cout << "\t\tDemand Tokens:" << std::endl;
      const std::vector<Token>& bdemand = base.GetDemandTokens();
      for (size_t i = 0 ; i < bdemand.size() ; ++i)
      {
        std::cout << "\t\t" << bdemand[i].Encoding() << std::endl;
      }

      std::cout << "\t\tSpaceport Deeds:" << std::endl;
      const std::vector<Token>& bdeeds = base.GetSpaceportDeeds();
      for (size_t i = 0 ; i < bdeeds.size() ; ++i)
      {
        std::cout << "\t\t" << bdeeds[i].Encoding() << std::endl;
      }
    }

    std::cout << "Relics: " << std::endl;
    const std::vector<Token>& relics = mo.GetRelicList();
    for (size_t i = 0 ; i < relics.size() ; ++i)
    {
      std::cout << relics[i].Encoding() << std::endl;
    }
  }
  catch(std::exception& e)
  {
    std::cout << "Fatal exception caught: " << e.what() << std::endl;
  }
}

  
