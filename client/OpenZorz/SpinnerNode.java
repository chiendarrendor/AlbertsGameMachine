package OpenZorz;


import javax.swing.*;
import java.awt.*;
import javax.swing.event.*;

import NodeInterfacePackage.GameEvent;


public class SpinnerNode extends ActionGeneratorNode implements ChangeListener
{
  JSpinner m_spinner;
  String m_VarName;
  boolean m_IsVariable;
  int m_InitialIndex;

  public SpinnerNode(String i_Name,int i_x,int i_y,String i_hidden,
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
    m_spinner = new JSpinner();
    m_spinner.addChangeListener(this);

    Point p = new Point(i_x,i_y);
    m_spinner.setVisible(IsVisible());
    i_ParentNode.AddComponent(m_spinner,p);
    SetComponent(m_spinner);

    if (!m_IsVariable)
    {
      SetSpinnerModel(m_VarName);
    }
    else
    {
      SetSpinnerModel("");
    }
  }

  public void SetIfSet()
  {
    String v = (String)m_spinner.getValue();
    if (!v.equals(""))
    {
      VarSet(v);
    }
  }

  public void SetSpinnerModel(String i_data)
  {
    SpinnerResettableTextModel srtm = new SpinnerResettableTextModel(i_data);
    m_spinner.setModel(srtm);
    SetIfSet();
  }
    
  public void stateChanged(ChangeEvent i_event)
  {
    VarSet((String)m_spinner.getValue());
  }

  public String ActionVariableGetValue()
  {
    return (String)m_spinner.getValue();
  }

  void DoActionReset()
  {
    DoReset();
  }

  void DoReset()
  {
    SpinnerResettableTextModel srtm = (SpinnerResettableTextModel)m_spinner.getModel();
    if (srtm == null)
    {
      return;
    }
    srtm.Reset();
    SetIfSet();
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

    SetSpinnerModel(contents);
  }
}
