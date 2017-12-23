package MerchantOfVenus;

import java.awt.*;
import javax.swing.*;
import javax.swing.border.*;
import java.util.*;
import MySwing.*;

public class BasePops implements TradeBaseData.TradeBaseListener
{
  private TradeBaseData tbd;
  private PopAndLockPanel palp;
  private Set<String> seenBases; 

  // properties for the PopAndLocklabel
  private static int starty = 100;
  private static int ysep = 30;
  private static int startx = PopAndLockPanel.OTHEREXTREME;

  // properties for the panel
  private static int width = 600;
  private static int height = 600;

  private class PALPHandle extends JPanel implements TradeBaseData.TradeBaseListener
  {
	private static final long serialVersionUID = 1L;
	private TradeBase tb;
    private JLabel handle;

    public String GetName()
    {
      String base = Utilities.GetRaceOfRaceId(tb.GetRaceId()) + "(" + tb.GetRaceId() + ")";
      SolarSystem ss = tb.GetSolarSystem();
      if (ss == null) return base;
      return base + "@" + ss.GetName();
    }

    public PALPHandle(TradeBase tb,int index)
    {
      super();

      this.tb = tb;
      tbd.addTradeBaseListener(this);
      
      handle = palp.Add(this,GetName(),startx,starty + index*ysep,true);
      handle.setForeground(Color.GREEN);
      
      setBackground(Color.BLACK);
      setBorder(new LineBorder(Color.BLUE,5,true));
    }

    public void TradeBaseChanged(TradeBase evt)
    {
      if (evt == null)
      {
        tbd.removeListener(this);
        palp.Remove(handle);
        return;
      }
      if (evt != tb) return;

      handle.setText(GetName());
      repaint();
    }


    public Dimension getMinimumSize() { return getPreferredSize(); }
    public Dimension getMaximumSize() { return getPreferredSize(); }
    public Dimension getPreferredSize() { return new Dimension(width,height); }

    public void paintComponent(Graphics g)
    {
      super.paintComponent(g);
      Graphics2D g2d = (Graphics2D)g;

      Vector<Token> ious = new Vector<Token>();
      Vector<Token> deeds = new Vector<Token>();
      Vector<Token> goods = new Vector<Token>();
      Vector<Token> factorygoods = new Vector<Token>();
      Vector<Token> fares = new Vector<Token>();
      Vector<Token> demands = new Vector<Token>();
      Vector<Token> ships = new Vector<Token>();
      Vector<Token> equip = new Vector<Token>();
      SplitTokens(ious,deeds,goods,factorygoods,fares,demands,ships,equip);

      g2d.setColor(Color.WHITE);


      DrawRow(g2d,"IOU: ",ious,0,false);
      DrawRow(g2d,"Deeds: ",deeds,1,false);
      DrawRow(g2d,"Goods: ",goods,2,true);
      DrawRow(g2d,"Factory Goods: ",factorygoods,3,true);
      DrawRow(g2d,"Fares: ",fares,4,false);
      DrawRow(g2d,"Demands: ",demands,5,false);
      DrawRow(g2d,"Ships: ",ships,6,false);
      DrawRow(g2d,"Equipment: ",equip,7,false);
    }

    int xc = 90;
    int ystart = 15;
    int isize = 64;
    int sep = 4;

    private void DrawRow(Graphics2D g2d,String label,Collection<Token> ts,int index,boolean addcount)
    {
      StringDrawer.DrawString(g2d,label,StringDrawer.BOTTOM,StringDrawer.RIGHT,xc,isize/2+ystart+index*(isize+sep));
      int count = 1;
      if (!addcount) count = ts.size();
      
      int tidx = 0;
      for (Token t : ts)
      {
        ImageIcon ii = IconManager.GetTokenIcon(t);
        int iwidth = ii.getIconWidth();
        ii.paintIcon(null,g2d,xc + sep + tidx * (iwidth+sep),ystart+index*(isize+sep));
        ++tidx;
        if (tidx == count) break;
      }

      if (addcount && ts.size() > 1)
      {
        StringDrawer.DrawString(g2d,"x"+ts.size(),StringDrawer.BOTTOM,StringDrawer.LEFT,
                                xc+sep+isize+sep+isize+sep,isize/2+ystart+index*(isize+sep));
      }
    }

    private void SplitTokens(Vector<Token> iou,
                             Vector<Token> deeds,
                             Vector<Token> goods,
                             Vector<Token> factorygoods,
                             Vector<Token> fares,
                             Vector<Token> demands,
                             Vector<Token> ships,
                             Vector<Token> equip)
    {
      for (Token t : tb.GetTokens())
      {
        if (t.GetSizeClass().equals("iou")) iou.add(t);
        else if (t.GetSizeClass().equals("deed")) deeds.add(t);
        else if (t.GetSizeClass().equals("good"))
        {
          if (t.IsFactoryGood()) factorygoods.add(t);
          else goods.add(t);
        }
        else if (t.GetSizeClass().equals("ship")) ships.add(t);
        else if (t.GetSizeClass().equals("fare")) fares.add(t);
        else if (t.GetSizeClass().equals("demand")) demands.add(t);
        else if (t.GetSizeClass().equals("equip")) equip.add(t);
        else if (t.GetSizeClass().equals("drive")) equip.add(t);
        else if (t.GetSizeClass().equals("relic")) equip.add(t);
        else System.out.println("What is this? " + t.GetEncoding());
      }
    }
  }

  public BasePops(PopAndLockPanel palp,TradeBaseData tbd)
  {
    this.palp = palp;
    this.tbd = tbd;
    seenBases = new HashSet<String>();
    tbd.addTradeBaseListener(this);
  }

  public void TradeBaseChanged(TradeBase t)
  {
    if (t == null) { seenBases = new HashSet<String>(); return; }
    if (seenBases.contains(t.GetRaceId())) return;

    int idx = seenBases.size();
    seenBases.add(t.GetRaceId());
    new PALPHandle(t,idx);
  }
}
    
    