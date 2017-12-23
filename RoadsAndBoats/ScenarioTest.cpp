#include "Scenario.hpp"
#include <iostream>
#include "ANSITextGrid.hpp"
#include "Board.hpp"








std::ostream &operator<<(std::ostream &o,const Scenario &scen)
{
	int i,j;
	ANSITextGrid atg(10 * scen.GetWidth() - 1 + ((scen.GetHeight() > 1) ? 5 : 0),
			(7 * (scen.GetHeight() - 1)) + 11);
 
	for (i = 0 ; i < scen.GetHeight() ; ++i)
	{
		for (j = 0 ; j < scen.GetWidth() ; ++j)
		{
			int cx = j*10+4+((i%2 == 1) ? 5 : 0);
			int cy = 7 * i + 5;

			const ScenarioHex &sh = scen.GetScenarioHex(j,i);
			if (sh.GetTileType() == EMPTY) continue;
			ANSIProperties attr,fore,back;
      char ch;

			// background for space
			switch(sh.GetTileType())
			{
			case WOODS: attr = DIM ; fore = FORE_BLACK ; back = BACK_GREEN; ch='#';break;
			case PASTURE: attr = BRIGHT ; fore = FORE_GREEN ; back = BACK_GREEN; ch=' ';break;
			case ROCK: attr = RESET ; fore = FORE_BLACK ; back = BACK_WHITE; ch='#';break;
			case MOUNTAINS: attr = DIM ; fore = FORE_RED ; back = BACK_RED ; ch=' ';break;
			case DESERT: attr = DIM ; fore = FORE_YELLOW ; back = BACK_YELLOW ; ch=' ';break;
			case SEA: attr = DIM ; fore = FORE_BLUE ; back = BACK_BLUE; ch=' ';break;
			}


			int a,b;
			for (a = -5 ; a <= 5 ; a++)
			{
				int r;
				switch(a)
				{
				case 5:
				case -5:
					r = 0; break;
				case 4:
				case -4:
					r = 1; break;
				case 3:
				case -3:
					r = 2; break;
				case 2:
				case -2:
					r = 3; break;
				default:
					r = 4;
				}


				for (b = -r ; b <= r ; ++b)
				{
					atg.SetChar(cx+b,cy+a,ch,attr,back,fore);
				}
			}

			attr = DIM ; fore = FORE_BLUE ; back=BACK_BLUE;  

			// rivers
			if (sh.GetRivers().size() > 0)
			{
				atg.SetChar(cx,cy,' ',attr,back,fore);
			}

			if (sh.GetRivers().find(1) != sh.GetRivers().end())
			{
				atg.SetChar(cx-1,cy-1,' ',attr,back,fore);
				atg.SetChar(cx-1,cy-2,' ',attr,back,fore);
				atg.SetChar(cx-2,cy-3,' ',attr,back,fore);
			}
			if (sh.GetRivers().find(2) != sh.GetRivers().end())
			{
				atg.SetChar(cx+1,cy-1,' ',attr,back,fore);
				atg.SetChar(cx+1,cy-2,' ',attr,back,fore);
				atg.SetChar(cx+2,cy-3,' ',attr,back,fore);
			}
			if (sh.GetRivers().find(3) != sh.GetRivers().end())
			{
				atg.SetChar(cx+1,cy,' ',attr,back,fore);
				atg.SetChar(cx+2,cy,' ',attr,back,fore);
				atg.SetChar(cx+3,cy,' ',attr,back,fore);
				atg.SetChar(cx+4,cy,' ',attr,back,fore);
			}	
			if (sh.GetRivers().find(4) != sh.GetRivers().end())
			{
				atg.SetChar(cx+1,cy+1,' ',attr,back,fore);
				atg.SetChar(cx+1,cy+2,' ',attr,back,fore);
				atg.SetChar(cx+2,cy+3,' ',attr,back,fore);
			}
			if (sh.GetRivers().find(5) != sh.GetRivers().end())
			{
				atg.SetChar(cx-1,cy+1,' ',attr,back,fore);
				atg.SetChar(cx-1,cy+2,' ',attr,back,fore);
				atg.SetChar(cx-2,cy+3,' ',attr,back,fore);
			}
			if (sh.GetRivers().find(6) != sh.GetRivers().end())
			{
				atg.SetChar(cx-1,cy,' ',attr,back,fore);
				atg.SetChar(cx-2,cy,' ',attr,back,fore);
				atg.SetChar(cx-3,cy,' ',attr,back,fore);
				atg.SetChar(cx-4,cy,' ',attr,back,fore);
			}	

			// starting spaces
			attr = DIM ; fore = FORE_MAGENTA ; back = BACK_MAGENTA;
			
			if (sh.GetStartingSpaces().find(1) != sh.GetStartingSpaces().end())
			{
				atg.SetChar(cx,cy-1,' ',attr,back,fore);
			}
			if (sh.GetStartingSpaces().find(2) != sh.GetStartingSpaces().end())
			{
				atg.SetChar(cx+2,cy-1,' ',attr,back,fore);
			}
			if (sh.GetStartingSpaces().find(3) != sh.GetStartingSpaces().end())
			{
				atg.SetChar(cx+2,cy+1,' ',attr,back,fore);
			}
			if (sh.GetStartingSpaces().find(4) != sh.GetStartingSpaces().end())
			{
				atg.SetChar(cx,cy+1,' ',attr,back,fore);
			}
			if (sh.GetStartingSpaces().find(5) != sh.GetStartingSpaces().end())
			{
				atg.SetChar(cx-2,cy+1,' ',attr,back,fore);
			}
			if (sh.GetStartingSpaces().find(6) != sh.GetStartingSpaces().end())
			{
				atg.SetChar(cx-2,cy-1,' ',attr,back,fore);
			}
		}
	}

	o << atg;
	return o;
}

void OldScenPrint(const Scenario &scen)
{

  int i,j;

  for (i = 0 ; i < scen.GetHeight() ; ++i)
  {
    if (i%2 == 1) std::cout << " ";

    for (j = 0 ; j < scen.GetWidth() ; ++j)
    {
      const ScenarioHex sh = scen.GetScenarioHex(j,i);

      std::cout << '\033' << "[0";

      switch (sh.GetTileType())
      {
      case EMPTY: std::cout << "m  "; break;
      case WOODS: std::cout << ";32;2mf "; break;
      case PASTURE: std::cout << ";32;1mp "; break;
      case ROCK: std::cout << "ms "; break;
      case MOUNTAINS: std::cout << ";31mm "; break;
      case DESERT: std::cout << ";33md "; break;
      case SEA: std::cout << ";34mw "; break;
      }
    }
    std::cout << std::endl;
  }
}

			



void DoScen(const std::string &i_sname)
{
  Scenario scen(i_sname);

  std::cout << "Name: " << scen.GetName() << std::endl;
  std::cout << "Comment: " << scen.GetComment();
  std::cout << "NumPlayers: " << scen.GetNumPlayers() << std::endl;
  std::cout << "Valid? " << scen.IsValid() << std::endl;

  if (!scen.IsValid())
  {
    std::cout << "Invalid: " << scen.Why() << std::endl;
    return;
  }

  std::cout << scen;

  Board b(scen);

}  





int main(int argc,char **argv)
{
  DoScen("Triangle.txt");
  std::cout << "--------------------------" << std::endl;
  DoScen("DividedKingdom.txt");
  std::cout << "--------------------------" << std::endl;
  DoScen("RowRow.txt");
  std::cout << "--------------------------" << std::endl;
  DoScen("RiverThrough.txt");
  std::cout << "--------------------------" << std::endl;
  DoScen("Mesopotamia.txt");
  std::cout << "--------------------------" << std::endl;
  DoScen("LordRing.txt");
  std::cout << "--------------------------" << std::endl;
  DoScen("Test.txt");
}




