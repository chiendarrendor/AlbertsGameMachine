package Outpost;

import NodeInterfacePackage.*;
import java.util.Vector;
import javax.swing.JCheckBox;
import javax.swing.JLabel;
import java.awt.event.ItemListener;
import java.awt.event.ItemEvent;

public class HandListener implements ItemListener,ActionVariableGenerator
{
  static final String s_name="DiscardString";
  private ActionTransferManager m_atm;
  private Vector<JCheckBox> m_checkboxes;
  private Vector<CommodityInfo> m_vci;
  private CommodityNotesPanel m_cnp;
    
  public HandListener(ActionTransferManager i_atm,
                      Vector<CommodityInfo> i_vci,
                      CommodityNotesPanel i_cnp)
  {
    m_atm = i_atm;
    m_vci = i_vci;
    m_cnp = i_cnp;
    m_atm.AddVariableGenerator(s_name,this);
    m_checkboxes = new Vector<JCheckBox>();
  }

  public void odispose()
  {
    m_atm.RemoveVariableGenerator(s_name,this);
  }


  public void ClearBoxes()
  {
    m_checkboxes.clear();
  }
    
  public void AddBox(JCheckBox i_box)
  {
    m_checkboxes.add(i_box);
    i_box.addItemListener(this);
  }

  private void ResetAllBoxes()
  {
    int i;
    for (i = 0 ; i < m_checkboxes.size() ; ++i)
    {
      m_checkboxes.get(i).setSelected(false);
      m_vci.get(i).SetDiscardState(false);
    }
    SetLabels();
  }

  private void SetLabels()
  {
    m_cnp.Redraw();
  }


  private String GetBoxString()
  {
    StringBuffer sb = new StringBuffer();
    int i;
    for (i = 0 ; i < m_checkboxes.size() ; ++i)
    {
      if (m_checkboxes.get(i).isSelected())
      {
        sb.append("1");
        m_vci.get(i).SetDiscardState(true);
      }
      else
      {
        sb.append("0");
        m_vci.get(i).SetDiscardState(false);
      }
    }
    return sb.toString();
  }

  // this is for ItemListener
  public void itemStateChanged(ItemEvent i_event)
  {
    m_atm.ReportNewValueToConsumers(s_name,GetBoxString());
    SetLabels();
  }

  // these are for ActionGenerator
  public void ActionVariableReset()
  {
    ResetAllBoxes();
  }

  public String ActionVariableGetValue()
  {
    return GetBoxString();
  }
}
