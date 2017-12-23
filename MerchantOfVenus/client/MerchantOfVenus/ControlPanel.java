package MerchantOfVenus;

import java.awt.*;
import javax.swing.*;
import javax.swing.border.*;
import NodeInterfacePackage.*;


public class ControlPanel 
{

  JLabel handle;
  ErrorPanel errorpanel;
  InitialPanel ipanel;
  SwitchablePanel switches;
  JumpFlyPanel jfp;
  CostlyFirstPanel cfp;
  CostlyMovePanel cmp;
  EndOfGamePanel eog;
  DieChoicePanel mulliganPanel;
  DieChoicePanel pilotNumberPanel;
  RelicPanel relicPanel;
  TradePanel tradePanel;

  public static void DecorateMasterPanel(JPanel p)
  {
    p.setBackground(Color.BLACK);
    p.setBorder(new LineBorder(Color.BLUE,5,true));
  }

  public static void DecoratePopAndLockHandle(JLabel l)
  {
    l.setForeground(Color.GREEN);
  }

  public static JLabel MakeDecoratedLabel()
  {
    JLabel result = new JLabel();
    result.setForeground(Color.WHITE);
    return result;
  }

  public static JLabel MakeDecoratedLabel(String text)
  {
    JLabel result = new JLabel(text);
    result.setForeground(Color.WHITE);
    return result;
  }

  
  public static JPanel MakeDecoratedPanel()
  {
    JPanel result = new JPanel();
    DecoratePanel(result);
    return result;
  }

  public static void DecoratePanel(JPanel p)
  {
    p.setBackground(Color.BLACK);
    p.setBorder(BorderFactory.createEmptyBorder(10,10,10,10));
  }

  
  public ControlPanel(PopAndLockPanel palp,String dir,ActionTransferManager atm,
                      PlayerData pdata,OverrideSpaceData spaceData,TradeBaseData tbd,Board b)
  {
    errorpanel = new ErrorPanel();

    handle = palp.Add(errorpanel,"Control Panel",500,0,false);
    DecoratePopAndLockHandle(handle);

    ipanel = new InitialPanel(pdata,atm);
    switches = new SwitchablePanel(atm);
    jfp = new JumpFlyPanel(atm);
    cfp = new CostlyFirstPanel(atm);
    cmp = new CostlyMovePanel(atm);
    eog = new EndOfGamePanel(pdata);
    mulliganPanel = new DieChoicePanel("Select Die To Reroll or <none>",
                                       "DICE","DICE.DiceString",
                                       "MULLIGAN","MULLIGAN.NumToReroll",
                                       "none","NOMULLIGAN",
                                       atm,dir);

    pilotNumberPanel = new DieChoicePanel("Select Pilot Number",
                                          "PILOTNUMBERCHOICE","PILOTNUMBERCHOICE.PilotNumbers",
                                          "SELECTPILOTNUMBER","SELECTPILOTNUMBER.PilotNumber",
                                          null,null,
                                          atm,dir);
    relicPanel = new RelicPanel(atm,pdata,spaceData);

    tradePanel = new TradePanel(atm,pdata,tbd,b,spaceData,palp,handle);


    palp.AddToLabel(handle,ipanel);
    palp.AddToLabel(handle,switches);
    palp.AddToLabel(handle,jfp);
    palp.AddToLabel(handle,cfp);
    palp.AddToLabel(handle,cmp);
    palp.AddToLabel(handle,eog);
    palp.AddToLabel(handle,mulliganPanel);
    palp.AddToLabel(handle,pilotNumberPanel);
    palp.AddToLabel(handle,relicPanel);
    palp.AddToLabel(handle,tradePanel);
  }

  void HandleEvent(GameEvent ev)
  {
    String ename = ev.GetVarValue("eventname");
    
    if (ename.equals("MOVERROR"))
    {
      errorpanel.raise(ev.GetVarValue("MOVERROR.Error"));
    }
    else if (ename.equals("NEWSTATE"))
    {
      jfp.lower();
      cfp.lower();
      cmp.lower();
    }


    ipanel.HandleEvent(ev);
    switches.HandleEvent(ev);
    eog.HandleEvent(ev);
    mulliganPanel.HandleEvent(ev);
    pilotNumberPanel.HandleEvent(ev);
    tradePanel.HandleEvent(ev);
  }

  void Reset()
  {
    errorpanel.lower();
    jfp.lower();
    cfp.lower();
    cmp.lower();
    ipanel.Reset();
    eog.Reset();
    mulliganPanel.Reset();
    pilotNumberPanel.Reset();
    tradePanel.Reset();
  }

  public void raiseJumpFly(String destination,EnabledPanel epanel)
  {
    jfp.raise(destination,epanel);
  }

  public void raiseCostlyFirst(int penalty,EnabledPanel epanel)
  {
    cfp.raise(penalty,epanel);
  }
  
  public void raiseCostlyMove(int penalty,Functor callOnMove,EnabledPanel epanel)
  {
    cmp.raise(penalty,callOnMove,epanel);
  }
}


    
  