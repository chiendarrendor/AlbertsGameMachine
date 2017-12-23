package Outpost;

import java.util.HashMap;
import NodeInterfacePackage.*;
import javax.swing.JPanel;
import javax.swing.JLabel;
import java.awt.BorderLayout;
import javax.swing.BoxLayout;
import java.awt.FlowLayout;


import Outpost.Item;

public class PlayerWindow implements UserDefinedInterface
{  
  JLabel m_turnnumber;
  HashMap<String,JLabel> m_itemcounts;

  public PlayerWindow(HashMap<String,String> i_Options,String i_DataLoc,
                      JPanel i_Panel,ActionTransferManager i_atm)
  {
    i_Panel.setLayout(new BorderLayout());
    JPanel statuspanel = new JPanel();
    statuspanel.setLayout(new BoxLayout(statuspanel,BoxLayout.Y_AXIS));
    i_Panel.add(statuspanel,BorderLayout.NORTH);

    m_turnnumber = new JLabel("Turn Number: 0");
    m_turnnumber.setAlignmentX(JLabel.LEFT_ALIGNMENT);
    statuspanel.add(m_turnnumber);

    JPanel itempanel = new JPanel(new FlowLayout());
    m_itemcounts = new HashMap<String,JLabel>();
    ItemByNumber.DoOnce(i_DataLoc);

    int i;
    for (i = 1; i <= 13 ; ++i)
    {
      Integer io = new Integer(i);
      JPanel ipanel = new JPanel();
      ipanel.setLayout(new BoxLayout(ipanel,BoxLayout.Y_AXIS));
      JLabel iicon = ItemByNumber.createJLabel(i);
      iicon.setAlignmentX(JLabel.CENTER_ALIGNMENT);
      ipanel.add(iicon);
      JLabel ilabel = new JLabel("0");
      ilabel.setAlignmentX(JLabel.CENTER_ALIGNMENT);
      m_itemcounts.put(io.toString(),ilabel);
      ipanel.add(ilabel);
      itempanel.add(ipanel);
    }

    itempanel.setAlignmentX(JLabel.LEFT_ALIGNMENT);
    statuspanel.add(itempanel);


  }

  public void HandleEvent(GameEvent i_event)
  {
    if (i_event.GetVarValue("eventname").equals("CURTURN"))
    {
      m_turnnumber.setText("Turn Number: " + i_event.GetVarValue("TurnNumber"));
    }
    if (i_event.GetVarValue("eventname").equals("ITEMSTOCK"))
    {
      m_itemcounts.get(i_event.GetVarValue("ItemNumber")).
        setText(i_event.GetVarValue("ItemCount"));
    }
  }

  public void Refresh()
  {
  }

  public void Reset()
  {
    m_turnnumber.setText("Turn Number: 0");
    int i;
    for (i = 1; i <= 13 ; ++i)
    {
      Integer io = new Integer(i);
      m_itemcounts.get(io.toString()).setText("0");
    }
  }
}
