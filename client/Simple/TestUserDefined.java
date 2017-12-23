package Simple;
import java.util.*;
import javax.swing.*;
import java.awt.*;

import NodeInterfacePackage.*;

public class TestUserDefined implements UserDefinedInterface
{
  private JPanel m_panel;
  private Color m_startcolor;
  public TestUserDefined(HashMap<String,String> i_Options,String i_dataloc,
                         JPanel i_Panel,ActionTransferManager i_atm)
  {
    m_panel = new JPanel();
    i_Panel.add(m_panel);

    m_startcolor = i_Panel.getBackground();

    m_panel.setLayout(new GridLayout(1,10,10,10));

    m_panel.add(new JButton("Hello World"));

    Iterator<String> it = i_Options.keySet().iterator();

    while(it.hasNext())
    {
      String nextkey = (String)it.next();

      m_panel.add(new JButton(nextkey + " = " +  i_Options.get(nextkey)));
    }
  }

  public void HandleEvent(GameEvent i_event)
  {
    if (i_event.GetVarValue("eventname").equals("CHANGECOLOR") &&
        i_event.GetVarValue("color") != null )
    {
      try
      {
        Color c = Color.decode(i_event.GetVarValue("color"));
        m_panel.setBackground(c);
      }
      catch(NumberFormatException nfe)
      {
        System.out.println("TUD HandleEvent: " + nfe.toString());
      }
    }
  }

  public void Refresh()
  {
  }

  public void Reset()
  {
    m_panel.setBackground(m_startcolor);
  }
}
