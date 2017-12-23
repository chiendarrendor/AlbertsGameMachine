package Outpost;

import MySwing.LockLayout;
import java.awt.Point;
import NodeInterfacePackage.*;
import javax.swing.JPanel;
import java.util.HashMap;
import javax.swing.JTextField;
import javax.swing.text.*;
import javax.swing.event.DocumentListener;
import javax.swing.event.DocumentEvent;

public class StateSensitiveTextBox implements UserDefinedInterface,DocumentListener,ActionVariableGenerator
{
  private ServerStateHistory m_ssh;
  private ActionTransferManager m_atm;
  String m_ActionVariableName;
  NumericTextField m_ntf;

  private class NumericTextField extends JTextField
  {
    public NumericTextField()
    {
      super(10);
    }

    protected Document createDefaultModel()
    {
      return new NumericDocument();
    }

    private class NumericDocument extends PlainDocument
    {
      public void insertString(int offs,String str,AttributeSet a) throws BadLocationException
      {
        if (str == null) return;

        StringBuffer sb = new StringBuffer();
        for (int i = 0 ; i < str.length() ; ++i)
        {
          if (Character.isDigit(str.charAt(i)))
          {
            sb.append(str.charAt(i));
          }
        }
        super.insertString(offs,new String(sb),a);
      }
    }
  }

  public StateSensitiveTextBox(HashMap<String,String> i_Options,
                               String i_DataLoc,
                               JPanel i_Panel,
                               ActionTransferManager i_atm)
  {
    m_atm = i_atm;
    m_ssh = new ServerStateHistory();

    if (!i_Options.containsKey("varname")) throw new NodeInterfacePackageException("missing option varname");
    m_ActionVariableName = i_Options.get("varname");

    m_atm.AddVariableGenerator(m_ActionVariableName,this);

    i_Panel.setLayout(new LockLayout());

    //    i_Panel.setAlignmentY((float)1.0);
    
    m_ntf = new NumericTextField();
    i_Panel.add(m_ntf,new Point(0,0));

    m_ntf.getDocument().addDocumentListener(this);
  }

  public void Reset()
  {
    m_ntf.setText("");
  }

  public void ActionVariableReset()
  {
    Reset();
  }

  public String ActionVariableGetValue()
  {
    return m_ntf.getText();
  }

  public void Refresh()
  {
  }

  public void HandleEvent(GameEvent i_event)
  {
    m_ssh.HandleEvent(i_event);

    if (i_event.GetVarValue("eventname").equals("NEWSTATE"))
    {
      if (!m_ssh.GetLastState().equals(m_ssh.GetCurState()))
      {
        Reset();
      }
    }
  }

  public void changedUpdate(DocumentEvent i_event)
  {
    m_atm.ReportNewValueToConsumers(m_ActionVariableName,ActionVariableGetValue());
  }

  public void insertUpdate(DocumentEvent i_event)
  {
    m_atm.ReportNewValueToConsumers(m_ActionVariableName,ActionVariableGetValue());
  }

  public void removeUpdate(DocumentEvent i_event)
  {
    m_atm.ReportNewValueToConsumers(m_ActionVariableName,ActionVariableGetValue());
  }



}

    
    
    
