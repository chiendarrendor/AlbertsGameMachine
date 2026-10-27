package NodeInterfacePackage;

import org.json.JSONObject;
import org.json.JSONException;
import java.util.Set;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import org.doghouse.jsonpath.JsonPath;

public class GuiPacketParser
{
  // for now, we're assuming that the room we're standing in only holds
  // one game, so if it's not login or roomgui, it must be for a specific game.
  public enum GuiType { LOGIN, ROOMGUI, GAMEGUI };

  private GuiType m_type;

  private JSONObject m_rawbody;

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
  private String m_uuid;

  private String m_statename;
  private String m_statedesc;
  private Set<String> m_legalactions = new HashSet<>();

  private String m_error;
  private JSONObject m_body;
  private ActionContentType m_actionContentType;
  private Map<String,Integer> m_sequencemap;

  public GuiType getGuiType() { return m_type; }

  // login methods
  public String getLoginConnectString() { return m_loginconnect; }
  public boolean isLoginAuthenticated() { return m_loginauthenticated; }
  public int getLoginOtherConnectionCount() { return m_loginotherconnections; }
  public String getLoginError() { return m_loginerror; }

  // regular packet methods
  public String getErrorMessage() { return m_error; }
  public String getMyName() { return m_me; }
  public String getGameName() { return m_gamename; }
  public String getXMLLoc() { return m_xmlloc; }
  public String getXMLFile() { return m_xmlfile; }
  public String getStateName() { return m_statename; }
  public String getStateDesc() { return m_statedesc; }
  public String getUUID() { return m_uuid; }
  public boolean hasLegalAction(String actionName) { return m_legalactions.contains(actionName); }
  public JSONObject getBody() { return m_body; }
  public JSONObject getRawBody() { return m_rawbody; } // this returns the JSONified version of the whole body
  public ActionContentType getActionContentType() { return m_actionContentType; }
  public Map<String,Integer> getSequenceMap() { return m_sequencemap; }

  // this class instanciates repeatedly to parse incoming JSON objects, so our
  // JsonPath objects must compile before the first constructor call.
  private static JsonPath guipath = JsonPath.compile("gui");

  private static JsonPath loginconnectpath = JsonPath.compile("connectstring");
  private static JsonPath loginauthenticatedpath = JsonPath.compile("authenticated");
  private static JsonPath loginotherconnectionspath = JsonPath.compile("otherconnections");
  private static JsonPath loginerrorpath = JsonPath.compile("error");

  private static JsonPath recipientpath = JsonPath.compile("recipient");
  private static JsonPath xmllocpath = JsonPath.compile("XMLLoc");
  private static JsonPath xmlfilepath = JsonPath.compile("XMLFile");
  private static JsonPath statenamepath = JsonPath.compile("fullState.state");
  private static JsonPath statedescpath = JsonPath.compile("fullState.stateDesc");
  private static JsonPath legalactionpath = JsonPath.compile("fullState.legalActions[]");
  private static JsonPath errorpath = JsonPath.compile("fullState.error");
  private static JsonPath bodypath = JsonPath.compile("fullState.body");
  private static JsonPath uuidpath = JsonPath.compile("fullState.gameInstanceId");
  private static JsonPath fullStateTypePath = JsonPath.compile("fullState.type");
  private static JsonPath sequencespath = JsonPath.compile("fullState.sequences{}");


  @SuppressWarnings("unchecked")
  public GuiPacketParser(String i_string)
  {
    // this throws a JSONException if it's not properly formed JSON, which is appropriate.
    m_rawbody = new JSONObject(i_string);

    String gui = (String)guipath.evaluate(m_rawbody);
    if (gui == null) throw new JSONException("packet has no gui field");
    

    if (gui.equals("LOGIN")) {
      m_type = GuiType.LOGIN;
      m_loginconnect = (String)loginconnectpath.evaluate(m_rawbody);
      m_loginauthenticated = (Boolean)loginauthenticatedpath.evaluate(m_rawbody); 
      m_loginotherconnections = (Integer)loginotherconnectionspath.evaluate(m_rawbody); 
      m_loginerror = loginerrorpath.evaluateString(m_rawbody, null);
    } else {
      m_type = gui.equals("ROOMGUI") ? GuiType.ROOMGUI : GuiType.GAMEGUI;
      m_gamename = gui;
      m_me = (String)recipientpath.evaluate(m_rawbody);
      m_xmlloc = (String)xmllocpath.evaluate(m_rawbody);
      m_xmlfile = (String)xmlfilepath.evaluate(m_rawbody);
      m_statename = (String)statenamepath.evaluate(m_rawbody);
      m_statedesc = (String)statedescpath.evaluate(m_rawbody);
      m_uuid = (String)uuidpath.evaluate(m_rawbody);
      m_sequencemap = (Map<String,Integer>)sequencespath.evaluate(m_rawbody);

      m_actionContentType = ActionContentType.getType((String)fullStateTypePath.evaluate(m_rawbody));

      List<String> legalActions = legalactionpath.evaluateStringList(m_rawbody);

      for (String legalAction : legalActions) {
        m_legalactions.add(legalAction);
      }

      m_error = errorpath.evaluateString(m_rawbody, null);
      m_body = (JSONObject)bodypath.evaluate(m_rawbody, null);
    }
  }
}
             
  
