package ROOMGUI;

import NodeInterfacePackage.*;
import javax.swing.JPanel;
import java.awt.FlowLayout;
import java.awt.BorderLayout;
import java.util.HashMap;
import java.util.Iterator;
import javax.swing.JTextField;

public class RoomPage implements UserDefinedInterface
{
  JPanel m_mainpanel;
  ActionTransferManager m_atm;
  HashMap<String,RoomWidget> m_Rooms;
  String m_iam;

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
    if (i_event.GetVarValue("eventname").equals("GUIIAM"))
    {
      m_iam = i_event.GetVarValue("myname");
    }

    if (i_event.GetVarValue("eventname").equals("GUIROOM"))
    {
      String rname = i_event.GetVarValue("roomname");
      if (!m_Rooms.containsKey(rname))
      {
        RoomWidget rw = new RoomWidget(rname,m_atm,m_iam);
        m_mainpanel.add(rw);
        m_Rooms.put(rname,rw);
        m_mainpanel.revalidate();
        m_mainpanel.repaint();
      }
      m_Rooms.get(rname).HandleEvent(i_event);
    }
    else
    {
      Iterator<RoomWidget> rwi = m_Rooms.values().iterator();
      while(rwi.hasNext())
      {
        rwi.next().HandleEvent(i_event);
      }
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

    Iterator<RoomWidget> rwi = m_Rooms.values().iterator();
    while(rwi.hasNext())
    {
      rwi.next().detach();
    }
    
    m_Rooms.clear();
  }
}
