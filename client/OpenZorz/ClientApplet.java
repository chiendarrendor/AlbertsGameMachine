package OpenZorz;

import java.applet.Applet;

public class ClientApplet extends Applet implements Runnable
{
  private static int m_port = 4356;
  private ClientManager m_cm = null;
  private Thread m_thread = null;

  private String m_server;
  private String m_uname;
  private String m_pass;

  public void run()
  {
    m_cm.mainloop();
  }


  public void init() 
  {
    m_server = getParameter("gameserverloc");
    m_uname = getParameter("username");
    m_pass = getParameter("password");
  }

  public void start() 
  {
    m_cm = new ClientManager(m_server,m_port,m_uname,m_pass);

    m_thread = new Thread(this);
    m_thread.start();
  }

  public void stop() 
  {
    m_thread.interrupt();
    m_cm.dispose();
    m_cm = null;
    m_thread = null;
  }

  public void destroy()
  {
  }
  private static final long serialVersionUID = 1;

}
