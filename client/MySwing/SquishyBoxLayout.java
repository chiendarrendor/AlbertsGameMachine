package MySwing;

import java.awt.LayoutManager;
import java.awt.Dimension;
import java.awt.Insets;
import java.awt.Component;
import java.awt.Container;

public class SquishyBoxLayout implements LayoutManager
{
  int m_maxSep;
  int m_minVisible;
  
  public SquishyBoxLayout(int i_maxSep,int i_minVisible)
  {
    m_maxSep = i_maxSep;
    m_minVisible = i_minVisible;
  }

  public void addLayoutComponent(String i_string,Component i_c)
  {
  }

  public void removeLayoutComponent(Component i_c)
  {
  }

  public Dimension preferredLayoutSize(Container i_con)
  {
    return minimumLayoutSize(i_con);
  }

  private Dimension internalPreferredLayoutSize(Container i_con)
  {
    Insets insets = i_con.getInsets();
    Dimension result = new Dimension(0,0);

    int i;
    int leftLoc = 0;
    int maxRightLoc = 0;

    for (i = i_con.getComponentCount() - 1 ; i >= 0 ; --i)
    {
      Component c = i_con.getComponent(i);
      if (result.height < c.getMinimumSize().height)
      {
        result.height = c.getMinimumSize().height;
      }

      maxRightLoc = leftLoc + c.getMinimumSize().width;
      leftLoc = maxRightLoc + m_maxSep;
    }
    result.height += insets.top + insets.bottom;
    result.width = maxRightLoc + insets.left + insets.right;

    return result;
  }

  public Dimension minimumLayoutSize(Container i_con)
  {
    Insets insets = i_con.getInsets();
    Dimension result = new Dimension(0,0);

    int i;
    int leftLoc = 0;
    int maxRightLoc = 0;
    for (i = i_con.getComponentCount() - 1 ; i >= 0 ; --i)
    {
      Component c = i_con.getComponent(i);
      if (result.height < c.getMinimumSize().height)
      {
        result.height = c.getMinimumSize().height;
      }

      int newright = leftLoc + c.getMinimumSize().width;
      if (newright > maxRightLoc)
      {
        maxRightLoc = newright;
      }
      leftLoc += m_minVisible;
    }
    result.height += insets.top + insets.bottom;
    result.width = maxRightLoc + insets.left + insets.right;

    return result;
  }

  public void layoutContainer(Container i_con)
  {
    Insets insets = i_con.getInsets();
    int i;

    int maxW = internalPreferredLayoutSize(i_con).width;
    int minW = minimumLayoutSize(i_con).width;
    
    double d;
    if (maxW == minW) 
    {
      d = 1;
    }
    else if (minW > i_con.getWidth()) 
    {
      d = 0;
    }
    else if (maxW < i_con.getWidth()) 
    {
      d = 1;
    }
    else
    {
      d = 1.0 / (double)(maxW - minW) * (double)(i_con.getWidth() - minW);
    }


    int pleft = 0;
    int pright = 0;

    if (i_con.getComponentCount() > 0)
    {
      int firstidx = i_con.getComponentCount() - 1;

      pleft = 0;
      pright = i_con.getComponent(firstidx).getMinimumSize().width;
      i_con.getComponent(firstidx).setSize(i_con.getComponent(firstidx).getMinimumSize());
      i_con.getComponent(firstidx).setLocation(insets.left,insets.top);
    }

    for (i = i_con.getComponentCount() - 2 ; i >= 0 ; --i)
    {
      Component c = i_con.getComponent(i);
      c.setSize(c.getMinimumSize());
      
      int minx = pleft + m_minVisible;
      int maxx = pright + m_maxSep;

      int ax = minx + (int)(d *(maxx - minx));
      
      c.setLocation(ax,insets.top);

      pleft = ax;
      pright = ax + c.getMinimumSize().width;
    }
  }
}

  
