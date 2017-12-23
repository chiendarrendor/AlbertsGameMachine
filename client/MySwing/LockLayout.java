
package MySwing;

import java.awt.*;
import java.util.Hashtable;

public class LockLayout implements LayoutManager2
{
    private Dimension m_dim;
    private Hashtable<Component,Point> m_complocs;

    public LockLayout()
    {
      m_dim = new Dimension();
      m_complocs = new Hashtable<Component,Point>();
    }

    public void addLayoutComponent(Component c,Object dim)
    {
      if (dim instanceof Point)
      {
        Point p = (Point)dim;
        if (p.x < 0 || p.y < 0)
        {
          throw new IllegalArgumentException("Point x or y < 0!");
        }
        m_complocs.put(c,p);
      }
      else
      {
        throw new IllegalArgumentException("cannot add to layout: constraint must be a Point");
      }
    }

    public void removeLayoutComponent(Component c)
    {
      m_complocs.remove(c);
    }

    public float getLayoutAlignmentX(Container c) { return 0.5f; }
    public float getLayoutAlignmentY(Container c) { return 0.5f; }
    public void invalidateLayout(Container c) {}
    public void addLayoutComponent(String s,Component c) {}

    public Dimension maximumLayoutSize(Container c)
    {
      return minimumLayoutSize(c);
    }

    public Dimension preferredLayoutSize(Container c)
    {
      return minimumLayoutSize(c);
    }

    public Dimension minimumLayoutSize(Container container)
    {
      Insets insets = container.getInsets();
      int ncomponents = container.getComponentCount();
      int i;

      m_dim.width = insets.left + insets.right;
      m_dim.height = insets.top + insets.bottom;

      for (i = 0 ; i < ncomponents ; ++i)
      {
        Component comp=container.getComponent(i);
        Point p = m_complocs.get(comp);
        Dimension d = comp.getPreferredSize();

        int right = insets.left + p.x + d.width;
        int bottom = insets.top + p.y + d.height;

        if (right > m_dim.width)
        {
          m_dim.width = right;
        }
        if (bottom > m_dim.height)
        {
          m_dim.height = bottom;
        }
      }

      return m_dim;
    }

    public void layoutContainer(Container container)
    {
      Insets insets = container.getInsets();
      int ncomponents = container.getComponentCount();
      int i;

      for (i = 0 ; i < ncomponents ; ++i)
      {
        Component comp=container.getComponent(i);
        Point p = m_complocs.get(comp);
        Dimension d = comp.getPreferredSize();
        comp.setBounds(insets.left + p.x,insets.top+p.y,
                       d.width,d.height);
      }
    }
}
