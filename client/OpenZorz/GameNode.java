package OpenZorz;

import javax.swing.*;
import java.awt.*;
import java.util.*;

import NodeInterfacePackage.GameEvent;
import MySwing.ImageLoader;


abstract public class GameNode implements EventHandlerNode
{
  private Component m_VisibilityComponent;
  private Component m_DrawComponent;
  private GameNode m_Parent;
  private java.util.List<GameNode> m_Children;
  protected boolean m_IsVisible;
  private boolean m_OrigVisible;
  private GameEventHandler m_handler;
  private boolean m_ResetLocked;

  public void dispose()
  {
  }

  private static XYPopup s_xypopup = null;

  public static void AddPopup(Component i_comp)
  {
    if (s_xypopup == null)
    {
      s_xypopup = new XYPopup();
    }

    s_xypopup.AddPopup(i_comp);
  }

  public static ImageIcon MakeImageIcon(String i_Name)
  {
    return ImageLoader.MakeImageIcon(i_Name);
  }

  public static JLabel MakeImageLabel(String i_Name)
  {
    return ImageLoader.MakeImageLabel(i_Name);
  }

  public GameNode(GameNode i_Parent,String i_hidden)
  {
    m_VisibilityComponent = null;
    m_DrawComponent = null;
    m_handler = null;
    m_Parent = i_Parent;
    m_Children = new ArrayList<GameNode>();
    m_IsVisible = true;
    m_OrigVisible = true;
    m_ResetLocked = false;

    String s = i_hidden;
    if (s != null)
    {
	    if (s.equals("y") || s.equals("Y") || s.equals("true"))
	    {
        m_IsVisible = false;
        m_OrigVisible = false;
	    }
      if (s.equals("locked"))
      {
        m_ResetLocked = true;
      }
    }
    if (i_Parent != null)	i_Parent.AddChild(this);

  }

  public GameEventHandler GetEventHandler()
  {
    return m_handler;
  }

  public void SetEventHandler(GameEventHandler i_handler)
  {
    m_handler = i_handler;
  }

  public void HandleEvent(GameEvent i_event)
  {
    MyHandleEvent(i_event);

    if (m_handler != null)
    {
	    m_handler.ParseEvent(i_event);

	    if (m_handler.IsVisible())
	    {
        AlterVisibility(true);
	    }
	    else if (m_handler.IsInvisible())
	    {
        AlterVisibility(false);
	    }

	    if (!m_handler.EventPasses()) return;
    }

    int i;

    for (i = 0 ; i < GetNumChildren() ; ++i)
    {
	    GetChild(i).HandleEvent(i_event);
    }
  }

  public void AlterVisibility(boolean i_visibility)
  {
    m_IsVisible = i_visibility;
    m_VisibilityComponent.setVisible(i_visibility);
  }


    
  public void MyHandleEvent(GameEvent i_event)
  {
  }

  public boolean IsVisible() 
  { 
    return m_IsVisible; 
  }
  public void AddChild(GameNode i_Child) { m_Children.add(i_Child); }
  public int GetNumChildren() { return m_Children.size(); }
  public GameNode GetChild(int i_ChildIndex)
  {
    if (i_ChildIndex < 0 || i_ChildIndex >= GetNumChildren())
    {
	    return null;
    }

    return m_Children.get(i_ChildIndex);
  }

  public void AddComponent(Component i_comp)
  {
    Container c = (Container)this.GetComponent();
    c.add(i_comp,-1);
  }

  public void AddComponent(Component i_comp,Point i_xy)
  {
    Container c = (Container)this.GetComponent();
    c.add(i_comp,i_xy,-1);
  }

  protected void SetComponent(Component i_comp) 
  { 
    m_VisibilityComponent = i_comp;
    m_DrawComponent = i_comp; 
  }
    
  protected void SetComponent(Component i_viscomp,Component i_drawcomp)
  {
    m_VisibilityComponent = i_viscomp;
    m_DrawComponent = i_drawcomp;
  }

  public Component GetVisibilityComponent() { return m_VisibilityComponent; }
  public Component GetComponent() { return m_DrawComponent; }
  public GameNode GetParent() { return m_Parent; }

  void RefreshMe()
  {
  }

  public void refresh()
  {
    RefreshMe();
	
    int i;

    for (i = 0 ; i < GetNumChildren() ; ++i)
    {
	    GetChild(i).refresh();
    }
  }

  void ResetMe()
  {
    if (!m_ResetLocked)
    {
      AlterVisibility(m_OrigVisible);
    }
  }

  public void reset()
  {
    ResetMe();

    int i;

    for (i = 0 ; i < GetNumChildren() ; ++i)
    {
	    GetChild(i).reset();
    }
  }
}

    

