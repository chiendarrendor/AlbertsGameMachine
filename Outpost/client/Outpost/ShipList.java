package Outpost;

import java.util.HashMap;
import java.util.List;
import java.util.Objects;
import NodeInterfacePackage.*;
import org.doghouse.jsonpath.JsonQueryException;
import org.json.JSONObject;
import javax.swing.JPanel;
import javax.swing.Box;
import javax.swing.BoxLayout;

public class ShipList implements UserDefinedInterface,ActionVariableGenerator,ShipActionGo
{
  private static final String s_name="ShipSlot";

  private static final GuiPacketQuery s_stateQuery = new GuiPacketQuery("{STATE}");
  private static final GuiPacketQuery s_bidderQuery = new GuiPacketQuery("currentBidderId");
  private static final GuiPacketQuery s_shipQuery = new GuiPacketQuery("supplyShip[]");
  private static final GuiPacketQuery s_purchaseQuery = new GuiPacketQuery("purchaseData[]");
  private static final GuiPacketQuery s_bidItemQuery = new GuiPacketQuery("bidInfo.itemShipIndex");
  private static final GuiPacketQuery s_robotsQuery = new GuiPacketQuery("options.robots");
  private static final GuiPacketQuery s_playersQuery = new GuiPacketQuery("players[]");
  private static final GuiPacketQuery s_legalOpenBidQuery = new GuiPacketQuery("{LEGALACTION:OPENBID}");

  private JPanel m_panel;
  private ShipItem[] m_slots = new ShipItem[0];
  private ActionTransferManager m_atm;
  private String m_Selection;

  private String m_lastState = null;
  private int m_lastBidderId = Integer.MIN_VALUE;

  public ShipList(HashMap<String,String> i_Options,String i_DataLoc,JPanel i_Panel,ActionTransferManager i_atm)
  {
    ItemByNumber.DoOnce(i_DataLoc);

    m_Selection = null;
    m_atm = i_atm;
    m_atm.AddVariableGenerator(s_name,this);

    m_panel = new JPanel();
    m_panel.setOpaque(false);
    m_panel.setLayout(new BoxLayout(m_panel,BoxLayout.X_AXIS));
    i_Panel.add(m_panel);
  }

  private void RebuildSlots(int i_count)
  {
    m_panel.removeAll();
    m_slots = new ShipItem[i_count];

    int i;
    for (i = 0 ; i < i_count ; ++i)
    {
      m_slots[i] = new ShipItem(this,Integer.toString(i));
      m_panel.add(m_slots[i]);
      m_panel.add(Box.createHorizontalStrut(5));
    }

    m_panel.revalidate();
    m_panel.repaint();
  }

  @SuppressWarnings("unchecked")
  public void HandleEvent(GameEvent i_event)
  {
    String curState = (String)s_stateQuery.evaluate(i_event);
    int curBidder = s_bidderQuery.evaluateInt(i_event,-1);
    if (!Objects.equals(curState,m_lastState) || curBidder != m_lastBidderId)
    {
      ShipActionGo(null);
    }
    m_lastState = curState;
    m_lastBidderId = curBidder;

    List<Object> shipList = (List<Object>)s_shipQuery.evaluate(i_event);
    if (shipList.size() != m_slots.length)
    {
      RebuildSlots(shipList.size());
    }

    int n = m_slots.length;
    if (n == 0) return;

    int[] itemIds = new int[n];
    for (int i = 0 ; i < n ; ++i)
    {
      itemIds[i] = (Integer)shipList.get(i);
    }

    int[] purchasedItemIds = new int[n];
    int[] purchasedPlayerIds = new int[n];
    int[] purchasedCosts = new int[n];
    try
    {
      for (Object o : (List<Object>)s_purchaseQuery.evaluate(i_event))
      {
        JSONObject p = (JSONObject)o;
        int idx = p.getInt("shipIndex");
        purchasedItemIds[idx] = p.getInt("itemId");
        purchasedPlayerIds[idx] = p.getInt("playerId");
        purchasedCosts[idx] = p.getInt("cost");
      }
    }
    catch (JsonQueryException jqe)
    {
    }

    HashMap<Integer,String> playerNames = new HashMap<Integer,String>();
    for (Object o : (List<Object>)s_playersQuery.evaluate(i_event))
    {
      JSONObject p = (JSONObject)o;
      playerNames.put(p.getInt("id"),p.getString("name"));
    }

    int bidShipIndex = s_bidItemQuery.evaluateInt(i_event,-1);
    boolean legalOpenBid = s_legalOpenBidQuery.evaluateBoolean(i_event,false);
    int robotIconNumber = 70 + (Integer)s_robotsQuery.evaluate(i_event);

    for (int i = 0 ; i < n ; ++i)
    {
      String buyerName = playerNames.get(purchasedPlayerIds[i]);
      m_slots[i].update(itemIds[i],robotIconNumber,legalOpenBid,i == bidShipIndex,
                        purchasedItemIds[i],purchasedPlayerIds[i],purchasedCosts[i],
                        buyerName == null ? "???" : buyerName);
    }
  }

  public void Refresh()
  {
  }

  public void ShipActionGo(String i_idx)
  {
    ActionVariableReset();
    m_atm.ReportNewValueToConsumers(s_name,i_idx);
    m_Selection = i_idx;
  }

  public void Reset()
  {
    RebuildSlots(0);
    m_lastState = null;
    m_lastBidderId = Integer.MIN_VALUE;
  }

  public void ActionVariableReset()
  {
    for (ShipItem item : m_slots)
    {
      item.ClearRed();
    }
  }

  public String ActionVariableGetValue()
  {
    return m_Selection;
  }
}
