package OpenZorz;

import java.util.*;

class GameEventType
{
    private String m_name;
    private ArrayList<String> m_vars;
    private HashSet<String> m_varlookup;

    public GameEventType(String i_name)
    {
      m_name = i_name;
      m_vars = new ArrayList<String>();
      m_varlookup = new HashSet<String>();
    }

    public String GetEventName() { return m_name; }
    
    public boolean HasVar(String i_varname)
    {
      return m_varlookup.contains(i_varname);
    }

    public void AddVar(String i_varname)
    {
      m_vars.add(i_varname);
      m_varlookup.add(i_varname);
    }
    
    public int GetNumVars()
    {
      return m_vars.size();
    }
    
    public String GetVarName(int i_index)
    {
      return m_vars.get(i_index);
    }
}

