package Outpost;

import NodeInterfacePackage.*;
import javax.swing.JPanel;
import javax.swing.JLabel;
import java.util.HashMap;
import javax.swing.JComboBox;
import java.util.Vector;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;


public class OptionSelector implements UserDefinedInterface,ActionVariableGenerator,ActionListener
{
  private JComboBox<String> m_box;
  private JLabel m_label;
  private ActionTransferManager m_atm;
  private Vector<String> m_names;
  private String m_varname;
  private boolean m_onecount;

  public OptionSelector(HashMap<String,String> i_Options,
                       String i_DataLoc,
                       JPanel i_Panel,
                       ActionTransferManager i_atm)
  {
    m_atm = i_atm;
    m_onecount = true;

    if (!i_Options.containsKey("varname")) throw new NodeInterfacePackageException("missing option varname");
    if (!i_Options.containsKey("values")) throw new NodeInterfacePackageException("missing option values");
    if (i_Options.containsKey("zerocount") && i_Options.get("zerocount").equals("true"))
    {
      m_onecount = false;
    }

    m_varname = i_Options.get("varname");

    m_atm.AddVariableGenerator(m_varname,this);

    JPanel jp = new JPanel();
    jp.setOpaque(false);
    JLabel namelabel = new JLabel(m_varname + ": ");
    jp.add(namelabel);

    m_box = new JComboBox<String>();
    jp.add(m_box);
    m_box.addActionListener(this);

    m_label = new JLabel();
    jp.add(m_label);
    i_Panel.add(jp);

    m_names = new Vector<String>();

    String[] lnames = i_Options.get("values").split("\\|");
    int i;
    for (i = 0 ; i < lnames.length ; ++i)
    {
      m_names.add(lnames[i]);
      m_box.addItem(lnames[i]);
    }

    Reset();
  }

  public void Reset()
  {
    ActionVariableReset();
    m_label.setText(m_names.get(0));
  }

  public void ActionVariableReset()
  {
    m_box.setSelectedIndex(0);
  }
  
  public String ActionVariableGetValue()
  {
    return Integer.toString(m_box.getSelectedIndex() + (m_onecount ? 1 : 0));
  }

  public void Refresh()
  {
  }

  public void HandleEvent(GameEvent i_event)
  {
    if (i_event.GetVarValue("eventname").equals("NEWSTATE"))
    {
      m_box.setVisible(i_event.GetVarValue("statename").equals("InitialState"));
    }

    if (i_event.GetVarValue("eventname").equals("OPTIONS"))
    {
      String field = i_event.GetVarValue(m_varname);
      int idx = Integer.parseInt(field) - (m_onecount ? 1 : 0);
      m_box.setSelectedIndex(idx);
      m_label.setText(m_names.get(idx));
    }
  }

  public void actionPerformed(ActionEvent i_event)
  {
    m_atm.ReportNewValueToConsumers(m_varname,ActionVariableGetValue());
  }
}    

      
