package Outpost;

import java.util.HashMap;
import javax.swing.JLabel;
import NodeInterfacePackage.NodeInterfacePackageException;
import MySwing.*;
import java.awt.Color;
import java.awt.image.BufferedImage;
import java.awt.Graphics2D;
import javax.swing.ImageIcon;
import javax.swing.Icon;
import java.awt.Font;

public class Commodity
{
  private static final int width=75;
  private static final int height=75;

  private static HashMap<String,Icon> s_Icons = new HashMap<String,Icon>();

  public static JLabel getJLabel(int i_commtype,String i_value,
                                 boolean i_ismega,boolean i_isavg,boolean i_isSelected)
  {
    Icon ii = getIcon(i_commtype,i_value,i_ismega,i_isavg,i_isSelected);

    JLabel result = new JLabel(ii);
    return result;
  }

  public static Icon getIcon(int i_commtype,String i_value,
                             boolean i_ismega,boolean i_isavg,boolean i_isSelected)
  {
    String key = i_commtype + "," + i_value + "," + i_ismega + "," + i_isavg + "," + i_isSelected;
    Icon ii;

    synchronized(s_Icons)
    {
      if (s_Icons.containsKey(key))
      {
        ii = s_Icons.get(key);
      }
      else
      {
        ii = makeIcon(i_commtype,i_value,i_ismega,i_isavg,i_isSelected);
        s_Icons.put(key,ii);
      }
    }
    return ii;
  }





  private static ImageIcon makeIcon(int i_commtype,String i_value,
                                    boolean i_ismega,boolean i_isavg,boolean i_isSelected)
  {
    Color forecolor;
    Color backcolor = Color.WHITE;
    String cname;

    switch(i_commtype)
    {
    case 1: cname = "ORE";               forecolor = new Color(128, 64,  0); break;
    case 2: cname = "WATER";             forecolor = new Color(  0,  0,255); break;
    case 3: cname = "TITANIUM";          forecolor = new Color(128,128,128); break;
    case 4: cname = "RESEARCH";          forecolor = new Color(  0,  0,  0); backcolor = Color.yellow; break;
    case 5: cname = "MICRO-\nBIOTICS";   forecolor = Color.magenta         ; break;
    case 6: cname = "NEW\nCHEMICALS";    forecolor = new Color(  0,128,  0); break;
    case 7: cname = "ORBITAL\nMEDICINE"; forecolor = new Color(  0,255,  0); break;
    case 8: cname = "RING ORE";          forecolor = new Color(128,128,255); break;
    case 9: cname = "MOON ORE";          forecolor = new Color(255,128, 64); break;
    default: throw new NodeInterfacePackageException("Illegal value of Commodity Type!");
    }

    BufferedImage image = new BufferedImage(width,height,BufferedImage.TYPE_INT_ARGB);
    Graphics2D g2d = image.createGraphics();

    g2d.setColor(backcolor);
    g2d.fillRoundRect(0,0,width-1,height-1,15,15);
    g2d.setColor(forecolor);
    g2d.drawRoundRect(0,0,width-1,height-1,15,15);

    StringDrawer.DrawString(g2d,cname,StringDrawer.TOP,StringDrawer.CENTER,width/2,4);

    if (i_ismega)
    {
	StringDrawer.DrawString(g2d,"MEGA (x4)",StringDrawer.BOTTOM,StringDrawer.CENTER,width/2,height-4);
    }
    if (i_isavg)
    {
        StringDrawer.DrawString(g2d,"AVERAGE",StringDrawer.BOTTOM,StringDrawer.CENTER,width/2,height-4);
    }

    Font fbigger = g2d.getFont().deriveFont((float)35).deriveFont(Font.BOLD);
    g2d.setFont(fbigger);

    StringDrawer.DrawString(g2d,i_value,StringDrawer.TOP,StringDrawer.CENTER,width/2,27);

    if (i_isSelected)
    {
      int axoff = 10;
      int ayloc = 27;
      int aheight = 30;
      int awidth = 14;
      int alwidth = 5;

      DrawRedArrow(g2d,axoff,ayloc,aheight,awidth,alwidth);
      DrawRedArrow(g2d,width-axoff,ayloc,aheight,awidth,alwidth);
    }

    return new ImageIcon(image);
  }

  private static void DrawRedArrow(Graphics2D i_g2d,int i_x,int i_y,int i_height,int i_width,int i_linewidth)
  {
    int bottomx = i_x;
    int bottomy = i_y+i_height;
    int delta = i_width / 2;
    
    i_g2d.setColor(Color.red);

    int i;
    for (i = -i_linewidth/2 ; i < i_linewidth/2 ; ++i)
    {
      i_g2d.drawLine(i_x+i,i_y,bottomx+i,bottomy);
      i_g2d.drawLine(bottomx-delta+i,bottomy-delta,bottomx+i,bottomy);
      i_g2d.drawLine(bottomx+delta+i,bottomy-delta,bottomx+i,bottomy);
    }
  }      

}
