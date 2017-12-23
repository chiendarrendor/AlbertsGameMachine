package MerchantOfVenus;

import java.awt.*;
import javax.swing.*;
import java.util.*;
import MySwing.*;

public class TradeBaseDisplay extends JPanel 
{
	private static final long serialVersionUID = 1L;

private class SolarSystemLabel extends JLabel implements TradeBaseData.SolarSystemListener
  {
	private static final long serialVersionUID = 1L;
	private Map<String,SolarSystem> solarsystems;
    public SolarSystemLabel(TradeBaseData tbd)
    {
      super("");
      tbd.addSolarSystemListener(this);
      solarsystems = new HashMap<String,SolarSystem>();
      
      Redraw();
    }

    public void SolarSystemChanged(SolarSystem ss)
    {
      if (ss == null)
      {
        solarsystems = new HashMap<String,SolarSystem>();
      }
      else
      {
        solarsystems.put(ss.GetName(),ss);
      }

      Redraw();
    }

    private void Redraw()
    {
      HTMLTable ht = new HTMLTable(3,solarsystems.size()+1,true);
      ht.GetCell(0,0).SetText("Name");
      ht.GetCell(1,0).SetText("Secrecy");
      ht.GetCell(2,0).SetText("Race");

      int currow = 1;
      for (SolarSystem ss : solarsystems.values())
      {
        ht.GetCell(0,currow).SetText(ss.GetName());
        ht.GetCell(1,currow).SetText(ss.GetSecrecy());
        ht.GetCell(2,currow).SetText(ss.GetBase() != null ? ss.GetBase().GetRaceId() : "?");
        ++currow;
      }
      setText(ht.GetHTML());
    }
  }

  private interface TokenBool
  {
    public boolean DoPrint(Token t);
  }


  private class TradeBaseLabel extends JLabel implements TradeBaseData.TradeBaseListener
  {
	private static final long serialVersionUID = 1L;
	private Map<String,TradeBase> bases;
    public TradeBaseLabel(TradeBaseData tbd)
    {
      super("TBD");
      tbd.addTradeBaseListener(this);
      bases = new TreeMap<String,TradeBase>();
      Redraw();
    }

    public void TradeBaseChanged(TradeBase tb)
    {
      if (tb == null) { bases = new TreeMap<String,TradeBase>(); }
      else
      {
        bases.put(tb.GetRaceId(),tb);
      }
      Redraw();
    }
    
    private String GetTokenString(Vector<Token> tokens,TokenBool tb)
    {
      String result = "";
      boolean first = true;
      
      Map<String,Integer> counts = new TreeMap<String,Integer>();

      for (Token t: tokens)
      {
        if (!tb.DoPrint(t)) continue;
        if (!counts.containsKey(t.GetName()))
        {
          counts.put(t.GetName(),new Integer(0));
        }
        int v = counts.get(t.GetName()).intValue();
        counts.put(t.GetName(),new Integer(v+1));
      }

      for (String s : counts.keySet())
      {
        if (!first) result += ",";
        int v = counts.get(s);
        if (v > 1) result += "" + v + "x";
        result += s;
        first = false;
      }

      return result;
    }

    private void Redraw()
    {
      HTMLTable ht = new HTMLTable(11,bases.size()+1,true);
      ht.GetCell(0,0).SetText("Race Id");
      ht.GetCell(1,0).SetText("Race Name");
      ht.GetCell(2,0).SetText("Solar System");
      ht.GetCell(3,0).SetText("IOU");
      ht.GetCell(4,0).SetText("Paperwork");
      ht.GetCell(5,0).SetText("Ships");
      ht.GetCell(6,0).SetText("Equipment");
      ht.GetCell(7,0).SetText("Relics");
      ht.GetCell(8,0).SetText("Goods");
      ht.GetCell(9,0).SetText("Fares");
      ht.GetCell(10,0).SetText("Demands");
      
      int currow = 1;
      for (TradeBase tb : bases.values())
      {
        ht.GetCell(0,currow).SetText(tb.GetRaceId());
        ht.GetCell(1,currow).SetText(Utilities.GetRaceOfRaceId(tb.GetRaceId()));
        ht.GetCell(2,currow).SetText(tb.GetSolarSystem() == null ? "?" : tb.GetSolarSystem().GetName());
        ht.GetCell(3,currow).SetText(GetTokenString(tb.GetTokens(),
                                                    new TokenBool()
                                                    {
                                                      public boolean DoPrint(Token t)
                                                      {
                                                        return 
                                                          t.GetSizeClass().equals("iou");
                                                      }
                                                    }
                                                    ));
        ht.GetCell(4,currow).SetText(GetTokenString(tb.GetTokens(),
                                                    new TokenBool()
                                                    {
                                                      public boolean DoPrint(Token t)
                                                      {
                                                        return 
                                                          t.GetSizeClass().equals("deed");
                                                      }
                                                    }
                                                    ));
        ht.GetCell(5,currow).SetText(GetTokenString(tb.GetTokens(),
                                                    new TokenBool()
                                                    {
                                                      public boolean DoPrint(Token t)
                                                      {
                                                        return 
                                                          t.GetSizeClass().equals("ship");
                                                      }
                                                    }
                                                    ));
                                                          
        ht.GetCell(6,currow).SetText(GetTokenString(tb.GetTokens(),
                                                    new TokenBool()
                                                    {
                                                      public boolean DoPrint(Token t)
                                                      {
                                                        return 
                                                          t.GetSizeClass().equals("equip") ||
                                                          t.GetSizeClass().equals("drive");
                                                      }
                                                    }
                                                    ));
                                                          

        ht.GetCell(7,currow).SetText(GetTokenString(tb.GetTokens(),
                                                    new TokenBool()
                                                    {
                                                      public boolean DoPrint(Token t)
                                                      {
                                                        return 
                                                          t.GetSizeClass().equals("relic");
                                                      }
                                                    }
                                                    ));

        ht.GetCell(8,currow).SetText(GetTokenString(tb.GetTokens(),
                                                    new TokenBool()
                                                    {
                                                      public boolean DoPrint(Token t)
                                                      {
                                                        return 
                                                          t.GetSizeClass().equals("good");
                                                      }
                                                    }
                                                    ));
                                                          

        ht.GetCell(9,currow).SetText(GetTokenString(tb.GetTokens(),
                                                    new TokenBool()
                                                    {
                                                      public boolean DoPrint(Token t)
                                                      {
                                                        return 
                                                          t.GetSizeClass().equals("fare");
                                                      }
                                                    }
                                                    ));
                                                          
        ht.GetCell(10,currow).SetText(GetTokenString(tb.GetTokens(),
                                                    new TokenBool()
                                                    {
                                                      public boolean DoPrint(Token t)
                                                      {
                                                        return 
                                                          t.GetSizeClass().equals("demand");
                                                      }
                                                    }
                                                    ));
                                                          
        ++currow;
      }
      setText(ht.GetHTML());
    }
  }

  public TradeBaseDisplay(TradeBaseData tbd)
  {
    super(new BorderLayout());
    add(new SolarSystemLabel(tbd),BorderLayout.WEST);
    add(new TradeBaseLabel(tbd),BorderLayout.EAST);
  }
}
 

  
  