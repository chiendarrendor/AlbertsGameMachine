package Outpost;

import java.awt.geom.*;
import java.awt.font.*;
import java.awt.*;
import java.awt.image.*;
import javax.swing.*;
import java.io.*;
import java.net.URL;
import MySwing.*;


public class Item extends DrawThing
{
  private static final int numdownoffset = 4+coffx;
  private static final int numrightoffset = 5;

  private static final int nameleftoffset = 2+coffx;
  private static final int namedownoffset = 15+coffy;

  private static final int costleftoffset = 2+coffx;
  private static final int costdownoffset = 40+coffy;

  private static final int veepleftoffset = 2+coffx;
  private static final int veepdownoffset = 55+coffy;

  private String m_imagename;

  public void setJLabel(JLabel i_jlabel)
  {
    super.setJLabel(i_jlabel);

    String s = m_imagename + ".jpg";
    if (!s.toLowerCase().startsWith("http:"))
    {
      s = "file:" + s;
    }
    String text="<HTML><img src=\"" + s + "\" alt=\"" + s + "\"></HTML>";

    i_jlabel.setToolTipText(text);
  }

  public Item(String i_DataLoc,int i_num,String i_name,int i_cost,String i_veep,String i_imgname)
  {
    super();
    m_imagename = i_DataLoc + "/" + i_imgname;

    Font f = m_g2d.getFont();
    Font fsmall = f.deriveFont((float)9);
    m_g2d.setFont(fsmall);

    StringDrawer.DrawString(m_g2d,Integer.toString(i_num),
			    StringDrawer.TOP,StringDrawer.RIGHT,
			    cwidth-numrightoffset,numdownoffset);

    Font fbigger = f.deriveFont((float)10);
    m_g2d.setFont(fbigger);


    StringDrawer.DrawString(m_g2d,"Cost: " + Integer.toString(i_cost),
			    StringDrawer.TOP,StringDrawer.LEFT,
			    costleftoffset,costdownoffset);
                                                         
    StringDrawer.DrawString(m_g2d,"VP: " + i_veep,
			    StringDrawer.TOP,StringDrawer.LEFT,
			    veepleftoffset,veepdownoffset);

    StringDrawer.DrawString(m_g2d,i_name,StringDrawer.TOP,StringDrawer.LEFT,
			    nameleftoffset,namedownoffset);

  }

}

