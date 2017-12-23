package MerchantOfVenus;

import java.awt.*;
import javax.swing.*;
import java.util.*;
import NodeInterfacePackage.*;

public class EndOfGamePanel extends JPanel implements PlayerData.PlayerListener
{
	private static final long serialVersionUID = 1L;
JLabel[] playerNames;
  JLabel[] playerWorths;

  private class PlayerOrderable implements Comparable<PlayerOrderable>
  {
    public String name;
    public int worth;

    public int compareTo(PlayerOrderable other)
    {
      Integer thing = new Integer(worth);
      return -thing.compareTo(new Integer(other.worth));
    }
  }

  Map<Integer,PlayerOrderable> players;

  
  public EndOfGamePanel(PlayerData pdata)
  {
    ControlPanel.DecorateMasterPanel(this);
    setLayout(new BorderLayout());
    pdata.addPlayerListener(this,false);
    add(ControlPanel.MakeDecoratedLabel("The Game is Over"),BorderLayout.NORTH);
    JPanel mainPanel = ControlPanel.MakeDecoratedPanel();
    add(mainPanel,BorderLayout.CENTER);
    GridLayout gl = new GridLayout(2,6);
    mainPanel.setLayout(gl);
    gl.setHgap(5);
    gl.setVgap(5);
    playerNames = new JLabel[6];
    playerWorths = new JLabel[6];
    for (int i = 0 ; i < 6 ; ++i) { mainPanel.add(playerNames[i] = ControlPanel.MakeDecoratedLabel("")); }
    for (int i = 0 ; i < 6 ; ++i) { mainPanel.add(playerWorths[i] = ControlPanel.MakeDecoratedLabel("")); }
    

    


    Reset();
  }

  public void PlayerChanged(Player p,int iam,int[] turnorder,int curplayer)
  {
    if (p == null) return;

    Integer pI = new Integer(p.GetId());
    if (!players.containsKey(pI))
    {
      PlayerOrderable order = new PlayerOrderable();
      order.name = p.GetName();
      order.worth = p.GetWorth();
      players.put(pI,order);
    }
    players.get(pI).worth = p.GetWorth();
  }


  public void HandleEvent(GameEvent e)
  {
    if (e.GetVarValue("eventname").equals("NEWSTATE") &&
        e.GetVarValue("statename").equals("TerminalState"))
    {
      PlayerOrderable[] sorted = players.values().toArray(new PlayerOrderable[0]);
      Arrays.sort(sorted);
      for (int i = 0 ; i < sorted.length ; ++i)
      {
        playerNames[i].setText(sorted[i].name);
        playerNames[i].setVisible(true);
        playerWorths[i].setText(""+sorted[i].worth);
        playerWorths[i].setVisible(true);
      }

      setVisible(true);
    }
  }
  
  public void Reset()
  {
    players = new HashMap<Integer,PlayerOrderable>();
    for (int i = 0 ; i < 6 ; ++i)
    {
      playerNames[i].setText("");
      playerNames[i].setVisible(false);
      playerWorths[i].setText("");
      playerWorths[i].setVisible(false);
    }

    setVisible(false);
  }
}