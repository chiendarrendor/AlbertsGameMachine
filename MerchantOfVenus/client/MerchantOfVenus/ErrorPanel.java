package MerchantOfVenus;

import java.awt.event.*;
import javax.swing.*;

public class ErrorPanel extends JPanel implements ActionListener
{

	private static final long serialVersionUID = 1L;
	JLabel errLabel;

  public ErrorPanel()
  {
    ControlPanel.DecorateMasterPanel(this);

    SpringLayout springs = new SpringLayout();
    setLayout(springs);
    errLabel = ControlPanel.MakeDecoratedLabel();
    add(errLabel);
    springs.putConstraint(SpringLayout.NORTH,errLabel,
                          20,
                          SpringLayout.NORTH,this);
    springs.putConstraint(SpringLayout.WEST,errLabel,
                          10,
                          SpringLayout.WEST,this);
    springs.putConstraint(SpringLayout.EAST,this,
                          10,
                          SpringLayout.EAST,errLabel);
    JButton closebutton = new JButton("OK");
    closebutton.addActionListener(this);
    add(closebutton);
    springs.putConstraint(SpringLayout.NORTH,closebutton,
                          20,
                          SpringLayout.SOUTH,errLabel);
    springs.putConstraint(SpringLayout.EAST,closebutton,
                          0,
                          SpringLayout.EAST,errLabel);
    springs.putConstraint(SpringLayout.SOUTH,this,
                          20,
                          SpringLayout.SOUTH,closebutton);
  }

  public void raise(String text)
  {
    errLabel.setText(text);
    validate();
    setVisible(true);
  }

  public void lower()
  {
    setVisible(false);
  }
      
  public void actionPerformed(ActionEvent e)
  {
    lower();
  }
}
