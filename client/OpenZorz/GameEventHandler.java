package OpenZorz;

import java.util.ArrayList;

import NodeInterfacePackage.GameEvent;

public class GameEventHandler
{
  private ArrayList<HandlerUnit> m_HandlerUnits;
  private boolean m_Shows;
  private boolean m_Hides;
  private boolean m_Blocks;
  private boolean m_DefaultBlock;
    
  public final static int UNIT_SHOW = 0;
  public final static int UNIT_HIDE = 1;
  public final static int UNIT_SHOW_IF_NOT = 2;
  public final static int UNIT_HIDE_IF_NOT = 3;
  public final static int UNIT_SHOWHIDE = 4;
  public final static int UNIT_HIDESHOW = 5;
  public final static int UNIT_PASSIF = 6;
  public final static int UNIT_BLOCKIF = 7;
  public final static int UNIT_PASSIFNOT = 8;
  public final static int UNIT_BLOCKIFNOT = 9;

  private class HandlerUnit
  {
    private int m_utype;
    private ArrayList<String> m_VarNames;
    private ArrayList<String> m_VarVals;

    public HandlerUnit(int i_utype)
    {
	    m_utype = i_utype;
	    m_VarNames = new ArrayList<String>();
	    m_VarVals = new ArrayList<String>();
    }
	
    public void AddVarVal(String i_VarName,String i_VarVal)
    {
	    m_VarNames.add(i_VarName);
	    m_VarVals.add(i_VarVal);
    }
	
    public int GetUnitType() { return m_utype; }
	
    public boolean Matches(GameEvent i_event)
    {
	    int i;
	    for (i = 0 ; i < m_VarNames.size() ; ++i)
	    {
        String vname = m_VarNames.get(i);
        String vval = m_VarVals.get(i);

        String evval = i_event.GetVarValue(vname);

        // System.out.println("      Matching: (" + vname + "," + vval + "): " + evval);
		
        if (evval == null) return false;
        if (!evval.equals(vval)) return false;
	    }
	    return true;
    }
  }

  public GameEventHandler()
  {
    m_HandlerUnits = new ArrayList<HandlerUnit>();
    m_DefaultBlock = false;
  }

  public void AddEntry(int i_entrytype)
  {
    if (i_entrytype == UNIT_PASSIF || i_entrytype == UNIT_PASSIFNOT)
    {
	    m_DefaultBlock = true;
    }

    HandlerUnit hu = new HandlerUnit(i_entrytype);
    m_HandlerUnits.add(hu);
  }

  public void AddVarVal(String i_name,String i_value)
  {
    String[] tokens = i_value.split("\\|",-1);

    int i;
    for (i = 0 ; i < tokens.length ; ++i)
    {
      (m_HandlerUnits.get(m_HandlerUnits.size() - 1)).AddVarVal(i_name,tokens[i]);
    }
  }

  public boolean IsVisible() { return m_Shows; }
  public boolean IsInvisible() { return m_Hides; }
  public boolean EventPasses() { return !m_Blocks; }

  public void ParseEvent(GameEvent i_event)
  {
    m_Shows = false;
    m_Hides = false;
    m_Blocks = m_DefaultBlock;

    //    System.out.println("Parsing Event: " + i_event);

    int i;
	
    for (i = 0 ; i < m_HandlerUnits.size() ; ++i)
    {

	    HandlerUnit hu = m_HandlerUnits.get(i);
	    int ut = hu.GetUnitType();
	    boolean m = hu.Matches(i_event);

      // System.out.println("   Entry: " + ut + " matches? " + m);
	    
	    switch(ut)
	    {
	    case UNIT_SHOW:
        if (m) 
        {
          m_Shows = true;
          m_Hides = false;
        }
        break;
	    case UNIT_HIDE:
        if (m) 
        {
          m_Hides = true;
          m_Shows = false;
        }
        break;
	    case UNIT_SHOW_IF_NOT:
        if (!m)
        {
          m_Shows = true;
          m_Hides = false;
        }
        break;
	    case UNIT_HIDE_IF_NOT:
        if (!m)
        {
          m_Hides = true;
          m_Shows = false;
        }
        break;
	    case UNIT_SHOWHIDE:
        if (m) 
        {
          m_Shows = true;
          m_Hides = false;
        }
        else 
        {
          m_Hides = true;
          m_Shows = false;
        }
        break;
	    case UNIT_HIDESHOW:
        if (m) 
        {
          m_Hides = true;
          m_Shows = false;
        }
        else 
        {
          m_Shows = true;
          m_Hides = false;
        }
        break;
	    case UNIT_PASSIF:
        if (m) m_Blocks = false;
        break;
	    case UNIT_PASSIFNOT:
        if (!m) m_Blocks = false;
        break;
	    case UNIT_BLOCKIF:
        if (m) m_Blocks = true;
        break;
	    case UNIT_BLOCKIFNOT:
        if (!m) m_Blocks = true;
        break;
	    }
    }
  }
}

	
	
	
	

