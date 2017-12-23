package Outpost;

import java.util.HashMap;
import NodeInterfacePackage.*;
import javax.swing.JPanel;
import java.io.File;
import javax.swing.JLabel;

import Outpost.Item;

public class ItemCard implements UserDefinedInterface
{
  int m_cardnum;
  JLabel m_jlabel;
  
  public ItemCard(HashMap i_Options,String i_DataLoc,JPanel i_Panel,ActionTransferManager i_atm)
  {
    if (!i_Options.containsKey("cardnum"))
    {
      throw new NodeInterfacePackageException("missing option cardnum");
    }

    m_cardnum = Integer.parseInt((String)i_Options.get("cardnum"));

    if (m_cardnum < -1 || m_cardnum > 13)
    {
      throw new NodeInterfacePackageException("illegal value for cardnum");
    }

    ItemByNumber.DoOnce(i_DataLoc);
    m_jlabel = ItemByNumber.createJLabel(m_cardnum);
    i_Panel.add(m_jlabel);
  }

  public void HandleEvent(GameEvent i_event)
  {
    if (m_cardnum != 7) return;

    if (i_event.GetVarValue("eventname").equals("OPTIONS"))
    {
      int robotsuffix = Integer.parseInt(i_event.GetVarValue("Robots"));
      ItemByNumber.alterJLabel(m_jlabel,70 + robotsuffix);
    }
  }

  public void Refresh()
  {
  }

  public void Reset()
  {
  }
}
