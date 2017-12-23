package Outpost;

import java.util.HashMap;
import NodeInterfacePackage.*;
import javax.swing.JPanel;
import javax.swing.JLabel;
import java.io.File;
import javax.swing.BoxLayout;

import Outpost.Item;

public class ItemList implements UserDefinedInterface
{
  private JPanel m_panel;
  private String m_PlayerNum;
  private int m_largestslot;
  private int m_robotnumber;

  public ItemList(HashMap<String,String> i_Options,String i_DataLoc,JPanel i_Panel,ActionTransferManager i_atm)
  {
    ItemByNumber.DoOnce(i_DataLoc);

    if (!i_Options.containsKey("playernum")) throw new NodeInterfacePackageException("missing option commtype");
    m_PlayerNum = i_Options.get("playernum");

    m_largestslot = -1;
    m_robotnumber = 7;

    m_panel = new JPanel();
    m_panel.setOpaque(false);
    m_panel.setLayout(new BoxLayout(m_panel,BoxLayout.X_AXIS));
    m_panel.setVisible(true);
    i_Panel.add(m_panel);

  }

  private int GetItemNumber(int i_item)
  {
    if (i_item == 7) return m_robotnumber;
    return i_item;
  }

  public void HandleEvent(GameEvent i_event)
  {
    if (i_event.GetVarValue("eventname").equals("PLAYERITEMS") &&
        i_event.GetVarValue("PlayerNumber").equals(m_PlayerNum))
    {
      int slot = Integer.parseInt(i_event.GetVarValue("PlayerSlotNumber"));
      int item = Integer.parseInt(i_event.GetVarValue("ItemNumber"));

      if (slot > m_largestslot)
      {
        int i;
        for (i = m_largestslot+1 ; i <= slot ; ++i)
        {
          m_panel.add(ItemByNumber.createJLabel(0));
        }
        m_panel.revalidate();
        m_panel.repaint();
        m_largestslot = slot;
      }
      ItemByNumber.alterJLabel((JLabel)(m_panel.getComponents()[slot]),GetItemNumber(item));
    }

    if (i_event.GetVarValue("eventname").equals("OPTIONS"))
    {
      m_robotnumber = 70 + Integer.parseInt(i_event.GetVarValue("Robots"));
    }
  }

  public void Refresh()
  {
  }

  public void Reset()
  {
    m_largestslot = -1;
    m_robotnumber = 7;
    m_panel.removeAll();
  }

  public void odispose()
  {
  }


}
