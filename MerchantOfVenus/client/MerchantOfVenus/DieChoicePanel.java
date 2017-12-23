package MerchantOfVenus;

import java.awt.event.*;
import java.awt.*;
import javax.swing.*;
import NodeInterfacePackage.*;
import MySwing.*;

class DieChoicePanel extends JPanel implements ActionGenerator
{
	private static final long serialVersionUID = 1L;
ActionTransferManager atm;
  JButton[] diebuttons;
  String oav;
  String oa;
  String dna;
  String ide;
  String idev;

  public DieChoicePanel(String prompt,
                        String inputDieEvent,String inputDieEventVar,
                        String outputAction, String outputActionVar,
                        String doNothingText, String doNothingAction,
                        ActionTransferManager i_atm,
                        String dir)
  {
    atm = i_atm;
    oav = outputActionVar;
    oa = outputAction;
    dna = doNothingAction;
    ide = inputDieEvent;
    idev = inputDieEventVar;

    atm.AddButtonPresser(outputAction,this);

    ControlPanel.DecorateMasterPanel(this);
    setLayout(new BorderLayout());
    add(ControlPanel.MakeDecoratedLabel(prompt),BorderLayout.NORTH);

    JPanel buttonpanel = ControlPanel.MakeDecoratedPanel();
    add(buttonpanel,BorderLayout.CENTER);
    
    GridLayout gl = new GridLayout(1,7);
    buttonpanel.setLayout(gl);
    gl.setHgap(5);

    diebuttons = new JButton[7];
    for (int i = 1 ; i <= 6 ; ++i)
    {
      diebuttons[i] = new JButton(ImageLoader.MakeImageIcon(dir+"/bigdie"+i+".png"));
      buttonpanel.add(diebuttons[i]);
      diebuttons[i].setActionCommand(""+i);

      diebuttons[i].addActionListener(
                                      new ActionListener()
                                      {
                                        public void actionPerformed(ActionEvent e)
                                        {
                                          atm.ReportNewValueToConsumers(oav,e.getActionCommand());
                                          atm.ButtonPressed(oa);
                                          DieChoicePanel.this.setVisible(false);
                                        }
                                      }
                                      );
    }
    if (doNothingText != null && doNothingAction != null)
    {
      JButton noButton = new JButton(doNothingText);
      noButton.addActionListener(
                                 new ActionListener()
                                 {
                                   public void actionPerformed(ActionEvent e)
                                   {
                                     atm.ButtonPressed(dna);
                                     DieChoicePanel.this.setVisible(false);
                                   }
                                 }
                                 );
      buttonpanel.add(noButton);
    }

    Reset();
  }

  public void HandleEvent(GameEvent e)
  {
    if (e.GetVarValue("eventname").equals(ide))
    {
      String ds = e.GetVarValue(idev);
      for (int i = 1 ; i <= 6 ; ++i) diebuttons[i].setVisible(false);
      
      for (int i = 0 ; i < ds.length() ; ++i)
      {
        String ss = ds.substring(i,i+1);
        if (ss.equals("?")) continue;
        diebuttons[Integer.parseInt(ss)].setVisible(true);
      }
    }
  } 
    
  public void Reset() 
  { 
    for (int i = 1 ; i <= 6 ; ++i) diebuttons[i].setVisible(false);
    setVisible(false); 
  }

  public void ActionLegalityChanged(boolean i_legal) { setVisible(i_legal); }
  public void ActionReadinessChanged(boolean i_legal) {}
}
 
                        
  