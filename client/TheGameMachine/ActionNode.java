package TheGameMachine;

import javax.swing.*;
import java.awt.*;
import java.util.*;

import java.awt.event.*;

import NodeInterfacePackage.*;
import org.json.JSONObject;


public class ActionNode extends GameNode implements DoActionHandler
{
  GameGui m_gamegui;
  HashMap<String,String> m_curvarvals;
  JButton m_button;
  String m_Name;
  VisibilityType m_vis;
  ActionTransferManager.Alerter m_alerter;
  boolean m_doesAlert;
  boolean m_currentlyLegal = false;
  GuiPacketQuery m_legalActionGPQ;
  
  public enum VisibilityType
  {
    ALWAYSON,
    NEWSTATEHIDES,
    ALWAYSHIDDEN,
    STATEDALWAYSHIDDEN
  }

  private class GoButton extends JButton implements ActionGenerator,ActionListener
  {
    GameGui m_gbgamegui;
    String m_actionname;

    public GoButton(String i_text,GameGui i_gamegui,String i_actionname)
    {
      super(i_text);
      m_actionname = i_actionname;
      m_gbgamegui = i_gamegui;

      setEnabled(false);
      addActionListener(this);
      
      m_gbgamegui.GetActionTransferManager().AddButtonPresser(m_actionname,this);
    }

    public void ActionLegalityChanged(boolean i_islegal)
    {
      // nothing happens here because the containing panel goes invisible or not
    }

    public void ActionReadinessChanged(boolean i_isready)
    {
      setEnabled(i_isready);
    }

    public void actionPerformed(ActionEvent i_event)
    {
      m_gbgamegui.GetActionTransferManager().ButtonPressed(m_actionname);
    }
    
    private static final long serialVersionUID = 1;

  }


  public ActionNode(GameNode i_Parent,String i_Name,String i_Text,String i_color,GameGui i_gamegui,
                    VisibilityType i_vis,String i_Alerts)
  {
    super(i_Parent,((i_vis != VisibilityType.ALWAYSON) ? "true" : null));
    m_gamegui = i_gamegui;
    m_alerter = m_gamegui.GetActionTransferManager().GetCurrentAlerter();
    m_curvarvals = new HashMap<String,String>();
    m_Name = i_Name;
    m_vis = i_vis;
    m_doesAlert = i_Alerts != null && i_Alerts.equals("true");
    m_legalActionGPQ = new GuiPacketQuery(String.format("{LEGALACTION:%s}",i_Name));

    JPanel panel = new JPanel();
    panel.setLayout(new BorderLayout());
    panel.setOpaque(false);

    if (i_Text == null)
    {
      m_button = new GoButton("Do " + i_Name,m_gamegui,m_Name);
    }
    else
    {
      m_button = new GoButton(i_Text,m_gamegui,m_Name);
    }

    if (i_color != null)
    {
      Color c = Color.decode(i_color);
      m_button.setBackground(c);
    }

    m_gamegui.GetActionTransferManager().AddActionHandler(m_Name,this);


    panel.add(m_button,BorderLayout.EAST);

    JPanel inpanel = new JPanel();
    inpanel.setLayout(new BoxLayout(inpanel,BoxLayout.X_AXIS));
    panel.add(inpanel,BorderLayout.CENTER);
    inpanel.setOpaque(false);

    i_Parent.AddComponent(panel);

    panel.setVisible(IsVisible());

    SetComponent(panel,inpanel);
  }

  public String GetName()
  {
    return m_Name;
  }

  private void ResetVars()
  {
    int i;

    for (i = 0 ; i < GetNumChildren() ; ++i)
    {
      ActionVarNode avn = (ActionVarNode)GetChild(i);
      avn.VarReset();
    }
    m_curvarvals.clear();
  }

  private void SetVarVal(String i_name,String i_value)
  {
    if (i_value == null || i_value.equals(""))
    {
      m_curvarvals.remove(i_name);
    }
    else
    {
      m_curvarvals.put(i_name,i_value);
    }
  }

  private void GetCurVarVals()
  {
    int i;

    for (i = 0 ; i < GetNumChildren() ; ++i)
    {
      ActionVarNode avn = (ActionVarNode)GetChild(i);

      String s = m_gamegui.GetActionTransferManager().GetVariableGeneratorValue(avn.GetName());

      SetVarVal(avn.GetName(),s);
    }
  }    

  public void VarValSet(String i_Varname, String i_VarVal)
  {
    SetVarVal(i_Varname,i_VarVal);
    m_gamegui.GetActionTransferManager().ActionReadinessChanged(m_Name,m_curvarvals.size() == GetNumChildren());
  }

  public void ActionGo()
  {
    JSONObject params = new JSONObject();

    int i;
    for(i = 0 ; i < GetNumChildren() ; i++)
    {
      ActionVarNode avn = (ActionVarNode)GetChild(i);
      String val = m_curvarvals.get(avn.GetName());
      params.put(avn.GetParamName(),avn.ParseTypedValue(val));
    }
    m_gamegui.SendAction(m_Name,params);
    ResetVars();
    GetCurVarVals();

    m_gamegui.GetActionTransferManager().ActionReadinessChanged(m_Name,m_curvarvals.size() == GetNumChildren());

  }

  public void MyHandleEvent(GameEvent i_Event)
  {
    boolean nowLegal = m_legalActionGPQ.evaluateBoolean(i_Event,false);
    ActionTransferManager atm = m_gamegui.GetActionTransferManager();

    // truth:  after the initial ingestion packet, _every_ FSS represents
    // a state change on the server end, because FSS only comes from transitions.
    // so let's do the stuff that is always done on a state change.

    ResetVars();
    GetCurVarVals();
    atm.ActionReadinessChanged(m_Name,m_curvarvals.size() == GetNumChildren());

    if (m_vis == VisibilityType.NEWSTATEHIDES) {
      AlterVisibility(nowLegal);
      if (!m_currentlyLegal && nowLegal && m_doesAlert) m_alerter.Alert();
    }

    if (m_vis == VisibilityType.NEWSTATEHIDES || m_vis == VisibilityType.STATEDALWAYSHIDDEN) {
      atm.ActionLegalityChanged(m_Name,nowLegal);
    }

    m_currentlyLegal = nowLegal;
  }
}



