package Outpost;

import java.util.HashMap;
import NodeInterfacePackage.*;
import javax.swing.JPanel;
import javax.swing.Box;
import javax.swing.JLabel;
import java.io.File;
import javax.swing.BoxLayout;

public class ShipList implements UserDefinedInterface,ActionVariableGenerator,ShipActionGo
{
  private static final String s_name="ShipSlot";
  private JPanel m_panel;
  private int m_largestslot;
  private ActionTransferManager m_atm;
  private String m_Selection;
  private ServerStateHistory m_ssh;
  HashMap<String,String> m_playernames;
  int m_robotnumber;

  public ShipList(HashMap<String,String> i_Options,String i_DataLoc,JPanel i_Panel,ActionTransferManager i_atm)
  {
    ItemByNumber.DoOnce(i_DataLoc);

    m_ssh = new ServerStateHistory();
    m_playernames = new HashMap<String,String>();
    m_robotnumber = 7;

    m_Selection = null;

    m_largestslot = -1;
    m_atm = i_atm;
    m_atm.AddVariableGenerator(s_name,this);

    m_panel = new JPanel();
    m_panel.setOpaque(false);
    m_panel.setLayout(new BoxLayout(m_panel,BoxLayout.X_AXIS));
    m_panel.setVisible(true);
    i_Panel.add(m_panel);



  }

  private void ExtendList(int i_newlast)
  {
    if (i_newlast > m_largestslot)
    {
      int i;
      for (i = m_largestslot+1 ; i <= i_newlast ; ++i)
      {
        m_panel.add(new ShipItem(this,Integer.toString(i),m_playernames));
        m_panel.add(Box.createHorizontalStrut(5));
      }

      m_panel.revalidate();
      m_panel.repaint();
      m_largestslot = i_newlast;
    }
  }    

  public ShipItem GetShipItem(int i_slot)
  {
    return (ShipItem)(m_panel.getComponents()[i_slot*2]);
  }

  public void HandleEvent(GameEvent i_event)
  {
    m_ssh.HandleEvent(i_event);

    if (i_event.GetVarValue("eventname").equals("PLAYER"))
    {
      m_playernames.put(i_event.GetVarValue("PlayerNumber"),i_event.GetVarValue("PlayerName"));
    }

    if (i_event.GetVarValue("eventname").equals("SUPPLYSHIP"))
    {
      int slot = Integer.parseInt(i_event.GetVarValue("ShipSlotNumber"));
      int item = Integer.parseInt(i_event.GetVarValue("ItemNumber"));

      ExtendList(slot);
      GetShipItem(slot).alterItem(item,m_robotnumber);
    }

    if (i_event.GetVarValue("eventname").equals("OPTIONS"))
    {
      m_robotnumber = 70 + Integer.parseInt(i_event.GetVarValue("Robots"));
    }


    if (i_event.GetVarValue("eventname").equals("NEWSTATE"))
    {
      if (!m_ssh.GetLastState().equals(m_ssh.GetCurState()))
      {
        ShipActionGo(null);
      }
    }

    int i;
    for (i = 0 ; i <= m_largestslot ; ++i)
    {
      GetShipItem(i).HandleEvent(i_event);
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
    m_largestslot = -1;
    m_robotnumber = 7;
    m_panel.removeAll();
  }

  public void ActionVariableReset()
  {
    int i;
    for (i = 0 ; i <= m_largestslot ; ++i)
    {
      GetShipItem(i).ClearRed();
    }
  }

  public String ActionVariableGetValue()
  {
    return m_Selection;
  }

}
