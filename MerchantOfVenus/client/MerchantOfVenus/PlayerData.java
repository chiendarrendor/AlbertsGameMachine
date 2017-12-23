package MerchantOfVenus;

import NodeInterfacePackage.*;
import MySwing.ImageLoader;
import java.util.*;
import java.util.concurrent.*;
import java.awt.Color;
import javax.swing.ImageIcon;

public class PlayerData
{
  // PlayerChanged called whenever info about a player changes.
  // iam will be the index of the IAM player, or -1 if it has not yet
  // been set.
  // if the change is for IAM to be set, then the IAM player will be passed
  // to PlayerChanged, if it exists, or null if no such player exists yet.
  // 
  // if PlayerChanged is called with null,-1, that means that a RESET has been executed
  public interface PlayerListener
  {
    public void PlayerChanged(Player p,int iam,int[] turnorder,int curplayer);
    public void Reset();
  }

  // if iamonly is true, this listener will only get PlayerChanged
  // whenever the IAM player changes, or the IAM value changes 
  public void addPlayerListener(PlayerListener listener,boolean iamonly)
  {
    if (iamonly) iamlisteners.add(listener);
    else listeners.add(listener);
  }

  public void removePlayerListener(PlayerListener listener)
  {
    listeners.remove(listener);
    iamlisteners.remove(listener);
  }

  private Map<Integer,Player> players;
  private int iamid;
  private int curplayer;
  private int[] turnorder;
  private List<PlayerListener> listeners;
  private List<PlayerListener> iamlisteners;

  public Player GetPlayer(int id)
  {
    Integer i = new Integer(id);
    if (players.containsKey(i)) return players.get(i);
    return null;
  }
  
  public Player GetCurPlayer()
  {
    return GetPlayer(curplayer);
  }

  public int GetIAm() { return iamid; }


  public class PlayerIconInfo
  {
    private Color c;
    private String colorName;
    private ImageIcon ii;
    public PlayerIconInfo(ImageIcon inImg,Color inColor,String cname)
    {
      c = inColor;
      colorName = cname;
      ii = IconManager.RecolorIcon(inImg,Color.WHITE,inColor);
    }
    public Color GetColor() { return c; }
    public ImageIcon GetIcon() { return ii; }
    public String GetName() { return colorName; }
  }

  private Vector<PlayerIconInfo> icons;

  public Vector<PlayerIconInfo> GetIconInfos() { return icons; }
  public PlayerIconInfo GetIconInfo(int idx) { return icons.elementAt(idx); }

  public void SwapColors(int i1,int i2)
  {
    if (i1 < 0 || i2 < 0 || i1 >=6 || i2 >= 6 || i1 == i2) return;
    PlayerIconInfo pii1 = icons.elementAt(i1);
    PlayerIconInfo pii2 = icons.elementAt(i2);
    icons.set(i2,pii1);
    icons.set(i1,pii2);

    Player actedPlayer = players.get(new Integer(i1));
    if (i1 == iamid) for (PlayerListener listener : iamlisteners) { listener.PlayerChanged(actedPlayer,iamid,turnorder,curplayer); }
    if (actedPlayer != null) for (PlayerListener listener : listeners) { listener.PlayerChanged(actedPlayer,iamid,turnorder,curplayer); }

    actedPlayer = players.get(new Integer(i2));
    if (i2 == iamid) for (PlayerListener listener : iamlisteners) { listener.PlayerChanged(actedPlayer,iamid,turnorder,curplayer); }
    if (actedPlayer != null) for (PlayerListener listener : listeners) { listener.PlayerChanged(actedPlayer,iamid,turnorder,curplayer); }
  }



  public PlayerData(String dir)
  {
    icons = new Vector<PlayerIconInfo>();
    ImageIcon ii = ImageLoader.MakeImageIcon(dir+"/shiplet.png"); 
    icons.add(new PlayerIconInfo(ii,Color.WHITE,"White"));
    icons.add(new PlayerIconInfo(ii,Color.YELLOW,"Yellow"));
    icons.add(new PlayerIconInfo(ii,Color.GREEN,"Green"));
    icons.add(new PlayerIconInfo(ii,Color.MAGENTA,"Magenta"));
    icons.add(new PlayerIconInfo(ii,Color.BLUE,"Blue"));
    icons.add(new PlayerIconInfo(ii,Color.RED,"Red"));

    listeners = new CopyOnWriteArrayList<PlayerListener>();
    iamlisteners = new CopyOnWriteArrayList<PlayerListener>();
    Reset();
  }

  public void HandleEvent(GameEvent i_event)
  {
    boolean anychanged = false;
    boolean iamchanged = false;
    Player actedPlayer = null;

    String ename = i_event.GetVarValue("eventname");

    if (ename.equals("IAM"))
    {
      iamid = i_event.asInt("IAM.PlayerId");
      anychanged = true;
      iamchanged = true;
      actedPlayer = players.get(new Integer(iamid));
      if (actedPlayer != null) actedPlayer.SetIAm();
    }
    else if (ename.equals("PLAYER"))
    {
      actedPlayer = new Player(i_event.GetVarValue("PLAYER.Name"),
                               i_event.asInt("PLAYER.PlayerId"),
                               this);
      players.put(new Integer(actedPlayer.GetId()),actedPlayer);
      anychanged = true;
      int[] newturnorder = new int[players.size()];
      if (turnorder != null) System.arraycopy(turnorder,0,newturnorder,0,players.size()-1);
      newturnorder[players.size()-1] = players.size()-1;
      turnorder = newturnorder;

      if (actedPlayer.GetId() == iamid)
      {
        actedPlayer.SetIAm();
        iamchanged = true;
      }
    }
    else if (ename.equals("PLAYEROKSTARTUP"))
    {
      actedPlayer = players.get(new Integer(i_event.asInt("PLAYEROKSTARTUP.PlayerId")));
      if (actedPlayer != null)
      {
        actedPlayer.SetOkStartup(i_event.GetVarValue("PLAYEROKSTARTUP.IsOk").equals("1"));
        anychanged = true;
        if (actedPlayer.GetId() == iamid) iamchanged = true;
      }
    }
    else if (ename.equals("TURNORDER"))
    {
      String to = i_event.GetVarValue("TURNORDER.order");
      turnorder = new int[to.length()];
      for (int i = 0 ; i < to.length() ; ++i)
      {
        turnorder[i] = Integer.parseInt(to.substring(i,i+1));
      }
      anychanged = true;
    }
    else if (ename.equals("PLAYERMONEY"))
    {
      actedPlayer = players.get(new Integer(i_event.asInt("PLAYERMONEY.PlayerId")));
      if (actedPlayer != null)
      {
        actedPlayer.SetMoney(i_event.asInt("PLAYERMONEY.Money"),
                             i_event.asInt("PLAYERMONEY.Barter"),
                             i_event.asInt("PLAYERMONEY.Worth"));
        anychanged = true;
        if (actedPlayer.GetId() == iamid) iamchanged = true;
      }
    }
    else if (ename.equals("PLAYERRACE"))
    {
      actedPlayer = players.get(new Integer(i_event.asInt("PLAYERRACE.PlayerId")));
      if (actedPlayer != null)
      {
        actedPlayer.SetRace(i_event.GetVarValue("PLAYERRACE.RaceId"));
        anychanged = true;
        if (actedPlayer.GetId() == iamid) iamchanged = true;
      }
    }
    else if (ename.equals("ADDTOPLAYER"))
    {
      actedPlayer = players.get(new Integer(i_event.asInt("ADDTOPLAYER.PlayerId")));
      if (actedPlayer != null)
      {
        actedPlayer.AddToken(new Token(i_event.GetVarValue("ADDTOPLAYER.TokenEncoding")));

        anychanged = true;
        if (actedPlayer.GetId() == iamid) iamchanged = true;
      }
    }
    else if (ename.equals("SUBTRACTFROMPLAYER"))
    {
      actedPlayer = players.get(new Integer(i_event.asInt("SUBTRACTFROMPLAYER.PlayerId")));
      if (actedPlayer != null)
      {
        actedPlayer.RemoveToken(i_event.GetVarValue("SUBTRACTFROMPLAYER.TokenEncoding"));
        anychanged = true;
        if (actedPlayer.GetId() == iamid) iamchanged = true;
      }
    }
    else if (ename.equals("PLAYERLOCATION"))
    {
      actedPlayer = players.get(new Integer(i_event.asInt("PLAYERLOCATION.PlayerId")));
      if (actedPlayer != null)
      {
        actedPlayer.SetLocation(i_event.GetVarValue("PLAYERLOCATION.SpaceName"));
        anychanged = true;
        if (actedPlayer.GetId() == iamid) iamchanged = true;
      }
    }
    else if (ename.equals("PLAYERWINS"))
    {
      actedPlayer = players.get(new Integer(i_event.asInt("PLAYERWINS.PlayerId")));
      if (actedPlayer != null)
      {
        actedPlayer.SetWins();
        anychanged = true;
        if (actedPlayer.GetId() == iamid) iamchanged = true;
      }
    }
    else if (ename.equals("REMAININGTRANSCOUNT"))
    {
      actedPlayer = players.get(new Integer(i_event.asInt("REMAININGTRANSCOUNT.PlayerId")));
      if (actedPlayer != null)
      {
        actedPlayer.SetRemaining(i_event.asInt("REMAININGTRANSCOUNT.NumBuys"),
                                 i_event.asInt("REMAININGTRANSCOUNT.NumSells"));
        anychanged = true;
        if (actedPlayer.GetId() == iamid) iamchanged = true;
      }
    }
    else if (ename.equals("CURPLAYER"))
    {
      curplayer = new Integer(i_event.asInt("CURPLAYER.PlayerId"));
      actedPlayer = players.get(curplayer);
      anychanged = true;
      if (actedPlayer.GetId() == iamid) iamchanged = true;
    }
    else if (ename.equals("AGEPLAYER"))
    {
    	actedPlayer = players.get(new Integer(i_event.asInt("AGEPLAYER.PlayerId")));
    	if (actedPlayer != null)
    	{
    		actedPlayer.Age();
    		anychanged = true;
    		if (actedPlayer.GetId() == iamid) iamchanged = true;
    	}
    }

    if (anychanged) for (PlayerListener listener : listeners) { listener.PlayerChanged(actedPlayer,iamid,turnorder,curplayer); }
    if (iamchanged) for (PlayerListener listener : iamlisteners) { listener.PlayerChanged(actedPlayer,iamid,turnorder,curplayer); }

  }
  
  public void Reset()
  {
    iamid = -1;
    curplayer = -1;
    players = new HashMap<Integer,Player> ();
    turnorder = null;
    for (PlayerListener listener : listeners) { listener.Reset(); }
    for (PlayerListener listener : iamlisteners) { listener.Reset(); }
  }

}