package Outpost;

import MySwing.LockLayout;
import java.awt.Point;
import NodeInterfacePackage.*;
import javax.swing.JPanel;
import java.util.HashMap;
import java.util.Objects;
import javax.swing.JTextField;
import javax.swing.text.*;
import javax.swing.event.DocumentListener;
import javax.swing.event.DocumentEvent;

public class StateSensitiveTextBox implements UserDefinedInterface,DocumentListener,ActionVariableGenerator
{
  private static final GuiPacketQuery s_stateQuery = new GuiPacketQuery("{STATE}");
  private static final GuiPacketQuery s_bidderQuery = new GuiPacketQuery("currentBidderId");

  private ActionTransferManager m_atm;
  String m_ActionVariableName;
  NumericTextField m_ntf;
  private String m_lastState = null;
  private int m_lastBidderId = Integer.MIN_VALUE;

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
    m_lastState = null;
    m_lastBidderId = Integer.MIN_VALUE;
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
    String curState = (String)s_stateQuery.evaluate(i_event);
    int curBidder = s_bidderQuery.evaluateInt(i_event,-1);

    if (!Objects.equals(curState,m_lastState) || curBidder != m_lastBidderId)
    {
      Reset();
    }

    m_lastState = curState;
    m_lastBidderId = curBidder;
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

    
    
    
