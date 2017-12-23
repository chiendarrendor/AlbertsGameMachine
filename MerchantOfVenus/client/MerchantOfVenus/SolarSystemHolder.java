package MerchantOfVenus;
import Voronoi.*;
import java.util.*;

class SolarSystemHolder
{
    static double radius = 70.0;
    static double count = 16.0;

    public Vector<Voronoi.Point> spaces = new Vector<Voronoi.Point>();
    public Vector<Voronoi.Point> extensions = new Vector<Voronoi.Point>();
    public Fortune voronoi = null;

    public int minx = Integer.MAX_VALUE;
    public int miny = Integer.MAX_VALUE;

    public SolarSystemHolder() {}

    public void AddPoint(int x,int y)
    {
      if (x < minx) minx = x;
      if (y < miny) miny = y;

      spaces.add(new Voronoi.Point(x,y));
      double dtheta = 2 * Math.PI / count;
      for (double theta = 0 ; theta < 2 * Math.PI ; theta += dtheta)
      {
        Voronoi.Point cir = new Voronoi.Point(x+radius*Math.cos(theta),
					      y+radius*Math.sin(theta) );
        extensions.add(cir);
        if (cir.x < minx) minx = (int)cir.x;
        if (cir.y < miny) miny = (int)cir.y;
      }
    }

    public void disableExtensions()
    {
      Vector<Voronoi.Point> finalV = new Vector<Voronoi.Point>();
      for(Voronoi.Point vp : spaces)
      {
        finalV.add(vp);
      }
      voronoi = new Fortune(finalV);
    }

    public void enableExtensions()
    {
	execute();
    }

    public void execute()
    {
      Vector<Voronoi.Point> finalV = new Vector<Voronoi.Point>();
      for(Voronoi.Point vp : spaces)
      {
        finalV.add(vp);
      }
      for (Voronoi.Point vp : extensions)
      {
        finalV.add(vp);
      }
      
      voronoi = new Fortune(finalV);
    }
}
