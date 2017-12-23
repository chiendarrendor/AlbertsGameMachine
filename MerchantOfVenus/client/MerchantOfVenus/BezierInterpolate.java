package MerchantOfVenus;

import java.awt.geom.*;

// given 4 points, returns a Cubic Curve (Cubic Bezier)
// that goes through those four points.
public class BezierInterpolate
{
  private static double u = 1.0 / 3.0;
  private static double v = 2.0 / 3.0;
  private static double nu = 1.0 - u;
  private static double nv = 1.0 - v;
  // the below calculations are from http://polymathprogrammer.com/2007/06/27/reverse-engineering-bezier-curves/
  public static CubicCurve2D DoBezierInterpolate(Point2D p0,Point2D p1,Point2D p2, Point2D p3)
  {
    double a = 3.0 * nu * nu * u;
    double b = 3.0 * nu * u * u;
    double c = 3.0 * nv * nv * v;
    double d = 3.0 * nv * v * v;
    double det = a * d - b * c;

    double control1X,control1Y;
    double control2X,control2Y;

    // the final cubic curve's start and end points are our start and end points.
    // just calculate the control points.
    double t1x,t1y;
    double t2x,t2y;

    t1x = p1.getX() - (nu*nu*nu*p0.getX() + u*u*u*p3.getX() );
    t1y = p1.getY() - (nu*nu*nu*p0.getY() + u*u*u*p3.getY() );

    t2x = p2.getX() - (nv*nv*nv*p0.getX() + v*v*v*p3.getX() );
    t2y = p2.getY() - (nv*nv*nv*p0.getY() + v*v*v*p3.getY() );

    control1X = (d * t1x - b * t2x ) / det;
    control1Y = (d * t1y - b * t2y ) / det;

    control2X = (-c*t1x + a*t2x) / det;
    control2Y = (-c*t1y + a*t2y) / det;

    return new CubicCurve2D.Double(
                                   p0.getX(),p0.getY(),
                                   control1X,control1Y,
                                   control2X,control2Y,
                                   p3.getX(),p3.getY());
  }

  public static Point2D PointOnBezier(CubicCurve2D input,double t)
  {
    double nt = 1.0 - t;
    double x = 
      nt*nt*nt * input.getP1().getX() + 
      3*nt*nt*t * input.getCtrlP1().getX() +
      3*nt*t*t * input.getCtrlP2().getX() +
      t*t*t * input.getP2().getX();
    double y = 
      nt*nt*nt * input.getP1().getY() + 
      3*nt*nt*t * input.getCtrlP1().getY() +
      3*nt*t*t * input.getCtrlP2().getY() +
      t*t*t * input.getP2().getY();
    return new Point2D.Double(x,y);
  }

  private static double r = 4.0 / 9.0;
  private static double s = 5.0 / 9.0;

  // this will return another CubicCurve2D containing only the part of the 
  // Curve between the two center points (from a CubicCurve2d calculated with
  // BezierInterpolate, above)

  public static CubicCurve2D MiddleThird(CubicCurve2D input)
  {
    // step 1. calculate the points on the bezier for r and s.
    return DoBezierInterpolate(PointOnBezier(input,u),
                             PointOnBezier(input,r),
                             PointOnBezier(input,s),
                             PointOnBezier(input,v));
  }
}


                                                  
    
