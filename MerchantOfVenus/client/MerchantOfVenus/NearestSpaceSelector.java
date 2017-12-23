package MerchantOfVenus;


import java.util.*;

public class NearestSpaceSelector
{
  private static int radius = 40;

  private class MultiMap
  {
    private NavigableMap<Integer,Vector<Board.MapSpace>> map;

    public MultiMap()
    {
      map = new TreeMap<Integer,Vector<Board.MapSpace>>();
    }
    
    public void add(int i,Board.MapSpace ms)
    {
      Integer ic = new Integer(i);
      if (!map.containsKey(ic))
      {
        map.put(ic,new Vector<Board.MapSpace>());
      }
      map.get(ic).add(ms);
    }
  
    // return a set of all items in this container
    // 
    public Set<Board.MapSpace> GetWithin(int center)
    {
      HashSet<Board.MapSpace> result = new HashSet<Board.MapSpace>();
      
      NavigableMap<Integer,Vector<Board.MapSpace>> sub = 
        map.subMap(new Integer(center-radius),true,
                   new Integer(center+radius),true);

      for(Vector<Board.MapSpace> v : sub.values())
      {
        for (Board.MapSpace ms : v )
        {
          result.add(ms);
        }
      }
      return result;
    }
  }

  MultiMap xcoords;
  MultiMap ycoords;
  Board theBoard;

  public NearestSpaceSelector(Board b)
  {
    theBoard = b;

    xcoords = new MultiMap();
    ycoords = new MultiMap();
    
    for (Board.MapSpace ms : theBoard.GetSpaces().values())
    {
      xcoords.add(ms.GetX(),ms);
      ycoords.add(ms.GetY(),ms);
    }
  }

  double CalcDist(Board.MapSpace ms,int x,int y)
  {
    double dx = ms.GetX() - x;
    double dy = ms.GetY() - y;
    return Math.sqrt(dx*dx + dy*dy);
  }


  public Board.MapSpace GetNearestSpace(int x,int y)
  {
    Set<Board.MapSpace> result = xcoords.GetWithin(x);
    result.retainAll(ycoords.GetWithin(y));

    if (result.size() == 0) return null;

    Board.MapSpace[] rar = result.toArray(new Board.MapSpace[0]);

    if (result.size() == 1)
    {
      if (CalcDist(rar[0],x,y) < radius) return rar[0];
      else return null;
    }

    double minDist = CalcDist(rar[0],x,y);
    Board.MapSpace minms = rar[0];

    for (int i = 1 ; i < rar.length ; ++i)
    {
      double tdist = CalcDist(rar[i],x,y);
      if (tdist < minDist)
      {
        minDist = tdist;
        minms = rar[i];
      }
    }
    if (minDist < radius) return minms;
    return null;
  }
}

    

    
     