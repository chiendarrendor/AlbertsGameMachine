#include "Players.hpp"
#include "SerializeUtility.hpp"
#include <boost/test/auto_unit_test.hpp>
#include <iostream>
#include <stdexcept>

void PrepPlayer(Player &i_pl,int i_numorefac,int i_nummbs)
{
  int i;
  std::string manning;

  // no item cost, generate 1 VP each
  for (i = 0 ; i < i_numorefac ; ++i)
  {
    i_pl.GetFactories().AddFactory(ORE_FACTORY);
    i_pl.GetFactories().AddFactory(ORE_FACTORY);
    manning += "HU";
  }
  i_pl.GetFactories().AlterManning(manning,i_numorefac,0);

  // cost 200 each, no VP
  for (i = 0 ; i < i_nummbs ; ++i)
  {
    i_pl.GetOwnedItems().AddItem(MOON_BASE);
  }
}



BOOST_AUTO_TEST_CASE( PlayersSomeTiesTurnOrderStochastic )
{
  Players pl;
  pl.add("Player0");
  pl.add("Player1");
  pl.add("Player2");
  pl.add("Player3");
  pl.add("Player4");
  pl.add("Player5");
  pl.add("Player6");
  pl.add("Player7");

  PrepPlayer(pl[0],7,4);
  PrepPlayer(pl[1],30,2);
  PrepPlayer(pl[2],18,0);
  PrepPlayer(pl[3],7,4);
  PrepPlayer(pl[4],7,4);
  PrepPlayer(pl[5],21,0);
  PrepPlayer(pl[6],30,2);
  PrepPlayer(pl[7],45,0);

  int i;
  int niters = 1000;

  std::vector<int> pair(3);
  std::vector<int> triplet(4);

  for (i = 0 ; i < niters ; ++i)
  {
    pl.DetermineTurnOrder();
    const std::vector<int> &turnorder = pl.GetTurnOrder();

    if (turnorder[1] == 1) pair[0]++;
    else if (turnorder[1] == 6) pair[1]++;
    else pair[2]++;

    if (turnorder[5] == 0) triplet[0]++;
    else if (turnorder[5] == 4) triplet[1]++;
    else if (turnorder[5] == 3) triplet[2]++;
    else triplet[3]++;
  }

  std::cout << "Some Ties Turn Order Stochastic:" << std::endl;
  std::cout << "       Real: ";
  std::cout << pair[0] << "  " << pair[1] << " (" << pair[2] << ")";
  std::cout << "      ";
  std::cout << triplet[0] << "  " << triplet[1] << "  " << triplet[2];
  std::cout << " (" << triplet[3] << ")" << std::endl;

  std::cout << "    Optimal: ";
  std::cout << niters/2 << "  " << niters/2 << " (0)";
  std::cout << "      ";
  std::cout << niters/3 << "  " << niters/3 << "  " << niters/3;
  std::cout << " (" << 0 << ")" << std::endl;
}


BOOST_AUTO_TEST_CASE( PlayersAllZeroTurnOrderStochastic )
{
  Players pl;
  pl.add("Player0");
  pl.add("Player1");
  pl.add("Player2");
  pl.add("Player3");
  pl.add("Player4");
  pl.add("Player5");
  pl.add("Player6");
  pl.add("Player7");

  int i;
  int numiters = 1000;
  std::vector<int> pcount(8);
  for (i = 0 ; i < numiters ; ++i)
  {
    pl.DetermineTurnOrder();
    pcount[pl.GetTurnOrder()[0]]++;
  }
  std::cout << "All Equal Turn Order Stochastic:" << std::endl;
  std::cout << "       Real: ";
  for (i = 0 ; i < 8 ; ++i)
  {
    std::cout << pcount[i] << "  ";
  }
  std::cout << std::endl;

  std::cout << "    Optimal: ";
  for (i = 0 ; i < 8 ; ++i)
  {
    std::cout << numiters / 8 << "  ";
  }
  std::cout << std::endl;

}
