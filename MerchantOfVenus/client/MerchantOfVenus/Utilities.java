package MerchantOfVenus;

import java.util.*;

public class Utilities
{
  public static String GetRaceOfRaceId(String raceid)
  {
    if (raceid.equals("1a")) return "Nillis";
    if (raceid.equals("1b")) return "Volois";
    if (raceid.equals("2")) return "Graw";
    if (raceid.equals("3")) return "Niks";
    if (raceid.equals("4a")) return "Dell";
    if (raceid.equals("4b")) return "Humans";
    if (raceid.equals("5")) return "Shenna";
    if (raceid.equals("6")) return "YXKLYX";
    if (raceid.equals("7a")) return "Zum";
    if (raceid.equals("7b")) return "EeepEeep";
    if (raceid.equals("8")) return "Whynoms";
    if (raceid.equals("9a")) return "Cholos";
    if (raceid.equals("9b")) return "Wollow";
    if (raceid.equals("10")) return "Qossuth";
    if (raceid.equals("base")) return "Tinkerers";
    return "????";
  }

  public static String join(String sep,Collection<String> c)
  {
    StringBuffer sb = new StringBuffer();
    boolean first = true;
    for(String s : c)
    {
      if (!first) sb.append(sep);
      first = false;
      sb.append(s);
    }
    return sb.toString();
  }

  // returns ship
  public static Token SplitPlayerTokens(Player player,
                                        Vector<Token> paperwork,
                                        Vector<Token> hull,
                                        Vector<Token> holds)
  {
    Token ship = null;
    Set<String> equip = new HashSet<String>();
    Vector<Token> smallHold = new Vector<Token>();
    for (Token t : player.GetTokens())
    {
      if (t.GetSizeClass().equals("ship")) ship = t;
      else if (t.GetSizeClass().equals("iou") || t.GetSizeClass().equals("deed")) paperwork.add(t);
      else if (t.GetSizeClass().equals("equip"))
      {
        if (equip.contains(t.GetName())) holds.add(t);
        else
        {
          hull.add(t);
          equip.add(t.GetName());
        }
      }
      else if (t.GetSizeClass().equals("relic")) hull.add(t);
      else if (t.GetSizeClass().equals("good")) holds.add(t);
      else smallHold.add(t);
    }
      
    holds.addAll(smallHold);
    
    return ship;
  }
}

