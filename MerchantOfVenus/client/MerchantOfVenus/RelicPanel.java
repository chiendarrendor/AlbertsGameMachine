package MerchantOfVenus;

import java.awt.event.*;
import java.awt.*;
import javax.swing.*;
import NodeInterfacePackage.*;

class RelicPanel extends JPanel implements ActionGenerator
{
	private static final long serialVersionUID = 1L;
ActionTransferManager atm;
  PlayerData pdata;
  OverrideSpaceData spaceData;
  JLabel iconLabel;

  public RelicPanel(ActionTransferManager i_atm,PlayerData pdata,OverrideSpaceData spaceData)
  {
    atm = i_atm;
    atm.AddButtonPresser("GETRELIC",this);
    this.pdata = pdata;
    this.spaceData = spaceData;

    ControlPanel.DecorateMasterPanel(this);
    setLayout(new BoxLayout(this,BoxLayout.Y_AXIS));
    setVisible(false);
    
    JLabel legend = ControlPanel.MakeDecoratedLabel("Pick Up This Relic?");
    iconLabel = new JLabel();
    add(legend);
    add(Box.createRigidArea(new Dimension(0,10)));
    add(iconLabel);
    add(Box.createRigidArea(new Dimension(0,10)));

    JPanel buttonPanel = ControlPanel.MakeDecoratedPanel();
    buttonPanel.setLayout(new BoxLayout(buttonPanel,BoxLayout.X_AXIS));
    add(buttonPanel);

    JButton pickupButton = new JButton("Pick Up");
    JButton endTurnButton = new JButton("Leave Behind");
    buttonPanel.add(pickupButton);
    buttonPanel.add(Box.createRigidArea(new Dimension(10, 0)));
    buttonPanel.add(endTurnButton);
    pickupButton.setActionCommand("pickup");
    endTurnButton.setActionCommand("end");

    legend.setAlignmentX(Component.CENTER_ALIGNMENT);
    iconLabel.setAlignmentX(Component.CENTER_ALIGNMENT);
    buttonPanel.setAlignmentX(Component.CENTER_ALIGNMENT);


    ActionListener al = 
      new ActionListener()
      {
        public void actionPerformed(ActionEvent e)
        {
          if (e.getActionCommand().equals("pickup"))
          {
            atm.ButtonPressed("GETRELIC");
          }
          else
          {
            atm.ButtonPressed("LEAVERELIC");
          }
          setVisible(false);
        }
      };
    pickupButton.addActionListener(al);
    endTurnButton.addActionListener(al);
  }

  public void ActionLegalityChanged(boolean i_legal) 
  { 
    setVisible(i_legal); 
    if (!i_legal) return;
    Player curPlayer = pdata.GetCurPlayer();
    if (curPlayer == null) return;
    OverrideSpaceData.Override o = spaceData.GetOverrides().get(curPlayer.GetLocation());
    if (o == null) return;
    IconManager.Decoding d = o.icon;
    if (d == null) return;
    if (d.icon == null) return;
    iconLabel.setIcon(d.icon);
  }
  public void ActionReadinessChanged(boolean i_legal) {}
}

    




    

