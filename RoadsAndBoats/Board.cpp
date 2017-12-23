#include "Board.hpp"
#include <iostream>
#include <sstream>

int Board::GetNextUniqueIdentifier()
{
  return m_nextUniqueIdentifier++;
}

void Board::MakeTilesAndSpaces(const Scenario &i_scenario)
{
  int i,j;

  for (i = 0 ; i < i_scenario.GetWidth() ; ++i)
  {
    for (j = 0 ; j < i_scenario.GetHeight() ; ++j)
    {
      const ScenarioHex &sh = i_scenario.GetScenarioHex(i,j);
      if (sh.GetTileType() == EMPTY) continue;

      Tile nt(i,j,sh.GetRivers(),sh.GetStartingSpaces());

      if (sh.GetRivers().size() == 0)
      {
        Space nsp(GetNextUniqueIdentifier(),nt.GetOID(),0,0);
        nt.AddSpaceOID(nsp.GetOID());
        m_spaces[nsp.GetOID()] = nsp;
      }        
      else if (sh.GetRivers().size() == 1)
      {
        int river = *(sh.GetRivers().begin());
        Space nsp(GetNextUniqueIdentifier(),nt.GetOID(),river,river);
        nt.AddSpaceOID(nsp.GetOID());
        m_spaces[nsp.GetOID()] = nsp;
      }
      else
      {
        int k,l;
        for (k = 1 ; k <= 6 ; ++k)
        {
          if (sh.GetRivers().find(k) == sh.GetRivers().end()) continue;

          for (l = k+1 ; l < 5+k ; ++l)
          {
            int ml = ((l-1)%6)+1;
            if (sh.GetRivers().find(ml) == sh.GetRivers().end()) continue;

            // if we're here, k is the clockwise, and ml is the counterclockwise

            Space nsp(GetNextUniqueIdentifier(),nt.GetOID(),k,ml);
            nt.AddSpaceOID(nsp.GetOID());
            m_spaces[nsp.GetOID()] = nsp;
            break;
          }
        }
      }
      
      m_tiles[nt.GetOID()] = nt;
    }
  }
}

void Board::MakeAndConnectBridges()
{
  std::map<TileOID,Tile>::iterator tit;

  for (tit = m_tiles.begin() ; tit != m_tiles.end() ; ++tit)
  {
    const Tile &tile = tit->second;
    if (tile.GetRiverEdges().size() < 2) continue;

    if (tile.GetRiverEdges().size() == 2)
    {
      std::set<int>::const_iterator spit = tile.GetSpaceOIDs().begin();
      int s1 = *(spit++);
      int s2 = *(spit++);

      Bridge newbridge(GetNextUniqueIdentifier(),s1,s2);
      m_spaces[s1].AddBridgeOID(0,newbridge.GetOID());
      m_spaces[s2].AddBridgeOID(0,newbridge.GetOID());
      m_bridges[newbridge.GetOID()] = newbridge;
    }
    else
    {
      // if we have more than two rives, we need to cross all of them.
      // for each river.
      std::set<int>::const_iterator rit;
      for (rit = tile.GetRiverEdges().begin() ; rit != tile.GetRiverEdges().end() ; ++rit)
      {
        int riverid = *rit;
        int sunoid = -1;
        int widoid = -1;
        // find the spaces that abut that river.
        std::set<int>::const_iterator sit;
        for (sit = tile.GetSpaceOIDs().begin() ; sit != tile.GetSpaceOIDs().end() ; ++sit)
        {
          Space &sp = m_spaces[*sit];
          if (riverid == sp.GetSunwiseRiver()) sunoid = *sit;
          if (riverid == sp.GetWiddershinsRiver()) widoid = *sit;
        }

        // make the bridge
        Bridge newbridge(GetNextUniqueIdentifier(),sunoid,widoid);
        m_spaces[sunoid].AddBridgeOID(*rit,newbridge.GetOID());
        m_spaces[widoid].AddBridgeOID(*rit,newbridge.GetOID());
        m_bridges[newbridge.GetOID()] = newbridge;
      }
    }
  }
}

Board::Board(const Scenario &i_scenario) :
  m_nextUniqueIdentifier(1)
{

  MakeTilesAndSpaces(i_scenario);
  MakeAndConnectBridges();

  std::map<int,Space>::iterator sit;
  for (sit = m_spaces.begin() ; sit != m_spaces.end() ; ++sit)
  {
    Space &sp = sit->second;
    Tile &tile = m_tiles[sp.GetTileOID()];

    std::cout << "Space:" << std::endl;
    std::cout << "  Loc: " << tile.GetX() << "," << tile.GetY() << std::endl;
    if (sp.GetWiddershinsRiver() != 0)
    {
      std::cout << "  Rivers: " << sp.GetWiddershinsRiver() << " to " << sp.GetSunwiseRiver() << std::endl;
    }
  }
}


      
