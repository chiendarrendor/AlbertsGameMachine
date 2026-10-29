package TheGameMachine;

import java.io.*;
import java.net.*;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import Utilities.StringUtility;
import javax.swing.JOptionPane;
import org.json.JSONObject;
import NodeInterfacePackage.GuiPacketParser;
import NodeInterfacePackage.ActionContentType;
import org.doghouse.jsonpath.JsonPath;


public class ClientManager
{
  volatile Socket m_connSocket = null;
  PrintWriter m_out;
  BufferedReader m_in;
  StatusWindows m_statuswindows = null;
  TabbedWindow m_tabs = null;
  GUIUnit m_rooms = null;
  GUIUnit m_game = null;
  String m_gamename = null;
  String m_currentroom = null;
  String m_gameuuid = null;
  LoginWindow m_lwindow = null;

  String m_HostName;
  int m_Port;
  String m_extUserName;
  String m_extPassword;

  boolean m_exiting;

  public ClientManager(String i_HostName,int i_Port,String i_extUserName,String i_extPassword)
  {
    m_HostName = i_HostName;
    m_Port = i_Port;
    m_extUserName = i_extUserName;
    m_extPassword = i_extPassword;
    m_exiting = false;

    String topdloc = "https://" + i_HostName;

    m_tabs = new TabbedWindow(550,550,topdloc);
    m_tabs.RefreshMe();
    m_statuswindows = new StatusWindows(m_tabs);

    // TabbedWindow (like every WindowNode-created frame) is deliberately
    // DO_NOTHING_ON_CLOSE, same as a popped-out tab's own frame (see
    // TabbedWindow.WindowWatcher) -- but unlike those, this is the one true
    // main window, so closing it needs to actually end the application:
    // clean up (close the socket, dispose every sub-window) via dispose(),
    // then exit. This used to be handled, by accident, by a vestigial
    // JFrame left over from the pre-Phase-3 Applet days (ClientApplication's
    // "Test" window, EXIT_ON_CLOSE but otherwise unused) -- now removed, so
    // this is the real mechanism instead.
    m_tabs.addWindowListener(new WindowAdapter()
    {
      public void windowClosing(WindowEvent i_event)
      {
        dispose();
        System.exit(0);
      }
    });
  }

  public void mainloop()
  {
    try
    {
      ConnectSocket(m_HostName,m_Port);
      
      LoginProcess(m_extUserName, m_extPassword);

      System.out.println("Login Process Completed");

      while(HandleOneLine())
      {
      }
    }
    catch(ClientManagerException cme)
    {
      m_statuswindows.ShowDebug("Fatal Client Error:" + cme);
    }
  }

  public void dispose()
  {
    try
    {
      m_exiting = true;
      if (m_connSocket != null) m_connSocket.close();
    }
    catch(IOException ioe)
    {
    }

    if (m_tabs != null) m_tabs.dispose();
    if (m_lwindow != null) m_lwindow.dispose();
    if (m_rooms != null) m_rooms.dispose();
    if (m_game != null) m_game.dispose();
  }

  private void ConnectSocket(String i_HostName,int i_Port)
  {
    try
    {
	    m_connSocket = new Socket(i_HostName,i_Port);
	    // Nothing else on this connection is periodic -- without this, a
	    // NAT/middlebox between client and server (irrelevant back when
	    // server and client always ran on the same machine) can silently
	    // drop a long-idle connection with neither side ever sending a
	    // real FIN/RST, see the "Connection reset" investigation, 2026-10-25.
	    m_connSocket.setKeepAlive(true);

	    m_out = new PrintWriter(m_connSocket.getOutputStream(), true);
	    m_in = new BufferedReader(new InputStreamReader(m_connSocket.getInputStream()));
    }
    catch(java.net.UnknownHostException uhe)
    {
      throw new ClientManagerException("Unknown Host in connection to " + i_HostName + ", " + i_Port + ": " + uhe);
    }
    catch(java.io.IOException ioe)
    {
      throw new ClientManagerException("IO Exception in connection to " + i_HostName + ", " + i_Port + ": " + ioe);
    }
  }

/*
  FSM for the Login Process:
  Initial: receive initial LOGIN packet
    a. if there is an external username and password, send them and go to HandleAuthenticate
    b. if there is not, go to RequestLogin
  RequestLogin: Raise Login Panel if it doesn't exist, wait for username/password, send them, and go to HandleAuthenticate
  HandleAuthenticate: receive login packet.  
    a. if authenticated = true and otherconnections = 0 go to login complete
    b. if authenticated = true and otherconnections > 0 go to  ElectLogoutOther
    c. if authenticated = false: set error in Login Panel (may have to pend this if LoginPanel doesn't exist yet) and go to RequestLogin
  ElectLogoutOther: raise logout-other confirm dialog and wait for response
    a. yes: send LOGOUTOTHER message and goto HandleAuthenticate
    b. no: throw exception of of login process (which should probably exit the app)
*/

  private enum LoginState{ LoginInitial, LoginRequestLogin, LoginHandleAuthenticate, LoginElectLogoutOther, LoginComplete }

  private GuiPacketParser getLoginMessage() {
    GuiPacketParser result = null;
    try {
      result = new GuiPacketParser(m_in.readLine());
    } catch (IOException ioe) {
      throw new ClientManagerException(ioe.toString());
    }
    if (result.getGuiType() != GuiPacketParser.GuiType.LOGIN) throw new ClientManagerException("Invalid LOGIN message");
    return result;
  }

  private void SendLoginString(String i_username,String i_password)
  {
    JSONObject params = new JSONObject();
    params.put("username",i_username);
    params.put("password",StringUtility.Mangle(i_password));

    JSONObject envelope = new JSONObject();
    envelope.put("namespace","login");
    envelope.put("action","AUTHENTICATE");
    envelope.put("type","transition");
    envelope.put("params",params);

    m_out.print(envelope.toString());
    m_out.print("\n");
    m_out.flush();
  }

  private void SendLogoutOther() {
    JSONObject params = new JSONObject();
    JSONObject envelope = new JSONObject();
    envelope.put("namespace","login");
    envelope.put("action","LOGOUTOTHER");
    envelope.put("type","transition");
    envelope.put("params",params);

    m_out.print(envelope.toString());
    m_out.print("\n");
    m_out.flush();
  }




  private void LoginProcess(String i_extUserName, String i_extPassword) {
    String errorMessage = null;
    LoginState curstate = LoginState.LoginInitial;
    GuiPacketParser gpp = null;

    while(true) {
      switch (curstate) {
        case LoginState.LoginInitial:
          gpp = getLoginMessage();
          System.out.println("Welcomed: " + gpp.getLoginConnectString());
          if (i_extUserName != null && i_extPassword != null) {
            SendLoginString(i_extUserName, i_extPassword);
            curstate = LoginState.LoginHandleAuthenticate;
          } else {
            curstate = LoginState.LoginRequestLogin;
          }
          break;

        case LoginState.LoginRequestLogin:
          if (m_lwindow == null) {
            m_lwindow = new LoginWindow();
          }
          m_lwindow.SetStatus(errorMessage);
          errorMessage = null;
          LoginWindow.LoginData ldata = m_lwindow.GetLogin();
          m_lwindow.SetStatus("Sending Login to Server");
          SendLoginString(ldata.m_login, ldata.m_password);
          curstate = LoginState.LoginHandleAuthenticate;
          break;


        case LoginState.LoginHandleAuthenticate:
          gpp = getLoginMessage();
          if (!gpp.isLoginAuthenticated()) {
            errorMessage = gpp.getLoginError();
            curstate = LoginState.LoginRequestLogin;
            break;
          }
          // if we get here, we're authenticated and we don't need  the login window any more.
          if (m_lwindow != null) m_lwindow.Deactivate();

          if (gpp.getLoginOtherConnectionCount() == 0) {
            curstate = LoginState.LoginComplete;
            break;
          }
          // I know we _could_ fall through here...but fallthrough cases in switch statements are the devil.
          curstate = LoginState.LoginElectLogoutOther;
          break;

        case LoginState.LoginElectLogoutOther:
          int optionpaneresult = JOptionPane.showConfirmDialog(m_tabs,
                                                             "You are already logged in.  Log yourself out?",
                                                             "You are already logged in",
                                                             JOptionPane.YES_NO_OPTION,
                                                             JOptionPane.QUESTION_MESSAGE);
          if (optionpaneresult == JOptionPane.YES_OPTION) {
            SendLogoutOther();
            curstate = LoginState.LoginHandleAuthenticate;
            break;
          }

          throw new ClientManagerException("She said down!");

        case LoginState.LoginComplete:
          return;
        default:
          throw new ClientManagerException("Unknown Login State!");
      }
    }
  }

  // So, the new server send mechanism sends full-state-sends (FSS) for roomgui and 
  // for the game that is in the player's current room, if there is one.
  // There are no longer an arbitrary number of different Guis that can be handled, just 
  // two, and each one, if it exists, consumes only its own FSS packet.
  // m_rooms
  // m_game
  // m_currentroom
  // m_gamename

  // Returns false once the connection itself is dead (clean close or a
  // socket-level exception, e.g. a NAT/middlebox silently dropping a long
  // idle connection -- see the "Connection reset" investigation, 2026-10-25)
  // so mainloop() stops hammering a permanently broken socket instead of
  // immediately retrying the same failing read forever (which, since
  // ShowDebug is a modal dialog, previously manifested as the popup
  // reappearing the instant you dismissed it). Returns true for anything
  // recoverable -- a single malformed/unparseable line shouldn't end the
  // session, only the connection itself dying should.
  private boolean HandleOneLine()
  {
    String instring;
    try {
      instring = m_in.readLine();
    } catch (IOException ioe) {
      if (m_statuswindows != null) {
        m_statuswindows.ShowDebug("Connection to server lost: " + ioe);
      }
      return false;
    }

    if (instring == null) {
      if (m_statuswindows != null) {
        m_statuswindows.ShowDebug("Connection to server lost: server closed the connection");
      }
      return false;
    }

    // Server sends this periodically, purely so a NAT/middlebox sees
    // traffic and doesn't expire the connection's idle mapping -- not a
    // real gui packet, nothing to parse or dispatch.
    if (instring.equals("PING")) {
      return true;
    }

    GuiPacketParser gpp;
    try {
      gpp = new GuiPacketParser(instring);
    } catch (Exception ex) {
      if (m_statuswindows != null) {
        m_statuswindows.ShowDebug("Error: " + ex);
      }
      return true;
    }

    // if we get here, we have a valid GPP.
    if (gpp.getGuiType() == GuiPacketParser.GuiType.LOGIN) {
      m_statuswindows.ShowDebug("Warning: Why did we get a Login packet?");
      return true;
    }

    if (gpp.getErrorMessage() != null) {
      if (gpp.getActionContentType() == ActionContentType.TRANSITION) {
        m_statuswindows.ShowDebug("Error from Server: " + gpp.getErrorMessage());
      } else {
        System.out.println("Error in Sequence Request: " + gpp.getErrorMessage());
      }
      return true;
    }
    
    // if we get here, we either have a room gui or a game gui
    if (gpp.getGuiType() == GuiPacketParser.GuiType.ROOMGUI) {
      HandleRoomPacket(gpp);
    } else {
      HandleGamePacket(gpp);
    }
    return true;
  }

  private JsonPath myroomnamepath = JsonPath.compile("myroom.name");
  private JsonPath myroomgamepath = JsonPath.compile("myroom.gameName"); 


  private void HandleRoomPacket(GuiPacketParser gpp) {

    if (m_rooms == null) {
      m_rooms = new GUIUnit(m_out,m_statuswindows,m_tabs,gpp);
      System.out.println("New Room Gui Created");
      ParseXML(m_rooms,gpp);
    }

    String newRoomName = (String)myroomnamepath.evaluate(gpp.getBody());
    String newRoomGame = (String)myroomgamepath.evaluate(gpp.getBody());

    if (m_game != null && m_gamename != null && !m_gamename.equals(newRoomGame)) {
      m_game.dispose();
      m_game = null;
      m_gameuuid = null;
    }

    m_currentroom = newRoomName;
    m_gamename = newRoomGame;
    m_rooms.HandleGuiPacketParser(gpp);
  }

  private void HandleGamePacket(GuiPacketParser gpp) {
    if (m_game != null && !m_gamename.equals(gpp.getGameName())) {
      m_game.dispose();
      m_game = null;
    }
    if (m_game == null) {
      m_game = new GUIUnit(m_out, m_statuswindows, m_tabs, gpp);
      System.out.println("New Game Gui created: " + gpp.getGameName());
      ParseXML(m_game,gpp);
    }

    if (m_gameuuid != null && !m_gameuuid.equals(gpp.getUUID())) {
      m_game.Reset();
    }


    m_gameuuid = gpp.getUUID();
    m_gamename = gpp.getGameName();

    m_game.HandleGuiPacketParser(gpp);
  }

  private void ParseXML(GUIUnit i_gu,GuiPacketParser i_gpp)
  {
    if (!i_gu.ParseXML(i_gpp.getGameName(),i_gpp.getXMLLoc(),i_gpp.getXMLFile()))
    {
      m_statuswindows.ShowDebug("GUI " + i_gpp.getGameName() + " XML not parsed");
    }
    else
    {
      System.out.println("GUI " + i_gpp.getGameName() + " XML successfully parsed");
    }
  }


}




    
