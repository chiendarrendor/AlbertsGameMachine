package MerchantOfVenus;

import NodeInterfacePackage.*;
import java.util.*;
import java.util.concurrent.*;

public class TradeBaseData
{
  private Map<String,TradeBase> tradebases;
  private Map<String,SolarSystem> solarsystems;

  private List<TradeBaseListener> baseListeners;

  private List<SolarSystemListener> solarsystemListeners;

  public Map<String,SolarSystem> GetSolarSystems() { return solarsystems; }
  public Map<String,TradeBase> GetTradeBases() { return tradebases; }

  // fires when a trade base is created or updated.
  // a null TradeBase is indicative of a reset.
  public interface TradeBaseListener
  {
    public void TradeBaseChanged(TradeBase t);
  }

  public interface SolarSystemListener
  {
    public void SolarSystemChanged(SolarSystem ss);
  }

  public void addTradeBaseListener(TradeBaseListener tbl)
  {
    baseListeners.add(tbl);
  }
  
  public void addSolarSystemListener(SolarSystemListener ssl)
  {
    solarsystemListeners.add(ssl);
  }

  public void removeListener(TradeBaseListener l)
  {
    baseListeners.remove(l);
  }

  public void removeListener(SolarSystemListener l)
  {
    solarsystemListeners.remove(l);
  }

  public TradeBaseData()
  {
    baseListeners = new CopyOnWriteArrayList<TradeBaseListener>();
    solarsystemListeners = new CopyOnWriteArrayList<SolarSystemListener>();
    Reset();
  }

  public void HandleEvent(GameEvent ev)
  {
    TradeBase base = null;
    SolarSystem ss = null;
    String en = ev.GetVarValue("eventname");

    if (en.equals("ADDTOTRADEBASE"))
    {
      String rid = ev.GetVarValue("ADDTOTRADEBASE.RaceId");
      base = tradebases.get(rid);
      if (base == null)
      {
        base = new TradeBase(rid);
        tradebases.put(base.GetRaceId(),base);
      }
      base.AddToken(new Token(ev.GetVarValue("ADDTOTRADEBASE.TokenEncoding")));
    }
    else if (en.equals("SUBTRACTFROMTRADEBASE"))
    {
      String rid = ev.GetVarValue("SUBTRACTFROMTRADEBASE.RaceId");
      base = tradebases.get(rid);
      if (base == null)
      {
        base = new TradeBase(rid);
        tradebases.put(base.GetRaceId(),base);
      }
      String enc = ev.GetVarValue("SUBTRACTFROMTRADEBASE.TokenEncoding");
      base.RemoveToken(enc);
    }
    else if (en.equals("SOLARSYSTEMPRIVACY"))
    {
      String ssname = ev.GetVarValue("SOLARSYSTEMPRIVACY.SolarSystemName");
      ss = solarsystems.get(ssname);
      if (ss == null)
      {
        ss = new SolarSystem(ssname);
        solarsystems.put(ss.GetName(),ss);
      }
      ss.SetSecrecy(ev.GetVarValue("SOLARSYSTEMPRIVACY.Secrecy"));
    }
    else if (en.equals("BASELOCATION"))
    {
      base = tradebases.get(ev.GetVarValue("BASELOCATION.RaceId"));
      ss = solarsystems.get(ev.GetVarValue("BASELOCATION.SolarSystemName"));
      base.SetSolarSystem(ss);
      ss.SetBase(base);
    }
    else if (en.equals("FACTORYOWNER"))
    {
      base = tradebases.get(ev.GetVarValue("FACTORYOWNER.RaceId"));
      base.SetFactoryOwner(ev.asInt("FACTORYOWNER.PlayerId"));
    }

    if (base != null) for (TradeBaseListener tbl : baseListeners) tbl.TradeBaseChanged(base);
    if (ss != null) for (SolarSystemListener ssl : solarsystemListeners) ssl.SolarSystemChanged(ss);
  }

  public void Reset()
  {
    tradebases = new HashMap<String,TradeBase>();
    solarsystems = new HashMap<String,SolarSystem>();
    for (TradeBaseListener tbl : baseListeners) tbl.TradeBaseChanged(null);
    for (SolarSystemListener ssl : solarsystemListeners) ssl.SolarSystemChanged(null);
  }    
}

  
