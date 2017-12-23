package Voronoi;

public class Tree
{
  public TreeNode root = null;

  // should only be called on a node with a left child
  public void RotateRight(TreeNode q)
  {
    TreeNode p = q.left;
    TreeNode parent = q.parent;
    q.left = p.right;
    q.left.parent = q;
    p.right = q;
    q.parent = p;
    p.parent = parent;
    
    if (parent != null && parent.left == q) parent.left = p;
    else if (parent != null) parent.right = p;
    else root = p;
  }

  // should only be called on a node with a right child.
  public void RotateLeft(TreeNode q)
  {
    TreeNode p = q.right;
    TreeNode parent = q.parent;
    q.right = p.left;
    q.right.parent = q;
    p.left = q;
    q.parent = p;
    p.parent = parent;

    if (parent != null && parent.left == q) parent.left = p;
    else if (parent != null) parent.right = p;
    else root = p;
  }

  public TreeNode Successor(TreeNode cur)
  {
    if (cur == null) return null;

    // step 1. go up until there is a right that wasn't us.
    TreeNode par = cur.parent;
    while(true)
    {
      if (cur == par.left) break;
      if (par.parent == null) return null;
      par = par.parent;
      cur = cur.parent;
    }
    // step 2, go down right, then left to the bottom
    cur = par.right;
    while (cur.left != null) cur = cur.left;
    return cur;
  }

  public TreeNode Predecessor(TreeNode cur)
  {
    if (cur == null) return null;

    // step 1. go up until there is a left that wasn't us.
    TreeNode par = cur.parent;
    while(true)
    {
      if (cur == par.right) 
      {
        break;
      }

      if (par.parent == null) 
      {
        return null;
      }
      par = par.parent;
      cur = cur.parent;
    }
    // step 2, go down left, then right to the bottom
    cur = par.left;
    while (cur.right != null) cur = cur.right;
    return cur;
  }

  public boolean Empty() { return root == null; }
  
  // should only be called if the tree is empty.
  public void InsertFirstSite(Point site)
  {
    root = new TreeNode(site);
  }

  // should only be called if the arc found has the same x coordinate as site's x.
  // (and, technically, that the tree isn't empty, but the previous case implies a non-empty tree)
  // this only comes up if there are more than one site with the same minimal x coordinate (i.e. before any
  // arcs actually are arcs)
  // given
  //  supertree (some breakpoint)
  //       \
  //       pred (right-most arc in tree)
  //    (we know this is the case because our priority queues breaks, for x-coordinate ties, y from smallest to largest,
  // so our new site should have a larger y-coordinate than anything already present.
  // 
  // and we are replacing pred with
  //           <pred,site>
  //             / \
  //          pred* site


  public void AppendParallelSite(TreeNode pred,Point site,Edge e)
  {
    TreeNode newBreak = new TreeNode(pred.arcPoint,site,e);
    TreeNode replArc = new TreeNode(pred.arcPoint);
    TreeNode newArc = new TreeNode(site);

    newBreak.left = replArc;
    replArc.parent = newBreak;

    newBreak.right = newArc;
    newArc.parent = newBreak;

    // final step...hook our new little treelet into the tree.
    // if our predecessor arc is the root of the tree, we need to become the root of the tree.
    if (pred == root) 
    {
      root = newBreak;
    }
    else
    {
      // if our predecessor _isn't_ the root of the tree, it will have some parent breakpoint.  adjust that one.
      TreeNode par = pred.parent;
      par.right = newBreak;
      newBreak.parent = par;
      // at this point, nothing points to pred...we drop it.
    }
  }
      
  // replace broken with:
  //
  //          <broken,site>
  //            /  \
  //       broken*  <site,broken>
  //                   / \
  //                 site broken*
  // returns site

  public TreeNode MakeNewArc(TreeNode broken,Point site, Edge e)
  {
    TreeNode leftBreak = new TreeNode(broken.arcPoint,site,e);
    TreeNode rightBreak = new TreeNode(site,broken.arcPoint,e);
    TreeNode leftArc = new TreeNode(broken.arcPoint);
    TreeNode rightArc = new TreeNode(broken.arcPoint);
    TreeNode newArc = new TreeNode(site);

    leftBreak.left = leftArc;
    leftArc.parent = leftBreak;
    
    leftBreak.right = rightBreak;
    rightBreak.parent = leftBreak;

    rightBreak.left = newArc;
    newArc.parent = rightBreak;

    rightBreak.right = rightArc;
    rightArc.parent = rightBreak;

    if (broken == root)
    {
      root = leftBreak;
    }
    else
    {
      TreeNode par = broken.parent;
      if (par.left == broken) par.left = leftBreak;
      else par.right = leftBreak;

      leftBreak.parent = par;
    }

    return newArc;
  }

    
     

    



  public TreeNode FindArc(double y,double sweepx)
  {
    return root.FindNode(y,sweepx);
  }



}
