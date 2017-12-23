package ROOMGUI;

import NodeInterfacePackage.*;
import javax.swing.JPanel;
import javax.swing.JLabel;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import javax.swing.BorderFactory;
import javax.swing.border.Border;
import java.awt.Color;
import java.util.HashSet;
import java.util.Iterator;

public class RoomWidget extends JPanel 
  implements ActionListener,ActionGenerator,ActionVariableGenerator
{
  String m_name;
  String m_iam;
  ActionTransferManager m_atm;
  JLabel m_gamelabel;
  JLabel m_statelabel;
  JLabel m_playerslabel;
  JButton m_gobutton;
  static final String s_varname = "TargetRoom";
  static final String s_actname = "CHANGEROOM";
  HashSet<String> m_players;

  public RoomWidget(String i_name,ActionTransferManager i_atm,String i_iam)
  {
    super();

    setBorder(BorderFactory.createLineBorder(Color.black));

    m_name = i_name;
    m_atm = i_atm;
    m_iam = i_iam;
    m_players = new HashSet<String>();

    m_atm.AddVariableGenerator(s_varname,this);
    m_atm.AddButtonPresser(s_actname,this);

    setLayout(new BoxLayout(this,BoxLayout.Y_AXIS));
    JLabel namelabel = new JLabel(i_name);
    add(namelabel);

    m_gamelabel = new JLabel("no game");
    add(m_gamelabel);

    m_statelabel = new JLabel("no state");
    add(m_statelabel);
    
    m_playerslabel = new JLabel("Players: ");
    add(m_playerslabel);
    
    m_gobutton = new JButton("Enter");
    m_gobutton.addActionListener(this);
    add(m_gobutton);
  }

  public void HandleEvent(GameEvent i_event)
  {
    boolean nameChange=false;

    if (i_event.GetVarValue("eventname").equals("GUIROOM"))
    {
      String gamename = i_event.GetVarValue("gamename");
      if (gamename.equals("")) gamename = "no game";

      String gamestate = i_event.GetVarValue("gamestate");
      if (gamestate.equals("")) gamestate = "no state";

      m_gamelabel.setText(gamename);
      m_statelabel.setText(gamestate);
    }
    if (i_event.GetVarValue("eventname").equals("INHABITANT"))
    {
      nameChange = true;
      if (i_event.GetVarValue("roomname").equals(m_name))
      {
        m_players.add(i_event.GetVarValue("inhabitantname"));
        if (m_iam != null && i_event.GetVarValue("inhabitantname").equals(m_iam))
        {
          setBorder(BorderFactory.createLineBorder(Color.green));
        }
      }
      else
      {
        m_players.remove(i_event.GetVarValue("inhabitantname"));
        if (m_iam != null && i_event.GetVarValue("inhabitantname").equals(m_iam))
        {
          setBorder(BorderFactory.createLineBorder(Color.black));
        }
      }
    }

    if (i_event.GetVarValue("eventname").equals("DROPINHABITANT"))
    {
      nameChange = true;
      m_players.remove(i_event.GetVarValue("inhabitantname"));
    }

    if (nameChange)
    {
      StringBuffer sb = new StringBuffer();
      sb.append("<HTML>Players:<ul>");
      Iterator<String> nameit = m_players.iterator();
      while(nameit.hasNext())
      {
        sb.append("<li>");
        sb.append(nameit.next());
        sb.append("</li>");
      }
      sb.append("</ul></html>");
      m_playerslabel.setText(sb.toString());
    }

  }
  
  public void detach()
  {
  }

  // ActionListener (swing calls this when button is pushed)
  public void actionPerformed(ActionEvent e)
  {
    m_atm.ReportNewValueToConsumers(s_varname,m_name);
    m_atm.ButtonPressed(s_actname);
  }

  // ActionVariableGenerator
  public void ActionVariableReset()
  {
  }

  public String ActionVariableGetValue()
  {
    return null;
  }

  // ActionGenerator
  public void ActionLegalityChanged(boolean i_islegal)
  {
  }

  public void ActionReadinessChanged(boolean i_isready)
  {
  }
}

