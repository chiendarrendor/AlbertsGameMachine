package MerchantOfVenus;

import java.util.Vector;
import java.util.Arrays;

public class Token
{
  String baseEncoding;
  String name;
  String sizeclass;
  int buyprice;
  int sellprice;
  String selltype;
  String selldestination;
  boolean isfactorygood;
  boolean isunlimited;
  Vector<String> buyerraces;
  String sellerrace;
  String deedtype;
  int defense;
  int attack;
  boolean isdisposable;
  String skipcolors;
  int speed;
  int holdcount;
  boolean isNew;

  public Token(String enc)
  {
    String[] pieces = enc.split("\\|");
    name = pieces[0];
    sizeclass = pieces[1];
    buyprice = Integer.parseInt(pieces[2]);
    sellprice = Integer.parseInt(pieces[3]);
    selltype = pieces[4];
    selldestination = pieces[5];
    isfactorygood = pieces[6].equals("fg");
    isunlimited = pieces[7].equals("unlimited");
    buyerraces = new Vector<String>(Arrays.asList(pieces[8].split("\\?")));
    sellerrace = pieces[9];
    deedtype = pieces[10];
    defense = Integer.parseInt(pieces[11]);
    attack = Integer.parseInt(pieces[12]);
    isdisposable = pieces[13].equals("disposable");
    skipcolors = pieces[14];
    speed = Integer.parseInt(pieces[15]);
    holdcount = Integer.parseInt(pieces[16]);
    isNew = pieces[17].equals("new");

    StringBuffer sb = new StringBuffer();
    for (int i = 0 ; i < pieces.length-1 ; ++i)
    {
      sb.append(pieces[i]);
      sb.append("|");
    }
    baseEncoding = sb.toString();

  }

  public String GetEncoding() { return baseEncoding + (isNew ? "new" : "old") ; }
  public String GetName() { return name; }
  public String GetSizeClass() { return sizeclass; }

  public int GetBuyPrice() { return buyprice; }
  public int GetSellPrice() { return sellprice; }
  public String GetSellType() { return selltype; } // does it turn into money or barter?
  public String GetSelDestination() { return selldestination; } // where does it go when sold?

  public boolean IsFactoryGood() { return isfactorygood; }
  public boolean IsUnlimited() { return isunlimited; } // true if tradebase selling this doesn't consume it
  public Vector<String> GetBuyerRaceIds() { return buyerraces; }
  public String GetSellerRaceId() { return sellerrace; }
  public String GetDeedType() { return deedtype; } // what kind of deed is it?
  public int GetDefense() { return defense; }
  public int GetAttack() { return attack; }
  public boolean IsDisposable() { return isdisposable; }
  public String GetSkipColors() { return skipcolors; } // Y and/or R
  public int GetSpeed() { return speed; }
  public int GetHoldCount() { return holdcount; }
  public boolean IsNew() { return isNew; }
  public void Age() { isNew = false; }
}
