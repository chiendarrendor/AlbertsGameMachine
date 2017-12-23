package OpenZorz;

import javax.swing.*;
import java.awt.*;

import MySwing.LockLayout;

public class WindowNode extends GameNode
{
  JFrame m_frame;
  int m_width;
  int m_height;
  int m_x;
  int m_y;
  int m_rx;
  int m_ry;
  boolean m_negx;
  boolean m_negy;
  private boolean m_defaultlayout;
  private String m_layouttype;

  public WindowNode(String i_title,String i_hidden,String i_backimage,
                    String i_width,String i_height,String i_x,String i_y,
                    String i_layout,String i_noscroll)
  {
    super(null,i_hidden);

    m_width = 1024;
    m_height = 768;
    m_x = 0;
    m_y = 0;
    m_rx = 0;
    m_ry = 0;
    m_negx = false;
    m_negy = false;

    boolean noscroll = i_noscroll != null && 
      (i_noscroll.equals("y") || i_noscroll.equals("Y") || i_noscroll.equals("true"));

    if (i_width != null)  m_width = Integer.parseInt(i_width);
    if (i_height != null) m_height = Integer.parseInt(i_height);

    if (i_x != null)
    {
      if (i_x.startsWith("-"))
      {
        m_negx = true;
        m_rx = - Integer.parseInt(i_x);
      }
      else
      {
        m_rx = Integer.parseInt(i_x);
      }
    }

    if (i_y != null)
    {
      if (i_y.startsWith("-"))
      {
        m_negy = true;
        m_ry = - Integer.parseInt(i_y);
      }
      else
      {
        m_ry = Integer.parseInt(i_y);
      }
    }

    if (i_title == null)
    {
	    i_title = "Unnamed Window";
    }

    m_frame = new JFrame(i_title);


    try
    {
      m_frame.setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);
    } 
    catch (java.security.AccessControlException ace)
    {
    }

    JPanel panel = new JPanel();

    if (noscroll)
    {
      m_frame.getContentPane().add(panel);
    }
    else
    {
      JScrollPane scrollp = new JScrollPane(panel);
      scrollp.getVerticalScrollBar().setUnitIncrement(15);
      m_frame.getContentPane().add(scrollp);
    }

    AddPopup(panel);

    m_defaultlayout = true;
    m_layouttype = i_layout;
    LayoutManager layout;
    if (m_layouttype == null || m_layouttype.equals("lock"))
    {
	    layout = new LockLayout();
    }
    else if (m_layouttype.equals("hbox"))
    {
	    layout = new BoxLayout(panel,BoxLayout.X_AXIS);
	    m_defaultlayout = false;
    }
    else if (m_layouttype.equals("vbox"))
    {
	    layout = new BoxLayout(panel,BoxLayout.Y_AXIS);
	    m_defaultlayout = false;
    }
    else if (m_layouttype.equals("flow"))
    {
      layout = new FlowLayout();
      m_defaultlayout = false;
    }
    else if (m_layouttype.equals("border"))
    {
      layout = new BorderLayout();
      m_defaultlayout = false;
    }
    else
    {
	    layout = new LockLayout();
    }
    panel.setLayout(layout);

    SetComponent(m_frame,panel);
  }

  void RefreshMe()
  {
    m_frame.setVisible(IsVisible());
    Insets iset = m_frame.getInsets();
    m_frame.setSize(m_width+iset.left+iset.right,m_height+iset.top+iset.bottom);

    if (m_negx)
    {
      m_x = GraphicsEnvironment.getLocalGraphicsEnvironment().getMaximumWindowBounds().width -
        (m_width+iset.left+iset.right) -
        m_rx;
    }
    else
    {
      m_x = m_rx;
    }

    if (m_negy)
    {
      m_y = GraphicsEnvironment.getLocalGraphicsEnvironment().getMaximumWindowBounds().height -
        (m_height+iset.top+iset.bottom) -
        m_ry;
    }
    else
    {
      m_y = m_ry;
    }

    m_frame.setLocation(m_x,m_y);


  }

  public void dispose()
  {
    m_frame.dispose();
  }

  public void AddComponent(Component i_comp,Point i_xy)
  {
    if (m_defaultlayout)
    {
	    super.AddComponent(i_comp,i_xy);
    }
    else
    {
	    Container c = (Container)this.GetComponent();
	    c.add(i_comp);
    }
  }
}
