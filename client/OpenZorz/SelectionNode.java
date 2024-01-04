package OpenZorz;


import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

import NodeInterfacePackage.GameEvent;
import Utilities.StringUtility;


public class SelectionNode extends ActionGeneratorNode implements ActionListener
{
  JComboBox<String> m_box;
  String m_VarName;
  boolean m_IsVariable;

  public SelectionNode(String i_Name,int i_x,int i_y,String i_hidden,
                       GameNode i_ParentNode,GameGui i_gamegui,boolean i_IsVariable,
                       String i_VarName)
  {
    super(i_ParentNode,i_hidden,i_gamegui,i_Name);
    m_VarName = i_VarName;
    m_IsVariable = i_IsVariable;

    InitializeNode(i_ParentNode,i_x,i_y);
  }


  public void InitializeNode(GameNode i_ParentNode,int i_x,int i_y)
  {
    m_box = new JComboBox<>();
    m_box.addActionListener(this);

    Point p = new Point(i_x,i_y);
    m_box.setVisible(IsVisible());
    i_ParentNode.AddComponent(m_box,p);
    SetComponent(m_box);

    if (!m_IsVariable)
    {
      String[] entries = m_VarName.split(",");
      int i;
      m_box.addItem("");
      for (i = 0 ; i < entries.length ; ++i)
      {
        m_box.addItem(entries[i]);
      }
    }
  }
    
  public void actionPerformed(ActionEvent i_event)
  {
    if (m_box.getItemCount() > 0 && m_box.getSelectedIndex() != 0)
    {
      VarSet((String)m_box.getSelectedItem());
    }
  }

  public String ActionVariableGetValue()
  {
    return (String)m_box.getSelectedItem();
  }

  void DoActionReset()
  {
    DoReset();
  }

  void DoReset()
  {
    if (m_box.getItemCount() > 0)
    {
      m_box.setSelectedIndex(0);
    }
  }

  public void MyHandleEvent(GameEvent i_Event)
  {
    super.MyHandleEvent(i_Event);

    if (!m_IsVariable)
    {
      return;
    }

    String contents = i_Event.GetVarValue(m_VarName);

    if (contents == null)
    {
      return;
    }

    String[] entries = contents.split(",");

    m_box.removeAllItems();

    m_box.addItem("");
    
    for (int i = 0 ; i < entries.length ; ++i)
    {
      m_box.addItem(StringUtility.ReComma(entries[i]));
    }
  }
}
