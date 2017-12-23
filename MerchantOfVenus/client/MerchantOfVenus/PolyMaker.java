package MerchantOfVenus;

// given a collection of edges P1,P2, 
// determine, if possible, a single polygon containing
// all those edges.


import java.awt.Point;
import java.awt.Polygon;
import java.util.*;

public class PolyMaker
{
    private class Link
    {
        public Point p;
        public Link linka = null;
        public Link linkb = null;
   
        public Link(Point pt) { p = pt; }
        public void DoLink(Link other)
        {
            if (linka == null) linka = other;
            else linkb = other;
        }

        public boolean IsFull()
        {
            return linka != null && linkb != null;
        }
    }

    private HashMap<Point,Link> linked = new HashMap<Point,Link>();
    private Link firstLink = null;
    private HashMap<Point,Link> pending = new HashMap<Point,Link>();
    private boolean dead = false;

    public PolyMaker()
    {
    }

    public void AddEdge(int x1,int y1,int x2,int y2)
    {
        if (dead) return;


        Point p1 = new Point(x1,y1);
        Point p2 = new Point(x2,y2);
        if (p1.equals(p2))
        {
            dead = true;
            return;
        }

        if (linked.containsKey(p1) || linked.containsKey(p2))
        {
            dead = true;
            return;
        }

        Link link1;
        Link link2;

        if (pending.containsKey(p1))   
        {
            link1 = pending.get(p1);
        }
        else
        {
            link1 = new Link(p1);
            pending.put(p1,link1);
        }
       
        if (pending.containsKey(p2))
        {
            link2 = pending.get(p2);
        }
        else
        {
            link2 = new Link(p2);
            pending.put(p2,link2);
        }

        link1.DoLink(link2);
        link2.DoLink(link1);

        if (link1.IsFull())
        {
            pending.remove(p1);
            linked.put(p1,link1);
            if (firstLink == null) firstLink = link1;
        }

        if (link2.IsFull())
        {
            pending.remove(p2);
            linked.put(p2,link2);
            if (firstLink == null) firstLink = link2;
        }
    }


    public Polygon GetPolygon()
    {
        if (dead) return null;
        if (pending.size() > 0) return null;
        if (firstLink == null) return null;

        Polygon result = new Polygon();
        Link curLink = firstLink;
        Link prevLink = null;

        while(true)
        {
            result.addPoint(curLink.p.x,curLink.p.y);

            if (prevLink == null)
            {
                prevLink = curLink;
                curLink = curLink.linka;
            }
            else
            {
                Link oldPrev = prevLink;
                prevLink = curLink;
                if (curLink.linka == oldPrev) curLink = curLink.linkb;
                else curLink = curLink.linka;
            }
            if (curLink == firstLink) break;
        }

        if (result.npoints != linked.size()) return null;
        return result;
    }
}
