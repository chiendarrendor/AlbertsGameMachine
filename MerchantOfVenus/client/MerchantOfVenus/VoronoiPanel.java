package MerchantOfVenus;
import java.awt.*;
import java.util.*;
import javax.swing.*;

public class VoronoiPanel extends Panel
{
    public SolarSystemHolderDomain solarsystems;
    public Vector<SolarSystemDrawer> drawers = new Vector<SolarSystemDrawer>();
    int deltax;
    int deltay;
    int indent = 10;

    public void init(String dataloc, String ss1string, String ss2string)
    {
	solarsystems = new SolarSystemHolderDomain(dataloc);
	// expecting ss1 and ss2 to be in the form:
	// <name>:[s][S][D][V][A]:[alpha]
	// where name is a valid solar system
	// s is the spaces of the solarsystem
	// S is the extended spaces
	// - means to disable the extended spaces
	// D is the Delaunay Triangulation
	// V is the Voronoi Diagram
	// A is the alpha shape (using the given alpha)
	// alpha is only required if A is present.
	if (ss1string != null) drawers.add(new SolarSystemDrawer(solarsystems,ss1string,Color.green,15));
	if (ss2string != null) drawers.add(new SolarSystemDrawer(solarsystems,ss2string,Color.blue,30));

	int minx = Integer.MAX_VALUE;
	int miny = Integer.MAX_VALUE;
	for (SolarSystemDrawer ssd : drawers)
	{
	    int tmx = ssd.ssh.minx;
	    int tmy = ssd.ssh.miny;
	    if (tmx < minx) minx = tmx;
	    if (tmy < miny) miny = tmy;
	}
	deltax = -minx + indent;
	deltay = -miny + indent;
    }

    public void paint(Graphics g)
    {
	g.setColor(getBackground());
	g.fillRect(0,0,getSize().width,getSize().height);
	for (SolarSystemDrawer ssd : drawers)
	{
	    ssd.paint(g,deltax,deltay);
	}
    }

    private static void createAndShowGUI(String dataloc, String ss1string, String ss2string)
    {
	JFrame frame = new JFrame("Voronoi Panel");
	frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

	VoronoiPanel va = new VoronoiPanel();
	va.init(dataloc, ss1string, ss2string);

	frame.getContentPane().add(va);
	frame.pack();
	frame.setVisible(true);
    }

    public static void main(String[] args)
    {
	if (args.length < 1)
	{
	    System.out.println("usage: VoronoiPanel <dataloc> [ss1string] [ss2string]");
	    System.exit(1);
	}

	final String dataloc = args[0];
	final String ss1string = args.length > 1 ? args[1] : null;
	final String ss2string = args.length > 2 ? args[2] : null;

	javax.swing.SwingUtilities.invokeLater(new Runnable()
	{
	    public void run()
	    {
		createAndShowGUI(dataloc, ss1string, ss2string);
	    }
	});
    }
}
