package ROOMGUI;

import NodeInterfacePackage.*;
import javax.swing.JPanel;
import java.awt.FlowLayout;
import java.awt.BorderLayout;
import java.util.HashMap;
import java.util.Iterator;
import javax.swing.JTextField;
import java.util.Set;
import java.util.HashSet;
import java.util.List;
import org.doghouse.jsonpath.JsonPath;
import org.json.JSONObject;
import org.json.JSONArray;

public class RoomPage implements UserDefinedInterface
{
  JPanel m_mainpanel;
  ActionTransferManager m_atm;
  HashMap<String,RoomWidget> m_Rooms;
  String m_iam;
  GuiPacketQuery roomsgpq = new GuiPacketQuery("rooms");
  GuiPacketQuery namegpq = new GuiPacketQuery("{ME}");
  JsonPath namepath = JsonPath.compile("name");

  public RoomPage(HashMap<String,String> i_Options,String i_DataLoc,JPanel i_Panel,ActionTransferManager i_atm)
  {
    m_atm = i_atm;
    m_iam = null;

    m_mainpanel = new JPanel(new FlowLayout(FlowLayout.LEFT,5,5));
    i_Panel.add(m_mainpanel);

    m_Rooms = new HashMap<String,RoomWidget>();

  }

  public void HandleEvent(GameEvent i_event)
  {
    JSONArray rooms = (JSONArray) roomsgpq.evaluate(i_event);
    m_iam = (String)namegpq.evaluate(i_event);

    Set<String> unhandledRoomNames = new HashSet<>(m_Rooms.keySet());

    // this loop will handle transport to all rooms the server believes exists.
    for (int i = 0 ; i < rooms.length() ; ++i) {
      JSONObject room = rooms.getJSONObject(i);
      String roomname = (String)namepath.evaluate(room);
      unhandledRoomNames.remove(roomname);

      if (!m_Rooms.containsKey(roomname)) {
        RoomWidget rw = new RoomWidget(roomname, m_atm, m_iam); 
        m_mainpanel.add(rw);
        m_Rooms.put(roomname,rw);
        m_mainpanel.revalidate();
        m_mainpanel.repaint();
      }
      m_Rooms.get(roomname).handleRoomUpdate(room);
    }

    // any rooms left in unhandledroomnames was never mentioned by the server, must be removed.
    boolean removed = false;
    for (String doomedname : unhandledRoomNames) {
      removed = true;
      RoomWidget rw = m_Rooms.get(doomedname);
      m_mainpanel.remove(rw);
      m_Rooms.remove(doomedname);
      rw.detach();
    }

    if (removed) {
      m_mainpanel.revalidate();
      m_mainpanel.repaint();
    }
  }

  public void Refresh()
  {
  }

  public void Reset()
  {
    m_mainpanel.removeAll();
    m_mainpanel.revalidate();
    m_mainpanel.repaint();
    m_iam = null;

    for (RoomWidget rw : m_Rooms.values()) {
      rw.detach();
    }
    
    m_Rooms.clear();
  }
}
