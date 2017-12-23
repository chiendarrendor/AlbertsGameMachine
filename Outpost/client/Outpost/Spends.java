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

  String parseBuyItem(String item)
  {
    if (item.equals("0")) return "robot";
    if (item.equals("1")) return "man";
    if (item.equals("2")) return "ore factory";
    if (item.equals("3")) return "water factory";
    if (item.equals("4")) return "titanium factory";
    if (item.equals("5")) return "research factory";
    if (item.equals("6")) return "new chemicals factory";
    return "???";
  }

  String parseBidItem(String item)
  {
    if (item.equals("1")) return "data library";
    if (item.equals("2")) return "warehouse";
    if (item.equals("3")) return "heavy equipment";
    if (item.equals("4")) return "nodule";
    if (item.equals("5")) return "scientists";
    if (item.equals("6")) return "orbital lab";
    if (item.equals("7")) return "robots";
    if (item.equals("8")) return "laboratory";
    if (item.equals("9")) return "ecoplants";
    if (item.equals("10")) return "outpost";
    if (item.equals("11")) return "space station";
    if (item.equals("12")) return "planetary cruiser";
    if (item.equals("13")) return "moon base";
    return "???";
  }

  public void HandleEvent(GameEvent i_event)
  {
    if (i_event.GetVarValue("eventname").equals("SPENDS"))
    {
      String turn = i_event.GetVarValue("TurnNumber");
      String player = i_event.GetVarValue("PlayerNumber");
      String type = i_event.GetVarValue("SpendType");
      String item = i_event.GetVarValue("SpendItem");
      String cost = i_event.GetVarValue("SpendCost");
      String res = i_event.GetVarValue("Resources");

      String line1;

      // Turn x Player y bought z for a
      // Turn x Player y won bid on z and paid a
      // Turn x Player y discarded at least a cards


      if (type.equals("0")) // item purchase
      {
        line1="bought " + parseBuyItem(item) + " for " + cost;
      }
      else if (type.equals("1")) // bid purchase
      {
        line1="won bid on " + parseBidItem(item) + " for a final price of " + cost;
      }
      else if (type.equals("2")) // discard
      {
        line1="discarded at least " + cost + " cards";
      }
      else if (type.equals("3")) // water mulligan
      {
        line1="spent all resources for a water factory";
      }
      else
      {
        line1 = "???";
      }

      m_jta.append("Turn " + turn + " Player " + player + " " + line1 + "\n");
      m_jta.append(res + "\n");
      m_jsb.setValue(m_jsb.getMaximum());
    }
  }

  public void Refresh()
  {
  }

  public void Reset()
  {
    m_jta.setText(null);
  }
}

