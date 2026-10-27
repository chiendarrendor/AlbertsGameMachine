package TheGameMachine;
import java.util.*;
import java.io.*;

import NodeInterfacePackage.*;
import DOMUtilities.*;
import org.json.JSONObject;

public class GameGui implements ActionSender
{
  private ClassLoader m_remoteClassLoader;
  private ArrayList<GameNode> m_Windows;
  private ActionTransferManager m_ActionManager;
  private SequenceManager m_SequenceManager;
  private ExternalAssets m_ExternalAssets;
  private PrintWriter m_ostream;
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

  // called when a game instance's UUID changes under an already-open GUI
  // (a new game replacing a finished one in the same room) -- the widgets
  // need to drop whatever per-instance state they're holding before the
  // first full state of the new instance arrives.
  public void Reset()
  {
    ResetWindows();
  }

  // The 5 reserved room-management actions live in namespaces the server
  // hardcodes ("roommanager"/"room" -- see RoomManager.cpp/Room::HandleAction)
  // regardless of which GameGui instance (a real game, or the shared ROOMGUI
  // instance) actually fires them -- so this split has to be hardcoded
  // client-side too, mirroring the server exactly, rather than sourced from
  // this instance's own identity.
  private String ReservedNamespaceFor(String i_ActionName)
  {
    if (i_ActionName.equals("NEWROOM") || i_ActionName.equals("CHANGEROOM"))
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

  public void SendAction(String i_ActionName, JSONObject i_Params) { 
    SendAction(i_ActionName, i_Params, ActionContentType.TRANSITION); 
  }
  
  public void SendAction(String i_ActionName, JSONObject i_Params, ActionContentType i_ContentType) 
  {
    String namespace = ReservedNamespaceFor(i_ActionName);
    if (namespace == null)
    {
      namespace = m_Namespace;
    }

    JSONObject envelope = new JSONObject();
    envelope.put("namespace",namespace);
    envelope.put("action",i_ActionName);
    // Every action sent today names a transition -- "sequence" is the only
    // other value, used by the (not yet built) sequence range-fetch request,
    // which doesn't go through this generic ActionNode-driven call site.
    envelope.put("type",i_ContentType.getName());
    envelope.put("params",i_Params);

    m_ostream.print(envelope.toString());
    m_ostream.print("\n");
    m_ostream.flush();
  }

  public void HandleFullState(GuiPacketParser gpp) {
    GameEvent ge = new GameEvent(gpp);
    m_SequenceManager.update(gpp);
    SendEventToWindows(ge);
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

  public ExternalAssets GetExternalAssets()
  {
    return m_ExternalAssets;
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
    m_SequenceManager = new SequenceManager(this);
    m_ExternalAssets = new ExternalAssets(m_ActionManager, m_SequenceManager);
    m_ostream = i_pw;
    m_Windows = new ArrayList<GameNode>();
  }
}







