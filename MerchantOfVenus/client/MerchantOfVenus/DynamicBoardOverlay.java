package MerchantOfVenus;

import NodeInterfacePackage.*;
import javax.swing.*;
import java.util.*;
import java.awt.*;
import MySwing.*;
import java.awt.event.*;


public class DynamicBoardOverlay extends JPanel implements PlayerData.PlayerListener,OverrideSpaceData.SpaceListener
{
	private static final long serialVersionUID = 1L;
private Board board;
  private Map<Integer,String> playerlocations;
  private Map<Integer,ImageIcon> playerships;
  private int iamid;
  private NearestSpaceSelector nss;
  private MoveManager mover;
  private OverrideSpaceData spaceData;

  Dimension preferred;
  int transX;
  int transY;
  int shipradius = 15; // how far from the center do we draw a ship that has to share?
  PlayerData pdata;
  ControlPanel cPanel;

  public DynamicBoardOverlay(String dir,Board b,PlayerData pdata,
                             ActionTransferManager i_atm,ControlPanel cp,OverrideSpaceData spaceData)
  {
    super();

    cPanel = cp;

    this.spaceData = spaceData;
    spaceData.AddSpaceListener(this);

    board = b;
    this.pdata = pdata;
    nss = new NearestSpaceSelector(board);
    mover = new MoveManager(dir,i_atm,this);

    preferred = new Dimension(board.GetBoundingBox().width+board.GetMargin()*2,
                              board.GetBoundingBox().height+board.GetMargin()*2);

    transX = board.GetMargin()-board.GetBoundingBox().x;
    transY = board.GetMargin()-board.GetBoundingBox().y;

    setOpaque(false);


    pdata.addPlayerListener(this,false);

    new ColorChangerPopperUpper(this);

    Reset();
  }

  public Dimension getPreferredSize() { return preferred; }

  public void SpaceChanged(OverrideSpaceData.Override o)
  {
    repaint();
  }

  public void HandleEvent(GameEvent ev)
  {
    if (mover.HandleEvent(ev)) repaint();
  }

  public void Reset()
  {
    playerlocations = new HashMap<Integer,String>();
    playerships = new HashMap<Integer,ImageIcon>();
    iamid = -1;
    mover.enable();

    repaint();
  }

  public void PlayerChanged(Player p,int iam,int[] turnorder,int curplayer)
  {
    iamid = iam;

    if (p == null) return;

    Integer plid = new Integer(p.GetId());
    if (!playerlocations.containsKey(plid))
    {
      playerlocations.put(plid,p.GetLocation());
      playerships.put(plid,p.GetShipIcon());
      repaint();
      return;
    }
    // if we get here, we've seen this player's location before.
    // only fire a repaint if the player has moved.

    boolean doRepaint = false;

    if (playerlocations.get(plid) != p.GetLocation())
    {
      playerlocations.put(plid,p.GetLocation());
      doRepaint = true;
    }      

    if (playerships.get(plid) != p.GetShipIcon())
    {
      playerships.put(plid,p.GetShipIcon());
      doRepaint = true;
    }

    if (doRepaint) repaint();
  }

  public void paintComponent(Graphics g)
  {
    super.paintComponent(g);
    Graphics2D g2d = (Graphics2D)g;
    g2d.translate(transX,transY);

    // draw space overlays first
    for (OverrideSpaceData.Override o : spaceData.GetOverrides().values())
    {
      if (o.icon == null) continue;

      ImageIcon ii;
      if (o.icon.icon != null)
      {
        ii = o.icon.icon;
      }
      else if (o.icon.playerPortId != -1)
      {
        ii = IconManager.GetPort(pdata.GetIconInfo(o.icon.playerPortId).GetColor());
      }
      else
      {
        continue;
      }

      ii.paintIcon(null,g2d,
                   o.mapspace.GetX()-ii.getIconWidth()/2,
                   o.mapspace.GetY()-ii.getIconHeight()/2);
        

      // these three lines put a wierd translucent square over icons that are not public.
      // if (o.secrecy.equals("public")) continue;
      // g2d.setColor(new Color(0,0,0,75));
      // g2d.fillRect(o.mapspace.GetX()-32,o.mapspace.GetY()-32,64,64);
    }

    // then ships
    // this finds, for every player we know about, where on the board they are.
    Map<String,Vector<Integer>> shiplocs = new HashMap<String,Vector<Integer>>();
    for (Map.Entry<Integer,String> plinfo : playerlocations.entrySet())
    {
      if (!shiplocs.containsKey(plinfo.getValue()))
      {
        shiplocs.put(plinfo.getValue(),new Vector<Integer>());
      }
      shiplocs.get(plinfo.getValue()).add(plinfo.getKey());
    }
    // now we can draw them.
    for (Map.Entry<String,Vector<Integer>> spinfo : shiplocs.entrySet())
    {
      if (spinfo.getValue().size() == 1) 
      {
        DrawShip(g2d,spinfo.getValue().elementAt(0).intValue(),spinfo.getKey(),0,0.0);
      }
      else
      {
        double dtheta = 2.0 * Math.PI / (double)spinfo.getValue().size();
        for (int i = 0 ; i < spinfo.getValue().size() ; ++i)
        {
          DrawShip(g2d,spinfo.getValue().elementAt(i),spinfo.getKey(),shipradius,dtheta * i);
        }
      }
    }
    // to-move highlight here?
    mover.paint(g2d);

  }
  
  // helper functions

  public void DrawShip(Graphics2D g2d,int plid,String loc,int radius,double theta)
  {
    ImageIcon ii = playerships.get(new Integer(plid));

    if (loc == null) return;

    Board.MapSpace ms = board.GetSpaces().get(loc);
    if (ms == null) return;

    double thx = radius * Math.cos(theta);
    double thy = -radius * Math.sin(theta);


    int x = ms.GetX() - ii.getIconWidth()/2 + (int)thx;
    int y = ms.GetY() - ii.getIconHeight()/2 + (int)thy;

    ii.paintIcon(null,g2d,x,y);
  }

  private class ColorChangerPopperUpper extends MouseAdapter
  {
    public ColorChangerPopperUpper(Component p)
    {
      p.addMouseListener(this);
    }

    public void mousePressed(MouseEvent e)
    {
      MaybeShow(e);
    }
    
    public void mouseReleased(MouseEvent e)
    {
      MaybeShow(e);
    }

    public void MaybeShow(MouseEvent e)
    {
      if (!e.isPopupTrigger()) return;
      if (iamid == -1) return;

      String[] possibilities = new String[pdata.GetIconInfos().size()];
      ImageIcon white = null;

      for (int i = 0 ; i < pdata.GetIconInfos().size() ; ++i)
      {
        possibilities[i] = pdata.GetIconInfos().elementAt(i).GetName();
        if (pdata.GetIconInfos().elementAt(i).GetColor() == Color.WHITE)
          white = pdata.GetIconInfos().elementAt(i).GetIcon();
      }
      
      String result = (String)JOptionPane.showInputDialog(
                                                          null,
                                                          "What Color would you like to be?",
                                                          "Color Chooser",
                                                          JOptionPane.PLAIN_MESSAGE,
                                                          white,
                                                          possibilities,
                                                          "????");

      if (result == null) return;

      // so, our index is iamid (which is a valid index)
      // let's find the other one
      int newColorIdx;
      for (newColorIdx = 0 ; newColorIdx < pdata.GetIconInfos().size() ; ++newColorIdx )
      {
        if (pdata.GetIconInfos().elementAt(newColorIdx).GetName().equals(result)) break;
      }

      // this will cause PlayerChanged to fire, which will cause a repaint
      pdata.SwapColors(iamid,newColorIdx);
    }
  }

  private class MoveManager extends MouseAdapter implements EnabledPanel
  {
    int disablecount;
    Component parent;
    ActionTransferManager atm;
    ImageIcon highlighter;
    boolean isSelectDestination;
    boolean isSelectPilotNumber;
    boolean isEndTurn;

    Destinations curDests;
    @SuppressWarnings("unused")
	Destinations.Destination theDest;

    public MoveManager(String icondir,ActionTransferManager i_atm,Component par)
    {
      disablecount = 0;
      parent = par;
      par.addMouseListener(this);
      atm = i_atm;
      highlighter = ImageLoader.MakeImageIcon(icondir+"/highlight.png"); 
      
      Object[] dice = new Object[7];
      dice[0] = "None";
      dice[1] =  ImageLoader.MakeImageIcon(icondir+"/die1.png");
      dice[2] =  ImageLoader.MakeImageIcon(icondir+"/die2.png");
      dice[3] =  ImageLoader.MakeImageIcon(icondir+"/die3.png");
      dice[4] =  ImageLoader.MakeImageIcon(icondir+"/die4.png");
      dice[5] =  ImageLoader.MakeImageIcon(icondir+"/die5.png");
      dice[6] =  ImageLoader.MakeImageIcon(icondir+"/die6.png");

      isSelectDestination = false;
      isSelectPilotNumber = false;
      isEndTurn = false;
      curDests = null;

      i_atm.AddButtonPresser("SELECTDESTINATION",
                             new ActionGenerator()
                             {
                               public void ActionLegalityChanged(boolean i_legal) 
                               { 
                                 isSelectDestination = i_legal; 
                                 parent.repaint();
                               }
                               public void ActionReadinessChanged(boolean i_ready) {}
                             }
                             );
      i_atm.AddButtonPresser("SELECTPILOTNUMBER",
                             new ActionGenerator()
                             {
                               public void ActionLegalityChanged(boolean i_legal)
                               {
                                 isSelectPilotNumber = i_legal;
                                 parent.repaint();
                               }
                               public void ActionReadinessChanged(boolean i_ready) {}
                             }
                             );
      i_atm.AddButtonPresser("ENDMOVE",
                             new ActionGenerator()
                             {
                               public void ActionLegalityChanged(boolean i_legal) 
                               { 
                                 isEndTurn = i_legal; 
                                 parent.repaint();
                               }
                               public void ActionReadinessChanged(boolean i_ready) {}
                             }
                             );

    }

    // EnabledPanel functions
    public void enable()
    {
      if (disablecount > 0)  --disablecount;
      DynamicBoardOverlay.this.repaint();
    }
  
    public void disable()
    {
      ++disablecount;
      DynamicBoardOverlay.this.repaint();
    }

    
    // this will return true iff the event we handle requires a redraw.
    // the redraw itself will be handled by our paint(Gaphics2D)
    public boolean HandleEvent(GameEvent ev)
    {
      boolean result = false;
      
      if (ev.GetVarValue("eventname").equals("DESTINATION"))
      {
        curDests = new Destinations(ev.GetVarValue("DESTINATION.CurSpacePenalty"),
                                    ev.GetVarValue("DESTINATION.IsFirst"),
                                    ev.GetVarValue("DESTINATION.Destinations"));
        if (curDests.GetDestinations().size() == 1)
        {
          theDest = curDests.GetDestinations().values().toArray(new Destinations.Destination[0])[0];
        }
        else
        {
          theDest = null;
        }

        result = true;
      }


      return result;
    }

    // paints the next move overlays.  the Graphics in question has already been
    // translated to Board coordinates.
    public void paint(Graphics2D g2d)
    {
      if (disablecount > 0) return;

      if (curDests != null && (isSelectDestination || isSelectPilotNumber))
      { 
        for (Destinations.Destination d : curDests.GetDestinations().values())
        {
          Board.MapSpace ms = board.GetSpaces().get(d.GetName());
          highlighter.paintIcon(null,g2d,
                                ms.GetX() - highlighter.getIconWidth()/2,
                                ms.GetY() - highlighter.getIconHeight()/2);
        }
      }
      if (isEndTurn && playerlocations.containsKey(new Integer(iamid)))
      {
        Board.MapSpace ms = board.GetSpaces().get(playerlocations.get(new Integer(iamid)));
        highlighter.paintIcon(null,g2d,
                                ms.GetX() - highlighter.getIconWidth()/2,
                                ms.GetY() - highlighter.getIconHeight()/2);
      }
    }

    private class ExecuteMoveToNextFunctor implements Functor
    {
      Board.MapSpace ms;
      public ExecuteMoveToNextFunctor(Board.MapSpace ms) { this.ms = ms; }
      public void execute()
      {
        Destinations.Destination d = curDests.GetDestinations().get(ms.GetSpaceName());
        int isJump = d.IsJump();
        theDest = d;
        
        if (isJump == -1)
        {
          cPanel.raiseJumpFly(d.GetName(),MoveManager.this);
        }
        else
        {
          atm.ReportNewValueToConsumers("SELECTDESTINATION.Destination",d.GetName());
          atm.ReportNewValueToConsumers("SELECTDESTINATION.IsJump",""+isJump);
          atm.ButtonPressed("SELECTDESTINATION");
        }
      }
    }



    public void mouseReleased(MouseEvent e)
    {
      if (disablecount > 0) return;

      if (e.getButton() != MouseEvent.BUTTON1) return;
      e.translatePoint(-transX,-transY);

      Board.MapSpace ms = nss.GetNearestSpace(e.getX(),e.getY());
      if (ms == null) return;

      if (isEndTurn && 
          playerlocations.containsKey(new Integer(iamid)) &&
          ms.GetSpaceName().equals(playerlocations.get(new Integer(iamid))))
      {
        // if this is the first space and the current space is a penalty and
        // the player elects to not move, let's prompt them before we allow that.
        if (curDests.GetCurSpacePenalty() > 0 && curDests.IsFirst())
        {
          cPanel.raiseCostlyFirst(curDests.GetCurSpacePenalty(), this);
        }
        else
        {
          atm.ButtonPressed("ENDMOVE");
        }
      }

      if (isSelectDestination &&
          curDests.GetDestinations().containsKey(ms.GetSpaceName()))
      {
        Functor f = new ExecuteMoveToNextFunctor(ms);

        if (curDests.GetCurSpacePenalty() > 0 && !curDests.IsFirst())
        {
          cPanel.raiseCostlyMove(curDests.GetCurSpacePenalty(),f,this);
        }
        else
        {
          f.execute();
        }
      }
    }
  }
}




  