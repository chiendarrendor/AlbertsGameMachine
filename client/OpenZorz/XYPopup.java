package OpenZorz;

import java.awt.event.*;
import javax.swing.*;
import java.awt.*;

class XYPopup extends MouseAdapter
{
  private JPopupMenu m_popup;
  private JMenuItem m_xitem;
  private JMenuItem m_yitem;

  public XYPopup()
  {
    m_popup = new JPopupMenu();
    
    m_xitem = new JMenuItem("x = 0");
    m_yitem = new JMenuItem("y = 0");

    m_popup.add(m_xitem);
    m_popup.add(m_yitem);
  }

  public void AddPopup(Component i_Comp)
  {
    i_Comp.addMouseListener(this);
  }

  public void mousePressed(MouseEvent e)
  {
    MaybeShowPopup(e);
  }

  public void mouseReleased(MouseEvent e)
  {
    MaybeShowPopup(e);
  }

  private void MaybeShowPopup(MouseEvent e)
  {
    if (e.isPopupTrigger())
    {
      m_xitem.setText("x = " + e.getX());
      m_yitem.setText("y = " + e.getY());

      m_popup.show(e.getComponent(),e.getX(),e.getY());
      System.out.println(e.getX() + " " + e.getY());
    }
  }      
}
