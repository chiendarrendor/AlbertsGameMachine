#include "TradeBases.hpp"
#include "MapData.hpp"
#include "Options.hpp"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( TestHideUnusedWeaponsRemovesLaserAndNovaBall )
{
  MapData md("../MerchantOfVenusMap.xml");

  Options hiddenoptions;
  hiddenoptions.SetHideUnusedWeapons(true);
  TradeBases tb(md,hiddenoptions);

  std::map<std::string,TradeBase>::const_iterator baseit;
  for (baseit = tb.GetBaseMap().begin() ; baseit != tb.GetBaseMap().end() ; ++baseit)
  {
    const std::vector<Token>& tokens = baseit->second.GetSellableTokens();
    for (size_t i = 0 ; i < tokens.size() ; ++i)
    {
      BOOST_CHECK(tokens[i].GetName() != "Laser 20");
      BOOST_CHECK(tokens[i].GetName() != "Novaball 20");
    }
  }
}

BOOST_AUTO_TEST_CASE( TestShowUnusedWeaponsIncludesLaserAndNovaBall )
{
  MapData md("../MerchantOfVenusMap.xml");

  Options shownoptions;
  shownoptions.SetHideUnusedWeapons(false);
  TradeBases tb(md,shownoptions);

  bool foundlaser = false;
  bool foundnovaball = false;
  std::map<std::string,TradeBase>::const_iterator baseit;
  for (baseit = tb.GetBaseMap().begin() ; baseit != tb.GetBaseMap().end() ; ++baseit)
  {
    const std::vector<Token>& tokens = baseit->second.GetSellableTokens();
    for (size_t i = 0 ; i < tokens.size() ; ++i)
    {
      if (tokens[i].GetName() == "Laser 20") foundlaser = true;
      if (tokens[i].GetName() == "Novaball 20") foundnovaball = true;
    }
  }
  BOOST_CHECK(foundlaser);
  BOOST_CHECK(foundnovaball);
}
