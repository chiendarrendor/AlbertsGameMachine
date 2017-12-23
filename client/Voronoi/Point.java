package Voronoi;

public class Point implements Comparable<Point>
{
  public double x;
  public double y;
  public Point(double ix,double iy) { x = ix; y = iy; }

  public int compareTo(Point other)
  {
    if (x != other.x) return Double.compare(x,other.x);
    return Double.compare(y,other.y);
  }
  public boolean equals(Object other)
  {
    Point vp = (Point)other;
    return x == vp.x && y == vp.y;
  }

  public String toString()
  {
    return "(" + x + "," + y + ")";
  }

}
