package OpenZorz;

import java.io.*;
import java.net.*;
import Utilities.StringUtility;
import javax.swing.JOptionPane;


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
      GetWelcome();

      m_lwindow = null;
      DoLogin(m_extUserName,m_extPassword);

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

  private void GetWelcome()
  {
    try
    {
      String welcomestring = m_in.readLine();
      System.out.println("welcomed: " + welcomestring);
    }
    catch (IOException ioe)
    {
      throw new ClientManagerException("IO Exception in get welcomestring");
    }
  }

  private void DoLogin(String i_extUserName,String i_extPassword)
  {
    
    String res = "";

    if (i_extUserName != null && i_extPassword != null)
    {
      res = SendLogin(i_extUserName,i_extPassword);
      // null return is good...that means that we got a valid login and the Server is happy with us.
      if (res == null) return;

      res = "External Login Invalid: " + res;
    }

    m_lwindow = new LoginWindow();
    m_lwindow.SetStatus(res);

    while(true)
    {
      LoginWindow.LoginData ldata = m_lwindow.GetLogin();
      m_lwindow.SetStatus("Sending Login To Server");
      String result = SendLogin(ldata.m_login,ldata.m_password);

      if (result == null)
      {
         m_lwindow.Deactivate();
        return;
      }

      m_lwindow.SetStatus(result);
    }
  }

  private void SendLoginLikeString(String i_type,String i_username,String i_password)
  {
    m_out.print(i_type+
                "," + 
                StringUtility.UnComma(i_username) + 
                "," + 
                StringUtility.UnComma(StringUtility.Mangle(i_password)) +
                "\n");
    m_out.flush();
  }




  private String SendLogin(String i_username,String i_password)
  {
    SendLoginLikeString("LOGIN",i_username,i_password);

    try
    {
	    String linein = m_in.readLine();
	    
	    if (linein.equals("WELCOME"))
	    {    
        return null;
	    }
      else if (linein.equals("ALREADYLOGGEDIN"))
      {
        int optionpaneresult = JOptionPane.showConfirmDialog(m_lwindow != null ? m_lwindow.GetFrame() : m_tabs,
                                                             "You are already logged in.  Log yourself out?",
                                                             "You are already logged in",
                                                             JOptionPane.YES_NO_OPTION,
                                                             JOptionPane.QUESTION_MESSAGE);
        if (optionpaneresult == JOptionPane.YES_OPTION)
        {
          SendLoginLikeString("LOGOUTOTHER",i_username,i_password);
          return SendLogin(i_username,i_password);
        }

        return "Login failed. You are already logged in elsewhere.";

      }
	    else
	    {
        return linein;
	    }
    }
    catch (java.io.IOException ioe)
    {
      throw new ClientManagerException("Error Sending Login: " + ioe);
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




    
