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
import org.json.JSONObject;
import org.json.JSONArray;
import org.doghouse.jsonpath.JsonPath;
import java.util.List;

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

  static JsonPath namePath = JsonPath.compile("name");
  static JsonPath gameNamePath = JsonPath.compile("gameName");
  static JsonPath inhabitantListPath = JsonPath.compile("inhabitants[]");

  public void handleRoomUpdate(JSONObject roomdata) {
    String roomName = (String)namePath.evaluate(roomdata);
    String gameName = gameNamePath.evaluateString(roomdata, "no game");
    List<String> inhabitants = inhabitantListPath.evaluateStringList(roomdata);

    m_statelabel.setText("<unknown>");
    m_gamelabel.setText(gameName);

    boolean inDaHouse = false;

    StringBuffer sb = new StringBuffer();
    sb.append("<HTML>Players:<ul>");
    for (String inhabitant : inhabitants) {
      sb.append("<li>");
      sb.append(inhabitant);
      sb.append("</li>");
      if (m_iam.equals(inhabitant)) inDaHouse = true;
    }
      sb.append("</ul></html>");
      m_playerslabel.setText(sb.toString());

      setBorder(BorderFactory.createLineBorder(inDaHouse ? Color.green : Color.black));

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

