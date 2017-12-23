#include "Players.hpp"
#include "MapData.hpp"
#include "MapOverlay.hpp"
#include "MoveMediator.hpp"

int main(int argc,char **argv)
{
  try
  {
    if (argc != 4)
    {
      std::cout << "bad command line: space hasJS hasAF" << std::endl;
      exit(1);
    }

    Players players;
    players.add("Foo Bar");
    players.RandomizeTurnOrder();

    MapData md("../MerchantOfVenusMap.xml");
    MapOverlay mo(md);

    MoveMediator mm(players,mo);

    std::cout << "Before:" << std::endl;
    for (size_t i = 1 ; i <= 6 ; ++i)
    {
      const MapSpace *pms = mo.GetTeleGate(i);
      if (pms == NULL) continue;
      std::cout << "Active TG " << i << ": " << pms->m_name << std::endl;
    }
    std::cout << "Activate:" << std::endl;

    std::map<std::string,MapSpace*>::const_iterator spit;
    for (spit = md.GetSpaceMap().begin() ; 
         spit != md.GetSpaceMap().end() ; ++spit)
    {
      const std::string& spname = spit->first;
      const std::pair<MapSpace,Secret> overlaid = 
        mo.GetMapSpace(spname);
      if (overlaid.first.m_type != TELEGATE) continue;
      std::cout << "TG: " << spname << " " << overlaid.first.m_gateid << std::endl;
      mo.MakeSpacePublic(spname);
    }
    std::cout << "After:" << std::endl;
    for (size_t i = 1 ; i <= 6 ; ++i)
    {
      const MapSpace *pms = mo.GetTeleGate(i);
      if (pms == NULL) continue;
      std::cout << "Active TG " << i << ": " << pms->m_name << std::endl;
    }

    players[0].SetLocation(argv[1]);

    if (std::string(argv[2]) == "y")
    {
      players[0].AddToken(Token::Relic("Jump Start",150));
    }

    if (std::string(argv[3]) == "y")
    {
      players[0].AddToken(Token::Relic("Air Foil",150));
    }


    mm.PrepareForMove();
    std::pair<int,std::string> result = mm.GetPreparedMove();
    std::cout << "cost: " << result.first << std::endl;
    std::cout << "moves: " << result.second << std::endl;


  }
  catch(std::exception& e)
  {
    std::cout << "Fatal exception caught: " << e.what() << std::endl;
  }
}
