package Voronoi;

public class TreeNode
{
  public TreeNode parent = null;
  public TreeNode left = null;
  public TreeNode right = null;

  // used if this is a breakpoint
  public Point leftarcPoint = null;
  public Point rightarcPoint = null;
  public Edge edge = null;
    
  // used if this is an arc
  public Point arcPoint = null;
  public Event circleEvent = null;

  public TreeNode(Point left, Point right,Edge e)
  {
    leftarcPoint = left;
    rightarcPoint = right;
    edge = e;
  }

  public TreeNode(Point arc)
  {
    arcPoint = arc;
  }

  // call this only if
  // a) this tree node is a breakpoint node
  // b) neither point is on sweep.
  public Point BreakPoint(double sweep)
  {
    Point result = MathUtilities.GetParabolaIntersection(leftarcPoint,rightarcPoint,sweep);
    return result;
  }

  // finds the arc TreeNode that this y-coord lands on, given the sweep x coord)
  public TreeNode FindNode(double y,double sweep)
  {
    // if we're on an arc, return it
    if (arcPoint != null) return this;
    
    // if we're here, we are on a breakpoint node.
    // if both nodes are on sweep, descend right.
    if (leftarcPoint.x == sweep && rightarcPoint.x == sweep) return right.FindNode(y,sweep);
    
    // if one is on sweep, use that y coordinate as separator (and lean towards ending up
    // on the arc in question, if it matches)
    if (leftarcPoint.x == sweep)
    {
      if (y > leftarcPoint.y) return right.FindNode(y,sweep);
      else return left.FindNode(y,sweep);
    }
      
    if (rightarcPoint.x == sweep)
    {
      if (y < rightarcPoint.y) return left.FindNode(y,sweep);
      else return right.FindNode(y,sweep);
    }

    Point bp = BreakPoint(sweep);

    if (y <= bp.y) return left.FindNode(y,sweep);
    return right.FindNode(y,sweep);
  }
}
