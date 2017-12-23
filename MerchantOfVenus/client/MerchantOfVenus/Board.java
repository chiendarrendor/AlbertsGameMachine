package MerchantOfVenus;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;  
import javax.xml.parsers.ParserConfigurationException;
 
import org.xml.sax.SAXException;  
import java.io.*;
import java.net.*;

import org.w3c.dom.Document;
import java.util.*;

import NodeInterfacePackage.*;
import DOMUtilities.*;

import java.util.regex.*;
import java.awt.Rectangle;
import java.awt.geom.*;


public class Board
{
  public enum SpaceType { DOT,ASTEROID,QBOX,PENALTY,CITY,STATION,TELEGATE};
  public enum SpaceColor { BLUE,RED,YELLOW };

  public class SolarSystemIcon
  {
    private String iconName;
    private int xpos;
    private int ypos;
    private int radius;
    private boolean isHomeworld;

    public SolarSystemIcon(String n,int x,int y,int r,boolean hw)
    {
      iconName = n;
      xpos = x;
      ypos = y;
      radius = r;
      isHomeworld = hw;
    }
    public String GetIconName() { return iconName; }
    public int GetX() { return xpos; }
    public int GetY() { return ypos; }
    public int GetR() { return radius; }
    public boolean IsHomeworld() { return isHomeworld; }
  }

  public class SolarSystem
  {
    private String name;
    private int xpos;
    private int ypos;
    private boolean habitable;
    private double alpha;
    private Vector<SolarSystemIcon> icons;
    private SolarSystemIcon homeworld;
    private Map<Integer,MapSpace> loopSpaces;
    private Vector<MapSpace> nonLoopSpaces;
    public SolarSystem(String ssname,int x,int y,boolean ishabitable,
                       double a)
    {
      name = ssname;
      xpos = x;
      ypos = y;
      alpha = a;

      if (xpos < minX) minX = xpos;
      if (ypos < minY) minY = ypos;
      if (xpos > maxX) maxX = xpos;
      if (ypos > maxY) maxY = ypos;

      habitable = ishabitable;
      loopSpaces = new HashMap<Integer,MapSpace>();
      nonLoopSpaces = new Vector<MapSpace>();
      icons = new Vector<SolarSystemIcon>();
      homeworld = null;
    }
    public String GetName() { return name; }
    public int GetX() { return xpos; } 
    public int GetY() { return ypos; }
    public double GetAlpha() { return alpha; }
    public boolean IsHabitable() { return habitable; }
    public void AddLoopSpace(MapSpace ms,int id) { loopSpaces.put(id,ms); }
    public void AddNonLoopSpace(MapSpace ms) { nonLoopSpaces.add(ms); }
    public int GetNumLoopSpaces() { return loopSpaces.size(); }
    public MapSpace GetLoopSpace(int idx) { return loopSpaces.get(idx); }
    public Vector<MapSpace> GetNonLoopSpaces() { return nonLoopSpaces; }

    public void AddIcon(SolarSystemIcon ss) 
    { 
      icons.add(ss); 
      if (ss.IsHomeworld()) homeworld = ss;
    }
    public Vector<SolarSystemIcon> GetIcons() { return icons; }
    public SolarSystemIcon GetHomeworld() { return homeworld; }

    public void LandCities()
    {
      SolarSystemIcon ssi = GetHomeworld();
      if (ssi == null) return;

      double radius = ssi.GetR() - 2;

      // look in non-loop spaces for cities.
      for (MapSpace city : nonLoopSpaces)
      {
        if (city.GetSpaceType() != SpaceType.CITY) continue;
        MapSpace up = GetUpCity(city);

        // for each city, recalculate its position to be 
        // radius out from the center of the homeworld icon,
        // along a line from it to its singular non-flyable adjacency.
        double dx = up.GetX() - ssi.GetX();
        double dy = up.GetY() - ssi.GetY();

        double ratio = radius / Math.sqrt(dy*dy + dx*dx);
        double nx = ssi.GetX() + dx * ratio;
        double ny = ssi.GetY() + dy * ratio;

        city.SetPos((int)nx,(int)ny);
      }
    }

    public void LinkLoop()
    {
      int maxloop = loopSpaces.size();
      for (int i = 1 ; i <= maxloop ; ++i)
      {
        MapSpace curSpace = loopSpaces.get(i);
        MapSpace prevSpace = (i == 1) ? loopSpaces.get(maxloop) : loopSpaces.get(i-1);
        MapSpace nextSpace = (i == maxloop) ? loopSpaces.get(1) : loopSpaces.get(i+1);

        curSpace.AddAdjacency(new Adjacency(prevSpace.GetSpaceName(),false,null,false));
        curSpace.AddAdjacency(new Adjacency(nextSpace.GetSpaceName(),false,null,false));
      }        
    }
  }

  public class Adjacency
  {
    private String nextSpace;
    private boolean isFlyable;
    private Set<Integer> pilotNumbers;
    private boolean isInvisible;
    private Point2D altVector;

    public Adjacency(String nsp,boolean flyable,String pnumberstring,boolean inv)
    {
      nextSpace = nsp;
      isFlyable = flyable;
      isInvisible = inv;
      altVector = null;
      pilotNumbers = new HashSet<Integer>();

      if (pnumberstring != null && pnumberstring.length() > 0)
      {
        String[] pnums = pnumberstring.split(",");
        for (String pnum : pnums)
        {
          pilotNumbers.add(new Integer(Integer.parseInt(pnum)));
        }
      }
    }

    public void SetAltVector(Point2D p) { altVector = p; }
    public Point2D GetAltVector() { return altVector; }

    public String GetNextSpace() { return nextSpace; }
    public boolean IsFlyable() { return isFlyable; }
    public boolean HasPilotNumbers() { return pilotNumbers.size() > 0; }
    public boolean HasPilotNumber(int pnum) { return pilotNumbers.contains(new Integer(pnum)); }
    public boolean IsInvisible() { return isInvisible; }
  }

  public class Orbit
  {
    private String name;
    private String orient;
    public Orbit(String n,String o) { name = n ; orient = o; }
    public String GetName() { return name; }
    public String GetOrientation() { return orient; }
  }

  public class MapSpace
  {
    private String regionName;
    private String spaceName;
    private SpaceType type;
    private Orbit orbit;
    private SpaceColor color;
    private int cost;
    private int gateid;
    private Vector<Adjacency> adjacencies;
    private String secrecy;
    private int xpos;
    private int ypos;

    public MapSpace(String rName,String sName,SpaceType stype,
                    SpaceColor c,int pCost,int gid,int x,int y)
    {
      regionName = rName;
      spaceName = sName;
      type = stype;
      orbit = null;
      color = c;
      cost = pCost;
      gateid = gid;
      secrecy = "public";
      xpos = x;
      ypos = y;

      if (xpos < minX) minX = xpos;
      if (ypos < minY) minY = ypos;
      if (xpos > maxX) maxX = xpos;
      if (ypos > maxY) maxY = ypos;

      adjacencies = new Vector<Adjacency>();
    }

    public boolean equals(Object other)
    {
      MapSpace ms = (MapSpace)other;
      return GetSpaceName().equals(ms.GetSpaceName());
    }

    public String GetRegionName() { return regionName; }
    public String GetSpaceName() { return spaceName; }
    
    public void SetOrbit(Orbit o) { orbit = o; }
    public Orbit GetOrbit() { return orbit; }
    public SpaceType GetSpaceType() { return type; }
    public SpaceColor GetColor() { return color; }
    public int GetPenalty() { return cost; }
    public int GetGateId() { return gateid; }
    public int GetX() { return xpos; }
    public int GetY() { return ypos; }
    public void SetPos(int x,int y) { xpos = x; ypos = y; }
    public boolean IsPublic() { return secrecy.equals("public"); }
    public boolean PlayerKnows(int playerId) 
    {
      if (IsPublic()) return true;
      if (secrecy.length() <= playerId) return false;
      return secrecy.charAt(playerId) == '1';
    }
    public void AddAdjacency(Adjacency adj)
    {
      for (Adjacency ladj : adjacencies)
      {
        if (ladj.GetNextSpace().equals(adj.GetNextSpace())) return;
      }
      adjacencies.add(adj);
    }
    public Vector<Adjacency> GetAdjacencies() { return adjacencies; }

    public boolean IsNavCircle()
    {
      for (Adjacency adj : adjacencies)
      {
        if (adj.HasPilotNumbers()) return true;
      }
      return false;
    }
  }

  public class MapLine
  {
    private String lineName;
    private String beginName;
    private String endName;
    private Vector<MapSpace> lineSpaces;
    private MapSpace beginSpace;
    private MapSpace endSpace;

    public MapSpace GetBeginSpace() { return beginSpace; }
    public MapSpace GetEndSpace() { return endSpace; }
    public Vector<MapSpace> GetLineSpaces() { return lineSpaces; }

    public MapLine(String lName,String bName,String eName)
    {
      lineName = lName;
      beginName = bName;
      endName = eName;

      lineSpaces = new Vector<MapSpace>();
    }

    public String GetLineName() { return lineName; }
    
    public void AddSpace(MapSpace m)
    {
      lineSpaces.add(m);
    }

    public void LinkSpaces()
    {
      for (int i = 0 ; i < lineSpaces.size() ; ++i)
      {
        Adjacency adj;
        if (i == 0) { adj = new Adjacency(beginName,false,"",false); }
        else { adj = new Adjacency(lineSpaces.elementAt(i-1).GetSpaceName(),false,"",false); }
        lineSpaces.elementAt(i).AddAdjacency(adj);

        if (i == lineSpaces.size() - 1) { adj = new Adjacency(endName,false,"",false); }
        else { adj = new Adjacency(lineSpaces.elementAt(i+1).GetSpaceName(),false,"",false); }
        lineSpaces.elementAt(i).AddAdjacency(adj);
      }
    }

    public void ProcessPoints()
    {
      beginSpace = spaces.get(beginName);
      endSpace = spaces.get(endName);

      double p1x = beginSpace.GetX();
      double p1y = beginSpace.GetY();
      double p2x = endSpace.GetX();
      double p2y = endSpace.GetY();
      double denom = (p2y-p1y) * (p2y-p1y) + (p2x-p1x) * (p2x-p1x);

      for (MapSpace cs : lineSpaces)
      {
        double p3x = cs.GetX();
        double p3y = cs.GetY();

        double u = ((p3x - p1x)*(p2x-p1x) + (p3y - p1y)*(p2y-p1y))/denom;
        double nx = p1x + u * (p2x-p1x);
        double ny = p1y + u * (p2y-p1y);
        cs.SetPos((int)nx,(int)ny);
      }
    }
  }

  private int minX = Integer.MAX_VALUE;
  private int minY = Integer.MAX_VALUE;
  private int maxX = Integer.MIN_VALUE;
  private int maxY = Integer.MIN_VALUE;

  public Rectangle GetBoundingBox()
  {
    return new Rectangle(minX,minY,maxX-minX,maxY-minY);
  }
  
  private Map<String,SolarSystem> solarsystems;
  private Map<String,MapSpace> spaces;
  private Vector<MapLine> lines;
  private AffineTransform affine;
  private static int margin = 60;

  public Map<String,MapSpace> GetSpaces() { return spaces; }
  public Map<String,SolarSystem> GetSolarSystems() { return solarsystems; }
  public Vector<MapLine> GetLines() { return lines; }

  public AffineTransform GetTransform() { return affine; }
  public int GetMargin() { return margin; }


  public MapSpace GetUpCity(MapSpace city)
  {
    if (city.GetSpaceType() != SpaceType.CITY)
      throw new NodeInterfacePackageException("can't call GetUpCity on a non-city!");

    Adjacency found = null;
    for (Adjacency adj : city.GetAdjacencies())
    {
      if (adj.IsFlyable()) 
      {
        continue;
      }
      found = adj;
      break;
    }

    if (found == null) 
      throw new NodeInterfacePackageException("city " + city.GetSpaceName() + 
                                              " has no non-flyable adjacencies!");

    return GetSpaces().get(found.GetNextSpace());
  }


  public Board(String i_BoardDir)
  {
    solarsystems = new HashMap<String,SolarSystem>();
    spaces = new HashMap<String,MapSpace>();
    lines = new Vector<MapLine>();

    String fname = i_BoardDir + "/MerchantOfVenusMap.xml";

    try
    {
      DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
      DocumentBuilder builder = factory.newDocumentBuilder();
      Document board = builder.parse(fname);
      org.w3c.dom.Node root = board.getChildNodes().item(0);
      if (!root.getNodeName().equals("board")) 
        throw new NodeInterfacePackageException("root is not 'board'");

      DOMChildIterator chit = new DOMChildIterator(root);
      org.w3c.dom.Node child;
      while((child = chit.GetNext()) != null)
      {
        if (child.getNodeName() == "#text" || child.getNodeName() == "#comment") continue;
        else if (child.getNodeName().equals("line")) ProcessLine(child);
        else if (child.getNodeName().equals("solarsystem")) ProcessSolarSystem(child);
        else
          throw new NodeInterfacePackageException("unknown root child: " + child.getNodeName());
      }
    }
    catch(UnknownHostException ex) { throw new NodeInterfacePackageException(ex.toString()); }
    catch(MalformedURLException ex) { throw new NodeInterfacePackageException(ex.toString()); }
    catch(SAXException ex) { throw new NodeInterfacePackageException(ex.toString()); }
    catch(ParserConfigurationException ex) 
    { 
      throw new NodeInterfacePackageException(ex.toString()); 
    }
    catch(IOException ex) { throw new NodeInterfacePackageException(ex.toString()); }

    for(MapLine ml : lines)
    {
      ml.ProcessPoints();
    }

    // creating an Affine Transform representing a shift of the origin
    // such that the upper left corner of the bounding box of all spaces is moved
    // to (margin,margin)
    Rectangle bb = GetBoundingBox();
    affine = AffineTransform.getTranslateInstance(margin-bb.x,margin-bb.y);



  }

  private void ProcessLine(org.w3c.dom.Node lineNode)
  {
    String begin = "";
    String end = "";
    String lineName = DOMUtil.GetAttributeWithName(lineNode,"name");

    DOMChildIterator dch = new DOMChildIterator(lineNode);
    org.w3c.dom.Node child;
    while((child=dch.GetNext()) != null)
    {
      if (child.getNodeName() == "#text" || child.getNodeName() == "#comment") continue;
      if (child.getNodeName() == "begin") begin = DOMUtil.GetAttributeWithName(child,"name");
      if (child.getNodeName() == "end") end = DOMUtil.GetAttributeWithName(child,"name");
    }
    
    MapLine result = new MapLine(lineName,begin,end);
    lines.add(result);
    dch = new DOMChildIterator(lineNode);
    while((child=dch.GetNext()) != null)
    {
      if (child.getNodeName() == "#text" || child.getNodeName() == "#comment") continue;
      if (child.getNodeName() == "begin" || child.getNodeName() == "end") continue;
      MapSpace newSpace = ProcessSpace(lineName,child);
      result.AddSpace(newSpace);
    }
    result.LinkSpaces();
  }

  private void ProcessSolarSystem(org.w3c.dom.Node ssNode)
  {
    AttributeChecker ach = new AttributeChecker();
    ach.AddAttribute("name",true);
    ach.AddAttribute("x",true);
    ach.AddAttribute("y",true);
    ach.AddAttribute("inhabited",false);
    ach.AddAttribute("ishabitable",false);
    ach.AddAttribute("alpha",false);
    ach.ValidateAttributes(ssNode);
    String ssname = DOMUtil.GetAttributeWithName(ssNode,"name");
    int x = Integer.parseInt(DOMUtil.GetAttributeWithName(ssNode,"x"));
    int y = Integer.parseInt(DOMUtil.GetAttributeWithName(ssNode,"y"));
    double alpha = 400;
    if (DOMUtil.GetAttributeWithName(ssNode,"alpha") != null)
    {
      alpha = Double.parseDouble(DOMUtil.GetAttributeWithName(ssNode,"alpha"));
    }

    boolean habitable = 
      ((DOMUtil.GetAttributeWithName(ssNode,"inhabited") != null &&
        DOMUtil.GetAttributeWithName(ssNode,"inhabited").equals("y")) ||
       (DOMUtil.GetAttributeWithName(ssNode,"ishabitable") != null &&
        DOMUtil.GetAttributeWithName(ssNode,"ishabitable").equals("y")));

    SolarSystem result = new SolarSystem(ssname,x,y,habitable,alpha);
    solarsystems.put(ssname,result);

    DOMChildIterator dch = new DOMChildIterator(ssNode);
    org.w3c.dom.Node child;
    while((child=dch.GetNext()) != null)
    {
      if (child.getNodeName() == "#text" || child.getNodeName() == "#comment") continue;
      if (child.getNodeName() == "icon") 
      {
        result.AddIcon(ProcessIcon(child));
      }
      else if (child.getNodeName() == "space") 
      {
        MapSpace newSpace = ProcessSpace(ssname,child);

        Pattern pattern = Pattern.compile("\\S+_loop_(\\d+)");
        Matcher matcher = pattern.matcher(newSpace.GetSpaceName());
        if (matcher.matches())
        {
          result.AddLoopSpace(newSpace,Integer.parseInt(matcher.group(1)));
        }
        else
        {
          result.AddNonLoopSpace(newSpace);
        }
      }
      else
      {
        throw new NodeInterfacePackageException("unknown solar system child " + child.getNodeName());
      }
    }
    result.LinkLoop();
    result.LandCities();
  }

  private SpaceType ParseType(String t)
  {
    if (t.equals("dot")) return SpaceType.DOT;
    if (t.equals("asteroid")) return SpaceType.ASTEROID;
    if (t.equals("qbox")) return SpaceType.QBOX;
    if (t.equals("penalty")) return SpaceType.PENALTY;
    if (t.equals("city")) return SpaceType.CITY;
    if (t.equals("station")) return SpaceType.STATION;
    throw new NodeInterfacePackageException("unknown space type: " + t);
  }

  private SpaceColor ParseColor(String c)
  {
    if (c == null) return SpaceColor.BLUE;
    if (c.equals("blue")) return SpaceColor.BLUE;
    if (c.equals("red")) return SpaceColor.RED;
    if (c.equals("yellow")) return SpaceColor.YELLOW;
    return SpaceColor.BLUE;
  }

  private SolarSystemIcon ProcessIcon(org.w3c.dom.Node iconNode)
  {
    AttributeChecker ach = new AttributeChecker();
    ach.AddAttribute("name",true);
    ach.AddAttribute("x",true);
    ach.AddAttribute("y",true);
    ach.AddAttribute("homeworld",false);
    ach.AddAttribute("radius",false);
    ach.ValidateAttributes(iconNode);

    String hws = DOMUtil.GetAttributeWithName(iconNode,"homeworld");
    boolean hw = hws != null && hws.equals("y");

    int radius = 0;
    if (DOMUtil.GetAttributeWithName(iconNode,"radius") != null)
      radius = Integer.parseInt(DOMUtil.GetAttributeWithName(iconNode,"radius"));


    if (radius == 0 && hw)
    {
      throw new NodeInterfacePackageException("homeworld icon node must have radius");
    }

    return new SolarSystemIcon(DOMUtil.GetAttributeWithName(iconNode,"name"),
                               Integer.parseInt(DOMUtil.GetAttributeWithName(iconNode,"x")),
                               Integer.parseInt(DOMUtil.GetAttributeWithName(iconNode,"y")),
                               radius,hw);
  }
    
    


  private MapSpace ProcessSpace(String regionName,org.w3c.dom.Node spaceNode)
  {
    AttributeChecker ach = new AttributeChecker();
    ach.AddAttribute("name",true);
    ach.AddAttribute("type",true);
    ach.AddAttribute("x",true);
    ach.AddAttribute("y",true);
    ach.AddAttribute("color",false);
    ach.AddAttribute("cost",false);
    ach.ValidateAttributes(spaceNode);

    SpaceType t = ParseType(DOMUtil.GetAttributeWithName(spaceNode,"type"));
    MapSpace result;
    switch(t)
    {
    case CITY:
    case QBOX:
    case ASTEROID:
    case STATION:
      ach = new AttributeChecker();
      ach.AddAttribute("name",true);
      ach.AddAttribute("type",true);
      ach.AddAttribute("x",true);
      ach.AddAttribute("y",true);
      ach.ValidateAttributes(spaceNode);
      result = new MapSpace(regionName,
                            DOMUtil.GetAttributeWithName(spaceNode,"name"),
                            t,SpaceColor.BLUE,0,0,
                            Integer.parseInt(DOMUtil.GetAttributeWithName(spaceNode,"x")),
                            Integer.parseInt(DOMUtil.GetAttributeWithName(spaceNode,"y")));
      break;
    case DOT:
      ach = new AttributeChecker();
      ach.AddAttribute("name",true);
      ach.AddAttribute("type",true);
      ach.AddAttribute("x",true);
      ach.AddAttribute("y",true);
      ach.AddAttribute("color",false);
      ach.ValidateAttributes(spaceNode);
      result = new MapSpace(regionName,
                            DOMUtil.GetAttributeWithName(spaceNode,"name"),
                            t,
                            ParseColor(DOMUtil.GetAttributeWithName(spaceNode,"color")),
                            0,0,
                            Integer.parseInt(DOMUtil.GetAttributeWithName(spaceNode,"x")),
                            Integer.parseInt(DOMUtil.GetAttributeWithName(spaceNode,"y")));
      break;
    case PENALTY:
      ach = new AttributeChecker();
      ach.AddAttribute("name",true);
      ach.AddAttribute("type",true);
      ach.AddAttribute("x",true);
      ach.AddAttribute("y",true);
      ach.AddAttribute("color",false);
      ach.AddAttribute("cost",false);
      ach.ValidateAttributes(spaceNode);
      result = new MapSpace(regionName,
                            DOMUtil.GetAttributeWithName(spaceNode,"name"),
                            t,
                            ParseColor(DOMUtil.GetAttributeWithName(spaceNode,"color")),
                            Integer.parseInt(DOMUtil.GetAttributeWithName(spaceNode,"cost")),
                            0,
                            Integer.parseInt(DOMUtil.GetAttributeWithName(spaceNode,"x")),
                            Integer.parseInt(DOMUtil.GetAttributeWithName(spaceNode,"y")));
      break;
    default:
      throw new NodeInterfacePackageException("unknown space type: " + t);
    }
    spaces.put(result.GetSpaceName(),result);
    
    DOMChildIterator dch = new DOMChildIterator(spaceNode);
    org.w3c.dom.Node child;
    while((child=dch.GetNext()) != null)
    {
      if (child.getNodeName() == "#text" || child.getNodeName() == "#comment") continue;

      if (child.getNodeName() == "adjacent") ProcessAdjacent(result,child);
      else if (child.getNodeName() == "orbit") ProcessOrbit(result,child);
      else throw new NodeInterfacePackageException("unknown space child: " + child.getNodeName());

    }

    return result;
  }

  private void ProcessAdjacent(MapSpace space,org.w3c.dom.Node adj)
  {
    AttributeChecker ach = new AttributeChecker();
    ach.AddAttribute("dest",true);
    ach.AddAttribute("flyable",false);
    ach.AddAttribute("pnumbers",false);
    ach.AddAttribute("invisible",false);
    ach.ValidateAttributes(adj);

    String fstring = DOMUtil.GetAttributeWithName(adj,"flyable");
    boolean flyable = fstring != null && fstring.equals("y");

    String istring = DOMUtil.GetAttributeWithName(adj,"invisible");
    boolean invisible  = istring != null && istring.equals("y");

    Adjacency result = new Adjacency(DOMUtil.GetAttributeWithName(adj,"dest"),
                                     flyable,
                                     DOMUtil.GetAttributeWithName(adj,"pnumbers"),
                                     invisible);
    space.AddAdjacency(result);
  }

  private void ProcessOrbit(MapSpace space,org.w3c.dom.Node orbit)
  {
    AttributeChecker ach = new AttributeChecker();
    ach.AddAttribute("name",true);
    ach.AddAttribute("orient",false);
    ach.ValidateAttributes(orbit);

    String orient="E";
    if (DOMUtil.GetAttributeWithName(orbit,"orient") != null)
      orient = DOMUtil.GetAttributeWithName(orbit,"orient");
    
    Orbit newo = new Orbit(DOMUtil.GetAttributeWithName(orbit,"name"),orient);
    space.SetOrbit(newo);
  }
}

      