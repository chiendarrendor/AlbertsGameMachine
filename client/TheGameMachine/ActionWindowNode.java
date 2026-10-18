package TheGameMachine;
import javax.swing.*;
import java.awt.*;
import java.util.*;

import NodeInterfacePackage.GameEvent;

public class ActionWindowNode extends GameNode
{
  JFrame m_frame;
  JLabel m_StateText;

  public ActionWindowNode(String i_Name)
  {
    super(null,"");

    m_frame = new JFrame(i_Name + " Action Selection");

    m_frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

    JPanel panel = new JPanel();
    panel.setOpaque(false);
    JScrollPane scrollp = new JScrollPane(panel);
    scrollp.setOpaque(false);

    m_frame.getContentPane().add(scrollp);

    panel.setLayout(new BorderLayout());

    m_StateText = new JLabel();
    m_StateText.setText("Current State:");
	
    panel.add(m_StateText,BorderLayout.SOUTH);

    JPanel mainpanel = new JPanel();
    mainpanel.setOpaque(false);

    panel.add(mainpanel,BorderLayout.CENTER);

    mainpanel.setLayout(new BoxLayout(mainpanel,BoxLayout.Y_AXIS));

    m_frame.setVisible(true);
	
    SetComponent(m_frame,mainpanel);
  }

  void RefreshMe()
  {
    int i;

    // the following code ensures that there are at least
    // 10 (numToReset) actions in this ActionWindowNode 
    // that are visible prior to pack() (which will 
    // fix the window size nicely), then invisibles 
    // all those that were made visible for this purpose.
    int numToReset = 10;
    ArrayList<GameNode> canreset= new ArrayList<GameNode>();
    ArrayList<GameNode> resets = new ArrayList<GameNode>();

    for (i = 0 ; i < this.GetNumChildren() ; ++i)
    {
      GameNode gn = this.GetChild(i);
      if (gn.IsVisible())
      {
        numToReset--;
      }
      else
      {
        canreset.add(gn);
      }
    }

    for (i = numToReset ; i >= 0 && !canreset.isEmpty() ; i--)
    {
      GameNode gn = canreset.remove(0);
      gn.AlterVisibility(true);
      resets.add(gn);
    }

    m_frame.pack();

    while(!resets.isEmpty())
    {
      GameNode gn = resets.remove(0);
      gn.AlterVisibility(false);
    }
  }

  public void MyHandleEvent(GameEvent i_event)
  {
      m_StateText.setText("Current State: " + i_event.getGPP().getStateDesc());
  }

  public void dispose()
  {
    m_frame.dispose();
  }

}
