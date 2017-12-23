package MerchantOfVenus;

import java.awt.*;
import java.applet.Applet;
import java.awt.event.*;

import javax.swing.*;
import java.util.*;

import Voronoi.*;
import Voronoi.Point;

public class VoronoiStep extends Applet
{
	private static final long serialVersionUID = 1L;
	private Fortune vf;
  private Vector<Voronoi.Point> sites = new Vector<Voronoi.Point>();
  private Map<Voronoi.Point,String> siteids = new HashMap<Voronoi.Point,String>();
  private EventPanel ep;
  private JCheckBox edgeCheck;
  private JCheckBox circleCheck;
  private JCheckBox delaunayCheck;
  private JCheckBox beachCheck;

  private class EventPanel extends JScrollPane implements IEventReporter
  {
	  JTextArea jta; 
	  
    public EventPanel()
    {
      super();
      jta = new JTextArea(40,30);
      jta.setEditable(false);
      this.setViewportView(jta);
      this.setVerticalScrollBarPolicy(VERTICAL_SCROLLBAR_ALWAYS);
    }
       
    public void Reset() { jta.setText(""); }
    
    private static final long serialVersionUID = 1;
    
    public void Step() { jta.append("--step--\n"); }
    public void ProcessSite(Point p) { jta.append("Processing Site: " + siteids.get(p) + "\n"); }
    public void DuplicatePoint(Point p) { jta.append("Point " + siteids.get(p) + " is a duplicate, skipping" + "\n"); }

    private String CString(Point pred,Point cur,Point succ)
    {
      return new String("Circle " + siteids.get(pred) + siteids.get(cur) + siteids.get(succ));
    }

    public void CircleInvalidated(Point pred, Point cur, Point succ)
    {
    	jta.append(CString(pred,cur,succ) + " is now invalid" + "\n");
    }

    private String EString(Edge e)
    {
      return new String("Edge " + siteids.get(e.site1)+siteids.get(e.site2));
    }

    public void EdgeCreated(Edge e)
    {
    	jta.append(EString(e) + " created" + "\n");      
    }

    public void EdgeTerminated(Edge e)
    {
    	jta.append(EString(e) + " terminated" + "\n");
    }

    public void EdgeExemplared(Edge e)
    {
    	jta.append(EString(e) + " exemplared" + "\n");
     
    }

    public void CircleCreated(Point pred, Point cur, Point succ)
    {
    	jta.append(CString(pred,cur,succ) + " created" + "\n");      
    }

    public void HandleCircle(Point pred, Point cur, Point succ)
    {
    	jta.append(CString(pred,cur,succ) + " handled" + "\n");
    }
  }
  
  
  

  private class DrawPanel extends JPanel implements ActionListener
  {
	  private AnimExecutor ae = null;

    private void DrawDot(Graphics g,int x,int y,int size)
    {
      g.fillOval(x-(size/2),y-(size/2),size,size);
    }

    private void DrawCircle(Graphics g, double cx, double cy , double r)
    {
      g.drawOval((int)(cx-r),(int)(cy-r),(int)(2*r),(int)(2*r));
    }

    private void FlattenTree(Voronoi.TreeNode node,Vector<Voronoi.TreeNode> list)
    {
      if (node == null) return;
      FlattenTree(node.left,list);
      list.add(node);
      FlattenTree(node.right,list);
    }
        
    public void DrawArc(Graphics g,Vector<Voronoi.TreeNode> nodes,int index,double sweep)
    {
      double miny = 0;
      double maxy = 1000;

      Voronoi.Point prev = null;
      Voronoi.Point next = null;

      Voronoi.TreeNode node = nodes.elementAt(index);
      Voronoi.Point ap = node.arcPoint;
      
      // special case:  if sweep is on arcpoint's x, we have to do something special
      if (Voronoi.MathUtilities.ddequals(sweep, ap.x))
      {    	  
    	  g.setColor(Color.CYAN);
    	  double basex;
    	  // one of several cases.
    	  // if I am first in the list, and on the line, this is the very first insert.
    	  if (node == nodes.firstElement())
    	  {
    		  basex = 0;
    	  }
    	  // if I am not first, then  someone else is, and I should be able to go 2 nodes back to get another arc.
    	  // if my predecessor is also on sweep, then we are on the first line
    	  else if (Voronoi.MathUtilities.ddequals(nodes.elementAt(index-2).arcPoint.x,sweep))
    	  {
    		  basex=0;
    	  }
    	  // otherwise, I've just broken some arc not on the sweep, and I need to find out where.
    	  else
    	  {
    		  Voronoi.Point cap = nodes.elementAt(index-2).arcPoint;
    		  basex = Voronoi.MathUtilities.GetXOfParabola(cap, sweep, ap.y);
    	  }
    		  
    	  g.drawLine((int)ap.x,(int)ap.y,(int)basex,(int)ap.y);
    	  return;
      }
      
      if (index > 0) 
      {
        prev = nodes.elementAt(index-1).leftarcPoint;
        if (Voronoi.MathUtilities.ddequals(prev.x,sweep)) miny = prev.y;
        else miny = nodes.elementAt(index-1).BreakPoint(sweep).y;

      }

      if (index < nodes.size()-1)
      {
          next = nodes.elementAt(index+1).rightarcPoint;
          if (Voronoi.MathUtilities.ddequals(next.x,sweep)) maxy = next.y;
          else maxy = nodes.elementAt(index+1).BreakPoint(sweep).y;
      }

      if (prev == null || next == null)
      {
        g.setColor(Color.BLACK);
      }
      else
      {
        // A = prev
        // B = cur
        // C = next
        double val = (ap.x-prev.x)*(next.y-prev.y) - (next.x - prev.x)*(ap.y - prev.y);
        if (val < 0) g.setColor(Color.RED);
        else if (val > 0) g.setColor(Color.GREEN);
        else g.setColor(Color.CYAN);
      }

      for (double cury = miny ; cury < maxy ; ++cury)
      {
    	  double curx = Voronoi.MathUtilities.GetXOfParabola(ap, sweep, cury);
    	  DrawDot(g,(int)curx,(int)cury,2);
      }
    }

    private void PaintSites(Graphics g)
    {
        for (Voronoi.Point vp : sites)
        {
          g.setColor(Color.BLUE);
          DrawDot(g,(int)vp.x,(int)vp.y,5);
          g.drawString(siteids.get(vp),(int)vp.x,(int)vp.y-15);
        }    	
    }
    
    
    double sweepx;
    
    private void PaintBeachLine(Graphics g,Vector<Voronoi.TreeNode> flattened)
    {		
		g.setColor(Color.YELLOW);
		g.drawLine((int)sweepx,0,(int)sweepx,getSize().height);
		
				
		for (int i = 0 ; i < flattened.size() ; ++i)
		{
			Voronoi.TreeNode node = flattened.elementAt(i);
		    if (node.arcPoint != null) 
		    {
		      DrawArc(g,flattened,i,sweepx);
		    }
		    else
		    {
		      if (Voronoi.MathUtilities.ddequals(node.leftarcPoint.x,sweepx) ||
		    	  Voronoi.MathUtilities.ddequals(node.rightarcPoint.x,sweepx)) continue;
		      Voronoi.Point p = node.BreakPoint(sweepx);
		      DrawDot(g,(int)p.x,(int)p.y,5);
		      g.drawString(siteids.get(node.leftarcPoint) + "-" + siteids.get(node.rightarcPoint),
		                   (int)p.x - 40,(int)p.y);
		    }
		}
    }
    
    private void PaintCircles(Graphics g,Vector<Voronoi.TreeNode> flattened)
    {
    	for (Voronoi.TreeNode node : flattened)
    	{
    		if (node.circleEvent != null && node.circleEvent.valid)
    		{
    	           g.setColor(Color.RED);
    	            DrawCircle(g,
    	                       node.circleEvent.center.x,
    	                       node.circleEvent.center.y,
    	                       node.circleEvent.critical.x - node.circleEvent.center.x);
    	            
    	            Voronoi.TreeNode pred = vf.GetTree().Predecessor(node);
    	            Voronoi.TreeNode succ = vf.GetTree().Successor(node);

    	            g.drawString(siteids.get(pred.arcPoint) +
    	                         siteids.get(node.arcPoint) +
    	                         siteids.get(succ.arcPoint),
    	                         (int)node.circleEvent.center.x,
    	                         (int)node.circleEvent.center.y-15);

    	            DrawDot(g,(int)node.circleEvent.center.x,(int)node.circleEvent.center.y,5);   			
    		}
    	}
    }
    
    private static final double arrowLength = 15.0;
    private static final double arrowWidth = 5.0;
    
    private void PaintEdges(Graphics g)
    {
        for (Voronoi.Edge e : vf.GetEdges())
        {
          Voronoi.Point p1 = e.GetParametric(-100);
          Voronoi.Point p2 = e.GetParametric(100);

          if ((e.start != null && e.end != null) || e.exemplar != null)
          {
            Vector<Voronoi.InterParameter> parms = e.GetInterParameters();
            for (Voronoi.InterParameter ip : parms)
            {
                if (ip.type != Voronoi.PointType.BOUNDING) continue;

                g.setColor(Color.GREEN);
                
                // a) find  a point 15 pixels interior of the
                //    bounding point (which we know is either the first or last parm)
                Voronoi.Point basep = 
                		e.GetParametric(ip == parms.firstElement() ? (ip.t + arrowLength) : (ip.t - arrowLength));
                // b) find points 5 pixels on either side of basep; (-1/m)
                double dy = e.site1.y - e.site2.y;
                double dx = e.site1.x - e.site2.x;
                double f = Math.sqrt(dx*dx + dy*dy);
                dx /= f;
                dy /= f;
                
                Voronoi.Point lp = new Voronoi.Point(basep.x-arrowWidth*dx, basep.y-arrowWidth*dy);
                Voronoi.Point rp = new Voronoi.Point(basep.x+arrowWidth*dx, basep.y+arrowWidth*dy);
                Polygon p = new Polygon();
                p.addPoint((int)ip.point.x, (int)ip.point.y);
                p.addPoint((int)lp.x,(int)lp.y);
                p.addPoint((int)rp.x,(int)rp.y);
                g.fillPolygon(p);
            }

            g.setColor(Color.GREEN);
            p1 = parms.firstElement().point;
            p2 = parms.lastElement().point;
          }
          else
          {
            g.setColor(Color.ORANGE);
          }

          g.drawLine((int)p1.x,(int)p1.y,(int)p2.x,(int)p2.y);
        }
    }
    
    private void PaintDelaunay(Graphics g)
    {
    	g.setColor(Color.MAGENTA);
        for (Voronoi.Edge e : vf.GetEdges())
        {
        		g.drawLine((int)e.site1.x,(int)e.site1.y,(int)e.site2.x,(int)e.site2.y);
        }
    }
    
    public void paint(Graphics g)
    {
      g.setColor(getBackground());
      g.fillRect(0,0,getSize().width,getSize().height);

      PaintSites(g);

      if (vf.GetTree() == null) return;
	  Vector<Voronoi.TreeNode> flattened = new Vector<Voronoi.TreeNode>();
	  FlattenTree(vf.GetTree().root,flattened);

      if (beachCheck.isSelected()) PaintBeachLine(g,flattened);
      if (circleCheck.isSelected()) PaintCircles(g,flattened);
      if (edgeCheck.isSelected()) PaintEdges(g);
      if (delaunayCheck.isSelected()) PaintDelaunay(g);
    }

    public void actionPerformed(ActionEvent e)
    {
    	if (ae != null) ae.stop();
    	
    	if (e.getActionCommand().equals("step"))
    	{
    		ep.Step();
    		vf.step();
    		sweepx = vf.GetSweepX();
    	}
    	else if (e.getActionCommand().equals("reset"))
    	{
    		vf = new Fortune(sites,ep);
    		ep.Reset();
    		sweepx = 0;
    	} 
    	else if (e.getActionCommand().equals("random"))
    	{
    		Random r = new Random();
    		sites = new Vector<Voronoi.Point>();
    		char c = 'A';
    		for (int i = 0 ; i < 6 ; ++i)
    		{
    			AddPoint(r.nextInt(400)+100,r.nextInt(400)+100,""+c,false);
    			++c;
    		}
    		
    		
    		ep.Reset();
    		vf = new Fortune(sites,ep);
    		sweepx = 0;
    	}
    	else if (e.getActionCommand().equals("animate"))
    	{
    		ae = new AnimExecutor();
    	}
    	repaint();
    }

    private class AnimExecutor implements ActionListener
     {
    	private static final int animTime = 25;
    	private javax.swing.Timer t;
    	public AnimExecutor()
    	{
    		vf = new Fortune(sites,ep);
    		ep.Reset();
    		sweepx = 0;
    		DrawPanel.this.repaint();
    	    t = new javax.swing.Timer(animTime,this);
    	    t.start();
    	}

		public void actionPerformed(ActionEvent e)
		{
			++sweepx;
			if (sweepx < vf.GetNextSweepX())
			{
				DrawPanel.this.repaint();
				return;
			}
			ep.Step();
			if (vf.step()) t.stop();
			sweepx = vf.GetSweepX();
			DrawPanel.this.repaint();
		}
		public void stop() { t.stop(); }
    }
    
    
    
    public Dimension getPreferredSize()
    {
      return new Dimension(900,500);
    }

    private static final long serialVersionUID = 1;
  }

  private double radius = 210.0;
  private double count = 8.0;
  public void AddPoint(int x,int y,String id,boolean extend)
  {
    Voronoi.Point cp = new Voronoi.Point(x,y);

    sites.add(cp);
    siteids.put(cp,id);
    if (extend == false) return;
    double dtheta = 2 * Math.PI / count;
    int idx = 0;
    for (double theta = 0 ; theta < 2 * Math.PI ; theta += dtheta,++idx)
    {
      Voronoi.Point cir = new Voronoi.Point(x+radius*Math.cos(theta),
                                            y+radius*Math.sin(theta) );
      sites.add(cir);
      siteids.put(cir,id + idx);
    }
  }

  public void init()
  {
    //    AddPoint(323,304,"A",true);
    //    AddPoint(506,304,"B",true);

    AddPoint(100,200,"A",false);
    AddPoint(200,100,"B",false);
    AddPoint(200,300,"C",false);
    AddPoint(300,200,"D",false);

    ep = new EventPanel();
    ep.Reset();


    vf = new Fortune(sites,ep);

    JPanel mp = new JPanel();
    mp.setLayout(new BorderLayout());
    DrawPanel dp = new DrawPanel();
    mp.add(dp,BorderLayout.CENTER);
//    mp.add(ep,BorderLayout.EAST);

    JPanel cpanel = new JPanel();
    cpanel.setLayout(new BoxLayout(cpanel,BoxLayout.X_AXIS));

    mp.add(cpanel,BorderLayout.SOUTH);

    JButton sb = new JButton("step");
    sb.addActionListener(dp);
    sb.setActionCommand("step");
    cpanel.add(sb);
    JButton rb = new JButton("reset");
    rb.addActionListener(dp);
    rb.setActionCommand("reset");
    cpanel.add(rb);
    JButton randb = new JButton("randomize");
    randb.addActionListener(dp);
    randb.setActionCommand("random");
    cpanel.add(randb);
    JButton animb = new JButton("animate");
    animb.addActionListener(dp);
    animb.setActionCommand("animate");
    cpanel.add(animb);
    
    // edge, circle, delaunay, beach
    edgeCheck = new JCheckBox("Edges",true);
    circleCheck = new JCheckBox("Circles",true);
    delaunayCheck = new JCheckBox("Delaunay",false);
    beachCheck = new JCheckBox("Beach Line",true);
    cpanel.add(edgeCheck);
    cpanel.add(circleCheck);
    cpanel.add(delaunayCheck);
    cpanel.add(beachCheck);
    edgeCheck.addActionListener(dp);
    circleCheck.addActionListener(dp);
    delaunayCheck.addActionListener(dp);
    beachCheck.addActionListener(dp);
    
    add(mp);
  }

  private static void createAndShowGUI()
  {
	 JFrame frame = new JFrame("Voronoi Step");
	 frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

	 VoronoiStep vs = new VoronoiStep();
	 vs.init();
	 
    frame.getContentPane().add(vs);
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
