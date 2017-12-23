package Outpost;
import javax.swing.JLabel;
import javax.swing.ImageIcon;
import java.awt.image.*;
import java.awt.*;




public class DrawThing
{
  protected static final int width = 79;
  protected static final int height = 84;
  protected static final int cwidth = 75;
  protected static final int cheight = 80;
  protected static final int coffx = 2;
  protected static final int coffy = 2;

  protected BufferedImage m_image;
  protected Graphics2D m_g2d;

  public void setJLabel(JLabel i_jlabel)
  {
    i_jlabel.setIcon(new ImageIcon(m_image));
  }

  public DrawThing()
  {
    m_image = new BufferedImage(width,height,BufferedImage.TYPE_INT_ARGB);
    m_g2d = m_image.createGraphics();

    m_g2d.setColor(Color.WHITE);
    m_g2d.fillRoundRect(coffx,coffy,cwidth-1,cheight-1,15,15);
    m_g2d.setColor(Color.BLACK);
    m_g2d.drawRoundRect(coffx,coffy,cwidth-1,cheight-1,15,15);
  }
}
