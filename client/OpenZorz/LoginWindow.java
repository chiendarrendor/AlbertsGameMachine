package OpenZorz;


import javax.swing.*;
import java.awt.*;
import java.awt.event.*;


public class LoginWindow implements ActionListener
{
  JFrame m_loginFrame;
  JTextField m_namefield;
  JPasswordField m_passfield;
  JButton m_okbutton;
  JLabel m_status;
  boolean m_okpressed;
  String m_namestring;
  String m_passstring;

  public JFrame GetFrame()
  {
    return m_loginFrame;
  }

  public class LoginData
  {
    public String m_login;
    public String m_password;
  }

  public void dispose()
  {
    m_loginFrame.dispose();
  }

  LoginWindow() 
  {
    try {
      javax.swing.SwingUtilities.invokeAndWait(new Runnable()
        { public void run()
          {
            MakeLoginWindow();
          }
        });
    }
    catch(InterruptedException ie) {
	    ie.printStackTrace();
    }  catch(java.lang.reflect.InvocationTargetException ite) {
	    ite.printStackTrace();
    }

  }

  void Deactivate()
  {
    m_loginFrame.setVisible(false);
  }

  void SetStatus(final String i_Status)
  {	
    try {
      javax.swing.SwingUtilities.invokeAndWait(new Runnable()
        { public void run()
          {
            m_status.setText(i_Status);
          }
        });
    }
    catch(InterruptedException ie) {
	    ie.printStackTrace();
    }  catch(java.lang.reflect.InvocationTargetException ite) {
	    ite.printStackTrace();
    }
  }





  LoginData GetLogin()
  {
    m_okpressed = false;
	
    m_okbutton.setEnabled(true);
    
    while(true)
    {
    	synchronized(this) { if (m_okpressed) break; }
    }
    
    LoginData ld = new LoginData();
    ld.m_login = m_namestring;
    ld.m_password = m_passstring;
    return ld;
  }



  private void MakeLoginWindow()
  {
    m_loginFrame = new JFrame("Login");

    try
    {
      m_loginFrame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    } 
    catch (java.security.AccessControlException ace)
    {
    }

    m_loginFrame.setVisible(true);

    m_namefield = new JTextField(30);
    JLabel namelabel = new JLabel("Login Name:");
    JPanel npanel = new JPanel();
    npanel.setLayout(new BorderLayout());
    npanel.add(namelabel,BorderLayout.WEST);
    npanel.add(m_namefield,BorderLayout.CENTER);

    m_passfield = new JPasswordField(30);
    JLabel passlabel = new JLabel("Password:");
    JPanel ppanel = new JPanel();
    ppanel.setLayout(new BorderLayout());
    ppanel.add(passlabel,BorderLayout.WEST);
    ppanel.add(m_passfield,BorderLayout.CENTER);

    m_okbutton = new JButton("Login");
	
    JPanel panel = new JPanel();
    panel.setLayout(new BoxLayout(panel,BoxLayout.Y_AXIS));
    m_loginFrame.getContentPane().add(panel);
	
    panel.add(npanel);
    panel.add(ppanel);

    JPanel buttonpanel = new JPanel();
    buttonpanel.setLayout(new BorderLayout());
    buttonpanel.add(m_okbutton,BorderLayout.WEST);
    panel.add(buttonpanel);

    m_status = new JLabel("     ");
    panel.add(m_status);

    m_okbutton.addActionListener(this);
    m_loginFrame.getRootPane().setDefaultButton(m_okbutton);

    m_loginFrame.pack();
  }
    
  public void actionPerformed(ActionEvent i_event)
  {
    m_okbutton.setEnabled(false);
    m_namestring = new String(m_namefield.getText());
    m_passstring = new String(m_passfield.getPassword());
    synchronized(this) { m_okpressed = true; }
  }
}

