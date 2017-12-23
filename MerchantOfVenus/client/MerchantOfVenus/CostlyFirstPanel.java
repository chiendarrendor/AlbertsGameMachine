package MerchantOfVenus;

import java.awt.event.*;
import java.awt.*;
import javax.swing.*;
import NodeInterfacePackage.*;

class CostlyFirstPanel extends JPanel 
{
	private static final long serialVersionUID = 1L;
ActionTransferManager atm;
  JLabel legend;
  EnabledPanel epanel;


  public CostlyFirstPanel(ActionTransferManager i_atm)
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
    JButton endButton = new JButton("End Turn Anyway");
    JButton noEndButton = new JButton("Move Along");
    buttonPanel.add(Box.createHorizontalGlue());
    buttonPanel.add(endButton);
    buttonPanel.add(Box.createRigidArea(new Dimension(10, 0)));
    buttonPanel.add(noEndButton);
    endButton.setActionCommand("end");
    noEndButton.setActionCommand("noend");

    ActionListener al = 
      new ActionListener()
      {
        public void actionPerformed(ActionEvent e)
        {
          if (epanel == null) throw new RuntimeException("Jump Fly active with no epanel!");
          if (e.getActionCommand().equals("end"))
          {
            atm.ButtonPressed("ENDMOVE");
          }
          lower();
        }
      };
    endButton.addActionListener(al);
    noEndButton.addActionListener(al);
    setVisible(false);
  }

  public void raise(int penalty,EnabledPanel ep)
  {
    if (epanel != null) throw new RuntimeException("Jump Fly raise called multiple");
    epanel = ep;
    epanel.disable();
    legend.setText("Ending your move will cost you " + penalty + " credits. ");
    setVisible(true);
  }

  public void lower()
  {
    if (epanel != null) epanel.enable();
    epanel = null;
    setVisible(false);
  }
}

    




    

