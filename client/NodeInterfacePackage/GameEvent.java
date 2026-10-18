package NodeInterfacePackage;

import java.util.HashMap;
import java.util.Set;

public class GameEvent
{
  private HashMap<String,String> m_varpairs;
  private GuiPacketParser m_gpp;

  public GameEvent(GuiPacketParser gpp) {
    m_gpp = gpp;
  }

  public GuiPacketParser getGPP() { return m_gpp; }

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
    if (m_varpairs == null)
    {
      // Thrown instead of a bare NPE on m_varpairs specifically so this is
      // self-diagnosing -- every remaining widget still calling the old
      // per-event API will hit this exact thing, one at a time, as full-state
      // delivery reaches it (ActionTabNode/ActionWindowNode/RoomPage/
      // RoomWidget already converted; most of Outpost's and MoV's hand-written
      // widgets have not). The requested var name is the fastest way to find
      // which XML declaration/widget is responsible, since a stack trace
      // through GameNode's generic recursive walk only shows class+method,
      // never which instance.
      throw new NodeInterfacePackageException(
          "GetVarValue(\"" + i_Name + "\") called on a full-state-based GameEvent (built from a "
          + "GuiPacketParser, not a named event) -- this widget hasn't been converted away from the "
          + "old per-event API yet. Use getGPP() and a GuiPacketQuery/JsonPath instead.");
    }
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
	
