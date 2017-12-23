package OpenZorz;

import java.awt.GraphicsEnvironment;
import java.awt.Point;
import java.awt.Rectangle;



class GraphicsInfo
{
  static void ShowGraphicsInfo()
  {
    GraphicsEnvironment genv = GraphicsEnvironment.getLocalGraphicsEnvironment();
    Point p = genv.getCenterPoint();
    Rectangle r = genv.getMaximumWindowBounds();

    System.out.println("Center Point: " + p);
    System.out.println("Max Bounds: " + r);
  }
}
