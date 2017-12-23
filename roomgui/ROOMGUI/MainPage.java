package ROOMGUI;

import NodeInterfacePackage.*;
import javax.swing.JPanel;
import java.awt.BorderLayout;
import java.util.HashMap;

public class MainPage implements UserDefinedInterface
{
  TalkPanel m_talkpanel;


  public MainPage(HashMap<String,String> i_Options,String i_DataLoc,
                  JPanel i_Panel,ActionTransferManager i_atm)
  {
    i_Panel.setLayout(new BorderLayout());
    m_talkpanel = new TalkPanel(i_Panel,i_atm);
  }

  public void HandleEvent(GameEvent i_event)
  {
    m_talkpanel.HandleEvent(i_event);
  }

  public void Refresh()
  {
  }

  public void Reset()
  {
  }
}
