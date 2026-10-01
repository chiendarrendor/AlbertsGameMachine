package TheGameMachine;
import java.util.*;
import java.io.*;

import NodeInterfacePackage.*;
import DOMUtilities.*;
import Utilities.StringUtility;
import org.json.JSONObject;

public class GameGui
{
  private ClassLoader m_remoteClassLoader;
  private ArrayList<GameNode> m_Windows;
  private HashMap<String,GameEventType> m_EventTypes;
  private ActionTransferManager m_ActionManager;
  private PrintWriter m_ostream;
  private AttributeChecker m_eventvarnames;
  private StatusWindows m_StatusWindows;
  private TabbedWindow m_tabs;
  private String m_Namespace;

  public void ShowDebug(String i_message)
  {
    m_StatusWindows.ShowDebug(i_message);
  }

  public ClassLoader getRemoteClassLoader() { return m_remoteClassLoader; }

  public void dispose()
  {
    int i;
    for (i = 0 ; i < m_Windows.size() ; ++i)
    {
      GameNode gnode = m_Windows.get(i);
      gnode.dispose();
    }
  }

  private void SendEventToWindows(GameEvent i_event)
  {
    int i;
    for (i = 0 ; i < m_Windows.size() ; ++i)
    {
	    GameNode node = m_Windows.get(i);
	    node.HandleEvent(i_event);
    }
  }

  private void ResetWindows()
  {
    int i;
    for (i = 0 ; i < m_Windows.size() ; ++i)
    {
	    GameNode node = m_Windows.get(i);
	    node.reset();
    }
  }




  // The 7 reserved room-management actions live in namespaces the server
  // hardcodes ("roommanager"/"room" -- see RoomManager.cpp/Room::HandleAction)
  // regardless of which GameGui instance (a real game, or the shared ROOMGUI
  // instance) actually fires them -- so this split has to be hardcoded
  // client-side too, mirroring the server exactly, rather than sourced from
  // this instance's own identity.
  private String ReservedNamespaceFor(String i_ActionName)
  {
    if (i_ActionName.equals("NEWROOM") || i_ActionName.equals("CHANGEROOM") ||
        i_ActionName.equals("ROOMTALK") || i_ActionName.equals("PLAYERTALK"))
    {
      return "roommanager";
    }
    if (i_ActionName.equals("NEWGAME") || i_ActionName.equals("LOADGAME") ||
        i_ActionName.equals("SAVEGAME"))
    {
      return "room";
    }
    return null;
  }

  public void SendAction(String i_ActionName, JSONObject i_Params)
  {
    String namespace = ReservedNamespaceFor(i_ActionName);
    if (namespace == null)
    {
      namespace = m_Namespace;
    }

    JSONObject envelope = new JSONObject();
    envelope.put("namespace",namespace);
    envelope.put("action",i_ActionName);
    envelope.put("params",i_Params);

    m_ostream.print(envelope.toString());
    m_ostream.print("\n");
    m_ostream.flush();
  }

  public void HandleEvent(String s)
  {
    // ShowDebug("Event Is: " + s);

    String[] tokens = s.split(",",-1);
    if (tokens.length == 0)
    {
	    ShowDebug("Empty Message: " + s);
	    return;
    }

    int i;
    for (i = 0 ; i < tokens.length ; ++i)
    {
	    tokens[i] = StringUtility.ReComma(tokens[i].trim());
    }

    String eventname = tokens[0];

    GameEventType et = m_EventTypes.get(eventname);
    if (et==null)
    {
	    // ShowDebug("Unknown Event: " + s);
	    return;
    }

    if (et.GetNumVars() != tokens.length - 1)
    {
	    ShowDebug("Event " + eventname + " should have " + et.GetNumVars() + " arguments: " + s);
	    return;
    }

    GameEvent ge = new GameEvent(eventname);

    for (i = 1 ; i < tokens.length ; ++i) 
    {
	    ge.AddVariable(et.GetVarName(i-1),tokens[i]);
    }

    //    System.out.println("Event Parsed: ");
    //    Iterator<String> keysetit = ge.GetKeys().iterator();
    //    while (keysetit.hasNext())
    //    {
    //	    String key = (String)keysetit.next();
    //      	    System.out.println("   Key: " + key + ": " + ge.GetVarValue(key));
    //    }

    if (eventname.equals("RESET"))
    {
      ResetWindows();
      return;
    }

    if (eventname.equals("ERROR"))
    {
      ShowDebug("Server Error shouldn't get here! " + ge.GetVarValue("messagetext"));
      return;
    }

    SendEventToWindows(ge);
  }

  public GameEventType GetEventType(String i_evName)
  {
    return m_EventTypes.get(i_evName);
  }

  public GameEventType CreateEventType(String i_evName)
  {
    GameEventType et = new GameEventType(i_evName);
    m_EventTypes.put(i_evName,et);
    return et;
  }

  public void AddWindow(GameNode i_Window)
  {
    m_Windows.add(i_Window);
  }

  public void RealizeWindows()
  {
    int i;
    for (i = 0 ; i < m_Windows.size() ; ++i)
    {
	    GameNode node = m_Windows.get(i);
	    node.refresh();
    }
  }

  public ActionTransferManager GetActionTransferManager()
  {
    return m_ActionManager;
  }

  public TabbedWindow GetTabbedWindow()
  {
    return m_tabs;

  }

  public GameGui(PrintWriter i_pw, StatusWindows i_StatusWindows, TabbedWindow i_tabs, ClassLoader i_remoteClassLoader,
                String i_namespace)
  {
    m_remoteClassLoader = i_remoteClassLoader;
    m_StatusWindows = i_StatusWindows;
    m_tabs = i_tabs;
    m_Namespace = i_namespace;
    m_ActionManager = new ActionTransferManager();
    m_ostream = i_pw;
    m_EventTypes = new HashMap<String,GameEventType>();
    m_Windows = new ArrayList<GameNode>();
    m_eventvarnames = new AttributeChecker();

    // special events for Action handling.
    m_eventvarnames.AddAttribute("eventname",false);

    GameEventType et = this.CreateEventType("NEWSTATE");
    et.AddVar("statename");
    et.AddVar("statedesc");
    m_eventvarnames.AddAttribute("statename",false);
    m_eventvarnames.AddAttribute("statedesc",false);

    et = this.CreateEventType("LEGALACTION");
    et.AddVar("actionname");
    m_eventvarnames.AddAttribute("actionname",false);

    et = this.CreateEventType("RESET");

    et = this.CreateEventType("MESSAGE");
    et.AddVar("messagetext");
    m_eventvarnames.AddAttribute("messagetext",false);

    et = this.CreateEventType("ERROR");
    et.AddVar("errortext");
    m_eventvarnames.AddAttribute("errortext",false);
  }

  public AttributeChecker GetAttributeChecker()
  {
    return m_eventvarnames;
  }
}







