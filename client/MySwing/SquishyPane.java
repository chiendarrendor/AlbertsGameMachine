package MySwing;

import javax.swing.JLayeredPane;
import java.awt.event.MouseListener;
import java.awt.event.MouseEvent;
import java.awt.Component;

public class SquishyPane extends JLayeredPane implements MouseListener
{
  private int m_heightcounter;
  private int m_rollheight;

  public SquishyPane(int i_maxSep,int i_minVisible)
  {
    super();
    setLayout(new SquishyBoxLayout(i_maxSep,i_minVisible));
    m_heightcounter = 0;
  }

  public boolean isOptimizedDrawingEnabled()
  {
    return false;
  }

  public Component add(Component i_c)
  {
    setLayer(i_c,m_heightcounter++);
    super.add(i_c);

    i_c.addMouseListener(this);
    return i_c;
  }

  public void mouseClicked(MouseEvent i_event)
  {
  }
  public void mouseEntered(MouseEvent i_event)
  {
    m_rollheight = getLayer(i_event.getComponent());
    setLayer(i_event.getComponent(),highestLayer()+1);
    update(getGraphics());
  }
  public void mouseExited(MouseEvent i_event)
  {
    setLayer(i_event.getComponent(),m_rollheight);
    update(getGraphics());
  }

  public void mousePressed(MouseEvent i_event)
  {
  }
  public void mouseReleased(MouseEvent i_event)
  {
  }

  private static final long serialVersionUID = 1;

}


