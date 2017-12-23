package MerchantOfVenus;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import java.util.*;

import Voronoi.*;

public class VoronoiTest
{

  static JLabel alphaLabel;
  static double rlimit = 400;

  private static boolean InAlphaBounds(Voronoi.Edge e,double alpha)
  {
    return e.InAlphaBounds(alpha);
  }


  private static class DrawPanel extends JPanel implements ChangeListener,ActionListener
  {
	private static final long serialVersionUID = 1L;
	Map<String,SolarSystemHolder> solarsystems;
    String curSolarSystem;

    public DrawPanel(Map<String,SolarSystemHolder> iss)
    { 
      solarsystems = iss;
      curSolarSystem = null;
    }

    public void paint(Graphics g)
    {
      g.setColor(getBackground());
      g.fillRect(0,0,getSize().width,getSize().height);
      
      if (curSolarSystem == null || !solarsystems.containsKey(curSolarSystem)) return;
      SolarSystemHolder ssh = solarsystems.get(curSolarSystem);

      if (ssh.voronoi != null)
      {
        g.setColor(Color.WHITE);
        for (Voronoi.Edge vs : ssh.voronoi.GetEdges())
        {
          if (!InAlphaBounds(vs,rlimit))
            g.drawLine((int)vs.site1.x,(int)vs.site1.y,(int)vs.site2.x,(int)vs.site2.y);
	      }

        g.setColor(Color.RED);
        for (Voronoi.Edge vs : ssh.voronoi.GetEdges())
        {
          if (InAlphaBounds(vs,rlimit))
            g.drawLine((int)vs.site1.x,(int)vs.site1.y,(int)vs.site2.x,(int)vs.site2.y);
        }
      }

      g.setColor(Color.BLUE);
      for (Voronoi.Point vp : ssh.extensions)
      {
        g.fillRect((int)vp.x-2,(int)vp.y-2,5,5);
      }
      for (Voronoi.Point vp : ssh.spaces)
      {
        g.fillRect((int)vp.x-4,(int)vp.y-4,9,9);
      }        

    }
    public Dimension getPreferredSize()
    {
      return new Dimension(800,800);
    }

    public void stateChanged(ChangeEvent e)
    {
      JSlider source = (JSlider)e.getSource();
      rlimit = (int)source.getValue();
      alphaLabel.setText(""+rlimit);
      repaint();
    }
    
    public void actionPerformed(ActionEvent e)
    {
	JComboBox jcb = (JComboBox)e.getSource();
      curSolarSystem = (String)jcb.getSelectedItem();
      repaint();
    }
  }

  private static double radius = 70.0;
  private static double count = 16.0;
  private static double indent = 200.0;

  private static class SolarSystemHolder
  {
    public Vector<Voronoi.Point> spaces = new Vector<Voronoi.Point>();
    public Vector<Voronoi.Point> extensions = new Vector<Voronoi.Point>();
    public Fortune voronoi = null;

    int minx = Integer.MAX_VALUE;
    int miny = Integer.MAX_VALUE;

    public SolarSystemHolder() {}
    public void AddPoint(int x,int y)
    {
      if (x < minx) minx = x;
      if (y < miny) miny = y;

      spaces.add(new Voronoi.Point(x,y));
      double dtheta = 2 * Math.PI / count;
      for (double theta = 0 ; theta < 2 * Math.PI ; theta += dtheta)
      {
        Voronoi.Point cir = new Voronoi.Point(x+radius*Math.cos(theta),
                                                                y+radius*Math.sin(theta) );
        extensions.add(cir);
        if (cir.x < minx) minx = (int)cir.x;
        if (cir.y < miny) miny = (int)cir.y;
      }
    }

    public void execute()
    {
      Vector<Voronoi.Point> finalV = new Vector<Voronoi.Point>();
      for(Voronoi.Point vp : spaces)
      {
        vp.x = vp.x - minx + indent;
        vp.y = vp.y - miny + indent;
        finalV.add(vp);
      }
      for (Voronoi.Point vp : extensions)
      {
        vp.x = vp.x - minx + indent;
        vp.y = vp.y - miny + indent;
        finalV.add(vp);
      }
      
      voronoi = new Fortune(finalV);
    }
  }

  private static void createAndShowGUI()
  {
    JFrame frame = new JFrame("Voronoi Test");
    frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

    Board b = new Board("../");
    Map<String,SolarSystemHolder> solarsystems = new HashMap<String,SolarSystemHolder>();

    for (Map.Entry<String,Board.SolarSystem> ssent : b.GetSolarSystems().entrySet())
    {
      if (!ssent.getValue().IsHabitable()) continue;

      SolarSystemHolder ssh = new SolarSystemHolder();
      solarsystems.put(ssent.getValue().GetName(),ssh);
      for (Board.MapSpace ms : ssent.getValue().GetNonLoopSpaces())
      {
        ssh.AddPoint(ms.GetX(),ms.GetY());
      }
      for (int i = 1 ; i <= ssent.getValue().GetNumLoopSpaces() ; ++i)
      {
        Board.MapSpace ms = ssent.getValue().GetLoopSpace(i);
        ssh.AddPoint(ms.GetX(),ms.GetY());
      }
      ssh.execute();
    }

    SolarSystemHolder ssh = new SolarSystemHolder();
    solarsystems.put("Test",ssh);
    ssh.AddPoint(1023,304);
    ssh.AddPoint(1084,304);
    ssh.AddPoint(1131,345);
    //    ssh.AddPoint(1157,398);
    ssh.execute();

    JPanel mp = new JPanel();
    mp.setLayout(new BorderLayout());
    DrawPanel dp = new DrawPanel(solarsystems);
    mp.add(dp,BorderLayout.CENTER);

    JPanel cpanel = new JPanel();
    cpanel.setLayout(new BoxLayout(cpanel,BoxLayout.Y_AXIS));

    mp.add(cpanel,BorderLayout.SOUTH);

    String[] ssnames = new String[solarsystems.size()];
    int idx = 0;
    for (String ssname : solarsystems.keySet())
    {
      ssnames[idx++] = ssname;
    }
    JComboBox jcb = new JComboBox(ssnames);
    jcb.addActionListener(dp);
    cpanel.add(jcb);

    JSlider alphaSlider = new JSlider(JSlider.HORIZONTAL,0,1000,400);
    alphaSlider.addChangeListener(dp);
    alphaSlider.setMajorTickSpacing(100);
    alphaSlider.setMinorTickSpacing(20);
    alphaSlider.setPaintTicks(true);
    alphaSlider.setPaintLabels(true);
    cpanel.add(alphaSlider);

    alphaLabel = new JLabel(""+rlimit);
    cpanel.add(alphaLabel);

    frame.getContentPane().add(mp);
    frame.pack();
    frame.setVisible(true);
  }

  public static void main(String[] args)
  {
    javax.swing.SwingUtilities.invokeLater(new Runnable()
      {
        public void run()
        {
          createAndShowGUI();
        }
      });
  }
}
