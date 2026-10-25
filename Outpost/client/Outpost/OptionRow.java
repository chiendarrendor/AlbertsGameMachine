package Outpost;

import java.util.HashMap;
import javax.swing.BoxLayout;
import javax.swing.JPanel;
import javax.swing.JCheckBox;
import javax.swing.JLabel;
import java.awt.event.ItemListener;
import java.awt.event.ItemEvent;
import NodeInterfacePackage.*;

public class OptionRow implements UserDefinedInterface,ItemListener,ActionVariableGenerator
{
  private static final GuiPacketQuery s_stateQuery = new GuiPacketQuery("{STATE}");

  private ActionTransferManager m_atm;
  private String m_varname;
  private GuiPacketQuery m_optionQuery;
  private JCheckBox m_checkbox;
  private JLabel m_valueLabel;

  public OptionRow(HashMap<String,String> i_Options,String i_DataLoc,JPanel i_Panel,ExternalAssets i_assets)
  {
    if (!i_Options.containsKey("varname")) throw new NodeInterfacePackageException("missing option varname");
    if (!i_Options.containsKey("optionpath")) throw new NodeInterfacePackageException("missing option optionpath");
    if (!i_Options.containsKey("label")) throw new NodeInterfacePackageException("missing option label");

    m_atm = i_assets.GetActionTransferManager();
    m_varname = i_Options.get("varname");
    m_optionQuery = new GuiPacketQuery(i_Options.get("optionpath"));

    m_atm.AddVariableGenerator(m_varname,this);

    i_Panel.setLayout(new BoxLayout(i_Panel,BoxLayout.X_AXIS));

    m_checkbox = new JCheckBox();
    m_checkbox.addItemListener(this);
    i_Panel.add(m_checkbox);

    m_valueLabel = new JLabel("false");
    i_Panel.add(m_valueLabel);

    i_Panel.add(new JLabel(i_Options.get("label")));
  }

  public void itemStateChanged(ItemEvent i_event)
  {
    m_atm.ReportNewValueToConsumers(m_varname,
        i_event.getStateChange() == ItemEvent.SELECTED ? "1" : "0");
  }

  public void ActionVariableReset()
  {
    m_checkbox.setSelected(false);
  }

  public String ActionVariableGetValue()
  {
    return m_checkbox.isSelected() ? "1" : "0";
  }

  public void HandleEvent(GameEvent i_event)
  {
    String state = (String)s_stateQuery.evaluate(i_event);
    m_checkbox.setVisible("InitialState".equals(state));

    boolean curval = (Boolean)m_optionQuery.evaluate(i_event);
    m_valueLabel.setText(String.valueOf(curval));
    m_checkbox.setSelected(curval);
    m_atm.ReportNewValueToConsumers(m_varname,curval ? "1" : "0");
  }

  public void Refresh()
  {
  }

  public void Reset()
  {
    m_checkbox.setSelected(false);
    m_valueLabel.setText("false");
  }
}
