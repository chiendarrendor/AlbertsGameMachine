package OpenZorz;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

import NodeInterfacePackage.GameEvent;

public class CheckBoxNode extends ActionGeneratorNode implements ItemListener
{
  JCheckBox m_cbox;
  String m_Name;

  public CheckBoxNode(String i_Name,int i_x,int i_y,String i_hidden,
                     GameNode i_ParentNode,GameGui i_gamegui)
  {
    super(i_ParentNode,i_hidden,i_gamegui,i_Name);
    m_Name = i_Name;

    InitializeNode(i_ParentNode,i_x,i_y);
  }


  public void InitializeNode(GameNode i_ParentNode,int i_x,int i_y)
  {
    m_cbox = new JCheckBox();
    m_cbox.addItemListener(this);

    m_cbox.setVisible(IsVisible());
    m_cbox.setOpaque(false);

    Point p = new Point(i_x,i_y);
    i_ParentNode.AddComponent(m_cbox,p);
    SetComponent(m_cbox);
  }
    
  public void itemStateChanged(ItemEvent i_event)
  {
    if (i_event.getStateChange() == ItemEvent.SELECTED)
    {
      VarSet("1");
    }
    else
    {
      VarSet("0");
    }
  }

  void DoActionReset()
  {
    DoReset();
  }

  void DoReset()
  {
    m_cbox.setSelected(false);
  }
  
  public String ActionVariableGetValue()
  {
    return m_cbox.isSelected() ? "1" : "0";
  }

  public void MyHandleEvent(GameEvent i_event)
  {
    String e = i_event.GetVarValue(m_Name);
    if (e == null) return;


    if (e.equals("1"))
    {
      m_cbox.setSelected(true);
      VarSet("1");
    }
    if (e.equals("0"))
    {
      m_cbox.setSelected(false);
      VarSet("0");
    }
  }
}
