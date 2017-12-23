package Voronoi;

public class MathUtilities
{
  // used to create halfline exemplars.  represents the amount by which we
  // exend the size of the sweep space in the x direction to create the exemplars.
  public static double MARGIN = 100.0;

  public static Point GetParabolaIntersection(Point left,Point right,double sweep)
  {
	  // some special cases:  if both points are on sweep, their parabolas will never intersect.
	  if (ddequals(left.x,sweep) && ddequals(right.x,sweep)) return null;
	  
	  // if one is on sweep, then that one's locus is actually a line extending to the left
	  // perpendicular to the sweep line.
	  if (ddequals(left.x,sweep))
	  {
		  // we know the y of the intersection...it's the y of the point on sweep.
		  return new Point(GetXOfParabola(right,sweep,left.y),left.y);
	  }
	  // similarly for the other way 'round
	  if (ddequals(right.x,sweep))
	  {
		  return new Point(GetXOfParabola(left,sweep,right.y),right.y);
	  }
	  
    // calculate y-coordinate of intersection.
    double zleft = 2.0 * (left.x - sweep);
    double zright = 2.0 * (right.x - sweep);
    double leftA = 1.0 / zleft;
    double leftB = -2.0 * left.y / zleft;
    double leftC = (left.x*left.x + left.y*left.y  - sweep*sweep)/zleft;
    double rightA = 1.0 / zright;
    double rightB = -2.0 * right.y / zright;
    double rightC = (right.x*right.x + right.y*right.y  - sweep*sweep)/zright;
      
    double A = leftA-rightA;
    double B = leftB-rightB;
    double C = leftC-rightC;

    double ypos;
    if (A == 0)
    {
      ypos = -C/B;
    }
    else 
    {
      ypos = (-B - Math.sqrt(B*B-4.0*A*C)) / (2.0*A);
    }

 
    double xpos = GetXOfParabola(left,sweep,ypos);
    return new Point(xpos,ypos);
  }
  
  // equation of a horizontal-axis parabola, apex to the right:
  // sweep - x = sqrt((cx - x)^2 + (cy - y)^2)
  // so, given a known y y0:
  // x = ((cy - y0)^2 - sweep^2)/(2cx - 2sweep)
  public static double GetXOfParabola(Point cp,double sweep,double y)
  {
	  	return ((cp.y-y)*(cp.y-y) - sweep*sweep + cp.x*cp.x) / (2.0 * cp.x - 2.0 * sweep);
  }
  

  public static Point GetCircleCenter(Point left,Point center,Point right)
  {
    // two lines between the three points are needed;
    Point line1a = left;
    Point line1b = center;
    Point line2a = center;
    Point line2b = right;

    // first see if both line1 and line2 are vertical.  
    // this is a special case of collinear, and means no circle.
    if (ddequals(line1a.x,line1b.x) && ddequals(line2a.x,line2b.x)) 
    {
      return null;
    }
    
    // so now, no more than one of them can be vertical
    // switch the one that is to the third line.
    if (ddequals(line1a.x,line1b.x)) line1b = right;
    else if (ddequals(line2a.x,line2b.x)) line2a = left;

    // now, we have two lines whose slope is calculatable.
    double slope1 = (line1a.y-line1b.y)/(line1a.x-line1b.x);
    double slope2 = (line2a.y-line2b.y)/(line2a.x-line2b.x);

    // if both slopes are the same, we have a non-circle
    if (ddequals(slope1,slope2)) return null;

    // so if we get here, we have two actual chords of a unique circle.  
    // in order to find the center of this circle, we use the following fact:
    // 'the perpendicular bisector of any chord of a circle goes through the center of a circle.'
    // since we have two such chords, we can solve for x and y, thus:
    // a) equation of perpendicular bisector:
    //    slope is negative reciprocal of slope of chord.
    //    a point on the perpendicular bisector is the midpoint of the chord
    // so:
    // given LC = (A+B)/2
    // Ypc = -1/slope ( x - LC.x) + LC.y
    //
    // so, setting the y coordinates of the two bisectors equal and solving, we get:
    // 
    // x = ((LC2.y - LC1.y) m1 m2 - LC1.x m2 + LC2.x m1) / (m1 - m2)
    // then plugging x back into one of the two equations for the perpendicular bisector;
    // y = -1/m1 ( x - LC1.x ) + LC1.y
    
    Point LC1 = new Point((line1a.x + line1b.x) / 2.0,(line1a.y + line1b.y) / 2.0);
    Point LC2 = new Point((line2a.x + line2b.x) / 2.0,(line2a.y + line2b.y) / 2.0);

    double cx = ((LC2.y - LC1.y) * slope1 * slope2 - LC1.x * slope2 + LC2.x * slope1) / 
      (slope1 - slope2);
    double cy = !ddequals(slope1,0.0) ? (-1 / slope1 * (cx - LC1.x) + LC1.y) : (-1/slope2 * (cx - LC2.x) + LC2.y);

    return new Point(cx,cy);
  }

  // there are generally two arc sequences in the beachfront that both
  // determine a circle... ABC, and CBA....we only want the one that converges, i.e.
  // the relationship between the nodes is such that the center arc disappears, not expands.

  public static boolean Converges(Point left,Point center,Point right)
  {
    double c = (center.x - left.x)*(right.y - left.y) - (right.x-left.x)*(center.y-left.y);
    return c < 0;
  }

  public static double D(Point a,Point b)
  {
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    return Math.sqrt(dx*dx + dy*dy);
  }

  private final static double EPSILON = 1.0e-10;
  public static boolean ddequals(double a,double b)
  {
    return Math.abs(a-b) < EPSILON;
  }


}
