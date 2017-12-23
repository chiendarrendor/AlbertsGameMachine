package Outpost;

import java.awt.event.MouseListener;
import java.awt.event.MouseEvent;
import javax.swing.JLabel;

public class MouseAwareJLabel extends JLabel implements MouseListener
{
  private int m_code;
  MyMouseListener m_fmm;
  
  public MouseAwareJLabel(int i_code,MyMouseListener i_fmm)
  {
    super();
    m_fmm = i_fmm;
    m_code = i_code;
    addMouseListener(this);
  }

  public void setListener(MyMouseListener i_fmm)
  {
    m_fmm = i_fmm;
  }
  
  public void mouseClicked(MouseEvent e)
  {
    if (m_fmm != null) m_fmm.mouseClicked(m_code);
  }
  
  public void mouseEntered(MouseEvent e)
  {
    if (m_fmm != null) m_fmm.mouseEntered(m_code);
  }
  
  public void mouseExited(MouseEvent e)
  {
    if (m_fmm != null) m_fmm.mouseExited(m_code);
  }
  
  public void mousePressed(MouseEvent e)
  {
    if (m_fmm != null) m_fmm.mousePressed(m_code);
  }
  
  public void mouseReleased(MouseEvent e)
  {
    if (m_fmm != null) m_fmm.mouseReleased(m_code);
  }
}

