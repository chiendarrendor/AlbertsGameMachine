package MerchantOfVenus;

import javax.swing.*;
import java.util.*;
import java.awt.event.*;
import java.awt.*;

public class StarPanel extends JPanel implements ActionListener
{
	private static final long serialVersionUID = 1L;
Dimension preferred;
  Random rand;
  private static final int animTime = 100;
  private static final int starSize = 4;
  private static final int starCount = 5000;
  private static final int deltaO = 50;
  private Vector<Star> stars = new Vector<Star>();
  private ShootingStar ss = null;

  private class Star
  {
    private int x;
    private int y;
    private int opacity;

    public Star(int ix,int iy) 
    { 
      x = ix; 
      y = iy; 
      opacity = 50 + rand.nextInt(206);
    }

    public void Draw(Graphics2D g)
    {
      // choose a a random new opacity between max(50,opacity-deltaO) and min(255,opacity+deltaO)
      int oMin = Math.max(50,opacity-deltaO);
      int oMax = Math.min(255,opacity+deltaO);
      
      opacity = oMin + rand.nextInt(oMax - oMin + 1);

      g.setColor(new Color(255,255,255,opacity));
      g.fillOval(x,y,starSize,starSize);
                 
    }
  }

 
  private class ShootingStar
  {
    private int startx;
    private int starty;
    private int endx;
    private int endy;

    private class Segment
    {
      public int x1;
      public int y1;
      public int x2;
      public int y2;
      public int width;
      public Segment(int ix1,int iy1,int ix2,int iy2,int iwidth) 
      {
    	  x1 = ix1; 
    	  y1 = iy1;
    	  x2 = ix2;
    	  y2 = iy2;
    	  width = iwidth;
      }
    }

    private Vector<Segment> segments = new Vector<Segment>();
    
    public ShootingStar()
    {
      startx = rand.nextInt(preferred.width);
      endx = rand.nextInt(preferred.width);
      starty = rand.nextInt(preferred.height);
      endy = rand.nextInt(preferred.height);
      
      segments.add(new Segment(startx,starty,endx,endy,10));
    }
    
    public void Draw(Graphics2D g)
    {
    	for (Segment s : segments)
    	{
    		g.setStroke(new BasicStroke(s.width));
    		g.drawLine(s.x1,s.y1,s.x2,s.y2);
    	}
    }
  }
 



  public StarPanel(Board b)
  {
    rand = new Random();
    preferred = new Dimension(b.GetBoundingBox().width+b.GetMargin()*2,
                              b.GetBoundingBox().height+b.GetMargin()*2);

    javax.swing.Timer t = new javax.swing.Timer(animTime,this);
    t.start();

    for (int i = 0 ; i < starCount ; ++i)
    {
      stars.add(new Star(rand.nextInt(preferred.width),rand.nextInt(preferred.height)));
    }
    
 //   ss = new ShootingStar();
  }

  public void paint(Graphics g)
  {
    Graphics2D g2d = (Graphics2D)g;

    int w = getPreferredSize().width;
    int h = getPreferredSize().height;

    // draw background
    g2d.setColor(Color.BLACK);
    g2d.fillRect(0,0,w,h);

    for (Star s : stars)
    {
      s.Draw(g2d);
    }
    
    if (ss != null) ss.Draw(g2d);
  }

  public Dimension getPreferredSize() { return preferred; }
  public void actionPerformed(ActionEvent e) { repaint(); }
}
