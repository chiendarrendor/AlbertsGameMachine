package Outpost;

import java.util.HashMap;
import NodeInterfacePackage.*;
import javax.swing.JLabel;
import javax.swing.JPanel;
import MySwing.HTMLTable;

public class ResourceDecks implements UserDefinedInterface
{
  private JLabel m_label;
  private HashMap<String,String>m_draws;
  private HashMap<String,String>m_discards;

  public ResourceDecks(HashMap<String,String> i_Options,String i_DataLoc,JPanel i_Panel,ActionTransferManager i_atm)
  {
    m_draws = new HashMap<String,String>();
    m_discards = new HashMap<String,String>();
    m_label = new JLabel();
    i_Panel.add(m_label);
    SetLabelText();
  }

  public void HandleEvent(GameEvent i_event)
  {
    if (i_event.GetVarValue("eventname").equals("COMMODITYSTOCK"))
    {
      String commno=i_event.GetVarValue("CommodityNumber");
      String drawno=i_event.GetVarValue("DrawCount");
      String discno=i_event.GetVarValue("DiscardCount");

      m_draws.put(commno,drawno);
      m_discards.put(commno,discno);

      SetLabelText();
    }
  }

  private void AppendResource(HTMLTable i_ht,int i_row,String i_bg,
                              String i_fg,String i_rname,
                              String i_idx)
  {
    HTMLTable.CellContents c1 = i_ht.GetCell(0,i_row);
    HTMLTable.CellContents c2 = i_ht.GetCell(1,i_row);
    HTMLTable.CellContents c3 = i_ht.GetCell(2,i_row);

    c1.SetText(i_rname).SetForeColor(i_fg).SetBackColor(i_bg);
    if (m_draws.containsKey(i_idx))
    {
      c2.SetText(m_draws.get(i_idx));
    }
    if (m_discards.containsKey(i_idx))
    {
      c3.SetText(m_discards.get(i_idx));
    }
  }


  private void SetLabelText()
  {
    HTMLTable ht = new HTMLTable(3,10,true);
    ht.GetCell(0,0).SetText("Resource");
    ht.GetCell(1,0).SetText("Cards in<p>Draw Pile");
    ht.GetCell(2,0).SetText("Cards in<p>Discard Pile");

    AppendResource(ht,1,null,"#804000","Ore","1");
    AppendResource(ht,2,null,"#0000FF","Water","2");
    AppendResource(ht,3,null,"#808080","Titanium","3");
    AppendResource(ht,4,"#FFFF00","#000000","Research","4");
    AppendResource(ht,5,null,"#FF00FF","Microbiotics","5");
    AppendResource(ht,6,null,"#008000","New Chemicals","6");
    AppendResource(ht,7,null,"#00FF00","Orbital<p>Medicine","7");
    AppendResource(ht,8,null,"#8080FF","Ring Ore","8");
    AppendResource(ht,9,null,"#FF8040","Moon Ore","9");

    m_label.setText(ht.GetHTML());
  }

  public void Refresh()
  {
  }

  public void Reset()
  {
    m_draws.clear();
    m_discards.clear();
    SetLabelText();
  }
}
