package Voronoi; 

// this interface is used with the 'step' interface of 
// Fortune.java; each time a particular event occurs, the
// corresponding method of the class with this interface is called.

public interface IEventReporter
{
  public void ProcessSite(Point p); // called when a site is removed from the Queue
  public void DuplicatePoint(Point p); // called if two points are within epsilon of each other
  // called if an existing circle is invalidated.
  public void CircleInvalidated(Point pred,Point cur, Point succ);
  public void EdgeCreated(Edge e);
  public void EdgeTerminated(Edge e);
  public void EdgeExemplared(Edge e);
 
  public void CircleCreated(Point pred,Point cur,Point succ);
  public void HandleCircle(Point pred,Point cur,Point succ);
}
 
    
