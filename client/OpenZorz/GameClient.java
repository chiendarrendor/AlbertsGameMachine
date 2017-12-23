package OpenZorz;

public class GameClient {
  static int m_port = 4356;

  static final int fixArgs = 1;
  static final int optArgs = 2;
  static final int hostArg = 0;
  static final int userArg = 1;
  static final int passArg = 2;
  static final String usage = "Usage: java GameClient hostname [username password]";

  public static void main(String argv[])
  {
    if (argv.length != fixArgs && argv.length != fixArgs + optArgs) {
      System.err.println(usage);
      System.exit(1);
    }

    String hostName = argv[hostArg];
    String user = null;
    String pass = null;
    if (argv.length == fixArgs + optArgs)
    {
      user = argv[userArg];
      pass = argv[passArg];
    }

    System.out.println("Server is: " + hostName);

    ClientManager cm = new ClientManager(hostName,m_port,user,pass);
    cm.mainloop();
  }
}



