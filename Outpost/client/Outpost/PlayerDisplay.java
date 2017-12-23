package Outpost;

import java.util.HashMap;
import java.util.Vector;
import NodeInterfacePackage.*;
import javax.swing.JPanel;
import javax.swing.JTabbedPane;
import javax.swing.JLabel;
import MySwing.LockLayout;
import java.awt.Color;
import java.awt.Point;


import Outpost.Item;

public class PlayerDisplay implements UserDefinedInterface
{  
  private class PlayerPane extends JPanel
  {
    private ItemList m_ilist;
    private CommodityHand m_chand;
    private Factories m_factories;
    
    public PlayerPane(String i_playernum,String i_DataLoc,ActionTransferManager i_atm)
    {
      super(new LockLayout());
      HashMap<String,String> inithash = new HashMap<String,String>();
      inithash.put("playernum",i_playernum);

      JPanel ipanel;


      ipanel = new JPanel();
      m_ilist = new ItemList(inithash,i_DataLoc,ipanel,i_atm);
      this.add(ipanel,new Point(0,0));

      ipanel = new JPanel();
      m_chand = new CommodityHand(inithash,i_DataLoc,ipanel,i_atm);
      this.add(ipanel,new Point(0,90));

      ipanel = new JPanel();
      m_factories = new Factories(inithash,i_DataLoc,ipanel,i_atm);
      this.add(ipanel,new Point(0,200));
    }

    public void odispose()
    {
      m_ilist.odispose();
      m_chand.odispose();
      m_factories.odispose();
    }

    public void HandleEvent(GameEvent i_event)
    {
      m_ilist.HandleEvent(i_event);
      m_chand.HandleEvent(i_event);
      m_factories.HandleEvent(i_event);
    }

    public void Refresh()
    {
      m_ilist.Refresh();
      m_chand.Refresh();
      m_factories.Refresh();
    }
  }

  private JTabbedPane m_tabs;
  private Vector<PlayerPane> m_players;
  private String m_DataLoc;
  private ActionTransferManager m_atm;

  public PlayerDisplay(HashMap<String,String> i_Options,String i_DataLoc,JPanel i_Panel,ActionTransferManager i_atm)
  {
    m_tabs = new JTabbedPane();
    i_Panel.add(m_tabs);

    m_players = new Vector<PlayerPane>();
    m_DataLoc = i_DataLoc;
    m_atm = i_atm;
  }

  public void HandleEvent(GameEvent i_event)
  {
    if (i_event.GetVarValue("eventname").equals("PLAYER"))
    {
      String pname = i_event.GetVarValue("PlayerName");
      String pnum = i_event.GetVarValue("PlayerNumber");
      int pint = Integer.parseInt(pnum);

      PlayerPane ppane = new PlayerPane(pnum,m_DataLoc,m_atm);
      m_players.add(ppane);
      m_tabs.add(pname,ppane);
      JLabel jl = new JLabel(pname);
      jl.setOpaque(true);
      m_tabs.setTabComponentAt(pint,jl);

    }

    if (i_event.GetVarValue("eventname").equals("IAM"))
    {
      String pnum = i_event.GetVarValue("PlayerNumber");
      int pint = Integer.parseInt(pnum);
      JLabel jl = (JLabel)m_tabs.getTabComponentAt(pint);
      jl.setBackground(Color.green);
      m_tabs.setSelectedIndex(pint);
    }

    int i;
    for (i = 0 ; i < m_players.size() ; ++i)
    {
      m_players.get(i).HandleEvent(i_event);
    }
  }

  public void Refresh()
  {
    int i;
    for (i = 0 ; i < m_players.size() ; ++i)
    {
      m_players.get(i).Refresh();
    }
  }

  public void Reset()
  {

    int i;
    for (i = 0 ; i < m_players.size() ; ++i)
    {
      m_players.get(i).odispose();
    }

    m_tabs.removeAll();
  }
}
