package TheGameMachine;

import java.io.*;
import java.net.*;
import Utilities.StringUtility;
import javax.swing.JOptionPane;
import org.json.JSONObject;


public class ClientManager
{
  volatile Socket m_connSocket = null;
  PrintWriter m_out;
  BufferedReader m_in;
  StatusWindows m_statuswindows = null;
  TabbedWindow m_tabs = null;
  GUIUnitMap m_gum;
  LoginWindow m_lwindow = null;

  String m_HostName;
  int m_Port;
  String m_extUserName;
  String m_extPassword;

  boolean m_exiting;

  class HandleLine implements GUIUnitMap.GUIUnitFunctor
  {
    private String m_s;
    public HandleLine(String i_s)
    {
      m_s = i_s;
    }

    public void Do(GUIUnit i_guiunit)
    {
      i_guiunit.HandleLine(m_s);
    }
  }


  public ClientManager(String i_HostName,int i_Port,String i_extUserName,String i_extPassword)
  {
    m_HostName = i_HostName;
    m_Port = i_Port;
    m_extUserName = i_extUserName;
    m_extPassword = i_extPassword;
    m_gum = new GUIUnitMap();
    m_exiting = false;

    String topdloc = "https://" + i_HostName;

    m_tabs = new TabbedWindow(550,550,topdloc);
    m_tabs.RefreshMe();
    m_statuswindows = new StatusWindows(m_tabs);
  }

  public void mainloop()
  {
    try
    {
      ConnectSocket(m_HostName,m_Port);
      
      LoginProcess(m_extUserName, m_extPassword);

      System.out.println("Login Process Completed");
      while(!m_exiting) {}

      while(true)
      {
        HandleOneLine();
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
    m_gum.dispose();
  }

  private void ConnectSocket(String i_HostName,int i_Port)
  {
    try
    {
	    m_connSocket = new Socket(i_HostName,i_Port);

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





  private void HandleOneLine()
  {
    String instring;
    try
    {
      instring = m_in.readLine();
    }
    catch(IOException ioe)
    {
      if (!m_exiting)
      {
        throw new ClientManagerException("Error in reading line: " + ioe);
      }
      else
      {
        return;
      }
    }

    if (instring == null)
    {
      throw new ClientManagerException("Server Closed Connection.");
    }

    GuiPacketParser gpp = new GuiPacketParser(instring);
    GUIUnit opguiunit;

    switch(gpp.GetGuiType())
    {
    case GuiPacketParser.NOTGUI:
      HandleLine hl = new HandleLine(instring);
      m_gum.ForEach(hl);
      break;
    case GuiPacketParser.SERVERERROR:
      m_statuswindows.ShowDebug("Error: " + gpp.GetText());
      break;
    case GuiPacketParser.MESSAGE:
      m_statuswindows.ShowDebug(gpp.GetText());
      break;
    case GuiPacketParser.RESETGUI:
      opguiunit = m_gum.Get(gpp.GetGuiName());
      if (opguiunit == null)
      {
        m_statuswindows.ShowDebug("Can't find GUI " + gpp.GetGuiName() + " to reset.");
      }
      else
      {
        opguiunit.HandleLine("RESET");
      }
      break;
    case GuiPacketParser.DROPGUI:
      opguiunit = m_gum.PullFromMap(gpp.GetGuiName());
      if (opguiunit != null)
      {
        opguiunit.dispose();
      }
      else
      {
        m_statuswindows.ShowDebug("Can't Find Gui to Drop: " + gpp.GetGuiName());
      }
      break;
    case GuiPacketParser.NEWGUI:
      opguiunit = null;
      if (!m_gum.IsInMap(gpp.GetGuiName()))
      {
        opguiunit = new GUIUnit(m_out,m_statuswindows,m_tabs,gpp);
        m_gum.AddToMap(gpp.GetGuiName(),opguiunit);
        System.out.println("New Gui Created: " + gpp.GetGuiName());
        ParseXML(opguiunit,gpp);
      }
      else
      {
        m_statuswindows.ShowDebug("We already have a gui with name: " + gpp.GetGuiName());
      }
      break;
    case GuiPacketParser.ADDGUI:
      opguiunit = m_gum.Get(gpp.GetGuiName());
      if (opguiunit != null)
      {
        ParseXML(opguiunit,gpp);
      }
      else
      {
        m_statuswindows.ShowDebug("Can't find gui with name: " + gpp.GetGuiName());
      }
      break;
    default:
      m_statuswindows.ShowDebug("Bad Gui-level packet: " + instring);
      break;
    }
  }

  private void ParseXML(GUIUnit i_gu,GuiPacketParser i_gpp)
  {
    if (!i_gu.ParseXML(i_gpp.GetGuiName(),i_gpp.GetXMLLoc(),i_gpp.GetXMLName()))
    {
      m_statuswindows.ShowDebug("GUI " + i_gpp.GetGuiName() + " XML not parsed");
    }
    else
    {
      System.out.println("GUI " + i_gpp.GetGuiName() + " XML successfully parsed");
    }
  }


}




    
