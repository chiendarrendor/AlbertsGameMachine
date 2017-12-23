package MerchantOfVenus;

public class TradeBase extends TokenHolder
{
  private String raceid;
  private int factoryOwner;
  private SolarSystem solarsystem;

  public String GetRaceId() { return raceid; }
  public SolarSystem GetSolarSystem() { return solarsystem; }
  public int GetFactoryOwner() { return factoryOwner; }

  public TradeBase(String r)
  {
    super();
    raceid = r;
    solarsystem=null;
    factoryOwner = -1;
  }

  public void SetSolarSystem(SolarSystem ss) { solarsystem = ss; }
  public void SetFactoryOwner(int o) { factoryOwner = o; }

}


    


