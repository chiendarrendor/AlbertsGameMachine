package MerchantOfVenus;

import java.util.*;
import java.awt.Color;
import javax.swing.ImageIcon;

public class Player extends TokenHolder
{
  private String name;
  private int id;
  private boolean iam;
  private boolean okstartup;
  private int money;
  private int barter;
  private int worth;
  private String race;
  private String location;
  private boolean wins;
  private int buysleft;
  private int sellsleft;

  private PlayerData pdata;

  public Player(String n,int i,PlayerData pdata) 
  { 
    super();
    name = n ; 
    id = i; 
    iam=false;  
    okstartup=false;
    money=-1;
    barter=-1;
    worth=-1;
    race="";
    location="";
    wins=false;
    this.pdata = pdata;
    buysleft = 0;
    sellsleft = 0;
  }


  public String GetName() { return name; }
  public int GetId() { return id; }
  public boolean IAm() { return iam; }
  public boolean OkStartup() { return okstartup; }
  public int GetMoney() { return money; }
  public int GetBarter() { return barter; }
  public int GetWorth() { return worth; }
  public String GetRace() { return race; }
  public String GetLocation() { return location; }
  public boolean Wins() { return wins; }
  public void SetIAm() { iam = true; }
  public void SetOkStartup(boolean oks) { okstartup = oks; }
  public void SetMoney(int m,int b,int w) { money = m ; barter = b; worth = w; }
  public void SetRace(String r) { race = r; }
  public void SetLocation(String loc) { location = loc; }
  public void SetWins() { wins = true; }
  private PlayerData.PlayerIconInfo GetIconInfo() { return pdata.GetIconInfo(GetId()); }
  public Color GetColor() { return GetIconInfo().GetColor(); }
  public String GetColorName() { return GetIconInfo().GetName(); }
  public ImageIcon GetShipIcon() { return GetIconInfo().GetIcon(); }

  public static final int INFINITE = -1;
  public int GetSellsLeft() { return sellsleft; }
  public int GetBuysLeft() { return buysleft; }
  public void SetRemaining(int buys,int sells) { buysleft = buys; sellsleft = sells; }
  

  public int GetHoldCount()
  {
    // a relic adds nothing to hold count
    return GetHoldCount(new Token("Dummy|relic|80|40|barter|culture|notfg|limited|1a?1b?2?3?4a?4b?5?6?7a?7b?8?9a?9b?10?base||notdeed|0|0|reusable||0|0|old"));
  }

  public int GetHoldCount(Token newItem)
  {
    int count = 0;
    Set<String> equip = new HashSet<String>();

    Vector<Token> v = new Vector<Token>();
    for (Token t : GetTokens()) v.add(t);
    v.add(newItem);

    for(Token t : v)
    {
      if (t.GetSizeClass().equals("ship") ||
          t.GetSizeClass().equals("deed") ||
          t.GetSizeClass().equals("iou") ||
          t.GetSizeClass().equals("relic")) continue;
      else if (t.GetSizeClass().equals("good")) count += 2;
      else if (t.GetSizeClass().equals("fare") ||
               t.GetSizeClass().equals("drive")) count += 1;
      else if (t.GetSizeClass().equals("equip"))
      {
        if (equip.contains(t.GetName())) count += 1;
        else equip.add(t.GetName());
      }
    }
    return count;
  }    

}
