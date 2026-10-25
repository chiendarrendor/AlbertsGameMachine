package Outpost;

import NodeInterfacePackage.*;
import MySwing.HTMLTable;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import javax.swing.JPanel;
import javax.swing.JLabel;
import org.json.JSONObject;
import org.json.JSONArray;
import org.doghouse.jsonpath.JsonQueryException;

public class TurnOrder implements UserDefinedInterface
{
  private static final GuiPacketQuery s_stateQuery = new GuiPacketQuery("{STATE}");
  private static final GuiPacketQuery s_myPlayerIdQuery = new GuiPacketQuery("myPlayerId");
  private static final GuiPacketQuery s_playersQuery = new GuiPacketQuery("players[]");
  private static final GuiPacketQuery s_turnOrderQuery = new GuiPacketQuery("turnOrder[]");
  private static final GuiPacketQuery s_winnersQuery = new GuiPacketQuery("winners[]");

  private JLabel m_label;

  // Plain per-delivery value object -- rebuilt fresh from players[] on every
  // HandleEvent, never persisted. Replaces the old sparse, incrementally-
  // grown Vector<PlayerInfo> that waited for ~8 different broadcast events
  // to trickle in; players[] already arrives as the complete picture every
  // single time, so there's nothing left to accumulate.
  private static class PlayerInfo
  {
    final int m_id;
    final String m_name;
    final boolean m_iam;
    final boolean m_okstartup;
    final int m_vp;
    final int m_costsum;
    final int m_pstate;
    final int m_bstate;
    final boolean m_haspurchased;
    final int m_highbid;
    final int m_lastbid;
    final boolean m_wins;
    final int m_resources;
    final int m_income;

    PlayerInfo(JSONObject i_player,Integer i_myPlayerId,Set<Integer> i_winners)
    {
      m_id = i_player.getInt("id");
      m_name = i_player.getString("name");
      m_iam = i_myPlayerId != null && i_myPlayerId == m_id;
      m_okstartup = i_player.getBoolean("okstartup");
      m_vp = i_player.getInt("victoryPoints");
      m_costsum = i_player.getInt("itemCostSum");
      m_pstate = i_player.getInt("purchaseState");
      m_bstate = i_player.getInt("bidState");
      m_haspurchased = i_player.getBoolean("hasPurchased");
      m_highbid = i_player.getInt("highBid");
      m_lastbid = i_player.getInt("lastBid");
      m_wins = i_winners.contains(m_id);

      int resources = 0;
      for (CommodityInfo ci : CommodityInfo.ParseCommodityString(i_player.getString("cardBackDescription")))
      {
        resources += ci.GetValue();
      }
      m_resources = resources;

      int income = 0;
      for (FactoryWidget fw : FactoryWidget.ParseFactoryString(i_player.getString("factoryDescription"),"",false,null))
      {
        if (fw.GetManType() != 'U')
        {
          income += fw.GetFactoryMeanValue();
        }
      }

      JSONArray items = i_player.getJSONArray("items");
      for (int i = 0 ; i < items.length() ; ++i)
      {
        int itemId = items.getInt(i);
        if (itemId == 5) income += 13;
        if (itemId == 6) income += 17;
      }
      m_income = income;
    }

    public String GetName() { return m_name; }
    public boolean IAm() { return m_iam; }
    public int GetVP() { return m_vp; }
    public int GetCostSum() { return m_costsum; }
    public int GetPurchaseState() { return m_pstate; }
    public int GetBidState() { return m_bstate; }
    public boolean HasPurchased() { return m_haspurchased; }
    public int GetHighBid() { return m_highbid; }
    public int GetLastBid() { return m_lastbid; }
    public boolean OkStartup() { return m_okstartup; }
    public boolean Wins() { return m_wins; }
    public int GetResources() { return m_resources; }
    public int GetIncome() { return m_income; }
  }

  public TurnOrder(HashMap<String,String> i_Options,String i_DataLoc,JPanel i_Panel,
                   ExternalAssets i_assets)
  {
    m_label = new JLabel();
    i_Panel.add(m_label);
  }

  private void SetPurchaseLabel(HTMLTable.CellContents i_jlabel,int i_purchasestate)
  {
    switch(i_purchasestate)
    {
    case 0: // unplayed
      i_jlabel.SetForeColor("#000000").SetBackColor("#00FF00").SetText("incipient").SetHAlign("center"); break;
    case 1: // active
      i_jlabel.SetForeColor("#000000").SetBackColor("#FF0000").SetText("active").SetHAlign("center"); break;
    case 2: // internalizing
      i_jlabel.SetForeColor("#000000").SetBackColor("#FFFF00").SetText("internalizing").SetHAlign("center"); break;
    case 3: // done
      i_jlabel.SetForeColor("#BEBEBE").SetBackColor("#FFFFFF").SetText("done").SetHAlign("center"); break;
    case 4: // internal done
      i_jlabel.SetForeColor("#BEBEBE").SetBackColor("#FFFFFF").SetText("internalized").SetHAlign("center"); break;
    case 5: // final done
      i_jlabel.SetForeColor("#BEBEBE").SetBackColor("#FFFFFF").SetText("DONE").SetHAlign("center"); break;
    case 6: // internal final done
      i_jlabel.SetForeColor("#BEBEBE").SetBackColor("#FFFFFF").SetText("INTERNALIZED").SetHAlign("center"); break;
    default: // ???
      i_jlabel.SetForeColor("#000000").SetBackColor("#FFFFFF").SetText(""+i_purchasestate).SetHAlign("center"); break;
    }
  }

  private void SetBidLabel(HTMLTable.CellContents i_jlabel,int i_bidstate,boolean i_bidStateActive)
  {
    if (!i_bidStateActive)
    {
      i_jlabel.SetForeColor("#BEBEBE").SetBackColor("#FFFFFF").SetText("n/a").SetHAlign("center");
      return;
    }

    switch(i_bidstate)
    {
    case 0: // UNBID
      i_jlabel.SetForeColor("#000000").SetBackColor("#00FF00").SetText("incipient").SetHAlign("center"); break;
    case 1: // CURBID
      i_jlabel.SetForeColor("#000000").SetBackColor("#FF0000").SetText("active").SetHAlign("center"); break;
    case 2: // PASS
      i_jlabel.SetForeColor("#000000").SetBackColor("#0000FF").SetText("pass").SetHAlign("center"); break;
    case 3: // PASS_OUT
      i_jlabel.SetForeColor("#BEBEBE").SetBackColor("#FFFFFF").SetText("passed out").SetHAlign("center"); break;
    case 4: // HASBID
      i_jlabel.SetForeColor("#000000").SetBackColor("#FFFF00").SetText("has bid").SetHAlign("center"); break;
    default: // ???
      i_jlabel.SetForeColor("#000000").SetBackColor("#FFFFFF").SetText(""+i_bidstate).SetHAlign("center"); break;
    }
  }

  private void RedrawPreGame(List<PlayerInfo> i_players)
  {
    HTMLTable ht = new HTMLTable(i_players.size()+1,2,false);

    ht.GetCell(0,0).SetText("Names:").SetHAlign("right");
    ht.GetCell(0,1).SetText("Okayed Start:").SetHAlign("right");

    for (int i = 0 ; i < i_players.size() ; ++i)
    {
      PlayerInfo pi = i_players.get(i);
      ht.GetCell(i+1,0).SetText(pi.GetName()).SetHAlign("center");
      if (pi.IAm())
      {
        ht.GetCell(i+1,0).SetBackColor("#00FF00");
      }
      ht.GetCell(i+1,1).SetText(pi.OkStartup() ? "OK" : "").SetHAlign("center");
    }

    m_label.setText(ht.GetHTML());
  }

  private void RedrawTerminal(List<PlayerInfo> i_players)
  {
    List<PlayerInfo> byVP = new ArrayList<PlayerInfo>(i_players);
    byVP.sort((a,b) -> b.GetVP() - a.GetVP());

    HTMLTable ht = new HTMLTable(byVP.size()+1,3,false);

    ht.GetCell(0,0).SetText("Names:").SetHAlign("right");
    ht.GetCell(0,1).SetText("VP:").SetHAlign("right");
    ht.GetCell(0,2).SetText("Won?").SetHAlign("right");

    for (int i = 0 ; i < byVP.size() ; ++i)
    {
      PlayerInfo pi = byVP.get(i);
      ht.GetCell(i+1,0).SetText(pi.GetName()).SetHAlign("center");
      if (pi.IAm())
      {
        ht.GetCell(i+1,0).SetBackColor("#00FF00");
      }
      ht.GetCell(i+1,1).SetText(""+pi.GetVP()).SetHAlign("center");
      ht.GetCell(i+1,2).SetText(pi.Wins() ? "YES!" : "").SetHAlign("center");
    }

    m_label.setText(ht.GetHTML());
  }

  private void RedrawInGame(List<PlayerInfo> i_playersInTurnOrder,boolean i_bidStateActive)
  {
    int legendcol = 0;
    int namerow = 0;
    int vprow = 1;
    int estrow = 2;
    int actrow = 3;
    int bidrow = 4;
    int bhistrow = 5;
    int hasprow = 6;

    HTMLTable ht = new HTMLTable(i_playersInTurnOrder.size()+1,7,false);

    ht.GetCell(legendcol,namerow).SetText("Names:").SetHAlign("right");
    ht.GetCell(legendcol,vprow).SetText("VP(Item Cost):").SetHAlign("right");
    ht.GetCell(legendcol,estrow).SetText("Est. Resources/Income:").SetHAlign("right");
    ht.GetCell(legendcol,actrow).SetText("Action Status:").SetHAlign("right");
    ht.GetCell(legendcol,bidrow).SetText("Bidding Status:").SetHAlign("right");
    ht.GetCell(legendcol,bhistrow).SetText("Last Bids:").SetHAlign("right");
    ht.GetCell(legendcol,hasprow).SetText("Has Purchased Item:").SetHAlign("right");

    if (i_bidStateActive)
    {
      ht.GetCell(legendcol,bidrow).SetBackColor("#00FF00");
    }

    for (int i = 0 ; i < i_playersInTurnOrder.size() ; ++i)
    {
      PlayerInfo pi = i_playersInTurnOrder.get(i);
      int playercol = i + 1;

      ht.GetCell(playercol,namerow).SetText(pi.GetName()).SetHAlign("center");
      if (pi.IAm())
      {
        ht.GetCell(playercol,namerow).SetBackColor("#00FF00");
      }

      ht.GetCell(playercol,vprow).SetText(pi.GetVP() + "(" + pi.GetCostSum() + ")").SetHAlign("center");
      SetPurchaseLabel(ht.GetCell(playercol,actrow),pi.GetPurchaseState());
      SetBidLabel(ht.GetCell(playercol,bidrow),pi.GetBidState(),i_bidStateActive);

      if (pi.GetLastBid() != -1)
      {
        ht.GetCell(playercol,bhistrow).SetText(""+pi.GetLastBid()).SetHAlign("center");
        if (pi.GetHighBid() != -1)
        {
          ht.GetCell(playercol,bhistrow).SetBackColor("#00FF00");
          if (pi.GetBidState() == 1) // cur bidder and high bid -> player is buying item.
          {
            ht.GetCell(playercol,bidrow).SetText("buying").SetHAlign("center").SetBackColor("#FF00FF");
          }
        }
      }
      ht.GetCell(playercol,hasprow).SetText(pi.HasPurchased() ? "X" : "").SetHAlign("center");
      ht.GetCell(playercol,estrow).SetText("" + pi.GetResources() + "/" + pi.GetIncome()).SetHAlign("center");
    }

    m_label.setText(ht.GetHTML());
  }

  @SuppressWarnings("unchecked")
  public void HandleEvent(GameEvent i_event)
  {
    List<Object> playersRaw = (List<Object>)s_playersQuery.evaluate(i_event);
    Integer myPlayerId = (Integer)s_myPlayerIdQuery.evaluate(i_event);

    // turnOrder/winners are both built on the far side of the same
    // InitialState/ValidateSetup early return in GetClientState -- neither
    // is present yet pre-STARTGAME. Reading one tells us about both.
    List<Object> turnOrderRaw;
    List<Object> winnersRaw;
    try
    {
      turnOrderRaw = (List<Object>)s_turnOrderQuery.evaluate(i_event);
      winnersRaw = (List<Object>)s_winnersQuery.evaluate(i_event);
    }
    catch (JsonQueryException jqe)
    {
      turnOrderRaw = null;
      winnersRaw = null;
    }

    Set<Integer> winners = new HashSet<Integer>();
    if (winnersRaw != null)
    {
      for (Object o : winnersRaw)
      {
        winners.add((Integer)o);
      }
    }

    HashMap<Integer,PlayerInfo> byId = new HashMap<Integer,PlayerInfo>();
    List<PlayerInfo> inPlayerOrder = new ArrayList<PlayerInfo>();
    for (Object o : playersRaw)
    {
      PlayerInfo pi = new PlayerInfo((JSONObject)o,myPlayerId,winners);
      byId.put(pi.m_id,pi);
      inPlayerOrder.add(pi);
    }

    if (turnOrderRaw == null)
    {
      RedrawPreGame(inPlayerOrder);
    }
    else if (!winners.isEmpty())
    {
      RedrawTerminal(inPlayerOrder);
    }
    else
    {
      List<PlayerInfo> inTurnOrder = new ArrayList<PlayerInfo>();
      for (Object o : turnOrderRaw)
      {
        inTurnOrder.add(byId.get((Integer)o));
      }
      boolean bidStateActive = "Bid".equals((String)s_stateQuery.evaluate(i_event));
      RedrawInGame(inTurnOrder,bidStateActive);
    }
  }

  public void Refresh()
  {
  }

  public void Reset()
  {
    m_label.setText("");
  }
}
