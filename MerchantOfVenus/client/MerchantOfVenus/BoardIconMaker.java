package MerchantOfVenus;

import java.awt.geom.*;
import java.awt.image.*;
import java.util.*;
import java.awt.*;
import javax.swing.ImageIcon;
import MySwing.ImageLoader;
import MySwing.StringDrawer;
import NodeInterfacePackage.NodeInterfacePackageException;

import Voronoi.*;

public class BoardIconMaker
{
  private static int pathWidth = 8;
  private static Color pathColor = Color.LIGHT_GRAY;

  private Board theBoard;
  private Vector<Shape> paths;
  private Map<String,ImageIcon> icons;
  private String theDir;

  public BoardIconMaker(Board b,String dir)
  {
    theBoard = b;
    theDir = dir;
    paths = null;
    icons = new HashMap<String,ImageIcon>();
    icons.put("orbit",ImageLoader.MakeImageIcon(dir + "/orbit.png"));
    icons.put("city",ImageLoader.MakeImageIcon(dir + "/city.png"));
    icons.put("station",ImageLoader.MakeImageIcon(dir + "/station.png"));
    icons.put("die1",ImageLoader.MakeImageIcon(dir + "/die1.png"));
    icons.put("die2",ImageLoader.MakeImageIcon(dir + "/die2.png"));
    icons.put("die3",ImageLoader.MakeImageIcon(dir + "/die3.png"));
    icons.put("die4",ImageLoader.MakeImageIcon(dir + "/die4.png"));
    icons.put("die5",ImageLoader.MakeImageIcon(dir + "/die5.png"));
    icons.put("die6",ImageLoader.MakeImageIcon(dir + "/die6.png"));
    icons.put("arrow",ImageLoader.MakeImageIcon(dir + "/arrow.png"));
  }


  private void CCircle(Graphics g,Color c,double cx,double cy, int radius)
  {
    g.setColor(c);
    g.fillOval(((int)cx)-radius,((int)cy)-radius,radius*2,radius*2);
  }
  
  private Color ColorOfColor(Board.SpaceColor c)
  {
    Color ret = Color.WHITE;
    switch(c)
    {
    case BLUE: ret = Color.BLUE; break;
    case RED:  ret = Color.RED; break;
    case YELLOW: ret = Color.YELLOW; break;
    }
    return ret;
  }

  private void PaintIconRotatedAt(Graphics2D g2d,ImageIcon ii,int x,int y,int xcenter,int ycenter,int xtarget,int ytarget)
  {
    double dx = xtarget - x;
    double dy = ytarget - y;
    double theta = Math.atan2(dy,dx);
    PaintRotatedIcon(g2d,ii,x,y,theta,xcenter,ycenter);
  }

  private void PaintRotatedIcon(Graphics2D g2d,ImageIcon ii,int x,int y,double theta,int xcenter,int ycenter)
  {
    AffineTransform at = new AffineTransform();
    at.setToTranslation(x-xcenter,y-ycenter);
    at.rotate(theta,xcenter,ycenter);
    g2d.drawImage(ii.getImage(),at,null);
  }

  private void PaintIcon(Graphics2D g2d,String imageName,int x,int y)
  {
    ImageIcon ii = icons.get(imageName);
    ii.paintIcon(null,g2d,x-ii.getIconWidth()/2,y-ii.getIconHeight()/2);
  }

  private void DrawAsteroid(Graphics2D g2d,int x,int y)
  {
    ImageIcon aster = IconManager.GetAsteroid();
    aster.paintIcon(null,g2d,x-aster.getIconWidth()/2,y-aster.getIconHeight()/2);
  }

  private void DrawCity(Graphics2D g2d,Board.MapSpace city)
  {
    Board.MapSpace up = theBoard.GetUpCity(city);
    ImageIcon ii = icons.get("city");
    PaintIconRotatedAt(g2d,ii,city.GetX(),city.GetY(),0,ii.getIconHeight()/2,
                       up.GetX(),up.GetY());
  }

  private void DrawOrbitString(Graphics2D g2d,Board.MapSpace ms)
  {
    // we'll only be here if the MapSpace has an orbit.
    Board.Orbit o = ms.GetOrbit();
    int delta = 30;
    int dx = 0;
    int dy = 0;
    int vj = StringDrawer.MIDDLE; // values: TOP, MIDDLE, BOTTOM
    int hj = StringDrawer.CENTER; // values: LEFT, CENTER, RIGHT

    if      (o.GetOrientation().equals("N" )) { dy = -delta; vj=StringDrawer.BOTTOM; }
    else if (o.GetOrientation().equals("S" )) { dy = +delta; vj=StringDrawer.TOP; }
    else if (o.GetOrientation().equals("E" )) { dx = +delta; hj = StringDrawer.LEFT; }
    else if (o.GetOrientation().equals("W" )) { dx = -delta; hj = StringDrawer.RIGHT; }
    else if (o.GetOrientation().equals("NE")) { dy = -delta ; dx = + delta; hj = StringDrawer.LEFT; vj=StringDrawer.BOTTOM;}
    else if (o.GetOrientation().equals("SE")) { dy = +delta ; dx = + delta; hj = StringDrawer.LEFT; vj=StringDrawer.TOP;}
    else if (o.GetOrientation().equals("NW" )) { dy = -delta; dx = -delta; hj = StringDrawer.RIGHT; vj=StringDrawer.BOTTOM;}
    else if (o.GetOrientation().equals("SW" )) { dy = +delta; dx = -delta; hj = StringDrawer.RIGHT; vj=StringDrawer.TOP;}
    else throw new NodeInterfacePackageException("unknown orbit orientation: " + o.GetOrientation());

    String newS = o.GetName().replace(' ','\n');
    StringDrawer.DrawString(g2d,newS,vj,hj,ms.GetX()+dx,ms.GetY()+dy);
  }

  private Font origFont = null;

  public ImageIcon GetBoardIcon()
  {
    Rectangle bb = theBoard.GetBoundingBox();
    int margin = theBoard.GetMargin();
    // two items from this:
    // a) we want a 10-pixel bufffer up down left right from the 
    //    most extreme positions for the size of the icon.
    int w = bb.width+margin*2;
    int h = bb.height+margin*2;
    Image temp = new BufferedImage(w,h,BufferedImage.TYPE_4BYTE_ABGR);
    Graphics g = temp.getGraphics();
    Graphics2D  g2d = (Graphics2D)g;
    origFont = g2d.getFont();

    // make sure that all icons are loaded.
    ValidateIconLoad();

    g2d.setTransform(theBoard.GetTransform());

    MakePaths();
    // draw stars?

    // make grey outlines of solar systems
    for (Board.SolarSystem ss : theBoard.GetSolarSystems().values())
    {
      BuildGlow(g2d,ss);
    }      

    // paths
    g2d.setColor(pathColor);
    g2d.setStroke(new BasicStroke(pathWidth));
    g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);

    for (Shape s : paths)
    {
      g2d.draw(s);
    }

    // spaces
    int radius = 6;
    Iterator<Board.MapSpace> sit = theBoard.GetSpaces().values().iterator();
    while(sit.hasNext())
    {
      Board.MapSpace ms = sit.next();
      
      switch(ms.GetSpaceType())
      {
      case DOT:
        if (ms.IsNavCircle())
        {
          DrawNavCircle(g2d,ms); // should include navcircle with orbit.
        }
        else if (ms.GetOrbit() != null)
        {
          g2d.setColor(Color.CYAN);
          int boxsize = 40;
          g2d.fillRect(ms.GetX()-boxsize/2,ms.GetY()-boxsize/2,boxsize,boxsize);
          g2d.setColor(Color.WHITE);
          g2d.setStroke(new BasicStroke(1));
          g2d.drawRect(ms.GetX()-boxsize/2,ms.GetY()-boxsize/2,boxsize,boxsize);
          PaintIcon(g2d,"orbit",ms.GetX(),ms.GetY());
          g2d.setFont(origFont);

          DrawOrbitString(g2d,ms);
        }
        else 
        {
          CCircle(g2d,ColorOfColor(ms.GetColor()),ms.GetX(),ms.GetY(),radius);
        }
        break;
      case PENALTY:
        DrawPenalty(g2d,ms.GetX(),ms.GetY(),ms.GetColor(),ms.GetPenalty());
        break;
      case CITY:
        DrawCity(g2d,ms);
        break;
      case STATION:
        PaintIcon(g2d,"station",ms.GetX(),ms.GetY());
        break;
      case QBOX:
          DrawAsteroid(g2d,ms.GetX(),ms.GetY());
          g2d.setColor(new Color(0,255,255,100));
          g2d.fillRect(ms.GetX()-32,ms.GetY()-32,64,64);
          g2d.setColor(Color.WHITE);
          g2d.drawRect(ms.GetX()-32,ms.GetY()-32,64,64);
          g2d.setFont(new Font("Monospaced",Font.BOLD,20));
          StringDrawer.DrawString(g2d,"?",StringDrawer.MIDDLE,StringDrawer.CENTER,ms.GetX(),ms.GetY());
          break;

      case ASTEROID:
        DrawAsteroid(g2d,ms.GetX(),ms.GetY());
        break;

      default:
        CCircle(g2d,Color.WHITE,ms.GetX(),ms.GetY(),radius);
        break;
      }
    }

    // drop the icons on the board.
    Iterator<Board.SolarSystem> issit = theBoard.GetSolarSystems().values().iterator();
    while(issit.hasNext())
    {
      Board.SolarSystem ss = issit.next();
      for (Board.SolarSystemIcon ssi : ss.GetIcons())
      {
        PaintIcon(g2d,ssi.GetIconName(),ssi.GetX(),ssi.GetY());
      }
    }
    
    // solar system names
    g2d.setColor(Color.WHITE);
    g2d.setFont(new Font("Monospaced",Font.BOLD,14));
    Iterator<Board.SolarSystem> ssit = theBoard.GetSolarSystems().values().iterator();
    while(ssit.hasNext())
    {
      Board.SolarSystem ss = ssit.next();
      String name = ss.GetName();
      name = name.replace(' ','\n');

      if (!ss.IsHabitable())
      {
        g2d.setColor(Color.WHITE);
        g2d.setFont(new Font("Monospaced",Font.BOLD,20));
        StringDrawer.DrawString(g2d,name,StringDrawer.MIDDLE,StringDrawer.CENTER,ss.GetX(),ss.GetY());
      }
      else
      {
        int boxwidth = 100;
        int boxheight = 100;
        g2d.setColor(Color.LIGHT_GRAY);
        g2d.fillRect(ss.GetX()-boxwidth/2,ss.GetY()-boxheight/2,boxwidth,boxheight);
        g2d.setColor(Color.BLUE);
        g2d.drawRect(ss.GetX()-boxwidth/2,ss.GetY()-boxheight/2,boxwidth,boxheight);
        g2d.setColor(Color.BLACK);
        g2d.setFont(new Font("Monospaced",Font.BOLD,14));
        
        int cx;
        int cy;
        if (ss.GetHomeworld() != null)
        {
          cx = ss.GetHomeworld().GetX();
          cy = ss.GetHomeworld().GetY();
        }
        else if (ss.GetNonLoopSpaces().size() > 0)
        {
          
          cx = ss.GetNonLoopSpaces().elementAt(0).GetX();
          cy = ss.GetNonLoopSpaces().elementAt(0).GetY();
        }
        else
        {
          cx = ss.GetLoopSpace(1).GetX();
          cy = ss.GetLoopSpace(1).GetY();
        }

        ImageIcon arrow = icons.get("arrow");
        PaintIconRotatedAt(g2d,arrow,ss.GetX(),ss.GetY()-boxheight/2+20+16,
                           arrow.getIconWidth()/2,arrow.getIconHeight()/2,
                           cx,cy);
        
        StringDrawer.DrawString(g2d,"CULTURE",StringDrawer.TOP,StringDrawer.CENTER,ss.GetX(),ss.GetY()-boxheight/2+2);
        StringDrawer.DrawString(g2d,name,StringDrawer.BOTTOM,StringDrawer.CENTER,ss.GetX(),ss.GetY()+boxheight/2-2);
      }
    }

    ImageIcon ii = new ImageIcon(temp);
    return ii;
  }

  private void DrawPenalty(Graphics2D g2d,int x,int y,Board.SpaceColor sc,int cost)
  {
    ImageIcon ii = IconManager.GetPenaltyOval(sc,cost);
    ii.paintIcon(null,g2d,x-ii.getIconWidth()/2,y-ii.getIconHeight()/2);
  }

  private ImageIcon MakeAdjacencyIcon(Board.Adjacency adj,int cradius)
  {
    // minxb is the smallest distance between the radius of the circle and the widening of the arrow
    int minxb = 20;
    // h is the width of the body of the arrow.  the wings of the arrow stick out h/2 above and below
    // the body, giving a total icon height of 2 * h
    // h is also the distance from the widening of the arrow to the front, giving the arror a 45 degree angle
    int h = 20;

    int dcount = 0;
    for (int i = 1 ; i <= 6 ; ++i) { if (adj.HasPilotNumber(i)) ++dcount; }

    int xb = minxb;
    if (dcount*18 > minxb + cradius)
    {
      xb = dcount*18 - cradius;
    }

    Image temp = new BufferedImage(cradius + xb + h,h*2,BufferedImage.TYPE_4BYTE_ABGR);
    Graphics2D g = (Graphics2D)temp.getGraphics();


    int[] xpoints = new int[7];
    int[] ypoints = new int[7];
    xpoints[0] = cradius-5; ypoints[0] = h/2;
    xpoints[1] = cradius + xb; ypoints[1] = h/2;
    xpoints[2] = cradius + xb; ypoints[2] = 0;
    xpoints[3] = cradius + xb + h ; ypoints[3] = h;
    xpoints[4] = cradius + xb; ypoints[4] = h*2;
    xpoints[5] = cradius + xb; ypoints[5] = h + h/2;
    xpoints[6] = cradius-5; ypoints[6] = h + h/2;
    
    g.setColor(Color.CYAN);
    g.fillPolygon(xpoints,ypoints,7);

    // now, calculate where on the edge of the arrow the edge of the circle lies.
    double xi = Math.sqrt(cradius*cradius-(h/2)*(h/2));

    xpoints[0] = (int)xi;
    xpoints[6] = (int)xi;
    g.setColor(Color.WHITE);
    g.drawPolyline(xpoints,ypoints,7);
    ypoints[0]++;ypoints[1]++;ypoints[2]++;
    ypoints[4]--;ypoints[5]--;ypoints[6]--;
    xpoints[1]++;xpoints[2]++;xpoints[3]--;
    xpoints[4]++;xpoints[5]++;
    g.drawPolyline(xpoints,ypoints,7);

    // finally, draw the appropriate dice
    int dicey = h;
    int dicex = cradius + xb - 8;
    for (int i = 6 ; i >= 1 ; --i)
    {
      if (!adj.HasPilotNumber(i)) continue;
      String di = "die" + i;
      PaintIcon(g,di,dicex,dicey);
      dicex -= 16 + 2;
    }

    return new ImageIcon(temp);
  }
                                   
    



  private void DrawNavCircle(Graphics2D g2d,Board.MapSpace ncircle)
  {
    int radius = 40;
    int x = ncircle.GetX();
    int y = ncircle.GetY();
    
    g2d.setColor(Color.CYAN);
    g2d.fillOval(x-radius,y-radius,radius*2,radius*2);
    g2d.setStroke(new BasicStroke(2));
    g2d.setColor(Color.WHITE);
    g2d.drawOval(x-radius,y-radius,radius*2,radius*2);

    for (Board.Adjacency adj : ncircle.GetAdjacencies())
    {
      ImageIcon ai = MakeAdjacencyIcon(adj,radius);

      double nx;
      double ny;
      if (adj.GetAltVector() != null)
      {
        nx = adj.GetAltVector().getX();
        ny = adj.GetAltVector().getY();
      }
      else
      {
        Board.MapSpace up = theBoard.GetSpaces().get(adj.GetNextSpace());
        nx = up.GetX();
        ny = up.GetY();
      }

      PaintIconRotatedAt(g2d,ai,x,y,0,ai.getIconHeight()/2,(int)nx,(int)ny);
    }

    // draw orbit box if space has an orbit
    if (ncircle.GetOrbit() != null)
    {
      PaintIcon(g2d,"orbit",ncircle.GetX(),ncircle.GetY()+20);
      g2d.setFont(origFont);
      StringDrawer.DrawString(g2d,ncircle.GetOrbit().GetName(),
                              StringDrawer.MIDDLE,StringDrawer.CENTER,
                              ncircle.GetX(),ncircle.GetY()+50);
    }
  }


  private void MakePaths()
  {
    // when a special space is drawn (loop or line), put its name in here
    Set<String> drawnSpaces = new HashSet<String>();
    
    // when a path is drawn (exempting loop-loop and line-line spaces) put it here
    // format is from,to
    Set<String> drawnPaths = new HashSet<String>();
    if (paths != null) return; // paths never change, so we only have to do this once.
    paths = new Vector<Shape>();

    // three kinds of paths:
    // loop paths (draw lines between all spaces in all loops)
    Iterator<Board.SolarSystem> ssit = theBoard.GetSolarSystems().values().iterator();
    while(ssit.hasNext())
    {
      Board.SolarSystem ss = ssit.next();
      // we need to create a path for every loop space in the solar system to the next one in sequence.
      // (loop spaces are 1-based)

      int lmax = ss.GetNumLoopSpaces();
      int p0idx,p1idx,p2idx,p3idx;
      for (p0idx = lmax,p1idx = 1,p2idx = 2,p3idx = 3;
           p1idx <= lmax;
           p0idx=p0idx==lmax?1:p0idx+1,++p1idx,p2idx=p2idx==lmax?1:p2idx+1,p3idx=p3idx==lmax?1:p3idx+1)
      {
        Board.MapSpace m0 = ss.GetLoopSpace(p0idx); Point2D p0 = new Point2D.Double(m0.GetX(),m0.GetY());
        Board.MapSpace m1 = ss.GetLoopSpace(p1idx); Point2D p1 = new Point2D.Double(m1.GetX(),m1.GetY());
        Board.MapSpace m2 = ss.GetLoopSpace(p2idx); Point2D p2 = new Point2D.Double(m2.GetX(),m2.GetY());
        Board.MapSpace m3 = ss.GetLoopSpace(p3idx); Point2D p3 = new Point2D.Double(m3.GetX(),m3.GetY());
        drawnSpaces.add(m1.GetSpaceName());

        CubicCurve2D c2d = BezierInterpolate.MiddleThird(BezierInterpolate.DoBezierInterpolate(p0,p1,p2,p3));

        // we are drawing a curve between
        // p1 (map space m1) and
        // p2 (map space m2)
        //
        // if we want a vector for rendering a direction along this path, the
        // intermediate control points will make a better vector, so let's remember them.
        // 1. find the Adjacency in m1 that points to m2.
        for (Board.Adjacency adj : m1.GetAdjacencies() )
        {
          if (!adj.GetNextSpace().equals(m2.GetSpaceName())) continue;
          adj.SetAltVector(BezierInterpolate.PointOnBezier(c2d,0.5));
        }
        // 2. find the Adjacency in m2 that points to m1
        for (Board.Adjacency adj : m2.GetAdjacencies() )
        {
          if (!adj.GetNextSpace().equals(m1.GetSpaceName())) continue;
          adj.SetAltVector(BezierInterpolate.PointOnBezier(c2d,0.5));
        }

        paths.add(c2d);
      }
    }

    // line paths (draw one line between start and end of Line, and adjust coords of spaces accordingly)
    //            (mark begin->first and last->end as 'drawn' so that leftovers don't redraw them)
    for (Board.MapLine ml : theBoard.GetLines() )
    {
      paths.add(new Line2D.Double(ml.GetBeginSpace().GetX(),
                                  ml.GetBeginSpace().GetY(),
                                  ml.GetEndSpace().GetX(),
                                  ml.GetEndSpace().GetY()));

      for (int j = 0 ; j < ml.GetLineSpaces().size() ; ++j)
      {
        Board.MapSpace lspace = ml.GetLineSpaces().elementAt(j);
        drawnSpaces.add(lspace.GetSpaceName());
        if (j == 0) drawnPaths.add(ml.GetBeginSpace().GetSpaceName() + "," +
                                   lspace.GetSpaceName());
        if (j == ml.GetLineSpaces().size()-1)
          drawnPaths.add(ml.GetEndSpace().GetSpaceName() + "," + lspace.GetSpaceName());
      }
    }

    // leftover paths (all spaces that are neither on loops nor lines.
    Iterator<Board.MapSpace> sit = theBoard.GetSpaces().values().iterator();
    while(sit.hasNext())
    {
      Board.MapSpace ms = sit.next();
      if (drawnSpaces.contains(ms.GetSpaceName())) continue;

      for (Board.Adjacency adj : ms.GetAdjacencies() )
      {
        if (adj.IsFlyable()) continue;
        if (drawnPaths.contains(adj.GetNextSpace() + "," + ms.GetSpaceName())) continue;
        Board.MapSpace dest = theBoard.GetSpaces().get(adj.GetNextSpace());

        if (!adj.IsInvisible())
        {
          paths.add(new Line2D.Double(ms.GetX(),
                                      ms.GetY(),
                                      dest.GetX(),
                                      dest.GetY()));
        }
        drawnPaths.add(ms.GetSpaceName() + "," + adj.GetNextSpace());
      }
    }
  }

  private void ValidateIconLoad()
  {
    Iterator<Board.SolarSystem> ssit = theBoard.GetSolarSystems().values().iterator();
    while(ssit.hasNext())
    {
      Board.SolarSystem ss = ssit.next();
      for (Board.SolarSystemIcon ssi : ss.GetIcons() )
      {
        if (icons.containsKey(ssi.GetIconName())) continue;
        icons.put(ssi.GetIconName(),ImageLoader.MakeImageIcon(theDir + "/" + ssi.GetIconName()));
      }
    }
  }

  private final double count = 16.0;
  private final double radius = 45.0;

  private void AddPointSites(Vector<Voronoi.Point> sites,int x,int y)
  {
    double dtheta = 2 * Math.PI / count;
    for (double theta = 0.0 ; theta < 2 * Math.PI ; theta += dtheta)
    {
      Voronoi.Point cir = new Voronoi.Point(x + radius * Math.cos(theta),
                                            y + radius * Math.sin(theta));
      sites.add(cir);
    }
  }

  private void BuildGlow(Graphics2D g, Board.SolarSystem ss)
  {
    // steps:
    // 1. for each space in this solar system, add a Vector of Voronoi.Point objects
    //    of points surrounding the space in a ring.
    Vector<Voronoi.Point> sites = new Vector<Voronoi.Point>();
    
    for (Board.MapSpace ms : ss.GetNonLoopSpaces())
    {
      AddPointSites(sites,ms.GetX(),ms.GetY());
    }
    for (int i = 1 ; i <= ss.GetNumLoopSpaces() ; ++i)
    {
      Board.MapSpace ms = ss.GetLoopSpace(i);
      AddPointSites(sites,ms.GetX(),ms.GetY());
    }

    // 2. call Fortune's algorithm on those points.
    Fortune f = new Fortune(sites);
    // 3. cull the edges such that only those whose min alpha and max alpha contain
    //    the solar system's alpha remain.
    // 4. build a polygon out of the remaining edges (error if the edges do not make a 
    //    single closed polygon, i.e. all points mentioned are mentioned exactly twice, and they
    //    all link up.
    PolyMaker pm = new PolyMaker();
    for (Voronoi.Edge e : f.GetEdges())
    {
      if (!e.InAlphaBounds(ss.GetAlpha())) continue;
      pm.AddEdge((int)Math.round(e.site1.x),(int)Math.round(e.site1.y),(int)Math.round(e.site2.x),(int)Math.round(e.site2.y));
    }

    // 5. draw filled polygon in a translucent bluish white.
    Polygon p = pm.GetPolygon();
    if (p == null)
    {
      System.out.println("Solar System " + ss.GetName() + " can't make a glow!");
      return;
    }

    g.setColor(new Color(0xCC,0xCC,0xFF,60));
    g.fill(p);
    Stroke s = g.getStroke();
    g.setStroke(new BasicStroke(10,BasicStroke.CAP_ROUND,BasicStroke.JOIN_ROUND));
    g.draw(p);
    g.setStroke(s);
  }
}
