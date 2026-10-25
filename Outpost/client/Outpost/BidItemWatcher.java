package Outpost;
import java.util.HashMap;
import NodeInterfacePackage.*;
import javax.swing.JPanel;
import javax.swing.JLabel;
import java.util.List;

public class BidItemWatcher implements UserDefinedInterface,ActionVariableConsumer
{
  private JLabel m_image;
  private static final String s_name="ShipSlot";
  private int[] m_shipitems = null;
  private boolean m_consumevariable;

  public BidItemWatcher(HashMap<String,String> i_Options,String i_DataLoc,JPanel i_Panel,ExternalAssets i_assets)
  {
    m_consumevariable = i_Options.containsKey("ConsumeVariable") &&
      i_Options.get("ConsumeVariable").equals("true");

    ItemByNumber.DoOnce(i_DataLoc);

    if (m_consumevariable)
    {
      i_assets.GetActionTransferManager().AddVariableConsumer(s_name,this);
    }

    i_Panel.add(new JLabel("on"));
    m_image = ItemByNumber.createJLabel(0);
    i_Panel.add(m_image);
  }

// supplyShip (array of int itemnumbers)
// bidInfo is only non-null if we are actively bidding.
// bidInfo.itemShipIndex (index into supplyShip)

  private static final GuiPacketQuery supplyShipGpq = new GuiPacketQuery("supplyShip[]");
  private static final GuiPacketQuery bidItemIndexGpq = new GuiPacketQuery("bidInfo.itemShipIndex");

  @SuppressWarnings("unchecked")
  public void HandleEvent(GameEvent i_event)
  {
    List<Object> shipItems = (List<Object>)supplyShipGpq.evaluate(i_event);
    m_shipitems = new int[shipItems.size()];
    for (int i = 0 ; i < shipItems.size() ; ++i) {
      m_shipitems[i] = (Integer)shipItems.get(i);
    }

    // if consumevariable is true, then we never update the JLabel in HandleEvent.
    if (m_consumevariable) return;

    int bidItemIndex = bidItemIndexGpq.evaluateInt(i_event, -1);
    if (bidItemIndex < 0) {
      ItemByNumber.alterJLabel(m_image,0);
    } else {
      ItemByNumber.alterJLabel(m_image, m_shipitems[bidItemIndex]);
    }
  }

  public void Refresh()
  {
  }

  public void Reset()
  {
    m_shipitems = null;
    ItemByNumber.alterJLabel(m_image,0);
  }

  public void ReportNewVariableValue(String i_NewValue)
  {
    if (i_NewValue == null || m_shipitems == null) {
      ItemByNumber.alterJLabel(m_image,0);
      return;
    }

    int newindex = Integer.parseInt(i_NewValue);
    if (newindex < 0 || newindex >= m_shipitems.length) {
      ItemByNumber.alterJLabel(m_image,0);
      return;
    }

    ItemByNumber.alterJLabel(m_image, m_shipitems[newindex]);
  }
}
