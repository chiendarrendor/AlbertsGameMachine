package MerchantOfVenus;

public class SolarSystem
{
  private String secrecy;
  private String solarsystemname;
  // will be null unless we know what the race is of this base.
  // (we should reasonably expect that we know of the base prior to the ss-race linkup
  private TradeBase base; 

  public String GetSecrecy() { return secrecy; }
  public String GetName() { return solarsystemname; }
  public TradeBase GetBase() { return base; }

  public SolarSystem(String name)
  {
    solarsystemname = name;
    secrecy = "";
    base = null;
  }

  public void SetSecrecy(String sec) { secrecy = sec; }
  public void SetBase(TradeBase tb) { base = tb; }
}


  
