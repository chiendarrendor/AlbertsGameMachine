package MerchantOfVenus;

import NodeInterfacePackage.*;
import java.util.*;

public class TradeItemFactory
{
  public enum TradeType { BASETYPE, PLAYERTYPE };
  ActionTransferManager atm;
  TradeBase theBase;
  Player curPlayer;
  boolean iam;
  String proprietor;
  String factoryOwner;
  Token ship;

  public TradeItemFactory(ActionTransferManager i_atm,TradeBase i_theBase,Player i_curPlayer,boolean i_iam,
                          String i_proprietor,String i_factoryOwner,Token i_ship)
  {
    atm = i_atm;
    theBase = i_theBase;
    curPlayer = i_curPlayer;
    iam = i_iam;
    proprietor = i_proprietor;
    factoryOwner = i_factoryOwner;
    ship = i_ship;
  }

  public TradeItem get(Token t,TradeType type,int count)
  {
    switch(type)
    {
    case BASETYPE: return getBaseItem(t,count);
    case PLAYERTYPE: return getPlayerItem(t,count);
    }
    return null;
  }

  private TradeItem getBaseItem(Token t,int count)
  {
    boolean hasBuys = curPlayer.GetBuysLeft() != 0;
    boolean hasSells = curPlayer.GetSellsLeft() != 0;
    int curFunds = curPlayer.GetMoney() + curPlayer.GetBarter();
    int numHolds = ship.GetHoldCount();
    Vector<Note> notes = new Vector<Note>();
    
    if (t.GetSizeClass().equals("fare"))
    {
      boolean canFit = curPlayer.GetHoldCount(t) <= numHolds;
      if (!canFit) notes.add(new Note(Note.NOTNOTE,"can't fit!"));
      return new TradeItem(atm,t,count,"PICKUP.FareEncoding","PICKUP","Pick Up",notes,iam&&canFit,TradeType.BASETYPE);
    }
    else if (t.GetSizeClass().equals("drive") ||
        t.GetSizeClass().equals("relic") ||
        t.GetSizeClass().equals("equip"))
    {
      boolean canFit = curPlayer.GetHoldCount(t) <= numHolds;
      boolean canPay = t.GetBuyPrice() <= curFunds;
      if (!canFit) notes.add(new Note(Note.NOTNOTE,"can't fit!"));
      if (!canPay) notes.add(new Note(Note.NOTNOTE,"no money!"));
      if (!hasBuys) notes.add(new Note(Note.NOTNOTE,"no buys"));
      if (proprietor != null) notes.add(new Note(Note.COMMISSIONNOTE,""+(t.GetBuyPrice()/10)+" to "+proprietor));
      return new TradeItem(atm,t,count,"BUY.ItemEncoding","BUY","Buy",notes,iam&&canFit&&canPay&&hasBuys,TradeType.BASETYPE);
    }
    else if (t.GetSizeClass().equals("deed"))
    {
      int buyPrice = t.GetBuyPrice();
      boolean isHome = curPlayer.GetRace().equals(theBase.GetRaceId());
      if (isHome)
      {
        notes.add(new Note(Note.DISCOUNTNOTE,""+(buyPrice/5)+" for home"));
        buyPrice -= buyPrice/5;
        notes.add(new Note(Note.FINALNOTE,""+buyPrice));
      }

      boolean canPay = buyPrice <= curFunds;

      if (!hasBuys) notes.add(new Note(Note.NOTNOTE,"no buys"));
      if (!canPay) notes.add(new Note(Note.NOTNOTE,"no money!"));
      if (proprietor != null) notes.add(new Note(Note.COMMISSIONNOTE,""+(buyPrice/10)+" to "+proprietor));
      return new TradeItem(atm,t,count,"BUY.ItemEncoding","BUY","Buy",notes,iam&&canPay&&hasBuys,TradeType.BASETYPE);
    }
    else if (t.GetSizeClass().equals("good"))
    {
      boolean canFit = curPlayer.GetHoldCount(t) <= numHolds;
      boolean canPay = t.GetBuyPrice() <= curFunds;
      if (!hasBuys) notes.add(new Note(Note.NOTNOTE,"no buys"));
      if (!canFit) notes.add(new Note(Note.NOTNOTE,"can't fit!"));
      if (!canPay) notes.add(new Note(Note.NOTNOTE,"no money!"));
      if (proprietor != null) notes.add(new Note(Note.COMMISSIONNOTE,""+(t.GetBuyPrice()/10)+" to "+proprietor));
      if (factoryOwner != null && t.IsFactoryGood())
        notes.add(new Note(Note.COMMISSIONNOTE,""+(t.GetBuyPrice()/2)+" to "+factoryOwner));
      return new TradeItem(atm,t,count,"BUY.ItemEncoding","BUY","Buy",notes,iam&&canFit&&canPay&&hasBuys,TradeType.BASETYPE);
    }      
    else if (t.GetSizeClass().equals("ship"))
    {
      // iam
      // hasBuys
      // hasSells
      int price = t.GetBuyPrice() - ship.GetSellPrice();
      boolean canPay = price <= curFunds;
      boolean canFit = curPlayer.GetHoldCount() <= t.GetHoldCount();

      if (t.GetName().equals(ship.GetName()))
      {
        notes.add(new Note(Note.NOTNOTE,"already own"));
        return new TradeItem(atm,t,count,null,null,null,notes,false,TradeType.BASETYPE);
      }

      if (!hasBuys) notes.add(new Note(Note.NOTNOTE,"no buys"));
      if (!hasSells) notes.add(new Note(Note.NOTNOTE,"no sells"));
      if (!canPay) notes.add(new Note(Note.NOTNOTE,"no money!"));
      if (!canFit) notes.add(new Note(Note.NOTNOTE,"can't fit!"));
      notes.add(new Note(Note.DISCOUNTNOTE,""+ship.GetSellPrice()+" to barter"));
      if (price < 0)
      {
        notes.add(new Note(Note.FINALNOTE,""+(-price)+" to barter"));
      }
      else
      {
        notes.add(new Note(Note.FINALNOTE,""+price+" from funds"));
      }
      if (proprietor != null) 
      {
        notes.add(new Note(Note.COMMISSIONNOTE,""+(t.GetBuyPrice()/10)+" to "+proprietor));
        notes.add(new Note(Note.COMMISSIONNOTE,""+(ship.GetSellPrice()/10)+" to "+proprietor));
      }
      return new TradeItem(atm,t,count,"TRADEINSHIP.ShipEncoding","TRADEINSHIP","Get Ship",notes,
                           iam&&hasBuys&&hasSells&&canPay&&canFit,TradeType.BASETYPE);
    }
    else if (t.GetSizeClass().equals("demand"))
    {
      return new TradeItem(atm,t,count,null,null,null,notes,false,TradeType.BASETYPE);
    }

    notes.add(new Note(Note.NOTNOTE,"what?"));
    return new TradeItem(atm,t,count,null,null,null,notes,false,TradeType.BASETYPE);
  }




  private TradeItem getPlayerItem(Token t,int count)
  {
    Vector<Note> notes = new Vector<Note>();
    boolean hasSells = curPlayer.GetSellsLeft() != 0;

    if (t.GetSizeClass().equals("iou"))
    {
      boolean isIssuer = theBase.GetRaceId().equals(t.GetBuyerRaceIds().elementAt(0));
      if (isIssuer)
      {
        notes.add(new Note(Note.BONUSNOTE,""+t.GetSellPrice()+" to barter"));
        if (proprietor != null)
        {
          notes.add(new Note(Note.COMMISSIONNOTE,""+(t.GetSellPrice()/10)+" to "+proprietor));
        }
      }
      else
      {
        return null;
      }
      return new TradeItem(atm,t,count,null,"REDEEMIOU","Redeem",notes,iam&&isIssuer,TradeType.PLAYERTYPE);
    }
    else if (t.GetSizeClass().equals("deed"))
    {
      return null;
    }
    else if (t.GetSizeClass().equals("fare"))
    {
      boolean isDestination = theBase.GetRaceId().equals(t.GetBuyerRaceIds().elementAt(0));
      boolean isNew = t.IsNew();

      if (isDestination)
      {
        return new TradeItem(atm,t,count,"DROPOFF.FareEncoding","DROPOFF","Drop Off",null,iam,TradeType.PLAYERTYPE);
      }
      else if (!isNew)
      {
        notes.add(new Note(Note.NOTNOTE,"not dest")); 
        return new TradeItem(atm,t,count,"JETTISON.ItemEncoding","JETTISON","Jettison",notes,iam,TradeType.PLAYERTYPE);
      }
      else
      {
        notes.add(new Note(Note.NOTNOTE,"not dest")); 
        notes.add(new Note(Note.NOTNOTE,"is new")); 
        return new TradeItem(atm,t,count,null,null,null,notes,false,TradeType.PLAYERTYPE);
      }
    }
    else if (t.GetSizeClass().equals("ship"))
    {
      return null;
    }
    else if (t.GetSizeClass().equals("good"))
    {
      boolean isDestination = t.GetBuyerRaceIds().contains(theBase.GetRaceId());
      //boolean isNew = t.IsNew();

      int sellPrice = t.GetSellPrice();
      int bonus = 0;

      if (!isDestination) 
      {
        notes.add(new Note(Note.NOTNOTE,"not buyer"));
      }
      else
      {
        for (Token demt : theBase.GetTokens())
        {
          if (demt.GetSizeClass().equals("demand") && demt.GetName().equals(t.GetName()))
          {
            bonus += demt.GetSellPrice();
          }
        }

        if (bonus > 0)
        {
          notes.add(new Note(Note.BONUSNOTE,""+bonus+" demands"));
          sellPrice += bonus;
          notes.add(new Note(Note.FINALNOTE,""+sellPrice+" money"));
        }

        if (!hasSells) notes.add(new Note(Note.NOTNOTE,"no sells"));
      
        if (proprietor != null) 
          notes.add(new Note(Note.COMMISSIONNOTE,""+(sellPrice/10)+" to "+proprietor));
      }
      if (!iam) return new TradeItem(atm,t,count,null,null,null,notes,false,TradeType.PLAYERTYPE);
      if (isDestination && hasSells)
        return new TradeItem(atm,t,count,"SELL.ItemEncoding","SELL","Sell",notes,true,TradeType.PLAYERTYPE);

      if (!t.IsNew())
        return new TradeItem(atm,t,count,"JETTISON.ItemEncoding","JETTISON","Jettison",notes,true,TradeType.PLAYERTYPE);
      notes.add(new Note(Note.NOTNOTE,"is new!"));
      return new TradeItem(atm,t,count,null,null,null,notes,false,TradeType.PLAYERTYPE);


    }
    else if (t.GetSizeClass().equals("equip") || t.GetSizeClass().equals("drive") || t.GetSizeClass().equals("relic"))
    {
      notes.add(new Note(Note.BASENOTE,""+t.GetSellPrice()+" in barter"));
      if (proprietor != null)
      {
        notes.add(new Note(Note.COMMISSIONNOTE,""+(t.GetSellPrice()/10)+" to "+proprietor));
      }
      if (!hasSells) notes.add(new Note(Note.NOTNOTE,"no sells"));

      return new TradeItem(atm,t,count,"SELL.ItemEncoding","SELL","Sell",notes,iam&&hasSells,TradeType.PLAYERTYPE);
    }

    notes.add(new Note(Note.NOTNOTE,"what?"));
    return new TradeItem(atm,t,count,null,null,null,notes,false,TradeType.PLAYERTYPE);

  }



}

    
  