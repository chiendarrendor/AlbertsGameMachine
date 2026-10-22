package Outpost;

import java.util.HashMap;
import java.util.List;
import javax.swing.JPanel;
import javax.swing.JLabel;
import java.awt.GridLayout;
import java.awt.BorderLayout;
import org.json.JSONObject;
import NodeInterfacePackage.*;
import javax.swing.SwingConstants;

public class ItemStockGrid implements UserDefinedInterface
{
  private static final GuiPacketQuery s_stockQuery = new GuiPacketQuery("itemStock[]");
  private static final GuiPacketQuery s_robotsQuery = new GuiPacketQuery("options.robots");

  private JLabel[] m_icons = new JLabel[14];
  private JLabel[] m_counts = new JLabel[14];

  public ItemStockGrid(HashMap<String,String> i_Options,String i_DataLoc,JPanel i_Panel,ActionTransferManager i_atm)
  {
    ItemByNumber.DoOnce(i_DataLoc);

    i_Panel.setLayout(new GridLayout(2,7));

    int i;
    for (i = 1 ; i <= 13 ; ++i)
    {
      JPanel itemPanel = new JPanel(new BorderLayout());

      JLabel icon = ItemByNumber.createJLabel(i);
      itemPanel.add(icon,BorderLayout.CENTER);

      JLabel count = new JLabel("0", SwingConstants.CENTER);
      itemPanel.add(count,BorderLayout.SOUTH);

      m_icons[i] = icon;
      m_counts[i] = count;

      i_Panel.add(itemPanel);
    }
  }

  @SuppressWarnings("unchecked")
  public void HandleEvent(GameEvent i_event)
  {
    for (Object o : (List<Object>)s_stockQuery.evaluate(i_event))
    {
      JSONObject item = (JSONObject)o;
      int itemId = item.getInt("itemId");
      m_counts[itemId].setText(String.valueOf(item.getInt("count")));
    }

    int robots = (Integer)s_robotsQuery.evaluate(i_event);
    ItemByNumber.alterJLabel(m_icons[7],70 + robots);
  }

  public void Refresh()
  {
  }

  public void Reset()
  {
    for (int i = 1 ; i <= 13 ; ++i)
    {
      m_counts[i].setText("0");
    }
    ItemByNumber.alterJLabel(m_icons[7],7);
  }
}
