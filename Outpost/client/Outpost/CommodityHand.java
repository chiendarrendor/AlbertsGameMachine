package Outpost;

import java.util.HashMap;
import java.util.Vector;
import NodeInterfacePackage.*;
import javax.swing.JPanel;
import javax.swing.JLabel;
import javax.swing.BoxLayout;
import javax.swing.JCheckBox;
import java.awt.Point;
import java.awt.event.ItemListener;
import java.awt.event.ItemEvent;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import javax.swing.JButton;
import javax.swing.Icon;
import org.json.JSONObject;
import org.doghouse.jsonpath.JsonPath;
import java.util.List;

public class CommodityHand implements UserDefinedInterface,ActionListener
{
  private String m_PlayerName;
  private boolean m_IAM;
  private JPanel m_panel;
  private CommodityNotesPanel m_notepanel;
  private JPanel m_cardpanel;
  private HandListener m_listener;
  private ActionTransferManager m_atm;
  private Vector<CommodityInfo> m_vci;
  private Vector<JCheckBox> m_boxes;
  private String m_lastPString = null;

  public CommodityHand(HashMap<String,String> i_Options,String i_DataLoc,JPanel i_Panel,ActionTransferManager i_atm)
  {
    m_atm = i_atm;
    m_listener = null;
    m_IAM = false;
    m_vci = new Vector<CommodityInfo>();
    m_boxes = new Vector<JCheckBox>();

    if (!i_Options.containsKey("playername")) throw new NodeInterfacePackageException("missing option playername");
    m_PlayerName = i_Options.get("playername");

    m_panel = new JPanel();
    m_panel.setOpaque(false);
    m_panel.setLayout(new BoxLayout(m_panel,BoxLayout.X_AXIS));
    m_panel.setVisible(true);
    i_Panel.add(m_panel);

    m_notepanel = new CommodityNotesPanel(m_vci);
    m_panel.add(m_notepanel);

    m_cardpanel = new JPanel();
    m_cardpanel.setOpaque(false);
    m_cardpanel.setVisible(true);
    m_cardpanel.setLayout(new BoxLayout(m_cardpanel,BoxLayout.X_AXIS));
    m_panel.add(m_cardpanel);
  }

  private static GuiPacketQuery smallResearchQuery = new GuiPacketQuery("options.smallresearch");
  private static GuiPacketQuery smallMicroQuery = new GuiPacketQuery("options.smallmicro");
  private static JsonPath playerHandSizePath = JsonPath.compile("handLimit");
  private static JsonPath cardBackDescriptionPath = JsonPath.compile("cardBackDescription");
  private static GuiPacketQuery playerCardFrontDescriptionQuery = new GuiPacketQuery("myHand");

  public void HandleEvent(GameEvent i_event)
  {
    String me = EventHelper.getMe(i_event);
    m_IAM = me.equals(m_PlayerName);
    JSONObject player = EventHelper.getJSONOfPlayer(m_PlayerName, i_event);

    m_notepanel.SetSmalls((Boolean)smallResearchQuery.evaluate(i_event),(Boolean)smallMicroQuery.evaluate(i_event));
    m_notepanel.SetMaxWeight((Integer)playerHandSizePath.evaluate(player));
    if (m_IAM) {
      m_notepanel.SetIAM();
      if (m_listener == null) m_listener = new HandListener(m_atm, m_vci, m_notepanel);
    }
    m_notepanel.Redraw();

    String cardBacks = (String)cardBackDescriptionPath.evaluate(player);
    // this can be null if me is not a player, or if we haven't gotten to commodity cards yet, but
    // the standard string for "no cards" is "||" so we'll keep that.
    String cardFronts = (String)playerCardFrontDescriptionQuery.evaluateString(i_event,"||");

    String pstring = m_IAM ? cardFronts : cardBacks;

    if (m_lastPString != null && pstring.equals(m_lastPString)) return;
    m_lastPString = pstring;

    try
    {
      Vector<CommodityInfo> vci = CommodityInfo.ParseCommodityString(pstring);
      Reset();

      m_vci.clear();
      m_boxes.clear();
      m_vci.addAll(vci);

      m_notepanel.Redraw();

      if (m_IAM) 
      {
        m_listener.ClearBoxes();
        if (m_vci.size() > 0)
        {
          JButton jb = new JButton("Select All");
          m_cardpanel.add(jb);
          jb.addActionListener(this);
        }
      }

      int i;
      for (i = 0 ; i < m_vci.size() ; ++i)
      {
        JPanel vpanel = new JPanel();
        vpanel.setOpaque(false);
        vpanel.setLayout(new BoxLayout(vpanel,BoxLayout.Y_AXIS));
        vpanel.setVisible(true);
        m_cardpanel.add(vpanel);

        JCheckBox jcb = new JCheckBox();
        Icon baseicon = Commodity.getIcon(m_vci.get(i).GetCommType(),
                                          m_vci.get(i).GetValueString(),
                                          m_vci.get(i).IsMega(),
                                          m_vci.get(i).IsAvg(),
                                          false);
        jcb.setIcon(baseicon);
        jcb.setDisabledIcon(baseicon);
        jcb.setSelectedIcon(Commodity.getIcon(m_vci.get(i).GetCommType(),
                                              m_vci.get(i).GetValueString(),
                                              m_vci.get(i).IsMega(),
                                              m_vci.get(i).IsAvg(),
                                              true));
        vpanel.add(jcb);
        jcb.setEnabled(false);

        if (m_IAM)
        {
          jcb.setEnabled(true);
          m_listener.AddBox(jcb);
          m_boxes.add(jcb);
        }
      }

      if (m_IAM)
      {
        m_listener.itemStateChanged(null);
      }

      m_cardpanel.revalidate();
      m_cardpanel.repaint();
    }
    catch(NodeInterfacePackageException nipe)
    {
      return;
    }
  }

  public void Refresh()
  {
  }

  public void odispose()
  {
    if (m_listener != null) m_listener.odispose();
  }

  public void Reset()
  {
    m_cardpanel.removeAll();
    m_lastPString = null;
  }

  // action listener
  public void actionPerformed(ActionEvent i_event)
  {
    int i;
    for (i = 0 ; i < m_boxes.size() ; ++i)
    {
      m_boxes.get(i).setSelected(true);
    }
  }


}
