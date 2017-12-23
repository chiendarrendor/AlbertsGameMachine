package MerchantOfVenus;

import java.awt.event.*;
import java.awt.*;
import javax.swing.*;
import java.util.*;
import NodeInterfacePackage.*;

class InitialPanel extends JPanel implements PlayerData.PlayerListener
{
	private static final long serialVersionUID = 1L;
Map<Integer,Player> players;
  int iam;
  boolean inInitial;
  ActionTransferManager atm;
  JButton[] endButtons;
  Color defColor;
  JLabel playerNames[];

  private class DoButton extends JButton implements ActionListener,ActionGenerator
  {
	private static final long serialVersionUID = 1L;
	String action;
    public DoButton(String prompt,String action,ActionTransferManager i_atm)
    {
      super(prompt);
      setVisible(false);
      this.action = action;
      i_atm.AddButtonPresser(action,this);
      addActionListener(this);
    }
    public void actionPerformed(ActionEvent e) { atm.ButtonPressed(action); }
    public void ActionLegalityChanged(boolean i_islegal) { setVisible(i_islegal); }
    public void ActionReadinessChanged(boolean i_isready) {}
  }

  public InitialPanel(PlayerData pdata,ActionTransferManager i_atm)
  {
    pdata.addPlayerListener(this,false);
    atm = i_atm;

    ControlPanel.DecorateMasterPanel(this);
    setLayout(new BorderLayout());

    JPanel westPanel = ControlPanel.MakeDecoratedPanel();
    westPanel.setLayout(new BoxLayout(westPanel,BoxLayout.Y_AXIS));
    add(westPanel,BorderLayout.WEST);

    JLabel winMoney = ControlPanel.MakeDecoratedLabel("Money to Win:");
    westPanel.add(winMoney);
    endButtons = new JButton[3];
    endButtons[0] = new JButton("1000");
    endButtons[1] = new JButton("2000");
    endButtons[2] = new JButton("3000");
    defColor = endButtons[0].getBackground();
    for (JButton b : endButtons) { b.setOpaque(true); }

    playerNames = new JLabel[6];
    JPanel players = ControlPanel.MakeDecoratedPanel();

    add(players,BorderLayout.CENTER);
    players.setLayout(new GridLayout(7,1,5,5));
    players.setBorder(BorderFactory.createEmptyBorder(10,10,10,10));

    JLabel pHeader = ControlPanel.MakeDecoratedLabel("Players:");
    players.add(pHeader);
    for (int i = 0 ; i < 6 ; ++i)
    {
      playerNames[i] = ControlPanel.MakeDecoratedLabel();
      players.add(playerNames[i]);
    }

    ActionListener al = 
      new ActionListener()
      {
        public void actionPerformed(ActionEvent e)
        {
          atm.ReportNewValueToConsumers("OPTIONS.WinMoney",e.getActionCommand());
          atm.ButtonPressed("OPTIONS");
        }
      };

    for (JButton b : endButtons) 
    { 
      westPanel.add(b); 
      b.setActionCommand(b.getText()); 
      b.addActionListener(al); 
    }

    JPanel southPanel = ControlPanel.MakeDecoratedPanel();
    southPanel.setLayout(new BoxLayout(southPanel,BoxLayout.X_AXIS));
    add(southPanel,BorderLayout.SOUTH);

    southPanel.add(new DoButton("Join the Game","JOIN",i_atm));
    southPanel.add(new DoButton("Leave the Game","UNJOIN",i_atm));
    southPanel.add(new DoButton("Start the Game","VALIDATESETUP",i_atm));
    southPanel.add(new DoButton("Accept Options and Players","LIKEOPTIONS",i_atm));
    southPanel.add(new DoButton("Alter Options or Players","DONTLIKEOPTIONS",i_atm));

    Reset();
  }




  public void HandleEvent(GameEvent ev)
  {
    String ename = ev.GetVarValue("eventname");
    if (ename.equals("NEWSTATE"))
    {
      inInitial = ev.GetVarValue("statename").equals("InitialState");
      boolean isValidate = ev.GetVarValue("statename").equals("ValidateSetup");
      if (!inInitial && !isValidate)
      {
        setVisible(false);
        return;
      }
      setVisible(true);
      for (JButton b : endButtons) 
      { 
        b.setEnabled(inInitial && iam != -1); 
      }

      if (inInitial) for (JLabel pn : playerNames) { pn.setForeground(Color.WHITE);}


    }
    if (ename.equals("OPTIONS"))
    {
      String amt = ev.GetVarValue("OPTIONS.WinMoney");
      for (JButton b : endButtons) { b.setBackground(defColor); }

      if (amt.equals("1000")) endButtons[0].setBackground(Color.GREEN);
      else if (amt.equals("2000")) endButtons[1].setBackground(Color.GREEN);
      else if (amt.equals("3000")) endButtons[2].setBackground(Color.GREEN);
      else System.out.println("got unexpected WinMoney: " + amt);
    }
  }


  public void Reset()
  {
    players = new HashMap<Integer,Player>();
    iam = -1;
    setVisible(false);
    inInitial = false;
    for (JButton b : endButtons) { b.setBackground(defColor); b.setEnabled(false); }
    for (JLabel pn : playerNames) { pn.setText(""); pn.setForeground(Color.WHITE);}

  }

  public void PlayerChanged(Player p,int iam,int[] turnorder,int curplayer)
  {
    this.iam = iam;
    
    for (JButton b : endButtons) 
    { 
      b.setEnabled(inInitial && iam != -1); 
    }
    

    if (p == null) return;
    Integer key = new Integer(p.GetId());
    if (!players.containsKey(key)) 
    {
      players.put(key,p);
      playerNames[p.GetId()].setText(p.GetName());
    }

    playerNames[p.GetId()].setForeground(p.OkStartup() ? Color.GREEN: Color.WHITE);
  }

}