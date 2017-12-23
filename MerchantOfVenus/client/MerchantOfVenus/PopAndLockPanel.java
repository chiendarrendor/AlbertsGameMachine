// this class creates a panel that is designed
// to 'go over' a jscrollpane and 'float'
// tab-like icons at the edges of the viewport.
// each icon represents a 'pop-up' sort of 
// panel.  
// Icons can be dragged around the edge of the viewport.
// by default, hovering the mouse over the icon will
// allow it to 'pop up' the panel, while the mouse
// leaving the panel causes it to 'pop down' again,
//  but we must allow
// for other mechanisms for visibility/invisibility.
//

package MerchantOfVenus;

import java.util.*;
import java.awt.*;
import javax.swing.*;
import java.awt.event.*;
import javax.swing.event.*;

public class PopAndLockPanel extends JLayeredPane
{
	private static final long serialVersionUID = 1L;
public static int OTHEREXTREME = -1000000;
  private static Integer LABELLAYER = new Integer(0);
  private static Integer PANELLAYER = new Integer(1);
  SpringLayout springs;

  // if x or y are OTHEREXTREME, that will 'lock' the component to the right/bottom of the pane
  public JLabel Add(Component c,Icon i,int x,int y,boolean defaultPop)
  {
    JLabel jl = Add(c,x,y,defaultPop);
    jl.setIcon(i);
    return jl;
  }    

  public JLabel Add(Component c,String s,int x,int y,boolean defaultPop)
  {
    JLabel jl = Add(c,x,y,defaultPop);
    jl.setText(s);
    return jl;
  }    

  public JLabel Add(Component c,int x,int y,boolean defaultPop)
  {
    c.setVisible(false);

    Point p = new Point(x,y);
    LockLabel result = new LockLabel(defaultPop,c);
    result.SetCurPoint(p);
    addItem(result,p,LABELLAYER);
    addItem(c,p,PANELLAYER);
    return result;
  }    

  public void AddToLabel(JLabel handle,Component c)
  {
    LockLabel ll = (LockLabel)handle;
    ll.AddPanel(c);
    Point loc = ll.GetCurPoint();
    Point edge = calcLabelPosition(c,loc);
    addItem(c,edge,PANELLAYER);
  }    

  public void Remove(JLabel label)
  {
    LockLabel ll = (LockLabel)label;
    ll.Remove();
  }

  private void addItem(Component c,Point p,Integer layer)
  {
    super.add(c,layer);
    Point newP = calcLabelPosition(c,p);
    constrain(c,newP);
  }

  private void constrain(Component c,Point p)
  {
    springs.removeLayoutComponent(c);
    if (p.x == OTHEREXTREME) 
      springs.putConstraint(SpringLayout.EAST,c,0,SpringLayout.EAST,this);
    else
      springs.putConstraint(SpringLayout.WEST,c,p.x,SpringLayout.WEST,this);

    if (p.y == OTHEREXTREME) 
      springs.putConstraint(SpringLayout.SOUTH,c,0,SpringLayout.SOUTH,this);
    else
      springs.putConstraint(SpringLayout.NORTH,c,p.y,SpringLayout.NORTH,this);
  }    

  private Point calcLabelPosition(Component c,Point p)
  {
    Point result = new Point();
    int maxx = getWidth();
    int maxy = getHeight();
    if (maxx == 0 && maxy == 0) { maxx = 1024; maxy = 768; }


    int h = c.getPreferredSize().height;
    int w = c.getPreferredSize().width;
    boolean xdone = false;
    boolean ydone = false;

    if (p.x == OTHEREXTREME || p.x >= maxx - w) 
    {
      result.x = OTHEREXTREME;
      xdone = true;
    }
    else if (p.x <= 0)
    {
      result.x = 0;
      xdone = true;
    }

    if (p.y == OTHEREXTREME || p.y >= maxy - h) 
    {
      result.y = OTHEREXTREME;
      ydone = true;
    }
    else if (p.y <= 0)
    {
      result.y = 0;
      ydone = true;
    }

    // <dir>done means that dimension was trivially locked to one edge or another
    // we don't have to go looking for an edge-lock
    // if both are locked, we're done.
    if (xdone && ydone) 
    {
      return result;
    }
    // if only one is locked, and the other isn't, then we just set the other
    // to whatever it was.
    if (ydone) 
    { 
      result.x = p.x; 
      return result; 
    }
    if (xdone) 
    { 
      result.y = p.y; 
      return result; 
    }
    // leaving the last case, neither are locked, so we have to figure out
    // which is the closest edge.
    // we know a priori that both x and y are within 1 and (max-1)

    int dup = p.y;
    int dleft = p.x;
    int ddown = maxy - p.y - h;
    int dright = maxx - p.x - w;

    if (dup <= ddown && dup <= dleft && dup <= dright) 
    {
      result.x = dleft; result.y = 0;
    }
    else if (dleft <= dup && dleft <= ddown && dleft <= dright) 
    {
      result.x = 0; result.y = dup;
    }
    else if (ddown <= dup && ddown <= dleft && ddown <= dright) 
    {
      result.x = dleft; result.y = OTHEREXTREME;
    }
    else if (dright <= dup && dright <= dleft && dright <= ddown) 
    {
      result.x = OTHEREXTREME; result.y = dup;
    }
    return result;
  }

  private class CloseListener implements MouseInputListener
  {
    Component c;
    public CloseListener(Component c) { this.c = c; }
    public void mouseClicked(MouseEvent ev) {}
    public void mouseEntered(MouseEvent ev) {}
    public void mouseExited(MouseEvent ev) { c.setVisible(false); }
    public void mousePressed(MouseEvent ev) {}
    public void mouseReleased(MouseEvent ev) {}
    public void mouseDragged(MouseEvent ev) {}
    public void mouseMoved(MouseEvent ev) {}
  }

  public void reconstrain(JLabel handle,Component c)
  {
    LockLabel ll = (LockLabel)handle;
    Point loc = ll.GetCurPoint();
    Point edge = calcLabelPosition(c,loc);
    constrain(c,edge);
  }    

  private class LockLabel extends JLabel implements MouseInputListener,ActionListener
  {
	private static final long serialVersionUID = 1L;
	boolean defaultPop;
    Vector<Component> myPanels;
    Point curPoint;
    javax.swing.Timer hoverTimer;

    public Point GetCurPoint() { return curPoint; }
    public void SetCurPoint(Point p) { curPoint = p; }

    public void AddPanel(Component c)
    {
      myPanels.add(c);
      if (defaultPop) c.addMouseListener(new CloseListener(c));
    }

    public LockLabel(boolean defaultPop,Component c)
    { 
      super(); 
      this.defaultPop = defaultPop;
      myPanels = new Vector<Component>();
      AddPanel(c);
      addMouseListener(this) ; 
      addMouseMotionListener(this); 

      hoverTimer = new javax.swing.Timer(0,this);
      hoverTimer.setInitialDelay(750);
      hoverTimer.setRepeats(false);
    }

    public void Remove()
    {
      for (Component c : myPanels)
      {
        springs.removeLayoutComponent(c);
        PopAndLockPanel.this.remove(c);
      }
      springs.removeLayoutComponent(this);
      PopAndLockPanel.this.remove(this);
    }

    public void actionPerformed(ActionEvent e)
    {
      if (defaultPop)
      {
        for (Component c : myPanels) c.setVisible(true);
      }
    }      

    public void mouseClicked(MouseEvent ev) 
    {
      hoverTimer.stop();
    }
    public void mouseEntered(MouseEvent ev) { hoverTimer.start(); }
    public void mouseExited(MouseEvent ev) { hoverTimer.stop(); }
    public void mousePressed(MouseEvent ev) { hoverTimer.stop(); }
    public void mouseReleased(MouseEvent ev) { hoverTimer.stop(); }
    public void mouseDragged(MouseEvent ev) 
    {
      hoverTimer.stop();
      MouseEvent nev = SwingUtilities.convertMouseEvent(this,ev,PopAndLockPanel.this);
      curPoint = calcLabelPosition(this,new Point(nev.getX(),nev.getY()));
      constrain(this,curPoint);
      for (Component c : myPanels)
      {
        constrain(c,calcLabelPosition(c,new Point(nev.getX(),nev.getY())));
      }
      PopAndLockPanel.this.revalidate();
      
    }
    public void mouseMoved(MouseEvent ev) {}
  }
    


  public PopAndLockPanel()
  {
    springs = new SpringLayout();
    setLayout(springs);
    setOpaque(false);
  }
}
