package MerchantOfVenus;

import java.awt.event.*;
import java.awt.*;
import javax.swing.*;
import java.util.*;
import NodeInterfacePackage.*;

class SwitchablePanel extends JPanel implements ActionGenerator,ActionListener
{
	private static final long serialVersionUID = 1L;
ActionTransferManager atm;

  private class OnOffPanel extends JPanel
  {
	private static final long serialVersionUID = 1L;

	boolean isOn;

    JButton onButton;
    JButton offButton;
    Color defColor;

    private void flip()
    {
      onButton.setBackground(isOn ? Color.GREEN : defColor);
      offButton.setBackground(isOn ? defColor : Color.GREEN);
    }

    public OnOffPanel(String title)
    {
      ControlPanel.DecoratePanel(this);
      setLayout(new BoxLayout(this,BoxLayout.Y_AXIS));
      JLabel jl = ControlPanel.MakeDecoratedLabel(title);
      add(jl);
      onButton = new JButton("ON");
      defColor = onButton.getBackground();
      onButton.addActionListener(new ActionListener() { public void actionPerformed(ActionEvent e) { isOn = true;  flip(); }});
      offButton = new JButton("OFF");
      offButton.addActionListener(new ActionListener() { public void actionPerformed(ActionEvent e) { isOn = false;  flip(); }});
      add(onButton);
      add(offButton);
      onButton.setOpaque(true);
      offButton.setOpaque(true);
    

      isOn = true;
      flip();
    }

    public boolean IsOn() { return isOn; }
  }


  private Map<String,OnOffPanel> switches;


  public SwitchablePanel(ActionTransferManager i_atm)
  {
    atm = i_atm;
    setVisible(false);
    ControlPanel.DecorateMasterPanel(this);
    setLayout(new BorderLayout());

    JLabel legend = ControlPanel.MakeDecoratedLabel("Component Power Control");

    add(legend,BorderLayout.NORTH);

    JPanel switchPanel = ControlPanel.MakeDecoratedPanel();
    switchPanel.setLayout(new BoxLayout(switchPanel,BoxLayout.X_AXIS));
    add(switchPanel,BorderLayout.CENTER);
    switches = new HashMap<String,OnOffPanel>();
    switches.put("YD",new OnOffPanel("Yellow Drive"));
    switches.put("RD",new OnOffPanel("Red Drive"));
    switches.put("CD",new OnOffPanel("Combo Drive"));
    switches.put("AP",new OnOffPanel("Auto Pilot"));
    switches.put("GL",new OnOffPanel("Gate Lock"));
    switches.put("JS",new OnOffPanel("Jump Start"));
    for (OnOffPanel oop : switches.values()) switchPanel.add(oop);

    JPanel buttonPanel = ControlPanel.MakeDecoratedPanel();
    buttonPanel.setLayout(new BoxLayout(buttonPanel,BoxLayout.X_AXIS));
    add(buttonPanel,BorderLayout.SOUTH);
    JButton activate = new JButton("Engage!");
    buttonPanel.add(Box.createHorizontalGlue());
    buttonPanel.add(activate);
    activate.addActionListener(this);

    atm.AddButtonPresser("SELECTSWITCHABLES",this);
  }

  public void HandleEvent(GameEvent ev)
  {
    if (!ev.GetVarValue("eventname").equals("SWITCHABLES")) return;
    for (OnOffPanel oop : switches.values()) oop.setVisible(false);
    String[] ss = ev.GetVarValue("SWITCHABLES.Switchables").split("-");
    for (String s : ss) switches.get(s).setVisible(true);
  }

  public void actionPerformed(ActionEvent e) 
  { 
    StringBuffer result = new StringBuffer();
    boolean first = true;

    for (Map.Entry<String,OnOffPanel> kvp : switches.entrySet())
    {
      if (!kvp.getValue().isVisible()) continue;
      if (!kvp.getValue().IsOn()) continue;
      if (!first) result.append("-");
      first = false;
      result.append(kvp.getKey());
    }

    if (first == true) result.append("-");

    atm.ReportNewValueToConsumers("SELECTSWITCHABLES.Switchables",result.toString());
    atm.ButtonPressed("SELECTSWITCHABLES"); 
  }


  public void ActionLegalityChanged(boolean i_islegal) 
  { 
    setVisible(i_islegal); 
  }
  public void ActionReadinessChanged(boolean i_isready) {}
}

