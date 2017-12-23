package OpenZorz;

import javax.swing.*;
import java.awt.*;
import javax.swing.border.*;

import MySwing.LockLayout;

public class PanelNode extends GameNode
{
  private String m_layouttype;
  private boolean m_defaultlayout;

  public PanelNode(GameNode i_Parent,int i_x,int i_y,String i_hidden,String i_bordercolor,String i_layout)
  {
    super(i_Parent,i_hidden);
    Point p = new Point(i_x,i_y);

    JPanel panel = new JPanel();

    if (i_bordercolor != null)
    {
      Border border = BorderFactory.createLineBorder(Color.decode(i_bordercolor));
      panel.setBorder(border);
    }

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
    panel.setOpaque(false);

    if (i_Parent != null) i_Parent.AddComponent(panel,p);

    SetComponent(panel);

    AlterVisibility(IsVisible());
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
