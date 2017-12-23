package OpenZorz;


//import javax.swing.JTextArea;
//import javax.swing.JScrollPane;
//import javax.swing.JScrollBar;

import javax.swing.JOptionPane;

public class StatusWindows
{
  private TabbedWindow m_tabs;
  //  private JTextArea m_StatusText;
  //  private JScrollPane m_jsp;

  public StatusWindows(TabbedWindow i_tabs)
  {
    m_tabs = i_tabs;

    //    m_StatusText = new JTextArea();
    //    m_StatusText.setLineWrap(true);
    //    m_StatusText.setWrapStyleWord(true);
    //    m_StatusText.setEditable(false);
    //    m_jsp = new JScrollPane(m_StatusText);
    //    i_tabs.addTab("Server Errors",m_jsp,false);
  }

  public void ShowDebug(String i_message)
  {
    JOptionPane.showMessageDialog(null,i_message,"Error",JOptionPane.ERROR_MESSAGE);

    //    m_StatusText.append(i_message + "\n");
    //    JScrollBar jsb = m_jsp.getVerticalScrollBar();
    //    jsb.setValue(jsb.getMaximum());
    //    m_tabs.Alert(m_jsp);
  }
  
  public TabbedWindow GetTabbedWindow() { return m_tabs; }
}
