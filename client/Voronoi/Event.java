package Voronoi;

public class Event implements Comparable<Event>
{
  // type a. 'site' (i.e. an input point)
  public Point site = null;

  // type b. a circle entry
  public TreeNode pNode = null;
  public TreeNode arcNode = null;
  public TreeNode nNode = null;
  public Point center;
  public Point critical;
  public boolean valid;

  public Event(Point p) { site = p; }
  public Event(TreeNode pn,TreeNode node,TreeNode nn,Point ctr,Point crit) 
  { 
    pNode = pn;
    arcNode = node; 
    nNode = nn;
    center = ctr ; 
    critical = crit;
    valid = true;
  }
    

  // sort by x coordinate.
  // same x coordinate, ties broken by circle first
  // ties by type broken by y coordinate. 

  public int compareTo(Event other)
  {
    boolean isSite = site != null;
    boolean isOSite = other.site != null;
    Point p = isSite ? site : critical;
    Point op = isOSite ? other.site : other.critical;

    if (!MathUtilities.ddequals(p.x,op.x)) return Double.compare(p.x,op.x);
    // if they are not the same, then the one that is the circle should be first.
    if (isSite != isOSite) return isSite ? 1 : -1;
    if (!MathUtilities.ddequals(p.y,op.y)) return Double.compare(p.y,op.y);
    return 0;
  }
    


  /*
    if (site != null && other.site != null) return site.compareTo(other.site);
    if (critical != null && other.critical != null) return critical.compareTo(other.critical);
    if (site != null && other.critical != null) return site.compareTo(other.critical);
    return critical.compareTo(other.site);
  }
  */
}
