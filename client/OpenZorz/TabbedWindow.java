package OpenZorz;


import javax.swing.JTabbedPane;
import javax.swing.JFrame;
import java.awt.Component;
import java.awt.Insets;

import java.util.TimerTask;
import java.util.Timer;
import java.awt.Color;
import javax.swing.JLabel;
import java.util.HashMap;
import MySwing.ImageLoader;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.BoxLayout;
import javax.swing.Box;
import javax.swing.JPanel;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;

public class TabbedWindow extends JFrame
{
  private JTabbedPane m_tabs;
  private int m_width;
  private int m_height;
  private Timer m_timer;
  private String m_dataloc;
  private HashMap<Component,TabInfo> m_tabinfo;

  private class TabInfo
  {
    public TimerTask m_task;
    public JLabel m_label;
    public String m_name;
    public JFrame m_frame;
  }

  private class MyFlasher extends TimerTask
  {
    private JLabel m_flashlabel;
    private Color m_basecolor;
    private Color m_flashcolor = Color.red;

    public MyFlasher(JLabel i_flashlabel)
    {
      super();
      m_flashlabel=i_flashlabel;
      m_basecolor = m_flashlabel.getForeground();
    }

    public void run()
    {
      if (m_flashlabel.getForeground().equals(m_basecolor))
      {
        m_flashlabel.setForeground(m_flashcolor);
      }
      else
      {
        m_flashlabel.setForeground(m_basecolor);
      }
    }

    public boolean cancel()
    {
      m_flashlabel.setForeground(m_basecolor);
      return super.cancel();
    }
  }      

  private class MyTabbedPane extends JTabbedPane
  {
    public void setSelectedIndex(int i_index)
    {
      super.setSelectedIndex(i_index);
      CancelComponentTimerTask(getComponentAt(i_index));
    }
    private static final long serialVersionUID = 1;

  }

  private class ExportButtonListener implements ActionListener
  {
    private Component m_component;

    public ExportButtonListener(Component i_component)
    {
      m_component = i_component;
    }

    public void actionPerformed(ActionEvent i_event)
    {
      MoveTabToWindow(m_component);
    }
  }

  private class WindowWatcher extends WindowAdapter
  {
    private Component m_component;

    public WindowWatcher(Component i_component)
    {
      m_component = i_component;
    }
    
    public void windowClosing(WindowEvent i_event)
    {
      MoveWindowToTab(m_component);
    }
  }

  void MoveWindowToTab(Component i_component)
  {
    TabInfo ti = m_tabinfo.get(i_component);
    ti.m_frame.getContentPane().remove(i_component);
    ti.m_frame = null;

    putTab(ti.m_name,i_component);
  }

  void MoveTabToWindow(Component i_component)
  {
    unTab(i_component);

    TabInfo ti = m_tabinfo.get(i_component);
    ti.m_label = null;

    JFrame jf = new JFrame(ti.m_name);
    jf.addWindowListener(new WindowWatcher(i_component));
    ti.m_frame = jf;

    jf.setVisible(true);
    Insets iset = jf.getInsets();
    jf.setSize(m_width+iset.left+iset.right,m_height+iset.top+iset.bottom);
    jf.setLocation(20,20);
    jf.getContentPane().add(i_component);
  }

  public void Alert(Component i_c)
  {
    TabInfo ti = m_tabinfo.get(i_c);
    if (ti == null) return; // not one of our components;
    if (ti.m_frame == null) // it is a tab
    {
      int index = m_tabs.indexOfComponent(i_c);
      if (index == m_tabs.getSelectedIndex()) return; // don't alert the selected tab

      if (ti.m_task != null) return; // don't realert.
      MyFlasher mf = new MyFlasher(ti.m_label);
      m_timer.schedule(mf,0,1000);
      ti.m_task = mf;
    }
    else
    {
      ti.m_frame.toFront();
    }
  }

  public void CancelComponentTimerTask(Component i_c)
  {
    TabInfo ti = m_tabinfo.get(i_c);
    if (ti.m_task != null)
    {
      ti.m_task.cancel();
      ti.m_task = null;
    }
  }


  public TabbedWindow(int i_width,int i_height,String i_dataloc)
  {
    super("Client Tabs");
    m_width = i_width;
    m_height = i_height;
    m_timer = new Timer();
    m_tabinfo = new HashMap<Component,TabInfo>();
    m_dataloc = i_dataloc;

    try
    {
      setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);
    } 
    catch (java.security.AccessControlException ace)
    {
    }

    m_tabs = new MyTabbedPane();

    getContentPane().add(m_tabs);
    
    setVisible(true);
    m_tabs.setOpaque(false);

  }

  private void putTab(String i_name,Component i_component)
  {
    m_tabs.addTab(i_name,i_component);
    m_tabs.revalidate();
    i_component.setMaximumSize(i_component.getSize(null)); 

    JPanel jp = new JPanel();
    jp.setOpaque(false);
    jp.setLayout(new BoxLayout(jp,BoxLayout.X_AXIS));
    //    jp.setLayout(new FlowLayout(FlowLayout.LEFT,1,0));

    ImageIcon ii = ImageLoader.MakeImageIcon(NetworkStuff.COMMONDISTRO + "/outbutton.PNG");
    JButton jb = new JButton(ii);
    jb.addActionListener(new ExportButtonListener(i_component));
    jb.setMargin(new Insets(0,0,0,0));

    JLabel jl = new JLabel(i_name);

    jp.add(jb);
    jp.add(Box.createHorizontalStrut(5));
    jp.add(jl);

    m_tabs.setTabComponentAt(m_tabs.indexOfComponent(i_component),jp);

    m_tabinfo.get(i_component).m_label = jl;
  }


  void addTab(String i_name,Component i_component,boolean i_switchto)
  {
    TabInfo ti = new TabInfo();
    ti.m_task = null;
    ti.m_label = null;
    ti.m_name = i_name;
    ti.m_frame = null;
    m_tabinfo.put(i_component,ti);

    putTab(i_name,i_component);

    if (i_switchto)
    {
      m_tabs.setSelectedIndex(m_tabs.indexOfComponent(i_component));
    }

  }

  // cleans the tab out of the tab window.
  private void unTab(Component i_component)
  {
    CancelComponentTimerTask(i_component);
    m_tabs.remove(i_component);
    m_tabs.revalidate();
  }


  void removeTab(Component i_component)
  {
    TabInfo ti = m_tabinfo.get(i_component);
    if (ti == null) return;

    if (ti.m_frame != null)
    {
      ti.m_frame.dispose();
    }
    else
    {
      unTab(i_component);
    }

    m_tabinfo.remove(i_component);
  }

  void RefreshMe()
  {
    setVisible(true);
    Insets iset = getInsets();
    setSize(m_width+iset.left+iset.right,m_height+iset.top+iset.bottom);
  }
  
  private static final long serialVersionUID = 1;

}

      
