package OpenZorz;
import javax.swing.*;
import java.awt.*;

public class BackgroundPanel extends JPanel
{
  ImageIcon m_backimage;

  BackgroundPanel(ImageIcon i_backimage)
  {
    m_backimage = i_backimage;
  }

  protected void paintComponent(Graphics g)
  {
    g.setColor(getBackground());
    g.fillRect(0,0,getWidth(),getHeight());

    if (m_backimage == null)
    {
      return;
    }

    m_backimage.paintIcon(this,g,0,0);
  }

  public Dimension getMinimumSize()
  {
    Dimension d;

    if (m_backimage == null)
    {
      d = new Dimension(0,0);
    }
    else
    {
      Insets insets = getInsets();
      d = new Dimension(insets.left + m_backimage.getIconWidth() + insets.right,
                        insets.top + m_backimage.getIconHeight() + insets.bottom);
    }

    return d;
  }

  public Dimension getPreferredSize()
  {
    return getMinimumSize();
  }
  
  private static final long serialVersionUID = 1;

}
    
