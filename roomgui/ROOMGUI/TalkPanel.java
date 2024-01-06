package ROOMGUI;

import Utilities.StringUtility;
import NodeInterfacePackage.*;
import javax.swing.JPanel;

import javax.swing.ButtonGroup;
import javax.swing.JRadioButton;
import javax.swing.JTextArea;
import javax.swing.JTextField;
import javax.swing.JLabel;
import javax.swing.JComponent;
import java.awt.event.*;
import javax.swing.JComboBox;
import javax.swing.BorderFactory;
import java.awt.Color;
import java.util.HashSet;
import java.util.Iterator;
import java.awt.BorderLayout;
import javax.swing.BoxLayout;
import javax.swing.JScrollPane;
import javax.swing.JScrollBar;

public class TalkPanel implements ActionListener
{
  ActionTransferManager m_atm;
  JRadioButton m_room;
  JRadioButton m_player;
  JComboBox<String> m_playerlist;
  JTextField m_message;
  JLabel m_err;
  JTextArea m_messages;
  AGReporter m_roomready;
  AGReporter m_playerready;
  ActionTransferManager.Alerter m_alerter;

  HashSet<String> m_playernames;
  String m_myname;

  JScrollBar m_jsb;


  private class JRBActivator implements ItemListener
  {
    JComponent m_activatee;
    JRadioButton m_activator;

    public JRBActivator(JComponent i_activatee,JRadioButton i_activator)
    {
      m_activatee=i_activatee;
      m_activator=i_activator;
    }

    public void itemStateChanged(ItemEvent i_event)
    {
      m_activatee.setEnabled(m_activator.isSelected());
    }
  }      

  private class AGReporter implements ActionGenerator
  {
    private boolean m_isready;

    public boolean IsReady() { return m_isready; }
    
    public AGReporter()
    {
      m_isready = false;
    }

    public void ActionLegalityChanged(boolean i_islegal)
    {
    }

    public void ActionReadinessChanged(boolean i_isready)
    {
      m_isready = i_isready;
    }
  }

  private class MessageAVG implements ActionVariableGenerator
  {
    private JTextField m_message;

    public MessageAVG(JTextField i_message)
    {
      m_message = i_message;
    }

    public void ActionVariableReset()
    {
      m_message.setText("");
    }

    public String ActionVariableGetValue()
    {
      return m_message.getText();
    }
  }

  private class PListAVG implements ActionVariableGenerator
  {
    private JComboBox<String> m_plist;

    public PListAVG(JComboBox<String> i_plist)
    {
      m_plist = i_plist;
    }

    public void ActionVariableReset()
    {
      m_plist.setSelectedItem(0);
    }

    public String ActionVariableGetValue()
    {
      return m_plist.getSelectedItem().toString();
    }
  }


  public TalkPanel(JPanel i_Panel,ActionTransferManager i_atm)
  {
    m_atm = i_atm;
    m_alerter = m_atm.GetCurrentAlerter();
    m_playernames = new HashSet<String>();
    m_myname = null;

    m_playerready = new AGReporter();
    m_atm.AddButtonPresser("PLAYERTALK",m_playerready);

    m_roomready = new AGReporter();
    m_atm.AddButtonPresser("ROOMTALK",m_roomready);

    // panel for player messages.
    ButtonGroup bg = new ButtonGroup();

    m_messages = new JTextArea();
    m_messages.setBorder(BorderFactory.createLineBorder(Color.black));
    m_messages.setEditable(false);
    m_messages.setLineWrap(true);
    m_messages.setWrapStyleWord(true);

    JScrollPane jsp = new JScrollPane(m_messages);
    m_jsb = jsp.getVerticalScrollBar();

    i_Panel.add(jsp,BorderLayout.CENTER);

    JPanel controlpanel = new JPanel();
    controlpanel.setLayout(new BoxLayout(controlpanel,BoxLayout.Y_AXIS));
    i_Panel.add(controlpanel,BorderLayout.SOUTH);

    JLabel jl = new JLabel("Send Message to:");
    jl.setAlignmentX(JLabel.LEFT_ALIGNMENT);
    controlpanel.add(jl); 
    

    m_room = new JRadioButton("All Players in Room: "); 
    m_room.setAlignmentX(JLabel.LEFT_ALIGNMENT);
    controlpanel.add(m_room); 
    bg.add(m_room);

    JPanel playerpanel = new JPanel();
    playerpanel.setLayout(new BoxLayout(playerpanel,BoxLayout.X_AXIS));
    playerpanel.setAlignmentX(JLabel.LEFT_ALIGNMENT);
    controlpanel.add(playerpanel);

    m_player = new JRadioButton("Specific Player:"); 
    playerpanel.add(m_player); 
    bg.add(m_player);

    m_playerlist = new JComboBox<String>();
    playerpanel.add(m_playerlist); 
    m_playerlist.setEnabled(false);
    JRBActivator jrba = new JRBActivator(m_playerlist,m_player);
    m_player.addItemListener(jrba);

    ActionVariableGenerator plistavg = new PListAVG(m_playerlist);
    m_atm.AddVariableGenerator("TalkPlayer",plistavg);

    m_message = new JTextField(); 
    m_message.setAlignmentX(JLabel.LEFT_ALIGNMENT);
    controlpanel.add(m_message);
    m_message.setBorder(BorderFactory.createLineBorder(Color.black));
    m_message.addActionListener(this);

    ActionVariableGenerator msgavg = new MessageAVG(m_message);
    m_atm.AddVariableGenerator("RoomMessage",msgavg);
    m_atm.AddVariableGenerator("PlayerMessage",msgavg);

    m_err = new JLabel("");
    m_err.setAlignmentX(JLabel.LEFT_ALIGNMENT);
    controlpanel.add(m_err);
  }

  private void SetPlayerList()
  {
    m_playerlist.removeAllItems();
    m_playerlist.addItem("");
    Iterator<String> plit = m_playernames.iterator();
    while(plit.hasNext())
    {
      m_playerlist.addItem(plit.next());
    }
  }

  public void HandleEvent(GameEvent i_event)
  {
    if (i_event.GetVarValue("eventname").equals("GUIIAM"))
    {
      m_myname = i_event.GetVarValue("myname");
    }

    if (i_event.GetVarValue("eventname").equals("INHABITANT"))
    {
      String iname = i_event.GetVarValue("inhabitantname");
      String rname = i_event.GetVarValue("roomname");
      int oldsize = m_playernames.size();
      m_playernames.add(iname);
      if (m_myname != null && m_myname.equals(iname))
      {
        m_room.setText("All Players in Room: " + rname);
      }
      if (oldsize != m_playernames.size())
      {
        SetPlayerList();
      }
    }

    if (i_event.GetVarValue("eventname").equals("DROPINHABITANT"))
    {
      m_playernames.remove(i_event.GetVarValue("inhabitantname"));
      SetPlayerList();
    }

    if (i_event.GetVarValue("eventname").equals("MESSAGE"))
    {
      m_messages.append(i_event.GetVarValue("messagetext"));
      m_messages.append("\n");
      m_alerter.Alert();
      m_jsb.setValue(m_jsb.getMaximum());
    }
  }

  public void actionPerformed(ActionEvent i_event)
  {
    if (!m_room.isSelected() && !m_player.isSelected())
    {
      m_err.setText("Must select a message destination.");
      return;
    }

    if (m_player.isSelected() && m_playerlist.getSelectedItem().toString().length() == 0)
    {
      m_err.setText("Must select a player.");
      return;
    }
    // ok...if we're here, then we have to be able to set a message
    m_err.setText("");

    if (m_room.isSelected())
    {
      m_atm.ReportNewValueToConsumers("RoomMessage",m_message.getText());
      if (!m_roomready.IsReady())
      {
        m_err.setText("bwah? why is room message not ready?");
        return;
      }
      m_atm.ButtonPressed("ROOMTALK");
      m_message.setText("");
    }

    if (m_player.isSelected())
    {
      m_atm.ReportNewValueToConsumers("PlayerMessage",m_message.getText());
      m_atm.ReportNewValueToConsumers("TalkPlayer",m_playerlist.getSelectedItem().toString());
      if (!m_playerready.IsReady())
      {
        m_err.setText("er?  why is player message not ready?");
        return;
      }
      String localecho = "(message to " + m_playerlist.getSelectedItem().toString() + ") " +
        m_message.getText();

      m_messages.append(localecho);
      m_messages.append("\n");

      m_atm.ButtonPressed("PLAYERTALK");
      m_message.setText("");
    }
  }
}
