package TheGameMachine;


import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.util.List;
import java.util.ArrayList;

import NodeInterfacePackage.GameEvent;
import NodeInterfacePackage.GuiPacketQuery;
import Utilities.StringUtility;


public class SelectionNode extends ActionGeneratorNode implements ActionListener
{
  JComboBox<String> m_box;
  String m_VarName;
  boolean m_IsVariable;
  GuiPacketQuery m_ListPathQuery;

  public SelectionNode(String i_Name,int i_x,int i_y,String i_hidden,
                       GameNode i_ParentNode,GameGui i_gamegui,boolean i_IsVariable,
                       String i_VarName,GuiPacketQuery i_ListPathQuery)
  {
    super(i_ParentNode,i_hidden,i_gamegui,i_Name);
    m_VarName = i_VarName;
    m_IsVariable = i_IsVariable;
    m_ListPathQuery = i_ListPathQuery;

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

    List<String> entries;

    if (m_ListPathQuery != null)
    {
      // A real JSON array's elements are already clean, unescaped strings --
      // no StringUtility.ReComma needed here, that was only ever undoing the
      // old listvar wire format's own hand-rolled comma-escaping.
      entries = m_ListPathQuery.evaluateStringList(i_Event);
    }
    else
    {
      String contents = i_Event.GetVarValue(m_VarName);

      if (contents == null)
      {
        return;
      }

      entries = new ArrayList<String>();
      for (String entry : contents.split(","))
      {
        entries.add(StringUtility.ReComma(entry));
      }
    }

    m_box.removeAllItems();

    m_box.addItem("");

    for (String entry : entries)
    {
      m_box.addItem(entry);
    }
  }
}
