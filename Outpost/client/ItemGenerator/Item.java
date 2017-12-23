import java.awt.geom.*;
import java.awt.font.*;
import java.awt.*;
import java.awt.image.*;
import javax.swing.*;
import java.io.*;


class Item
{
  private static final int width = 75;
  private static final int height = 80;

  private static final int numdownoffset = 4;
  private static final int numrightoffset = 5;

  private static final int nameleftoffset = 2;
  private static final int namedownoffset = 15;

  private static final int costleftoffset = 2;
  private static final int costdownoffset = 40;

  private static final int veepleftoffset = 2;
  private static final int veepdownoffset = 55;

  private BufferedImage m_image;
  private String m_imagename;

  public JLabel createJLabel()
  {
    JLabel result = new JLabel(new ImageIcon(m_image));
    result.setToolTipText("<HTML><img src=\"file:" + m_imagename + ".jpg\"></HTML>");
    return result;
  }

  public Item(int i_num,String i_name,int i_cost,String i_veep,String i_imgname)
  {
    m_image = new BufferedImage(width,height,BufferedImage.TYPE_INT_ARGB);
    m_imagename = i_imgname;
    Graphics2D g2d = m_image.createGraphics();

    try
    {
      g2d.setColor(Color.WHITE);
      g2d.fillRoundRect(0,0,width-1,height-1,15,15);
      g2d.setColor(Color.BLACK);
      g2d.drawRoundRect(0,0,width-1,height-1,15,15);

      Font f = g2d.getFont();
      Font fsmall = f.deriveFont((float)9);
      g2d.setFont(fsmall);

      StringDrawer.DrawString(g2d,Integer.toString(i_num),
                              StringDrawer.TOP,StringDrawer.RIGHT,
                              width-numrightoffset,numdownoffset);

      Font fbigger = f.deriveFont((float)10);
      g2d.setFont(fbigger);


      StringDrawer.DrawString(g2d,"Cost: " + Integer.toString(i_cost),
                              StringDrawer.TOP,StringDrawer.LEFT,
                              costleftoffset,costdownoffset);
                                                         
      StringDrawer.DrawString(g2d,"VP: " + i_veep,
                              StringDrawer.TOP,StringDrawer.LEFT,
                              veepleftoffset,veepdownoffset);

      StringDrawer.DrawString(g2d,i_name,StringDrawer.TOP,StringDrawer.LEFT,
                              nameleftoffset,namedownoffset);

    }
    catch(StringDrawerException sde)
    {
      System.out.println("Exception caught: " + sde);
    }
  }

}

