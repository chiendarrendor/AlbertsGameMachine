package Outpost;
import java.util.HashMap;
import NodeInterfacePackage.*;
import javax.swing.JPanel;
import javax.swing.JLabel;

public class BidItemWatcher implements UserDefinedInterface,ActionVariableConsumer
{
  private JLabel m_image;
  private static final String s_name="ShipSlot";
  private HashMap<String,String> m_shipitems;
  private boolean m_consumevariable;

  public BidItemWatcher(HashMap<String,String> i_Options,String i_DataLoc,JPanel i_Panel,ActionTransferManager i_atm)
  {
    m_consumevariable = i_Options.containsKey("ConsumeVariable") &&
      i_Options.get("ConsumeVariable").equals("true");

    m_shipitems = new HashMap<String,String>();
    ItemByNumber.DoOnce(i_DataLoc);
    
    if (m_consumevariable)
    {
      i_atm.AddVariableConsumer(s_name,this);
    }

    i_Panel.add(new JLabel("on"));
    m_image = ItemByNumber.createJLabel(0);
    i_Panel.add(m_image);
  }

  public void HandleEvent(GameEvent i_event)
  {
    if (i_event.GetVarValue("eventname").equals("SUPPLYSHIP"))
    {
      m_shipitems.put(i_event.GetVarValue("ShipSlotNumber"),i_event.GetVarValue("ItemNumber"));
    }

    if (!m_consumevariable && i_event.GetVarValue("eventname").equals("BIDITEM"))
    {
      String ssn = i_event.GetVarValue("ShipSlotNumber");

      if (m_shipitems.containsKey(ssn))
      {
        ItemByNumber.alterJLabel(m_image,Integer.parseInt(m_shipitems.get(ssn)));
      }
      else
      {
        ItemByNumber.alterJLabel(m_image,0);
      }
    }
  }

  public void Refresh()
  {
  }

  public void Reset()
  {
    m_shipitems.clear();
    ItemByNumber.alterJLabel(m_image,0);
  }

  public void ReportNewVariableValue(String i_NewValue)
  {
    if (i_NewValue == null || !m_shipitems.containsKey(i_NewValue))
    {
      ItemByNumber.alterJLabel(m_image,0);
    }
    else
    {
      ItemByNumber.alterJLabel(m_image,Integer.parseInt(m_shipitems.get(i_NewValue)));
    }
  }
}
