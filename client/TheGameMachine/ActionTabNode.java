package TheGameMachine;

import NodeInterfacePackage.GameEvent;
import NodeInterfacePackage.ActionTransferManager;
import NodeInterfacePackage.GuiPacketQuery;
import javax.swing.JPanel;
import javax.swing.JLabel;
import java.awt.BorderLayout;
import javax.swing.BoxLayout;

public class ActionTabNode extends TabNode
{
  JLabel m_StateText;
  GuiPacketQuery gpq;

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
    gpq = new GuiPacketQuery("{STATEDESC}");
  }

  public void MyHandleEvent(GameEvent i_event)
  {
    m_StateText.setText("Current State: " + gpq.evaluateString(i_event,""));
  }

  void ResetMe()
  {
    super.ResetMe();
    m_StateText.setText("Current State: ");
  }
}
