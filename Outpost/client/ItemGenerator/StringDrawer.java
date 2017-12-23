import java.awt.geom.*;
import java.awt.font.*;
import java.awt.*;

class StringDrawer
{
  public static final int TOP=0;
  public static final int MIDDLE=1;
  public static final int BOTTOM=2;

  public static final int LEFT=3;
  public static final int CENTER=4;
  public static final int RIGHT=5;

  private static final int LINESEP=2;

  public static void DrawString(Graphics2D i_g2d,
                                String i_string,
                                int i_vjustify,int i_hjustify,
                                int i_x,int i_y)
    throws StringDrawerException
  {


    String[] linestring = i_string.split("\n");
    int i;
    int ysize = -1 * LINESEP;

    for (i = 0 ; i < linestring.length ; ++i)
    {
      TextLayout tl = new TextLayout(linestring[i],i_g2d.getFont(),i_g2d.getFontRenderContext());
      Rectangle2D ssize = tl.getBounds();
      int h = (int)Math.ceil(ssize.getHeight());

      ysize += h + LINESEP;
    }

    int ybase;

    switch(i_vjustify)
    {
    case TOP:
      ybase = i_y - LINESEP;
      break;
    case MIDDLE:
      ybase = i_y - (ysize/2) - LINESEP;
      break;
    case BOTTOM:
      ybase = i_y - ysize - LINESEP;
      break;
    default:
      throw new StringDrawerException("Unknown i_vjustify");
    }

    for (i = 0 ; i < linestring.length ; ++i)
    {
      TextLayout tl = new TextLayout(linestring[i],i_g2d.getFont(),i_g2d.getFontRenderContext());
      Rectangle2D ssize = tl.getBounds();

      int x;
      switch(i_hjustify)
      {
      case LEFT:
        x = i_x;
        break;
      case CENTER:
        x = i_x - ((int)Math.ceil(ssize.getWidth())/2);
        break;
      case RIGHT:
        x = i_x - (int)Math.ceil(ssize.getWidth());
        break;
      default:
        throw new StringDrawerException("Unknown i_hjustify");
      };

      int dy = (int)Math.ceil(ssize.getHeight());

      ybase += dy + LINESEP;

      i_g2d.drawString(linestring[i],x,ybase);
    }
  }
}




    
    
