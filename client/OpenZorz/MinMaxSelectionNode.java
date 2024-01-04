package OpenZorz;


import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

import NodeInterfacePackage.GameEvent;


public class MinMaxSelectionNode extends ActionGeneratorNode implements ActionListener
{
  private JComboBox<String> m_box;
  private String m_Min;
  private String m_Max;
  boolean m_varmin;
  boolean m_varmax;
  boolean m_minok;
  boolean m_maxok;
  private int m_minint;
  private int m_maxint;

  public MinMaxSelectionNode(String i_VarName,int i_x,int i_y,String i_hidden,
                             GameNode i_ParentNode,GameGui i_gamegui,
                             String i_min,String i_max)
  {
    super(i_ParentNode,i_hidden,i_gamegui,i_VarName);
    m_Min = i_min;
    m_Max = i_max;

    InitializeBox(i_ParentNode,i_x,i_y);

    try
    {
      m_minint = Integer.parseInt(m_Min);
      m_varmin = false;
      m_minok = true;
    }
    catch(NumberFormatException nfe)
    {
      m_minint = 0;
      m_varmin = true;
      m_minok = false;
    }

    try
    {
      m_maxint = Integer.parseInt(m_Max);
      m_varmax = false;
      m_maxok = true;
    }
    catch(NumberFormatException nfe)
    {
      m_maxint = -1;
      m_varmax = true;
      m_maxok = false;
    }

    BuildBox();
  }


  public void InitializeBox(GameNode i_ParentNode,int i_x,int i_y)
  {
    m_box = new JComboBox<>();
    m_box.addActionListener(this);

    Point p = new Point(i_x,i_y);
    m_box.setVisible(IsVisible());
    i_ParentNode.AddComponent(m_box,p);
    SetComponent(m_box);
  }

  void BuildBox()
  {
    if (!m_minok || !m_maxok) 
    {
      return;
    }

    int i;

    m_box.removeAllItems();
    m_box.addItem("");
    
    for (i = m_minint ; i <= m_maxint ; ++i)
    {
      m_box.addItem(Integer.toString(i));
    }
    m_box.setSelectedIndex(0);
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
    if (m_box.getItemCount() > 0)
    {
      m_box.setSelectedIndex(0);
    }
  }

  void DoReset()
  {
    if (m_varmin)
    {
      m_minok = false;
      m_box.removeAllItems();
      m_box.addItem("");
    }

    if (m_varmax)
    {
      m_maxok = false;
      m_box.removeAllItems();
      m_box.addItem("");
    }
    m_box.setSelectedIndex(0);
  }

  public void MyHandleEvent(GameEvent i_Event)
  {
    super.MyHandleEvent(i_Event);

    boolean update = false;

    if (m_varmin)
    {
      try
      {
        String ms = i_Event.GetVarValue(m_Min);
        m_minint = Integer.parseInt(ms);
        m_minok = true;
        update = true;
      }
      catch (NumberFormatException nfe)
      {
      }
    }

    if (m_varmax)
    {
      try
      {
        String ms = i_Event.GetVarValue(m_Max);
        m_maxint = Integer.parseInt(ms);
        m_maxok = true;
        update = true;
      }
      catch (NumberFormatException nfe)
      {
      }
    }

    if (update) 
    {
      BuildBox();
    }
    else
    {
    }
  }
}
