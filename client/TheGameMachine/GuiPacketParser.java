package TheGameMachine;

import org.json.JSONObject;
import org.json.JSONException;
import org.json.JSONArray;

class GuiPacketParser
{
  // for now, we're assuming that the room we're standing in only holds
  // one game, so if it's not login or roomgui, it must be for a specific game.
  public enum GuiType { LOGIN, ROOMGUI, GAMEGUI };

  private GuiType m_type;

  // data for login:
  private String m_loginconnect;
  private boolean m_loginauthenticated;
  private int m_loginotherconnections;
  private String m_loginerror;

  // data for everyone else;
  private String m_me;
  private String m_gamename;
  private String m_xmlloc;
  private String m_xmlfile;

  private String m_statename;
  private String m_statedesc;
  private String[] m_legalactions;

  private boolean m_isError;
  private String m_error;
  private JSONObject m_body;

  public GuiType getGuiType() { return m_type; }

  // login methods
  public String getLoginConnectString() { return m_loginconnect; }
  public boolean isLoginAuthenticated() { return m_loginauthenticated; }
  public int getLoginOtherConnectionCount() { return m_loginotherconnections; }
  public String getLoginError() { return m_loginerror; }

  // regular packet methods
  public boolean isErrorMessage() { return m_isError; }
  public String getErrorMessage() { return m_error; }
  public String getMyName() { return m_me; }
  public String getGameName() { return m_gamename; }
  public String getXMLLoc() { return m_xmlloc; }
  public String getXMLFile() { return m_xmlfile; }
  public String getStateName() { return m_statename; }
  public String getStateDesc() { return m_statedesc; }
  public String[] getLegalActions() { return m_legalactions; }
  public JSONObject getBody() { return m_body; }


  boolean good(JSONObject thing, String name) { return thing.has(name) && !thing.isNull(name); }

  public GuiPacketParser(String i_string)
  {
    // this throws a JSONException if it's not properly formed JSON, which is appropriate.
    JSONObject packetJSON = new JSONObject(i_string); 

    if (!packetJSON.has("gui") || packetJSON.isNull("gui")) throw new JSONException("packet has no gui field");
    String gui = packetJSON.getString("gui");

    if (gui.equals("LOGIN")) {
      m_type = GuiType.LOGIN;
      m_loginconnect = packetJSON.getString("connectstring");
      m_loginauthenticated = packetJSON.getBoolean("authenticated");
      m_loginotherconnections = packetJSON.getInt("otherconnections");
      m_loginerror = good(packetJSON,"error") ?  packetJSON.getString("error") : null;
    } else {
      m_type = gui.equals("ROOMGUI") ? GuiType.ROOMGUI : GuiType.GAMEGUI;
      m_gamename = gui;
      m_me = packetJSON.getString("recipient");
      m_xmlloc = packetJSON.getString("XMLLoc");
      m_xmlfile = packetJSON.getString("XMLFile");

      JSONObject fullState = packetJSON.getJSONObject("fullState");
      m_statename = fullState.getString("state");
      m_statedesc = fullState.getString("stateDesc");

      JSONArray legalActionJSON = fullState.getJSONArray("legalActions");
      m_legalactions = new String[legalActionJSON.length()];
      for (int i = 0 ; i < legalActionJSON.length() ; ++i) {
        m_legalactions[i] = legalActionJSON.getString(i);
      }

      m_error = good(fullState,"error") ? fullState.getString("error") : null;
      m_body = good(fullState, "body" ? fullState.getJSONObject("body") : null;
    }
  }


  // these are no longer valid, but I want to be able to compile this for now:
  public String GetXMLLoc() { return null; }
  public String GetGuiName() { return null; }
  public String GetXMLName() { return null; }
  public static final int RESETGUI = -7;
  public static final int DROPGUI = -6;
  public static final int MESSAGE = -5;
  public static final int ADDGUI = -4;
  public static final int NEWGUI = -3;
  public static final int NOTGUI = -2;
  public static final int SERVERERROR = -1;
  public String GetText() { return null; }
  public int GetGuiType() { return -1; }
}
             
  
