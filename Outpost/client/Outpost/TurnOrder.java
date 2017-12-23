package Outpost;

import NodeInterfacePackage.*;
import MySwing.HTMLTable;
import java.util.HashMap;
import java.util.Vector;
import javax.swing.JPanel;
import javax.swing.JLabel;
import java.awt.Color;
import java.awt.GridLayout;
import javax.swing.BorderFactory;
import javax.swing.SwingConstants;

public class TurnOrder implements UserDefinedInterface
{
  Vector<PlayerInfo> m_playerinfos;
  int[] m_turnorders;
  boolean m_winstate;
  boolean m_bidstate;
  JLabel m_label;

  
  private class PlayerInfo
  {
    String m_name;
    boolean m_iam;
    int m_vp;
    int m_costsum;
    int m_pstate;
    int m_bstate;
    boolean m_haspurchased;
    int m_highbid;
    int m_lastbid;
    boolean m_okstartup;
    boolean m_wins;
    int m_resources;
    int m_factoryincome;
    HashMap<String,String> m_items;

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

    public int GetIncome() 
    { 
      int itemincome = 0;
      Object[] values = m_items.values().toArray();
      int i;
      for (i = 0 ; i < values.length ; ++i)
      {
        if (((String)values[i]).equals("5")) itemincome += 13;
        if (((String)values[i]).equals("6")) itemincome += 17;
      }

      return m_factoryincome + itemincome; 
    }

    PlayerInfo(String i_name)
    {
      m_name = i_name;
      m_iam = false;
      m_vp = 0;
      m_costsum = 0;
      m_pstate = 3;
      m_bstate = 3;
      m_haspurchased = false;
      m_highbid = -1;
      m_lastbid = -1;
      m_okstartup = false;
      m_wins = false;
      m_resources = 0;
      m_factoryincome = 0;
      m_items = new HashMap<String,String>();
    }

    void SetResources(int i_resources)
    {
      m_resources = i_resources;
    }
    
    void SetItem(String i_itemindex,String i_itemvalue)
    {
      m_items.put(i_itemindex,i_itemvalue);
    }

    void SetFactoryIncome(int i_factoryincome)
    {
      m_factoryincome = i_factoryincome;
    }

    void SetWin()
    {
      m_wins = true;
    }

    void SetIAm(boolean i_iam)
    {
      m_iam = i_iam;
    }

    void SetOkStartup(boolean i_oktostart)
    {
      m_okstartup = i_oktostart;
    }

    void SetPlayerState(int i_vp,int i_costsum,int i_pstate,int i_bstate,
                        boolean i_haspurchased,int i_highbid,int i_lastbid)
    {
      m_vp = i_vp;
      m_costsum = i_costsum;
      m_pstate = i_pstate;
      m_bstate = i_bstate;
      m_haspurchased = i_haspurchased;
      m_highbid = i_highbid;
      m_lastbid = i_lastbid;
    }
  }



  public TurnOrder(HashMap<String,String> i_Options,String i_DataLoc,JPanel i_Panel,
                   ActionTransferManager i_atm)
  {
    m_winstate = false;
    m_bidstate = false;
    m_playerinfos = new Vector<PlayerInfo>();
    m_turnorders = null;

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

  private void SetBidLabel(HTMLTable.CellContents i_jlabel,int i_bidstate)
  {
    if (m_bidstate == false)
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

  private void Redraw()
  {
    // display:
    // prior to turnorder being established:
    //   name (>name< if IAM is true)
    //   OK or blank (okstartup)
    // after turnorder being established:
    //   name (>name< if IAM is true)
    //   VP(costsum)
    //   Purchase State: DONE/INTERNAL DONE is gray, UNPLAYED is green, INTERNALIZING is yellow, ACTIVE is red
    //   Bid State: PASS_OUT is gray, UNBID is green, PASS is blue, HAS BID is yellow, CURBID is red
    //   High Bid: only display if not -1
    //   Purchased: mark with X or not.

    int newRows;
    int i;

    if (m_turnorders == null) newRows = 2;
    else if (m_winstate) newRows = 3;
    else newRows = 7;

    int newCols = m_playerinfos.size() + 1;

    HTMLTable ht = new HTMLTable(newCols,newRows,false);

    ht.GetCell(0,0).SetText("Names:").SetHAlign("right");

    if (newRows == 2)
    {
      ht.GetCell(0,1).SetText("Okayed Start:").SetHAlign("right");
    }
    else if (newRows == 3)
    {
      ht.GetCell(0,1).SetText("VP:").SetHAlign("right");
      ht.GetCell(0,2).SetText("Won?").SetHAlign("right");
    }
    else
    {
      ht.GetCell(0,1).SetText("VP(Item Cost):").SetHAlign("right");
      ht.GetCell(0,2).SetText("Est. Resources/Income:").SetHAlign("right");
      ht.GetCell(0,3).SetText("Action Status:").SetHAlign("right");
      ht.GetCell(0,4).SetText("Bidding Status:").SetHAlign("right");
      ht.GetCell(0,5).SetText("Last Bids:").SetHAlign("right");
      ht.GetCell(0,6).SetText("Has Purchased Item:").SetHAlign("right");
    }


    if (newRows == 2)
    {
      for (i = 0 ; i < m_playerinfos.size() ; ++i)
      {
        PlayerInfo pi = m_playerinfos.get(i);
        ht.GetCell(i+1,0).SetText(pi.GetName()).SetHAlign("center");
        
        if (pi.IAm())
        {
          ht.GetCell(i+1,0).SetBackColor("#00FF00");
        }
        ht.GetCell(i+1,1).SetText(pi.OkStartup() ? "OK" : "").SetHAlign("center");
      }
    }
    else if (newRows == 3)
    {
      int[] ivec = new int[m_playerinfos.size()];
      for (i = 0 ; i < m_playerinfos.size() ; ++i)
      {
        ivec[i] = i;
      }

      int iteridx;
      for (iteridx = 0 ; iteridx < m_playerinfos.size() ; ++iteridx)
      {
        int highidx = -1;
        int highvalue = -1;

        for (i = 0 ; i < m_playerinfos.size() ; ++i)
        {
          if (ivec[i] == -1) continue;
          PlayerInfo pi = m_playerinfos.get(ivec[i]);
          if (pi.GetVP() > highvalue)
          {
            highvalue = pi.GetVP();
            highidx = i;
          }
        }

        PlayerInfo pi = m_playerinfos.get(ivec[highidx]);
        ivec[highidx] = -1;

        ht.GetCell(iteridx+1,0).SetText(pi.GetName()).SetHAlign("center");
        
        if (pi.IAm())
        {
          ht.GetCell(iteridx+1,0).SetBackColor("#00FF00");
        }
        ht.GetCell(iteridx+1,1).SetText("" + pi.GetVP()).SetHAlign("center");
        ht.GetCell(iteridx+1,2).SetText(pi.Wins() ? "YES!" : "").SetHAlign("center");
      }
    }      
    else
    {
      int legendcol = 0;
      int namerow = 0;
      int vprow = 1;
      int estrow = 2;
      int actrow = 3;
      int bidrow = 4;
      int bhistrow = 5;
      int hasprow = 6;

      if (m_bidstate == true)
      {
        ht.GetCell(legendcol,bidrow).SetBackColor("#00FF00");
      }

      for (i = 0 ; i < m_playerinfos.size() ; ++i)
      {
        PlayerInfo pi = m_playerinfos.get(m_turnorders[i]);
        int playercol = i + 1;
        ht.GetCell(playercol,namerow).SetText(pi.GetName()).SetHAlign("center");
        
        if (pi.IAm())
        {
          ht.GetCell(playercol,namerow).SetBackColor("#00FF00");
        }


        ht.GetCell(playercol,vprow).SetText(pi.GetVP() + "(" + pi.GetCostSum() + ")").SetHAlign("center");
        SetPurchaseLabel(ht.GetCell(playercol,actrow),pi.GetPurchaseState());
        SetBidLabel(ht.GetCell(playercol,bidrow),pi.GetBidState());

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
        ht.GetCell(playercol,estrow).SetText("" + pi.GetResources() + "/" + pi.GetIncome() ).SetHAlign("center");
      }
    }
    m_label.setText(ht.GetHTML());
  }

  public void HandleEvent(GameEvent i_event)
  {
    String ename = i_event.GetVarValue("eventname");

    if (ename.equals("PLAYER"))
    {
      int pnum = Integer.parseInt(i_event.GetVarValue("PlayerNumber"));
      int i;
      for (i = m_playerinfos.size() ; i <= pnum ; ++i)
      {
        m_playerinfos.add(null);
      }
      if (m_playerinfos.get(pnum) == null)
      {
        m_playerinfos.set(pnum,new PlayerInfo(i_event.GetVarValue("PlayerName")));
      }
      Redraw();
    }

    if (ename.equals("IAM"))
    {
      int pnum = Integer.parseInt(i_event.GetVarValue("PlayerNumber"));
      if (m_playerinfos.get(pnum) != null)
      {
        m_playerinfos.get(pnum).SetIAm(true);
        Redraw();
      }
    }

    if (ename.equals("PLAYERCARDBACKS"))
    {
      int pnum = Integer.parseInt(i_event.GetVarValue("PlayerNumber"));
      Vector<CommodityInfo> vci = CommodityInfo.ParseCommodityString(i_event.GetVarValue("BackString"));
      int i;
      int isum = 0;
      for (i = 0 ; i < vci.size() ; ++i)
      {
        isum += vci.get(i).GetValue();
      }
      m_playerinfos.get(pnum).SetResources(isum);
      Redraw();
    }

    if (ename.equals("PLAYERFACTORIES"))
    {
      int pnum = Integer.parseInt(i_event.GetVarValue("PlayerNumber"));
      Vector<FactoryWidget> vfw = FactoryWidget.ParseFactoryString(i_event.GetVarValue("FactoryString"),"",false,null);
      int i;
      int isum = 0;
      for (i = 0 ; i < vfw.size() ; ++i)
      {
        FactoryWidget fw = vfw.get(i);
        if (fw.GetManType() == 'U') continue;
        isum += fw.GetFactoryMeanValue();
      }
      m_playerinfos.get(pnum).SetFactoryIncome(isum);
      Redraw();
    }

    if (ename.equals("PLAYERITEMS"))
    {
      int pnum = Integer.parseInt(i_event.GetVarValue("PlayerNumber"));
      m_playerinfos.get(pnum).SetItem(i_event.GetVarValue("PlayerSlotNumber"),
                                      i_event.GetVarValue("ItemNumber"));
      Redraw();
    }

    if (ename.equals("PLAYERSTATE"))
    {
      int pnum = Integer.parseInt(i_event.GetVarValue("PlayerNumber"));
      if (m_playerinfos.get(pnum) != null)
      {
        m_playerinfos.get(pnum).SetPlayerState(Integer.parseInt(i_event.GetVarValue("VictoryPoints")),
                                               Integer.parseInt(i_event.GetVarValue("ItemCostSum")),
                                               Integer.parseInt(i_event.GetVarValue("PurchaseState")),
                                               Integer.parseInt(i_event.GetVarValue("BidState")),
                                               i_event.GetVarValue("HasPurchased").equals("1") ? true : false,
                                               Integer.parseInt(i_event.GetVarValue("HighBid")),
                                               Integer.parseInt(i_event.GetVarValue("LastBid"))
                                               );
        Redraw();
      }
    }

    if (ename.equals("PLAYEROKSTARTUP"))
    {
      int pnum = Integer.parseInt(i_event.GetVarValue("PlayerNumber"));
      if (m_playerinfos.get(pnum) != null)
      {
        m_playerinfos.get(pnum).SetOkStartup(i_event.GetVarValue("OkToStart").equals("1"));
        Redraw();
      }
    }

    if (ename.equals("TURNORDER"))
    {
      int pnum = Integer.parseInt(i_event.GetVarValue("PlayerNumber"));
      if (m_turnorders == null)
      {
        m_turnorders = new int[m_playerinfos.size()];
        int i;
        for (i = 0 ; i < m_turnorders.length ; ++i)
        {
          m_turnorders[i] = 0;
        }
      }
      m_turnorders[Integer.parseInt(i_event.GetVarValue("TurnOrder"))-1] = pnum;
      Redraw();
    }

    if (ename.equals("PLAYERWINS"))
    {
      m_winstate = true;
      int pnum = Integer.parseInt(i_event.GetVarValue("PlayerNumber"));
      if (m_playerinfos.get(pnum) != null)
      {
        m_playerinfos.get(pnum).SetWin();
      }
      Redraw();
    }

    if (ename.equals("NEWSTATE"))
    {
      m_bidstate = i_event.GetVarValue("statename").equals("Bid");
      Redraw();
    }
  }

  public void Refresh()
  {
  }

  public void Reset()
  {
    m_winstate = false;
    m_turnorders = null;
    m_playerinfos.removeAllElements();
    Redraw();
  }
}
