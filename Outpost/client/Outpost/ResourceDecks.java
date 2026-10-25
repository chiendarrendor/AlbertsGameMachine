package Outpost;

import java.util.HashMap;
import NodeInterfacePackage.*;
import javax.swing.JLabel;
import javax.swing.JPanel;
import MySwing.HTMLTable;
import java.util.List;
import org.doghouse.jsonpath.JsonPath;
import org.json.JSONObject;

public class ResourceDecks implements UserDefinedInterface
{
  private JLabel m_label;
  private HashMap<Integer, Integer>m_draws;
  private HashMap<Integer, Integer>m_discards;

  public ResourceDecks(HashMap<String,String> i_Options,String i_DataLoc,JPanel i_Panel,ExternalAssets i_assets)
  {
    m_draws = new HashMap<Integer,Integer>();
    m_discards = new HashMap<Integer,Integer>();
    m_label = new JLabel();
    i_Panel.add(m_label);
    SetLabelText();
  }

  private static GuiPacketQuery stockgpq = new GuiPacketQuery("commodityStock[]");
  private static JsonPath commIdPath = JsonPath.compile("commodityId");
  private static JsonPath drawCountPath = JsonPath.compile("drawCount");
  private static JsonPath discardCountPath = JsonPath.compile("discardCount");

  @SuppressWarnings("unchecked")
  public void HandleEvent(GameEvent i_event)
  {
    List<JSONObject> comms = (List<JSONObject>)stockgpq.evaluate(i_event);

    for (JSONObject comm : comms) {
      int commid = (Integer)commIdPath.evaluate(comm);
      int drawcount = (Integer)drawCountPath.evaluate(comm);
      int discardcount = (Integer)discardCountPath.evaluate(comm);

      m_draws.put(commid,drawcount);
      m_discards.put(commid,discardcount);
    }

    SetLabelText();
  }

  //since the resources are a 1-based indexing, we can use  i_row for both HTMLTable row index as well as resource id.
  private void AppendResource(HTMLTable i_ht,int i_row,String i_bg,
                              String i_fg,String i_rname)
  {
    HTMLTable.CellContents c1 = i_ht.GetCell(0,i_row);
    HTMLTable.CellContents c2 = i_ht.GetCell(1,i_row);
    HTMLTable.CellContents c3 = i_ht.GetCell(2,i_row);

    c1.SetText(i_rname).SetForeColor(i_fg).SetBackColor(i_bg);
    if (m_draws.containsKey(i_row))
    {
      c2.SetText(""+m_draws.get(i_row));
    }
    if (m_discards.containsKey(i_row))
    {
      c3.SetText(""+m_discards.get(i_row));
    }
  }


  private void SetLabelText()
  {
    HTMLTable ht = new HTMLTable(3,10,true);
    ht.GetCell(0,0).SetText("Resource");
    ht.GetCell(1,0).SetText("Cards in<p>Draw Pile");
    ht.GetCell(2,0).SetText("Cards in<p>Discard Pile");

    AppendResource(ht,1,null,"#804000","Ore");
    AppendResource(ht,2,null,"#0000FF","Water");
    AppendResource(ht,3,null,"#808080","Titanium");
    AppendResource(ht,4,"#FFFF00","#000000","Research");
    AppendResource(ht,5,null,"#FF00FF","Microbiotics");
    AppendResource(ht,6,null,"#008000","New Chemicals");
    AppendResource(ht,7,null,"#00FF00","Orbital<p>Medicine");
    AppendResource(ht,8,null,"#8080FF","Ring Ore");
    AppendResource(ht,9,null,"#FF8040","Moon Ore");

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
