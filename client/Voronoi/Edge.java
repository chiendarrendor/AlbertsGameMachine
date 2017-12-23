package Voronoi;

import java.util.*;
  
public class Edge
{
  // the adjacent two site points that define the existence of this edge.
  // this edge will be normal to the line between these two points.
  public Point site1;
  public Point site2;

  // this point will be non-null for any line or half-line
  // edge types (i.e. not terminated at both ends by a vertex)
  // and will be, for a half-line, a point not only on the line
  // implied by the normal to the site1-site2 line, but on the
  // half-line in question.
  public Point exemplar;
    
  // non-empty if this edge is created by a circle event.
  public Point start = null; 
  // non-empty if this edge is terminated by a circle event.
  public Point end = null;

  // the attributes of the parametric equations for this edge:
  // x = cx + t dx
  // y = cy + t dy
  private double cx;
  private double cy;
  private double dx;
  private double dy;

  public Edge(Point s1,Point s2) 
  { 
    site1 = s1 ; 
    site2 = s2 ; 

    // x0,y0 of the parametric equation is the midpoint of the line joining the sites.
    cx = (site1.x + site2.x) / 2.0;
    cy = (site1.y + site2.y) / 2.0;
    // dx,dy are straightforward
    dx = site1.y - site2.y;
    dy = site2.x - site1.x;
    // but scaled such that there is a 1-1 relationship between parameter value
    // and distance.
    double scale = MathUtilities.D(site1,site2);
    dx /= scale;
    dy /= scale;
  }

  public void Terminate(Point e)
  {
    if (start == null) 
    {
      start = e;
    }
    else if (end == null)
    {
      end = e;
    }
    else
    {
      throw new VoronoiException("three ends!?");
    }
  }

  public void SetExemplar(Point e)
  {
    exemplar = e;
  }

  public Point GetParametric(double t)
  {
    return new Point(cx+t*dx,cy+t*dy);
  }
      
  // parameter of point on line: t = (x-cx)/dx or (y-cy/dy) if dx is 0
  private double GetT(Point p) 
  {
    return MathUtilities.ddequals(dx,0.0) ? ((p.y-cy)/dy) : ((p.x-cx)/dx);
  }

  private InterParameter MakeIP(PointType type,double t)
  {
    return new InterParameter(type,t,GetParametric(t),site1);
  }

  // the list of interparameters are all the interesting parameters on the line,
  // half-line, or segment, by type:
  // center: always has t = 0.0, is the crossing between
  // this line and the line that passes between the two sites. 
  // vertex:
  //   t of either start or end.
  // bounding:
  //   MARGIN pixels beyond last interesting point (either center or vertex) towards unboundedness
  // 
  // the vector will contain up to three InterParameter objects, alway in order from
  // smallest t value to largest.

  public Vector<InterParameter> GetInterParameters()
  {
    Vector<InterParameter> result = new Vector<InterParameter>();
    // case 1, both unterminated.
    if (start == null && end == null)
    {
      result.add(MakeIP(PointType.BOUNDING,-MathUtilities.MARGIN));
      result.add(MakeIP(PointType.CENTER,0.0));
      result.add(MakeIP(PointType.BOUNDING,MathUtilities.MARGIN));
    }
    // case 2...both terminated
    else if (start != null && end != null)
    {
      double st = GetT(start);
      double et = GetT(end);
      result.add(MakeIP(PointType.VERTEX,(st < et) ? st : et));
      if ((st < 0 && et > 0) || (et < 0 && st > 0)) result.add(MakeIP(PointType.CENTER,0.0));
      result.add(MakeIP(PointType.VERTEX,(st < et) ? et : st));
    }
    // case 3...one terminated
    else
    {
      double exT = GetT(exemplar);
      double vT = start != null ? GetT(start) : GetT(end);
      double cT = 0;

      // known properties:  
      // exT will never be equal to cT or vT
      // vT may be equal to cT
      // exemplar is not an interesting point, but it tells where on the half-line the interesting point is.
      // six cases:
      // 1. exemplar vertex center
      if (exT < vT && vT <= cT)
      {
        result.add(MakeIP(PointType.BOUNDING,vT-MathUtilities.MARGIN));
        result.add(MakeIP(PointType.VERTEX,vT));
      }
      // 2. exemplar center vertex
      else if (exT < cT && cT < vT)
      {
        result.add(MakeIP(PointType.BOUNDING,-MathUtilities.MARGIN));
        result.add(MakeIP(PointType.CENTER,0.0));
        result.add(MakeIP(PointType.VERTEX,vT));
      }
      // 3. center vertex exemplar
      else if (cT <= vT && vT < exT)
      {
        result.add(MakeIP(PointType.VERTEX,vT));
        result.add(MakeIP(PointType.BOUNDING,vT+MathUtilities.MARGIN));
      }
      // 4. vertex center exemplar
      else if (vT < cT && cT < exT)
      {
        result.add(MakeIP(PointType.VERTEX,vT));
        result.add(MakeIP(PointType.CENTER,0.0));
        result.add(MakeIP(PointType.BOUNDING,+MathUtilities.MARGIN));
      }
      // 5. center exemplar vertex
      else if (cT < exT && exT < vT)
      {
        result.add(MakeIP(PointType.BOUNDING,-MathUtilities.MARGIN));
        result.add(MakeIP(PointType.CENTER,0.0));
        result.add(MakeIP(PointType.VERTEX,vT));
      }
      // 6. vertex exemplar center
      else if (vT < exT && exT < cT)
      {
        result.add(MakeIP(PointType.VERTEX,vT));
        result.add(MakeIP(PointType.CENTER,0.0));
        result.add(MakeIP(PointType.BOUNDING,+MathUtilities.MARGIN));
      }
      else
      {
        throw new VoronoiException("Interparameter creation case should never happen: " +
                                   " vT = " + vT + " exT = " + exT + " cT = " + cT);
      }
    }
    return result;
  }

  // returns true iff the given alpha is no smaller than the min alpha of the edge,
  // and no larger than the max alpha of the edge (if the edge has a max alpha;
  // halflines and lines do not)
  public boolean InAlphaBounds(double alpha)
  {
    Vector<InterParameter> ips = GetInterParameters();
    double min = Double.MAX_VALUE;
    double max = Double.MIN_VALUE;
    double vcount = 0;
    for (InterParameter ip : ips)
    {
      if (ip.alpha < min) min = ip.alpha;
      if (ip.alpha > max) max = ip.alpha;
      if (ip.type == PointType.VERTEX) ++ vcount;
    }

    if (vcount < 2) max = Double.MIN_VALUE;
    if (alpha < min) return false;
    if (max == Double.MIN_VALUE) return true;
    return max >= alpha;
  }

}
