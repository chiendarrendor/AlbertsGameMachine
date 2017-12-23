package Outpost;

import java.awt.*;

public class NonItem extends DrawThing
{
  protected static final int doffx = 2;
  protected static final int doffy = 5;
  protected static final int oradius = 35;
  protected static final int iradius = 30;
  protected static final int lwidth = 5;

  public NonItem()
  {
    super();

    m_g2d.setColor(Color.RED);
    m_g2d.fillOval(doffx+coffx,doffy+coffy,oradius*2,oradius*2);

    m_g2d.setColor(Color.WHITE);
    m_g2d.fillOval(doffx+coffx+(oradius-iradius),
                   doffy+coffy+(oradius-iradius),
                   iradius*2,iradius*2);

    m_g2d.setColor(Color.RED);

    // okie...some calculations.
    // the center of the circles are at 2+70/2,5+70/2 = 37,40, and the inner circle's radius is 30
    // the parametric equations for a circle are 
    // x = cx + r cos t
    // y = cy + r sin t
    //
    double cx = doffx+coffx+oradius;
    double cy = doffy+coffy+oradius;
    double cr = (oradius+iradius)/2.0;

    double ulx = cx + cr * Math.cos(Math.toRadians(135.0));
    double uly = cy - cr * Math.sin(Math.toRadians(135.0));

    double lrx = cx + cr * Math.cos(Math.toRadians(135.0+180.0));
    double lry = cy - cr * Math.sin(Math.toRadians(135.0+180.0));

    Polygon p = new Polygon();
    
    int loff = lwidth/2;

    p.addPoint((int)ulx+loff,(int)uly-loff);
    p.addPoint((int)ulx-loff,(int)uly+loff);
    p.addPoint((int)lrx-loff,(int)lry+loff);
    p.addPoint((int)lrx+loff,(int)lry-loff);

    m_g2d.fillPolygon(p);

  }
}
