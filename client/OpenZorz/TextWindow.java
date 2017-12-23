package OpenZorz;

import javax.swing.*;
import java.awt.*;


public class TextWindow
{
  private JFrame m_frame;
  private JTextArea m_textarea;
  public TextWindow(String i_Name)
  {
    m_frame = new JFrame(i_Name);
    try
    {
      m_frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    } 
    catch (java.security.AccessControlException ace)
    {
    }
    
    m_textarea = new JTextArea();
    m_textarea.setLineWrap(true);
    m_textarea.setWrapStyleWord(true);
    m_textarea.setEditable(false);

    JScrollPane areaScrollPane = new JScrollPane(m_textarea);
    areaScrollPane.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
    areaScrollPane.setPreferredSize(new Dimension(300,500));
    m_frame.getContentPane().add(areaScrollPane);
    m_frame.setVisible(true);
    m_frame.pack();
  }

  public void dispose()
  {
    m_frame.dispose();
  }
  
  public void append(String i_string)
  {
    m_textarea.append(i_string);
  }
}
