package MerchantOfVenus;

import java.awt.event.*;
import java.awt.*;
import javax.swing.*;
import NodeInterfacePackage.*;

public class CostlyMovePanel extends JPanel 
{
	private static final long serialVersionUID = 1L;
ActionTransferManager atm;
  JLabel legend;
  EnabledPanel epanel;
  Functor func;


  public CostlyMovePanel(ActionTransferManager i_atm)
  {
    atm = i_atm;
    epanel = null;
    ControlPanel.DecorateMasterPanel(this);
    setLayout(new BorderLayout());
    
    legend = ControlPanel.MakeDecoratedLabel("");
    add(legend,BorderLayout.CENTER);

    JPanel buttonPanel = ControlPanel.MakeDecoratedPanel();
    buttonPanel.setLayout(new BoxLayout(buttonPanel,BoxLayout.X_AXIS));
    add(buttonPanel,BorderLayout.SOUTH);

    // via telegate
    // along path
    JButton goButton = new JButton("Move Along Anyway");
    JButton stopButton = new JButton("End Turn");
    buttonPanel.add(Box.createHorizontalGlue());
    buttonPanel.add(goButton);
    buttonPanel.add(Box.createRigidArea(new Dimension(10, 0)));
    buttonPanel.add(stopButton);
    goButton.setActionCommand("go");
    stopButton.setActionCommand("stop");

    ActionListener al = 
      new ActionListener()
      {
        public void actionPerformed(ActionEvent e)
        {
          if (epanel == null) throw new RuntimeException("Jump Fly active with no epanel!");
          if (e.getActionCommand().equals("stop"))
          {
            atm.ButtonPressed("ENDMOVE");
            lower();
          }
          else
          {
            func.execute();
            lower();
          }
        }
      };
    goButton.addActionListener(al);
    stopButton.addActionListener(al);
    setVisible(false);
  }

  public void raise(int penalty,Functor goFunc,EnabledPanel ep)
  {
    if (epanel != null) throw new RuntimeException("Jump Fly raise called multiple");
    epanel = ep;
    func = goFunc;
    epanel.disable();
    legend.setText("continuing your move will cost you " + penalty + " credits. ");
    setVisible(true);
  }

  public void lower()
  {
    if (epanel != null) epanel.enable();
    epanel = null;
    func = null;
    setVisible(false);
  }
}

    




    

