package Outpost;

import java.util.HashMap;
import NodeInterfacePackage.*;
import javax.swing.JPanel;
import javax.swing.JLabel;
import java.io.File;
import javax.swing.BoxLayout;
import org.json.JSONObject;
import org.doghouse.jsonpath.JsonPath;
import java.util.List;

public class ItemList implements UserDefinedInterface
{
  private JPanel m_panel;
  private String m_PlayerName;
  private int m_robotnumber;

  public ItemList(HashMap<String,String> i_Options,String i_DataLoc,JPanel i_Panel,ActionTransferManager i_atm)
  {
    ItemByNumber.DoOnce(i_DataLoc);

    if (!i_Options.containsKey("playername")) throw new NodeInterfacePackageException("missing option playername");
    m_PlayerName = i_Options.get("playername");

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

  private static JsonPath itemListPath = JsonPath.compile("items[]");
  @SuppressWarnings("unchecked")
  public void HandleEvent(GameEvent i_event)
  {
    m_robotnumber = EventHelper.getRobotItemId(i_event);
    JSONObject player = EventHelper.getJSONOfPlayer(m_PlayerName, i_event);
    List<Integer> items = (List<Integer>)itemListPath.evaluate(player);

    m_panel.removeAll();
    for (int item : items) {
      m_panel.add(ItemByNumber.createJLabel(GetItemNumber(item)));
    }
    m_panel.revalidate();
    m_panel.repaint();
  }

  public void Refresh()
  {
  }

  public void Reset()
  {
    m_robotnumber = 7;
    m_panel.removeAll();
  }

  public void odispose()
  {
  }


}
