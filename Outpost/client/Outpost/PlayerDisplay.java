package Outpost;

import java.util.HashMap;
import java.util.Set;
import java.util.Map;
import java.util.HashSet;
import java.util.Vector;
import java.util.List;
import NodeInterfacePackage.*;
import javax.swing.JPanel;
import javax.swing.JTabbedPane;
import javax.swing.JLabel;
import MySwing.LockLayout;
import java.awt.Color;
import java.awt.Point;
import org.json.JSONObject;
import org.doghouse.jsonpath.JsonPath;


import Outpost.Item;

public class PlayerDisplay implements UserDefinedInterface
{  
  private class PlayerPane extends JPanel
  {
    private ItemList m_ilist;
    private CommodityHand m_chand;
    private Factories m_factories;
    
    public PlayerPane(String i_playername, String i_DataLoc, ActionTransferManager i_atm)
    {
      super(new LockLayout());
      // this is the standard var/value configuration for a UserDefinedInterface, which ItemList, CommodityHand, and Factories all implement)
      HashMap<String,String> inithash = new HashMap<String,String>();
      inithash.put("playername",i_playername);

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
  private Map<String,PlayerPane> m_playermap;
  private String m_DataLoc;
  private ActionTransferManager m_atm;

  public PlayerDisplay(HashMap<String,String> i_Options,String i_DataLoc,JPanel i_Panel,ActionTransferManager i_atm)
  {
    m_tabs = new JTabbedPane();
    i_Panel.add(m_tabs);

    m_playermap = new HashMap<>();;
    m_DataLoc = i_DataLoc;
    m_atm = i_atm;
  }

  private static GuiPacketQuery selfnamegpq = new GuiPacketQuery("{ME}");
  private static GuiPacketQuery playergpq = new GuiPacketQuery("players[]");
  // these read data from the Player JSON object.
  private static JsonPath playerNameQuery = JsonPath.compile("name");

  // we are assuming that whatver player list we are sent is always right, even if
  // it differs from a previous on (which will happen when players JOIN or UNJOIN)
  // however, once the game starts, the list of players will be fixed.
  // Because the tabbed pane and the server player list keep track of player id's exactly the same
  // way, we could continue to use player id as index into tabbed pane, but I think it was
  // safer to handle them entirely independently here; this code only understands 
  // player names to Player Panes, and queries the Tabbed Pane whenever we want the tab index.
  @SuppressWarnings("unchecked")
  public void HandleEvent(GameEvent i_event)
  {
    String me = (String)selfnamegpq.evaluate(i_event);
    Set<String> playersNotSeen = new HashSet<>(m_playermap.keySet());
    
    for (JSONObject player : (List<JSONObject>)playergpq.evaluate(i_event)) {
      String pname = (String)playerNameQuery.evaluate(player);
      playersNotSeen.remove(pname);

      if (!m_playermap.containsKey(pname)) {
        PlayerPane ppane = new PlayerPane(pname, m_DataLoc, m_atm);
        m_playermap.put(pname, ppane);
        m_tabs.add(pname, ppane);

        JLabel jl = new JLabel(pname);
        jl.setOpaque(true);
        // we are not going to assume that the tabs ended up in numeric order, but 
        // fortunately, the JTabbedPane keeps track of everything by inserted component identifier
        int curtabindex = m_tabs.indexOfComponent(ppane);
        if (me.equals(pname)) {
          jl.setBackground(Color.green);
          m_tabs.setSelectedIndex(curtabindex);
        }
        m_tabs.setTabComponentAt(curtabindex, jl);
      }
      // once we get here, we know that there's an entry in the map for the current player.
      m_playermap.get(pname).HandleEvent(i_event);
    }

    // once we get here, we've handled all the players in the FSS.  If there's a player tab left over, it doesn't belong to us.
    for (String doomedname : playersNotSeen) {
      PlayerPane doomedpane = m_playermap.get(doomedname);
      doomedpane.odispose();
      m_playermap.remove(doomedname);
      m_tabs.remove(doomedpane);
    }
  }

  public void Refresh()
  {
    for (PlayerPane pp : m_playermap.values()) pp.Refresh();
  }

  public void Reset()
  {
    for (PlayerPane pp : m_playermap.values()) pp.odispose();
    m_playermap.clear();
    m_tabs.removeAll();
  }
}
