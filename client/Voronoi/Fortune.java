package Voronoi;

import java.util.*;

public class Fortune
{
  private Vector<Edge> edges = new Vector<Edge>();
  private Tree tree = new Tree();
  private PriorityQueue<Event> queue = new PriorityQueue<Event>();
  private double sweepx;
  private IEventReporter reporter = null;

  public Vector<Edge> GetEdges() { return edges; }

  public Fortune(Vector<Point> sites)
  {
    for (Point site : sites)
    {
      queue.add(new Event(site));
    }

    while(!queue.isEmpty())
    {
      Event ev = queue.poll();
      if (ev.site != null)
      {
        sweepx = ev.site.x;
        HandleSite(ev.site);
      }
      else
      {
        sweepx = ev.critical.x;
        HandleCircle(ev);
      }
    }

    FinalizeHalfLines(tree.root);
  }

  private void HandleSite(Point site)
  {
    if (reporter != null) reporter.ProcessSite(site);
    // if tree is empty, make one.
    if (tree.Empty())
    {
      tree.InsertFirstSite(site);
      return;
    }
    
    // otherwise, find arc in beachline for this y.
    TreeNode vn = tree.FindArc(site.y,sweepx);

    // several cases.  
    // a) node found has same x and y as site...drop this site.
    if (MathUtilities.ddequals(vn.arcPoint.x,site.x) && MathUtilities.ddequals(vn.arcPoint.y,site.y)) 
    {
      if (reporter != null) reporter.DuplicatePoint(site);
      return;
    }

    // ok, we're going to insert this site, and therefore replace vn.
    // let's invalidate vn's circle event, if it has one.
    if (vn.circleEvent != null) 
    {
      if (reporter != null) reporter.CircleInvalidated(vn.circleEvent.pNode.arcPoint,
                                                       vn.circleEvent.arcNode.arcPoint,
                                                       vn.circleEvent.nNode.arcPoint);
      vn.circleEvent.valid = false;
    }

    // b) node found has same x (sweep) as us...we go after and parallel.
    // this situation only arises if the sweep line hits these points as the very first
    // sites in the algorithm.  As such, they need a specially calculated
    // exemplar, because the beachline can never see this edge prior to the
    // sweep line start.
    if (MathUtilities.ddequals(vn.arcPoint.x,site.x))
    {
      Edge e = new Edge(vn.arcPoint,site); 
      edges.add(e);
      // given that, since the two points have the same x coordinates and
      // different y coordinates, we choose an x coord back <-- that way (negative x) some
      // and the y coordinate is the midpoint of the two site y's
      e.SetExemplar(new Point(site.x-MathUtilities.MARGIN,(site.y+vn.arcPoint.y)/2.0));
      // we use the special AppendParallelSite to join the new arc to the existing beachfront
      if (reporter != null) 
      {
        reporter.EdgeCreated(e);
        reporter.EdgeExemplared(e);
      }
      
      tree.AppendParallelSite(vn,site,e);
    }

    // this is the normal case.  we replace the interrupted arc with:
    // vn <vn,site> site <site,vn> vn
    else
    {
      Edge e = new Edge(vn.arcPoint,site);
      edges.add(e);

      if (reporter != null) reporter.EdgeCreated(e);
      
      TreeNode siteNode = tree.MakeNewArc(vn,site,e);

      // finally, we check for creation of circle events.
      TreeNode pred = tree.Predecessor(siteNode);
      TreeNode p2 = tree.Predecessor(pred);
      TreeNode succ = tree.Successor(siteNode);
      TreeNode s2 = tree.Successor(succ);

      CreateCircleEvent(p2,pred,siteNode);
      CreateCircleEvent(siteNode,succ,s2);
    }
  }

  private void CreateCircleEvent(TreeNode pnode,TreeNode cnode,TreeNode nnode)
  {
    if (pnode == null || nnode == null) return;

    if (!MathUtilities.Converges(pnode.arcPoint,cnode.arcPoint,nnode.arcPoint)) return;

    Point center = MathUtilities.GetCircleCenter(pnode.arcPoint,cnode.arcPoint,nnode.arcPoint);
    if (center == null) return;
    
    double radius = MathUtilities.D(cnode.arcPoint,center);

    // critical point is the right-most point of the circle, and is where this circle event
    // will be sorted in the priority queue
    Point crit = new Point(center.x+radius,center.y);

    // one final check... if the circle's critical point is already behind the sweep line,
    // we leave it out.
    if (crit.x < sweepx) return;

    Event circleEvent = new Event(pnode,cnode,nnode,center,crit);

    if (cnode.circleEvent != null) 
    {
      if (reporter != null) reporter.CircleInvalidated(cnode.circleEvent.pNode.arcPoint,
                                                       cnode.circleEvent.arcNode.arcPoint,
                                                       cnode.circleEvent.nNode.arcPoint);
      cnode.circleEvent.valid = false;
    }
    cnode.circleEvent = circleEvent;
    if (reporter != null) reporter.CircleCreated(pnode.arcPoint,cnode.arcPoint,nnode.arcPoint);
   
    queue.add(circleEvent);
  }

  private void HandleCircle(Event cev)
  {
    // list of things to do.
    // 0) ignore this event if it has been marked invalid
    // a) remove from the Tree the event's node, and the break point to left and right
    // b) replace all that with a breakpoint between left neighbor and right neighbor
    // c) inform the edges owned by the breakpoints removed in a) that they have a terminus at circle center
    // d) inform the new edge owned by the new breakpoint that it has a terminus at circle center
    // e) check circle creation for (left.pred,left,right) and (left,right,right,succ)

   
    if (!cev.valid) return;

    TreeNode arcNode = cev.arcNode;

    // by definition, we're here because we have a predecessor and a successor...
    TreeNode pnode = tree.Predecessor(arcNode);
    TreeNode snode = tree.Successor(arcNode);

    if (reporter != null) reporter.HandleCircle(pnode.arcPoint, arcNode.arcPoint, snode.arcPoint);
    
    Edge newe = new Edge(pnode.arcPoint,snode.arcPoint);
    edges.add(newe);

    if (reporter != null) reporter.EdgeCreated(newe);
    
    TreeNode newbp = new TreeNode(pnode.arcPoint,snode.arcPoint,newe);
    
    // we need to remove <pnode,arcNode> , arcNode , <arcNode.snode> from the tree.
    // assumptions: 
    // * both breakpoints are in the parent chain from arcNode
    // * one of the breakpoints is the parent of arcNode


    TreeNode lchild;
    TreeNode rchild;

    TreeNode par1 = arcNode.parent;
    boolean isParLeft;
    Point testLeft;
    Point testRight;

    if (par1.leftarcPoint == pnode.arcPoint && par1.rightarcPoint == arcNode.arcPoint) 
    {
      isParLeft = true;
      testLeft = arcNode.arcPoint;
      testRight = snode.arcPoint;
    }
    else if (par1.leftarcPoint == arcNode.arcPoint && par1.rightarcPoint == snode.arcPoint) 
    {
      isParLeft = false;
      testLeft = pnode.arcPoint;
      testRight = arcNode.arcPoint;
    }
    else 
    {
      throw new VoronoiException("eep! bad assumption in HandleCircle!");
    }

    TreeNode par2 = par1.parent;
    while(par2 != null && (par2.leftarcPoint != testLeft || par2.rightarcPoint != testRight))
    {
      par2 = par2.parent;
    }
    if (par2 == null) 
    {
      throw new VoronoiException("eep eep! bad assumption in HandleCircle");
    }

    // ok, if we get here, we have found both parents, and we know which one is on which side.
    // par1 is right above our disappearing arc...and par2 is somewhere up there.
    // given the properties of par2, we ought to be able to rotate it down until
    // it is right above par1.

    while(par1.parent != par2)
    {
      if (isParLeft) tree.RotateRight(par2);
      else tree.RotateLeft(par2);
    }
    
    // _now_ if everything went to plan, we can process relying on the 
    // assuption that par1 is a child of par2.

    if (par1.left == arcNode)
    {
      rchild = par1.right;
      lchild = par2.left;
    }
    else
    {
      lchild = par1.left;
      rchild = par2.right;
    }

    newbp.left = lchild;
    lchild.parent = newbp;
    
    newbp.right = rchild;
    rchild.parent = newbp;

    if (par2.parent == null)
    {
      tree.root = newbp;
    }
    else if (par2.parent.left == par2)
    {
      par2.parent.left = newbp;
      newbp.parent = par2.parent;
    }
    else
    {
      par2.parent.right = newbp;
      newbp.parent = par2.parent;
    }

    par1.edge.Terminate(cev.center);
    par2.edge.Terminate(cev.center);
    newbp.edge.Terminate(cev.center);
    
    if (reporter != null)
    {
      reporter.EdgeTerminated(par1.edge);
      reporter.EdgeTerminated(par2.edge);
      reporter.EdgeTerminated(newbp.edge);
    }

    if (tree.Predecessor(pnode) != null) CreateCircleEvent(tree.Predecessor(pnode),pnode,snode);
    if (tree.Successor(snode) != null) CreateCircleEvent(pnode,snode,tree.Successor(snode));
  }

  // this method will do an inorder walk of the 
  // remaining tree at the end.  By definition, all remaining
  // breakpoints in the tree go out to infinity (or we would have made
  // and completed a circle event)  and so, by increasing the sweep line
  // by some nonzero margin, we can guarantee that we can calculate 
  // and store some exemplar point on the halfline for each halfline.
  private void FinalizeHalfLines(TreeNode node)
  {
    if (node == null) return;
    if (node.arcPoint != null) return; // we only want breakpoints.


    node.edge.SetExemplar(node.BreakPoint(sweepx+MathUtilities.MARGIN));
    if (reporter != null) reporter.EdgeExemplared(node.edge);
    
    FinalizeHalfLines(node.left);
    FinalizeHalfLines(node.right);
  }

  // below is an alternate interface used for visually stepping through the
  // algorithm.
  public Fortune(Vector<Point> sites,IEventReporter ier)
  {
    reporter = ier;
	  
    for (Point site : sites)
    {
      queue.add(new Event(site));
    }
  }    

  private boolean finished = false;
  public boolean step()
  {
    if (!queue.isEmpty())
    {
      Event ev = queue.poll();
      if (ev.site != null)
      {
        sweepx = ev.site.x;
        HandleSite(ev.site);
      }
      else
      {
        sweepx = ev.critical.x;
        HandleCircle(ev);
        if (ev.valid == false) return step();
      }
    }
    else if (!finished)
    {
      FinalizeHalfLines(tree.root);
      finished = true;
    }
    return finished;
  }

  public Tree GetTree() { return tree; }
  public double GetSweepX() { return sweepx; }
  public double GetNextSweepX()
  {
	  if (!queue.isEmpty())
	  {
		  Event nev = queue.peek();
		  if (nev.site != null) return nev.site.x;
		  else return nev.critical.x;
	  }
	  else
	  {
		  return sweepx + MathUtilities.MARGIN;
	  }
  }
}

  
  





