package Voronoi;

// returned as part of an Edge.  An InterParameter represents
// a particular point on the edge where something interesting occurs,
// based on PointType, being one of
// CENTER (the midpoint between the two sites, and the min alpha if present, t=0 by definition)
// VERTEX (a voronoi vertex as calculated by Fortune's algorithm)
// BOUNDING (an 'example' extremum of a line or halfline
//
// alpha: radius of circle touching both sites and centered on this point.

public class InterParameter
{
  public PointType type;
  public double t;
  public double alpha;
  public Point point;
  public InterParameter(PointType type, double t,Point thisPoint,Point site) 
  { this.type = type ; 
    this.t = t; 
    point = thisPoint;
    alpha = MathUtilities.D(point,site);
  }
}
