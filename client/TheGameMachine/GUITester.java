package TheGameMachine;

import NodeInterfacePackage.GuiPacketParser;

import java.io.*;


public class GUITester {
  static PrintWriter m_out;
  static BufferedReader m_in;

  static final String usage = "Usage: java GUITester.java GUIName XMLLoc XMLFile";

  public static void main(String argv[])
  {
    if (argv.length != 3)
    {
      System.err.println(usage);
      System.exit(1);
    }

    m_out = new PrintWriter(System.out,true);
    m_in = new BufferedReader(new InputStreamReader(System.in));

    try
    {
      TabbedWindow twin = new TabbedWindow(500,500,"");
      twin.RefreshMe();

      StatusWindows statuswindows = new StatusWindows(twin);
      GuiPacketParser gpp = new GuiPacketParser("dummy"); // TODO, if we ever use this again, figure out what this should parse
      GUIUnit testgui = new GUIUnit(m_out,statuswindows,twin,gpp);
      
      if (!testgui.ParseXML(argv[0],argv[1],argv[2]))
      {
        System.out.println("GUI not parsed.");
        System.exit(1);
      }
      else
      {
        System.out.println("GUI parsed");
      }

      // TODO, if we ever use this again: GUIUnit's old line-based HandleLine()
      // (comma-separated event text typed at this stdin prompt) is gone along
      // with GameGui's comma-event parsing -- driving this tester now means
      // feeding it a full GuiPacketParser via HandleGuiPacketParser(gpp)
      // instead, the same way ClientManager does for the real wire protocol.
      while(true)
      {
        String instring = m_in.readLine();
      }
    } catch (IOException ioe) {
      // I/O error
      ioe.printStackTrace();
    }
  } // main
}



