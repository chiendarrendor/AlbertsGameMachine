package MerchantOfVenus;
import java.awt.*;
import java.applet.Applet;
import java.util.*;

public class VoronoiApplet extends Applet
{
    public SolarSystemHolderDomain solarsystems;
    public Vector<SolarSystemDrawer> drawers = new Vector<SolarSystemDrawer>();
    int deltax;
    int deltay;
    int indent = 10;

    public void init()
    {
	String dataloc = getParameter("dataloc");

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
	String ss1string = getParameter("ss1");
	String ss2string = getParameter("ss2");
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

    public void stop()
    {
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
}
