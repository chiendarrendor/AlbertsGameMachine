
package MerchantOfVenus;

import java.awt.*;
import javax.swing.*;
import javax.swing.border.*;
import java.util.*;
import MySwing.*;

public class PlayerPops implements PlayerData.PlayerListener
{
  private PlayerData pdata;
  private PopAndLockPanel palp;
  private Set<Integer> seenPlayers;

  // properties for landing the PopAndLock label
  private static int starty = 100;
  private static int ysep = 30;
  private static int startx = 0;

  // properties for the panel itself
  private static int width = 600;
  private static int height = 375;

  private class PALPHandle extends JPanel implements PlayerData.PlayerListener
  {
	private static final long serialVersionUID = 1L;
	private Player player;
    private JLabel handle;

    public PALPHandle(Player p,int iam,int[] turnorder,int curplayer)
    {
      super();

      this.player = p;
      pdata.addPlayerListener(this,false);

      handle = palp.Add(this,p.GetName(),startx,starty+p.GetId()*ysep,true);
      handle.setIcon(p.GetShipIcon());
      handle.setForeground(Color.GREEN);

      setLayout(new LockLayout());
      setBackground(Color.BLACK);
      
      PlayerChanged(p,iam,turnorder,curplayer);
    }

    public void Reset()
    {
        pdata.removePlayerListener(this);
        palp.Remove(handle);
    }


    public void PlayerChanged(Player evp,int iam,int[] turnorder,int curplayer)
    {
      int turn;
      String name = player.GetName();
      // turnorder is the list of playerids in turn order

      for (turn = 0 ; turn < turnorder.length ; ++turn)
      {
        if (turnorder[turn] == player.GetId()) break;
      }
      if (turn < turnorder.length) name = "(" + (turn+1) + ")" + name;
      if (curplayer == player.GetId()) name = ">" + name + "<";

      handle.setText(name);

      if (evp == null) return;

      // deal with all the items in the updated player object
      handle.setIcon(player.GetShipIcon());
      setBorder(new LineBorder(player.GetColor(),5,true));
      
      repaint();
    }
    
    public Dimension getMinimumSize() { return getPreferredSize(); }
    public Dimension getMaximumSize() { return getPreferredSize(); }
    public Dimension getPreferredSize() { return new Dimension(width,height); }


    public void paintComponent(Graphics g)
    {
      super.paintComponent(g);
      
      Graphics2D g2d = (Graphics2D)g;
      g2d.setColor(Color.WHITE);

      int cx = 85;
      int dy = 14;
      int ys = 55;

      StringDrawer.DrawString(g2d,     "Race: ",StringDrawer.BOTTOM,StringDrawer.RIGHT,cx,ys+dy*0);
      StringDrawer.DrawString(g2d,    "Money: ",StringDrawer.BOTTOM,StringDrawer.RIGHT,cx,ys+dy*1);
      StringDrawer.DrawString(g2d,   "Barter: ",StringDrawer.BOTTOM,StringDrawer.RIGHT,cx,ys+dy*2);
      StringDrawer.DrawString(g2d,"Net Worth: ",StringDrawer.BOTTOM,StringDrawer.RIGHT,cx,ys+dy*3);

      StringDrawer.DrawString(g2d,Utilities.GetRaceOfRaceId(player.GetRace()) + "(" + player.GetRace() + ")",
                              StringDrawer.BOTTOM,StringDrawer.LEFT,cx+5,ys+dy*0);
      StringDrawer.DrawString(g2d,""+player.GetMoney(),
                              StringDrawer.BOTTOM,StringDrawer.LEFT,cx+5,ys+dy*1);
      StringDrawer.DrawString(g2d,""+player.GetBarter(),
                              StringDrawer.BOTTOM,StringDrawer.LEFT,cx+5,ys+dy*2);
      StringDrawer.DrawString(g2d,""+player.GetWorth(),
                              StringDrawer.BOTTOM,StringDrawer.LEFT,cx+5,ys+dy*3);


      Vector<Token> paperwork = new Vector<Token>();
      Vector<Token> hull = new Vector<Token>();
      Vector<Token> holds = new Vector<Token>();
      Token ship = Utilities.SplitPlayerTokens(player,paperwork,hull,holds);
      if (ship != null)
      {
        StringDrawer.DrawString(g2d,  "Ship Class: ",StringDrawer.BOTTOM,StringDrawer.RIGHT,cx,ys+dy*4);
        StringDrawer.DrawString(g2d,  "Ship Speed: ",StringDrawer.BOTTOM,StringDrawer.RIGHT,cx,ys+dy*5);
        StringDrawer.DrawString(g2d,"Ship Trade-in: ",StringDrawer.BOTTOM,StringDrawer.RIGHT,cx,ys+dy*6);
        StringDrawer.DrawString(g2d,""+ship.GetName(),StringDrawer.BOTTOM,StringDrawer.LEFT,cx+5,ys+dy*4);
        StringDrawer.DrawString(g2d,""+ship.GetSpeed(),StringDrawer.BOTTOM,StringDrawer.LEFT,cx+5,ys+dy*5);
        StringDrawer.DrawString(g2d,""+ship.GetSellPrice(),StringDrawer.BOTTOM,StringDrawer.LEFT,cx+5,ys+dy*6);
      }


      int hully = ys+dy*7 + 5;
      StringDrawer.DrawString(g2d,"Hull: ",StringDrawer.BOTTOM,StringDrawer.RIGHT,cx,hully+32);
      int sx = cx + 5;
      int dx = 4;
      int isize = 64;

      for (int i = 0 ; i < hull.size() ; ++i)
      {
        ImageIcon ii = IconManager.GetTokenIcon(hull.elementAt(i));
        ii.paintIcon(null,g2d,sx+(isize+dx)*i,hully);
      }

      int papery = hully + isize + 5;

      StringDrawer.DrawString(g2d,"Paperwork: ",StringDrawer.BOTTOM,StringDrawer.RIGHT,cx,papery+32);
      
      int curpapery = papery;
      int xcount = 0;

      for (Token t : paperwork)
      {
        ImageIcon ii = IconManager.GetTokenIcon(t);

        int iwidth = ii.getIconWidth();
        int xloc = sx + (isize+dx)*xcount;

        if (xloc + iwidth > width) 
        {
          curpapery += isize + 5;
          xcount = 0;
        }
        ii.paintIcon(null,g2d,xloc,curpapery);

        ++xcount;
        if (iwidth > isize) ++xcount;
      }

      // upper left corner of first box
      int hy = 20;
      int hx = cx + 5 + dx + isize + 10;
      // dx = separator (both horiz and vert
      // isize (size of (single) icon)
      // printing strategy:
      // 014589
      // 2367 
      // 002244
      // 1133
      // 001122
      // 0011

      if (ship != null)
      {
        g2d.setColor(Color.WHITE);
        for (int i = 0 ; i < ship.GetHoldCount() ; ++i )
        {
          int rx = hx + (i/4)*(isize+dx)*2 + i%2*(isize+dx);
          int ry = hy + i/2%2 * (isize+dx);
          g2d.drawRect(rx,ry,isize,isize);
        }
      }

      int hidx = 0;
      for (int i = 0 ; i < holds.size() ; ++i)
      {
        int rx = hx + (hidx/4)*(isize+dx)*2 + hidx%2*(isize+dx);
        int ry = hy + hidx/2%2 * (isize+dx);

        Token t = holds.elementAt(i);
        ImageIcon ii = IconManager.GetTokenIcon(t);
        ii.paintIcon(null,g2d,rx,ry);
        hidx += t.GetSizeClass().equals("good") ? 2 : 1;
      }
    }

  }
  

  public PlayerPops(PopAndLockPanel palp,PlayerData pdata)
  {
    this.palp = palp;
    this.pdata = pdata;
    seenPlayers = new HashSet<Integer>();
    pdata.addPlayerListener(this,false);
  }

  public void Reset()
  {
    seenPlayers = new HashSet<Integer>();
  }

  public void PlayerChanged(Player p,int iam,int[] turnorder,int curplayer)
  {
    if (p == null) return;
    
    Integer pid = new Integer(p.GetId());
    if (seenPlayers.contains(pid)) return;

    seenPlayers.add(pid);
    new PALPHandle(p,iam,turnorder,curplayer);
  }
}


    