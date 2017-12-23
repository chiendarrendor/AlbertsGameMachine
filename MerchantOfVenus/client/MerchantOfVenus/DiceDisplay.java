package MerchantOfVenus;

import java.awt.*;
import javax.swing.*;
import java.util.*;
import MySwing.*;
import NodeInterfacePackage.*;

public class DiceDisplay extends JPanel
{
	private static final long serialVersionUID = 1L;
JLabel handle;
  JLabel firstPrompt;
  JLabel mpleft;
  JLabel[] diceLabels;

  
  int[] dice;
  int curplayer;
  int iam;
  boolean isFirst;
  int pilotNum;
  boolean isPermanent;

  Map<String,ImageIcon> dieicons;

  public DiceDisplay(PopAndLockPanel palp,String dir)
  {
    dieicons = new HashMap<String,ImageIcon>();
    dieicons.put("0",ImageLoader.MakeImageIcon(dir+"/bigdieq.png"));
    dieicons.put("1",ImageLoader.MakeImageIcon(dir+"/bigdie1.png"));
    dieicons.put("2",ImageLoader.MakeImageIcon(dir+"/bigdie2.png"));
    dieicons.put("3",ImageLoader.MakeImageIcon(dir+"/bigdie3.png"));
    dieicons.put("4",ImageLoader.MakeImageIcon(dir+"/bigdie4.png"));
    dieicons.put("5",ImageLoader.MakeImageIcon(dir+"/bigdie5.png"));
    dieicons.put("6",ImageLoader.MakeImageIcon(dir+"/bigdie6.png"));

    handle = palp.Add(this,"Dice",500,PopAndLockPanel.OTHEREXTREME,false);
    handle.setForeground(Color.GREEN);

    setLayout(new BorderLayout());
    JPanel dPanel = new JPanel();
    dPanel.setLayout(new BoxLayout(dPanel,BoxLayout.X_AXIS));
    add(dPanel,BorderLayout.CENTER);
    firstPrompt = new JLabel("Select First Space");
    add(firstPrompt,BorderLayout.SOUTH);
    firstPrompt.setVisible(false);

    diceLabels = new JLabel[5];
    for (int i = 0 ; i < 5 ; ++i)
    {
      diceLabels[i] = new JLabel(dieicons.get("?"));
      dPanel.add(diceLabels[i]);
      diceLabels[i].setVisible(true);
    }
    mpleft = new JLabel("? MP");
    dPanel.add(mpleft);

    Reset();
  }

  public void Reset()
  {
    this.setVisible(false);
    curplayer = -2;
    iam = -1;
    isFirst = false;
    pilotNum = -1;
    isPermanent = false;
    dice = null;
  }

  public void HandleEvent(GameEvent ev)
  {
    String ename = ev.GetVarValue("eventname");
    if (ename.equals("NEWSTATE"))
    {
      String sname = ev.GetVarValue("statename");
      boolean isVisible = 
        sname.equals("SelectMove") || 
        sname.equals("SelectPilotNumber") ||
        sname.equals("Mulligan");
      setVisible(isVisible);
    }
    else if (ename.equals("DICE"))
    {
      String dstring = ev.GetVarValue("DICE.DiceString");
      dice = new int[dstring.length()];
      for (int i = 0 ; i < dstring.length() ; ++i)
      {
        String sub = dstring.substring(i,i+1);
        if (sub.equals("?")) mpleft.setText("? MP");
        dice[i] = sub.equals("?") ? 0 : Integer.parseInt(sub);
      }
      refreshDice();
    }
    else if (ename.equals("MPLEFT"))
    {
      String mpl = ev.GetVarValue("MPLEFT.MpLeft");
      mpleft.setText(mpl + " MP");
    }
    else if (ename.equals("IAM"))
    {
      iam = ev.asInt("IAM.PlayerId");
    }
    else if (ename.equals("CURPLAYER"))
    {
      curplayer = ev.asInt("CURPLAYER.PlayerId");
    }
    else if (ename.equals("PILOTNUMBERCHOSEN"))
    {
      pilotNum = ev.asInt("PILOTNUMBERCHOSEN.PilotNumber");
      if (pilotNum == 0) pilotNum = -1;
      isPermanent = ev.GetVarValue("PILOTNUMBERCHOSEN.IsPermanent").equals("1");
      refreshDice();
    }
    
    firstPrompt.setVisible(isFirst && iam == curplayer);
  }


  private void refreshDice()
  {
    for (int i = 0 ; i < diceLabels.length ; ++i)
    {
      if (dice != null && i <= dice.length - 1)
      {
        diceLabels[i].setVisible(true);
        ImageIcon ii = dieicons.get(""+dice[i]);
        if (pilotNum == dice[i]) 
          ii = IconManager.RecolorIcon(ii,Color.WHITE,isPermanent ? Color.RED : Color.GREEN);
        diceLabels[i].setIcon(ii);
      }
      else
      {
        diceLabels[i].setVisible(false);
      }
    }
  }      
      
                 


}