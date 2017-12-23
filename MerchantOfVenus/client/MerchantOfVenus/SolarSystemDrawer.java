package MerchantOfVenus;

import java.awt.*;

class SolarSystemDrawer
{
    private String badString = null;
    private Color baseColor;
    private int badY;
    private boolean drawSpaces;
    private boolean drawExtended;
    private boolean drawDelaunay;
    private boolean drawVoronoi;
    private boolean drawAlpha;
    private double alphaValue;
    public SolarSystemHolder ssh;

    public SolarSystemDrawer(SolarSystemHolderDomain sshd,
			     String param,Color bc,int ey)
    {
	baseColor = bc;
	badY = ey;
	String[] parts = param.split(":");
	if (parts.length != 2 && parts.length != 3)
	{
	    badString="bad input parameter";
	    return;
	}

	if (parts[0] == null || parts[1] == null)
	{
	    badString="bad input parameter, null parts";
	    return;
	}

	if (!sshd.GetSolarSystemHolders().containsKey(parts[0]))
	{
	    badString="unknown solar system " + parts[0];
	    return;
	}
	ssh = sshd.GetSolarSystemHolders().get(parts[0]);

	drawSpaces = parts[1].indexOf('s') != -1;
	drawExtended = parts[1].indexOf('S') != -1;
	drawDelaunay = parts[1].indexOf('D') != -1;
	drawVoronoi = parts[1].indexOf('V') != -1;
	drawAlpha = parts[1].indexOf('A') != -1;

	if  (parts[1].indexOf('-') != -1)
	{
	    ssh.disableExtensions();
	}

	if (drawAlpha)
	{
	    if (parts.length != 3 || parts[2] == null)
	    {
		badString="missing alpha value";
		return;
	    }

	    try
	    {
		alphaValue = Double.parseDouble(parts[2]);
	    }
	    catch(NumberFormatException nfx)
	    {
		badString = "bad alpha value";
		return;
	    }
	}
    }

    public void paint(Graphics g,int dx,int dy)
    {
	if (badString != null)
	{
	    g.setColor(baseColor);
	    g.drawString(badString,15,badY);
	    return;
	}
	
	if (drawDelaunay)
	{
	    g.setColor(Color.black);
	    for (Voronoi.Edge vs : ssh.voronoi.GetEdges())
	    {
		g.drawLine((int)vs.site1.x+dx,
			   (int)vs.site1.y+dy,
			   (int)vs.site2.x+dx,
			   (int)vs.site2.y+dy);
	    }
	}

	// drawVoronoi

	if (drawAlpha)
	{
	    g.setColor(Color.red);
	    for (Voronoi.Edge vs : ssh.voronoi.GetEdges())
	    {
		if (!vs.InAlphaBounds(alphaValue)) continue;
		g.drawLine((int)vs.site1.x+dx,
			   (int)vs.site1.y+dy,
			   (int)vs.site2.x+dx,
			   (int)vs.site2.y+dy);
	    }
	}

	if (drawSpaces)
	{
	    g.setColor(baseColor);
	    for (Voronoi.Point vp : ssh.spaces)
	    {
		g.fillRect((int)vp.x-4+dx,
			   (int)vp.y-4+dy,
			   9,9);
	    }
	}
	
	if (drawExtended)
	{
	    g.setColor(baseColor.darker());
	    for (Voronoi.Point vp : ssh.extensions)
	    {
		g.fillRect((int)vp.x-2+dx,
			   (int)vp.y-2+dy,
			   5,5);
	    }
	}	    
    }
}
	


	
	