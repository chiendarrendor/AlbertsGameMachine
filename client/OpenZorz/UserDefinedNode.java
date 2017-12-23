package OpenZorz;

import javax.swing.*;
import java.util.HashMap;
import java.awt.*;
import java.lang.reflect.*;

import NodeInterfacePackage.*;

public class UserDefinedNode extends GameNode
{
  private UserDefinedInterface m_udi;

  public UserDefinedNode(GameNode i_Parent,GameGui i_gamegui,
                         int i_x,int i_y,String i_hidden,
                         String i_ClassName,HashMap<String,String> i_Options,String i_DataLoc)
    throws GameXMLException

  {
    super(i_Parent,i_hidden);

    Point p = new  Point(i_x,i_y);
    ActionTransferManager atm = i_gamegui.GetActionTransferManager();

    JPanel panel = new JPanel();

    panel.setOpaque(false);
    i_Parent.AddComponent(panel,p);
    panel.setVisible(IsVisible());
    SetComponent(panel);


    m_udi = ConstructUserDefinedInterface(i_ClassName,i_DataLoc,i_Options,panel,atm);

  }

  public void MyHandleEvent(GameEvent i_event)
  {
    m_udi.HandleEvent(i_event);
  }

  public void RefreshMe()
  {
    m_udi.Refresh();
  }

  public void ResetMe()
  {
    super.ResetMe();
    m_udi.Reset();
  }

  // -----------------------------
  private UserDefinedInterface ConstructUserDefinedInterface(String i_ClassName,
                                                             String i_DataLoc,
                                                             HashMap<String,String> i_Options,
                                                             JPanel i_Panel,
                                                             ActionTransferManager i_atm)
    throws GameXMLException
  {
    Object newo;

    String errorbase = "User Defined class " + i_ClassName + " ";

    try
    {
      Class<?> newclass = this.getClass().getClassLoader().loadClass(i_ClassName);
      Class<?>[] contypearg = new Class[4];
      contypearg[0] = i_Options.getClass();
      contypearg[1] = i_DataLoc.getClass();
      contypearg[2] = i_Panel.getClass();
      contypearg[3] = i_atm.getClass();

      Constructor<?> con = newclass.getConstructor(contypearg);

      Object[] conargs = new Object[4];
      conargs[0] = i_Options;
      conargs[1] = i_DataLoc;
      conargs[2] = i_Panel;
      conargs[3] = i_atm;

      newo = con.newInstance(conargs);
    }
    catch(NodeInterfacePackageException nipe)
    {
      throw new GameXMLException(nipe.toString());
    }
    catch (java.lang.ClassNotFoundException ex)
    {
      throw new GameXMLException(errorbase + "not found");
    }
    catch (java.lang.NoSuchMethodException ex)
    {
      throw new GameXMLException(errorbase + "is missing constructor.");
    }
    catch (java.lang.InstantiationException ex)
    {
      throw new GameXMLException(errorbase + "is abstract!?");
    }
    catch (java.lang.IllegalAccessException ex)
    {
      throw new GameXMLException(errorbase + "constructor is not public?!");
    }
    catch (java.lang.reflect.InvocationTargetException ex)
    {
      System.out.println("ITE stack trace from UserDefinedNode:");
      ex.getCause().printStackTrace(System.out);
      throw new GameXMLException(errorbase + "constructor threw!" + ex.getCause().toString());
    }

    return (UserDefinedInterface)newo;
  }
}

    
