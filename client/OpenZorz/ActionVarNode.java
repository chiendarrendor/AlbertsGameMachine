package OpenZorz;

import javax.swing.*;
import java.awt.*;

import NodeInterfacePackage.*;
import MySwing.LockLayout;

public class ActionVarNode extends PanelNode implements ActionVariableConsumer
{
  ActionNode m_Action;
  String m_Name;
  JLabel m_VarValueLabel;
  String m_defaultValue;
  boolean m_noReset;
  boolean m_hasTextBox;
  ActionTransferManager m_actionmanager;

  public ActionVarNode(String i_name,String i_text,ActionNode i_ActionNode,
                       GameGui i_gamegui,String i_Hidden,String i_noReset,String i_hasTextBox)
  {
    super(i_ActionNode,0,0,i_Hidden,null,null);
    m_Action = i_ActionNode;
    m_Name = i_name;
    m_noReset = i_noReset != null && i_noReset.equals("y");
    m_hasTextBox = i_hasTextBox != null && i_hasTextBox.equals("y");
    m_actionmanager = i_gamegui.GetActionTransferManager();

    Point panelpoint = new Point(0,5);


    if (m_hasTextBox)
    {
      TextNode tnode = new TextNode(this,5,0,null);
      tnode.AddText(i_text + ":");
      m_VarValueLabel = new JLabel();
      AddComponent(m_VarValueLabel,new Point(5,20));
      TextBoxNode tbn = new TextBoxNode(i_name,10,5,40,null,this,i_gamegui);
      tbn.DoReset();
      panelpoint = new Point(5,60);
    }

    i_gamegui.GetActionTransferManager().AddVariableConsumer(m_Name,this);

    JPanel panel = new JPanel();
    panel.setLayout(new LockLayout());
    panel.setOpaque(false);
    panel.setVisible(true);
    AddComponent(panel,panelpoint);
    SetComponent(GetComponent(),panel);


  }

  public void VarReset()
  {
    if (this.NoReset())
    {
      return;
    }
    m_actionmanager.ResetVariableGenerators(this.GetName());

    if (m_hasTextBox)
    {
      m_VarValueLabel.setText("");
    }
  }

  public boolean NoReset()
  {
    return m_noReset;
  }

  public String GetName()
  {
    return m_Name;
  }

  public void ReportNewVariableValue(String i_newvalue)
  {
    if (m_hasTextBox)
    {
      m_VarValueLabel.setText(i_newvalue);
    }

    m_Action.VarValSet(m_Name,i_newvalue);
  }

  public void MyHandleEvent(GameEvent i_Event)
  {
  }
}
