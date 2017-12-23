package Outpost;

import javax.swing.JPanel;
import javax.swing.JLabel;
import javax.swing.JButton;
import NodeInterfacePackage.*;
import MySwing.LockLayout;
import java.awt.Point;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.util.HashMap;


public class ShipItem extends JPanel implements ActionListener
{
  JLabel m_unboughtjl;
  JLabel m_redback;
  JLabel m_greenback;
  JButton m_jb;
  String m_idx;
  int m_item;
  boolean m_rightstate;
  ShipActionGo m_sag;
  HashMap<String,String> m_playernames;

  int m_lastboughtitem;

  JLabel m_boughtjl;
  JLabel m_buyername;
  JLabel m_buyercost;




  public ShipItem(ShipActionGo i_sag,String i_idx,HashMap<String,String> i_playernames)
  {
    super();

    m_playernames = i_playernames;
    m_lastboughtitem = 0;

    m_sag = i_sag;
    m_idx = i_idx;
    m_item = 0;
    m_rightstate = false;

    setLayout(new LockLayout());

    m_unboughtjl = ItemByNumber.createJLabel(0);
    add(m_unboughtjl,new Point(0,0));

    m_boughtjl = ItemByNumber.createJLabel(0);
    add(m_boughtjl,new Point(0,0));
    m_boughtjl.setVisible(false);
    m_boughtjl.setEnabled(false);

    m_redback = ItemByNumber.createJLabel(-1);
    add(m_redback,new Point(0,0));
    m_redback.setVisible(false);

    m_greenback = ItemByNumber.createJLabel(-2);
    add(m_greenback,new Point(0,0));
    m_greenback.setVisible(false);

    m_jb = new JButton("Bid");
    add(m_jb,new Point(10,90));
    m_jb.setVisible(false);
    m_jb.addActionListener(this);

    m_buyername = new JLabel();
    add(m_buyername,new Point(0,90));

    m_buyercost = new JLabel("Cost: ");
    add(m_buyercost,new Point(0,105));
  }

  public void actionPerformed(ActionEvent i_event)
  {
    m_sag.ShipActionGo(m_idx);
    m_redback.setVisible(true);
  }

  public void alterItem(int i_newitem,int i_robotnumber)
  {
    if (i_newitem == 7) ItemByNumber.alterJLabel(m_unboughtjl,i_robotnumber);
    else ItemByNumber.alterJLabel(m_unboughtjl,i_newitem);

    m_item = i_newitem;
    m_jb.setVisible(m_item != 0 && m_rightstate);

    RecalculateVisible();
  }

  private void RecalculateVisible()
  {
    if (m_item == 0 && m_lastboughtitem != 0)
    {
      m_boughtjl.setVisible(true);
      m_unboughtjl.setVisible(false);
      m_buyername.setVisible(true);
      m_buyercost.setVisible(true);
    }
    else
    {
      m_boughtjl.setVisible(false);
      m_unboughtjl.setVisible(true);
      m_buyername.setVisible(false);
      m_buyercost.setVisible(false);
    }
  }

  public void ClearRed()
  {
    m_redback.setVisible(false);
  }

  public void HandleEvent(GameEvent i_event)
  {
    if (i_event.GetVarValue("eventname").equals("BIDITEM"))
    {
      ClearRed();
      if (i_event.GetVarValue("ShipSlotNumber").equals(m_idx))
      {
        m_greenback.setVisible(true);
      }
      else
      {
        m_greenback.setVisible(false);
      }
    }


    if (i_event.GetVarValue("eventname").equals("NEWSTATE"))
    {
      m_jb.setVisible(false);
    }
    
    if (i_event.GetVarValue("eventname").equals("LEGALACTION") &&
        i_event.GetVarValue("actionname").equals("OPENBID"))
    {
      m_jb.setVisible(m_item != 0);
    }


    if (i_event.GetVarValue("eventname").equals("PURCHASEDATA") &&
        i_event.GetVarValue("ShipSlotNumber").equals(m_idx))
    {
      String istring= i_event.GetVarValue("ItemNumber");

      m_lastboughtitem = Integer.parseInt(istring);
      RecalculateVisible();

      ItemByNumber.alterJLabel(m_boughtjl,m_lastboughtitem);
      
      String plidx = i_event.GetVarValue("PlayerNumber");
      if (plidx.equals("3000"))
      {
        m_buyername.setText("");
      }
      else if (m_playernames.containsKey(plidx))
      {
        m_buyername.setText(m_playernames.get(plidx));
      }
      else
      {
        m_buyername.setText("???");
      }

      m_buyercost.setText("Cost: " + i_event.GetVarValue("Cost"));
    }
  }
}

        
