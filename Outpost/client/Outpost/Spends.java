package Outpost;

import java.util.HashMap;
import NodeInterfacePackage.*;
import javax.swing.JPanel;
import javax.swing.JLabel;
import javax.swing.JScrollPane;
import javax.swing.JScrollBar;
import javax.swing.JTextArea;
import java.awt.FlowLayout;
import java.awt.Dimension;
import java.awt.Insets;
import java.awt.Color;
import java.awt.BorderLayout;
import java.util.List;
import org.doghouse.jsonpath.JsonPath;
import org.json.JSONObject;

public class Spends implements UserDefinedInterface
{
  JTextArea m_jta;
  JScrollBar m_jsb;
  JPanel m_parPanel;

  public Spends(HashMap<String,String> i_Options,String i_DataLoc,JPanel i_Panel,ActionTransferManager i_atm)
  {
    m_parPanel = i_Panel;
    m_parPanel.setLayout(new BorderLayout());

    m_jta = new JTextArea();
    m_jta.setLineWrap(true);
    m_jta.setWrapStyleWord(true);
    m_jta.setEditable(false);
    JScrollPane jsp = new JScrollPane(m_jta);
    m_jsb = jsp.getVerticalScrollBar();
    i_Panel.add(jsp,BorderLayout.CENTER);
  }

  String[] buyItems = { "robot", "man",  "ore factory", "water factory", "titanium factory", "research factory", "new chemicals factory" };

  String parseBuyItem(int item)
  {
    if (item < 0 || item >= buyItems.length) return "???";
    return buyItems[item];
  }

  String[] bidItems = { 
    "???", "data library", "warehouse", "heavy equipment", "nodule", "scientists", "orbital lab", "robots", "laboratory",
    "ecoplants", "outpost", "space station", "planetary cruiser", "moon base"};

  String parseBidItem(int item)
  {
    if (item < 0 || item >= bidItems.length) return "???";
    return bidItems[item];
  }


  private static GuiPacketQuery spendsQuery = new GuiPacketQuery("spends[]");
  @SuppressWarnings("unchecked")
  public void HandleEvent(GameEvent i_event) {
    m_jta.setText(null);

    List<JSONObject> spends = (List<JSONObject>) spendsQuery.evaluate(i_event);
    for (JSONObject spend : spends) HandleOneSpend(spend);

    m_jsb.setValue(m_jsb.getMaximum());
  }

  private static JsonPath turnNumberPath = JsonPath.compile("turnNum");
  private static JsonPath playerNumberPath = JsonPath.compile("playerId");
  private static JsonPath spendTypePath = JsonPath.compile("spendType");
  private static JsonPath spendItemPath = JsonPath.compile("spendItem");
  private static JsonPath spendCostPath = JsonPath.compile("spendCost");
  private static JsonPath spendResourcePath = JsonPath.compile("resources");

  public void HandleOneSpend(JSONObject i_spend)
  {
   
    int turn = (Integer)turnNumberPath.evaluate(i_spend);
    int player = (Integer)playerNumberPath.evaluate(i_spend);
    int type = (Integer)spendTypePath.evaluate(i_spend);
    int item = (Integer)spendItemPath.evaluate(i_spend);
    int cost = (Integer)spendCostPath.evaluate(i_spend);
    String res = (String)spendResourcePath.evaluate(i_spend);

    String line1;

    // Turn x Player y bought z for a
    // Turn x Player y won bid on z and paid a
    // Turn x Player y discarded at least a cards

    switch(type) {
      case 0: // item purchase
        line1 = "bought " + parseBuyItem(item) + " for " + cost;
        break;
      case 1: // bit purchase
        line1 = "won bid on " + parseBidItem(item) + " for a final price of " + cost;
        break;
      case 2: // discard
        line1 = "discarded at least " + cost + " cards";
        break;
      case 3: // water mulligan
        line1 = "spent all resources for a water factory";
        break;
      default:
        line1 = "???";
    }

    m_jta.append("Turn " + turn + " Player " + player + " " + line1 + "\n");
    m_jta.append(res + "\n");

  }

  public void Refresh()
  {
  }

  public void Reset()
  {
    m_jta.setText(null);
  }
}

