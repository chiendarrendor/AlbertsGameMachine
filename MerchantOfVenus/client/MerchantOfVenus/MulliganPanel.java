package MerchantOfVenus;

import java.awt.event.*;
import javax.swing.*;
import NodeInterfacePackage.*;

public class MulliganPanel extends JPanel implements ActionGenerator,ActionListener
{
	private static final long serialVersionUID = 1L;
ActionTransferManager atm;

  public MulliganPanel(ActionTransferManager atm)
  {
    this.atm = atm;
    atm.AddButtonPresser("MULLIGAN",this);




    setVisible(false);
  }

  public void actionPerformed(ActionEvent e)
  {

    setVisible(false);
  }

  public void Reset() { setVisible(false); }
  public void ActionLegalityChanged(boolean i_legal) { setVisible(i_legal); }
  public void ActionReadinessChanged(boolean i_legal) {}
}

  




