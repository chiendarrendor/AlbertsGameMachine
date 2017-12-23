package MerchantOfVenus;

import java.util.*;

public class Destinations
{
  // Destination:
  // '/' separated
  // <space name>!<jump/walk>!<mp>[!<legal pilot numbers>]
  public class Destination
  {
    private String name;
    private int isJump; // 1 = jump, 0 = fly, -1 = ask which one.
    private int mp;
    private SortedSet<Integer> pNumbers;

    public Destination(String dString)
    {
      String[] dparts = dString.split("!");
      if (dparts.length != 3 && dparts.length != 4) throw new IllegalArgumentException("string (" + dString + ") not a destination!");
      name = dparts[0];
      isJump = dparts[1].equals("jump") ? 1 : 0;
      mp = Integer.parseInt(dparts[2]);
      pNumbers = new TreeSet<Integer>();
      if (dparts.length == 4)
      {
        for (int i = 0 ; i < dparts[3].length() ; ++i)
        {
          pNumbers.add(new Integer(Integer.parseInt(dparts[3].substring(i,i+1))));
        }
      }
    }
    
    public String GetName() { return name; }
    public int IsJump() { return isJump; }
    public void SetDuplicate() { isJump = -1; }
    public int GetMovementPoints() { return mp; }
    public SortedSet<Integer> GetPilotNumbers() { return pNumbers; }
  }

  private int curSpacePenalty;
  private boolean isFirst;
  private Map<String,Destination> dests;

  public Destinations(String penString, String firstString, String destsString)
  {
    curSpacePenalty = Integer.parseInt(penString);
    isFirst = firstString.equals("1");
    dests = new HashMap<String,Destination>();

    if (destsString.equals("")) return;

    String[] destStrings = destsString.split("/");
    for (String destString : destStrings)
    {
      Destination d = new Destination(destString);
      if (dests.containsKey(d.GetName()))
      {
        dests.get(d.GetName()).SetDuplicate();
      }
      else
      {
        dests.put(d.GetName(),d);
      }
    }
  }
     
  public int GetCurSpacePenalty() { return curSpacePenalty; }
  public boolean IsFirst() { return isFirst; }
  public Map<String,Destination> GetDestinations() { return dests; }
  
}
