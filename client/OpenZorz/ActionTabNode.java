package OpenZorz;

import NodeInterfacePackage.GameEvent;
import NodeInterfacePackage.ActionTransferManager;
import javax.swing.JPanel;
import javax.swing.JLabel;
import java.awt.BorderLayout;
import javax.swing.BoxLayout;

public class ActionTabNode extends TabNode
{
  JLabel m_StateText;

  public ActionTabNode(ActionTransferManager i_atm,String i_Name,TabbedWindow i_tabloc)
  {
    super(i_atm,i_tabloc,i_Name + " Action Selection",null,true);

    JPanel jp = (JPanel)GetComponent();

    jp.setLayout(new BorderLayout());
    m_StateText = new JLabel("Current State: ");
    jp.add(m_StateText,BorderLayout.SOUTH);
    
    JPanel mainpanel = new JPanel();
    jp.add(mainpanel,BorderLayout.CENTER);

    mainpanel.setLayout(new BoxLayout(mainpanel,BoxLayout.Y_AXIS));

    SetComponent(GetVisibilityComponent(),mainpanel);
  }

  public void MyHandleEvent(GameEvent i_event)
  {
    String e = i_event.GetVarValue("eventname");

    if (e.equals("NEWSTATE"))
    {
	    String v = i_event.GetVarValue("statedesc");
	    m_StateText.setText("Current State: " + v);
    }
  }
}
