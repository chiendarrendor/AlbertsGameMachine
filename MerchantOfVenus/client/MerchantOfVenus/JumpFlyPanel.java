package MerchantOfVenus;

import java.awt.event.*;
import java.awt.*;
import javax.swing.*;
import NodeInterfacePackage.*;

class JumpFlyPanel extends JPanel 
{
	private static final long serialVersionUID = 1L;
ActionTransferManager atm;
  String location;
  EnabledPanel epanel;


  public JumpFlyPanel(ActionTransferManager i_atm)
  {
    atm = i_atm;
    epanel = null;
    ControlPanel.DecorateMasterPanel(this);
    setLayout(new BorderLayout());
    
    JLabel legend = ControlPanel.MakeDecoratedLabel("Select Transit Type");
    add(legend,BorderLayout.CENTER);

    JPanel buttonPanel = ControlPanel.MakeDecoratedPanel();
    buttonPanel.setLayout(new BoxLayout(buttonPanel,BoxLayout.X_AXIS));
    add(buttonPanel,BorderLayout.SOUTH);

    // via telegate
    // along path
    JButton jumpButton = new JButton("Via Telegate");
    JButton flyButton = new JButton("Along Path");
    JButton cancelButton = new JButton("Cancel");
    buttonPanel.add(Box.createHorizontalGlue());
    buttonPanel.add(cancelButton);
    buttonPanel.add(Box.createRigidArea(new Dimension(10, 0)));
    buttonPanel.add(jumpButton);
    buttonPanel.add(Box.createRigidArea(new Dimension(10, 0)));
    buttonPanel.add(flyButton);
    jumpButton.setActionCommand("jump");
    flyButton.setActionCommand("fly");
    cancelButton.setActionCommand("cancel");

    ActionListener al = 
      new ActionListener()
      {
        public void actionPerformed(ActionEvent e)
        {
          if (epanel == null) throw new RuntimeException("Jump Fly active with no epanel!");
          if (!e.getActionCommand().equals("cancel"))
          {
            int isJump = e.getActionCommand().equals("jump") ? 1 : 0;
            atm.ReportNewValueToConsumers("SELECTDESTINATION.Destination",location);
            atm.ReportNewValueToConsumers("SELECTDESTINATION.IsJump",""+isJump);
            atm.ButtonPressed("SELECTDESTINATION");
          }
          lower();
        }
      };
    jumpButton.addActionListener(al);
    flyButton.addActionListener(al);
    cancelButton.addActionListener(al);
    setVisible(false);
  }

  public void raise(String location,EnabledPanel ep)
  {
    if (epanel != null) throw new RuntimeException("Jump Fly raise called multiple");
    epanel = ep;
    epanel.disable();
    this.location = location;
    setVisible(true);
  }

  public void lower()
  {
    if (epanel != null) epanel.enable();
    epanel = null;
    setVisible(false);
  }
}

    




    

