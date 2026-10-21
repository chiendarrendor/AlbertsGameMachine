package Outpost;

import javax.swing.JPanel;
import javax.swing.JLabel;
import javax.swing.JButton;
import MySwing.LockLayout;
import java.awt.Point;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;

public class ShipItem extends JPanel implements ActionListener
{
  // Outpost.hpp's NO_PLAYER_INDEX (C++-side only, nothing to import here) --
  // the sentinel GetUnownedItems().ClearPurchases() uses for "not bought".
  private static final int NO_PLAYER_INDEX = 3000;

  JLabel m_unboughtjl;
  JLabel m_redback;
  JLabel m_greenback;
  JButton m_jb;
  String m_idx;
  ShipActionGo m_sag;

  JLabel m_boughtjl;
  JLabel m_buyername;
  JLabel m_buyercost;

  public ShipItem(ShipActionGo i_sag,String i_idx)
  {
    super();

    m_sag = i_sag;
    m_idx = i_idx;

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

  public void ClearRed()
  {
    m_redback.setVisible(false);
  }

  // Called once per delivery by ShipList, which has already resolved every
  // value below from the FSS -- this class knows nothing about GuiPacketQuery
  // or the wire format at all, same split as RoomWidget/RoomPage.
  public void update(int i_itemId,int i_robotIconNumber,boolean i_canBid,boolean i_isBeingBidOn,
                     int i_purchasedItemId,int i_buyerPlayerId,int i_cost,String i_buyerName)
  {
    ItemByNumber.alterJLabel(m_unboughtjl,i_itemId == 7 ? i_robotIconNumber : i_itemId);
    m_greenback.setVisible(i_isBeingBidOn);

    boolean sold = (i_itemId == 0 && i_purchasedItemId != 0);
    m_boughtjl.setVisible(sold);
    m_unboughtjl.setVisible(!sold);
    m_buyername.setVisible(sold);
    m_buyercost.setVisible(sold);

    if (sold)
    {
      ItemByNumber.alterJLabel(m_boughtjl,i_purchasedItemId);
      m_buyername.setText(i_buyerPlayerId == NO_PLAYER_INDEX ? "" : i_buyerName);
      m_buyercost.setText("Cost: " + i_cost);
    }

    m_jb.setVisible(i_canBid && i_itemId != 0);
  }
}
