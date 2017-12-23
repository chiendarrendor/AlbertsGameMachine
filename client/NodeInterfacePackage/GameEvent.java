package NodeInterfacePackage;

import java.util.HashMap;
import java.util.Set;

public class GameEvent
{
  private HashMap<String,String> m_varpairs;

  public GameEvent(String i_name)
  {
    m_varpairs = new HashMap<String,String>();
    m_varpairs.put("eventname",i_name);
  }

  public void AddVariable(String i_name,String i_value)
  {
    m_varpairs.put(i_name,i_value);
  }

  public Set<String> GetKeys()
  {
    return m_varpairs.keySet();
  }

  public String GetVarValue(String i_Name)
  {
    return m_varpairs.get(i_Name);
  }

  public boolean asBoolean(String i_Name)
  {
    return Boolean.parseBoolean(GetVarValue(i_Name));
  }
  
  public int asInt(String i_Name)
  {
    return Integer.parseInt(GetVarValue(i_Name));
  }
}
	
