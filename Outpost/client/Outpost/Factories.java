package Outpost;

import java.util.HashMap;
import java.util.Vector;
import NodeInterfacePackage.*;
import MySwing.ImageLoader;
import MySwing.LockLayout;
import MySwing.SquishyPane;
import javax.swing.JPanel;
import javax.swing.JLabel;
import javax.swing.BoxLayout;
import javax.swing.JCheckBox;
import java.awt.Image;
import java.awt.Point;
import java.awt.event.ItemListener;
import java.awt.event.ItemEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseEvent;
import java.awt.Cursor;
import java.awt.Toolkit;

public class Factories implements UserDefinedInterface
{
  private static String s_actionvarname = "ManString";
  private static String s_actionname = "ALTERMANNING";
  private String m_PlayerNum;
  private String m_DataLoc;
  private JPanel m_mainpanel;
  private JPanel m_itempanel;
  private JLabel m_mancountlabel;
  private JLabel m_robotcountlabel;
  private MouseAwareJLabel m_manjl;
  private MouseAwareJLabel m_robotjl;
  private JLabel m_tmlabel;
  private JLabel m_trlabel;
  private JLabel m_mmlabel;
  private JLabel m_mrlabel;
  private int m_mancount;
  private int m_robotcount;
  private int m_usedmen;
  private int m_usedrobots;
  private int m_maxmen;
  private int m_maxrobots;
  private boolean m_IAM;
  private Vector<FactoryWidget> m_factories;
  private Cursor m_mancursor;
  private Cursor m_robotcursor;
  private static final int NO_DRAGGER = -3;
  private static final int MAN_SOURCE_DRAGGER = -2;
  private static final int ROBOT_SOURCE_DRAGGER = -1;
  private FactoryMousingManager m_fmm;
  private ActionTransferManager m_atm;


  private class FactoryMousingManager implements ActionVariableGenerator,MyMouseListener,ActionGenerator
  {
    private int m_mouseloc;
    private int m_dragloc;
    private String m_unchangedManning;
    private boolean m_isready;
    private boolean m_islegal;

    public FactoryMousingManager()
    {
      m_mouseloc = NO_DRAGGER;
      m_dragloc = NO_DRAGGER;
      m_isready = false;
      m_islegal = false;
      ResetBaseline();
    }

    public void ActionLegalityChanged(boolean i_isLegal)
    {
      m_islegal = i_isLegal;
      setLabelEnabled(i_isLegal);
      int i;
      for (i = 0 ; i < m_factories.size() ; ++i)
      {
        m_factories.get(i).SetManEnabled(i_isLegal);
      }
    }

    public void ActionReadinessChanged(boolean i_isReady)
    {
      m_isready = i_isReady;
    }

    public void ResetBaseline()
    {
      m_unchangedManning = GetManningString();
    }

    public void mouseClicked(int i_code)
    {
    }

    public void mouseExited(int i_code)
    {
      m_mouseloc = NO_DRAGGER;
    }

    public void mouseEntered(int i_code)
    {
      m_mouseloc = i_code;
    }

    public void mousePressed(int i_code)
    {
      if (!m_islegal) return;

      if (i_code == MAN_SOURCE_DRAGGER)
      {
        if ((m_mancount - m_usedmen) <= 0)
        {
          return;
        }
      }
      else if (i_code == ROBOT_SOURCE_DRAGGER)
      {
        if ((m_robotcount - m_usedrobots) <= 0)
        {
          return;
        }
      }
      else
      {
        if (m_factories.get(i_code).GetManType() == 'U')
        {
          return;
        }
      }

      m_dragloc = i_code; 

      if (i_code == MAN_SOURCE_DRAGGER)
      {
        m_mainpanel.setCursor(m_mancursor);
      }
      else if (i_code == ROBOT_SOURCE_DRAGGER)
      {
        m_mainpanel.setCursor(m_robotcursor);
      }
      else if (m_factories.get(i_code).GetManType() == 'H')
      {
        m_mainpanel.setCursor(m_mancursor);
      }
      else
      {
        m_mainpanel.setCursor(m_robotcursor);
      }
   }

    public String GetManningString()
    {
      StringBuffer s = new StringBuffer();
      int i;
      for (i = 0 ; i < m_factories.size() ; ++i)
      {
        s.append(m_factories.get(i).GetManType());
      }

      return s.toString();
    }

    private String GetOutboundManningString()
    {
      String s = GetManningString();
      if (s.equals(m_unchangedManning)) return null;
      return s;
    }

    public void mouseReleased(int i_code)
    {
      if (m_mouseloc == NO_DRAGGER ||
          m_dragloc == NO_DRAGGER ||
          m_mouseloc == m_dragloc ||
          (m_mouseloc < 0 && m_dragloc < 0) ||
          !m_islegal)
      {
      }
      else
      {
        // mouseloc is where we are when we drop
        // dragloc is where we were when we started.

        // dragging from man source
        if (m_dragloc == MAN_SOURCE_DRAGGER)
        {
          m_factories.get(m_mouseloc).SetManType('H');
        }
        // dragging from robot source
        else if (m_dragloc == ROBOT_SOURCE_DRAGGER)
        {
          m_factories.get(m_mouseloc).SetManType('B');
        }
        // dragging from another factory back to a source
        else if (m_mouseloc == MAN_SOURCE_DRAGGER || m_mouseloc == ROBOT_SOURCE_DRAGGER)
        {
          m_factories.get(m_dragloc).SetManType('U');
        }
        // last case is factory to factory.
        else
        {
          char it = m_factories.get(m_dragloc).GetManType();
          m_factories.get(m_mouseloc).SetManType(it);
          m_factories.get(m_dragloc).SetManType('U');
        }

        RealizeFactories();
        UpdateCounts();

        m_atm.ReportNewValueToConsumers(s_actionvarname,GetOutboundManningString());

        if (m_isready && m_islegal)
        {
          m_atm.ButtonPressed(s_actionname);
        }

      }

      m_mainpanel.setCursor(Cursor.getDefaultCursor());
      m_dragloc = NO_DRAGGER;
    }

    public void ActionVariableReset()
    {
      int i;
      for (i = 0 ; i < m_factories.size() ; ++i)
      {
        m_factories.get(i).ResetManning();
      }

      RealizeFactories();
      UpdateCounts();
    }

    public String ActionVariableGetValue()
    {
      return GetOutboundManningString();
    }
    


  }






    


  public Factories(HashMap<String,String> i_Options,String i_DataLoc,JPanel i_Panel,ActionTransferManager i_atm)
  {
    Toolkit tk = Toolkit.getDefaultToolkit();
    Image manimage = ImageLoader.MakeImageIcon(this.getClass(), "/Man.PNG").getImage();
    Image robotimage = ImageLoader.MakeImageIcon(this.getClass(), "/Robot.PNG").getImage();
    m_mancursor = tk.createCustomCursor(manimage,
                                        new Point(manimage.getWidth(null)/2,manimage.getHeight(null)/2),
                                        "ManCursor"
                                        );
    m_robotcursor = tk.createCustomCursor(robotimage,
                                          new Point(robotimage.getWidth(null)/2,robotimage.getHeight(null)/2),
                                          "RobotCursor"
                                          );



    if (!i_Options.containsKey("playernum")) throw new NodeInterfacePackageException("missing option commtype");
    m_PlayerNum = i_Options.get("playernum");

    m_fmm = null;
    m_DataLoc = i_DataLoc;
    m_IAM = false;
    m_mancount = 0;
    m_robotcount = 0;
    m_usedmen = 0;
    m_usedrobots = 0;
    m_maxmen = 0;
    m_maxrobots = 0;
    m_atm = i_atm;

    m_mainpanel = new JPanel();
    m_mainpanel.setOpaque(false);
    m_mainpanel.setVisible(true);
    m_mainpanel.setLayout(new LockLayout());
    i_Panel.add(m_mainpanel);

    
    m_itempanel = new JPanel();
    m_itempanel.setLayout(new BoxLayout(m_itempanel,BoxLayout.X_AXIS));
    //m_itempanel = new SquishyPane(0,15);

    m_itempanel.setOpaque(false);
    m_itempanel.setVisible(true);

    m_mainpanel.add(m_itempanel,new Point(100,0));

    m_tmlabel = new JLabel();
    m_mainpanel.add(m_tmlabel,new Point(0,5));

    m_mmlabel = new JLabel();
    m_mainpanel.add(m_mmlabel,new Point(0,20));

    m_trlabel = new JLabel();
    m_mainpanel.add(m_trlabel,new Point(0,65));

    m_mrlabel = new JLabel();
    m_mainpanel.add(m_mrlabel,new Point(0,80));


    m_manjl = new MouseAwareJLabel(MAN_SOURCE_DRAGGER,m_fmm);
    m_manjl.setIcon(ImageLoader.MakeImageIcon(this.getClass(), "/Man.PNG"));
    m_mainpanel.add(m_manjl,new Point(45,0));

    m_mancountlabel = new JLabel();
    m_mainpanel.add(m_mancountlabel,new Point(60,42));

    m_robotjl = new MouseAwareJLabel(ROBOT_SOURCE_DRAGGER,m_fmm);
    m_robotjl.setIcon(ImageLoader.MakeImageIcon(this.getClass(), "/Robot.PNG"));
    m_mainpanel.add(m_robotjl,new Point(45,60));

    m_robotcountlabel = new JLabel();
    m_mainpanel.add(m_robotcountlabel,new Point(60,102));

    m_factories = FactoryWidget.ParseFactoryString("||",m_DataLoc,m_IAM,m_fmm);


    UpdateCounts();
  }

  void setLabelEnabled(boolean i_enabled)
  {
    if (i_enabled)
    {
      m_manjl.setEnabled((m_mancount - m_usedmen) > 0);
      m_robotjl.setEnabled((m_robotcount - m_usedrobots) > 0);
    }
    else
    {
      m_manjl.setEnabled(false);
      m_robotjl.setEnabled(false);
    }
  }      


  void UpdateCounts()
  {
    m_mancountlabel.setText(String.valueOf(m_mancount-m_usedmen));
    m_robotcountlabel.setText(String.valueOf(m_robotcount-m_usedrobots));

    setLabelEnabled(true);

    m_tmlabel.setText("Cur: " + m_mancount);
    m_trlabel.setText("Cur: " + m_robotcount);
    m_mmlabel.setText("Max: " + m_maxmen);

    if (m_maxrobots < 0)
    {
      m_mrlabel.setText("UNLIM");
    }
    else
    {
      m_mrlabel.setText("Max: " + m_maxrobots);
    }
  }


  void RealizeFactories()
  {    
    m_itempanel.removeAll();
    m_usedmen = 0;
    m_usedrobots = 0;
    
    int i;
    for (i = 0 ; i < m_factories.size() ; ++i)
    {
      FactoryWidget fw = m_factories.get(i);
      fw.Realize(m_itempanel);
      if (fw.GetManType() == 'H') m_usedmen++;
      if (fw.GetManType() == 'B') m_usedrobots++;
    }

    m_itempanel.revalidate();
    m_itempanel.repaint();
  }

  public void HandleEvent(GameEvent i_event)
  {
    if (i_event.GetVarValue("eventname").equals("IAM") &&
        i_event.GetVarValue("PlayerNumber").equals(m_PlayerNum))
    {
      m_IAM = true;
      m_fmm = new FactoryMousingManager();
      m_manjl.setListener(m_fmm);
      m_robotjl.setListener(m_fmm);
      m_atm.AddVariableGenerator(s_actionvarname,m_fmm);
      m_atm.AddButtonPresser(s_actionname,m_fmm);
      RealizeFactories();
      UpdateCounts();
      
      return;
    }

    if (i_event.GetVarValue("eventname").equals("PLAYERSTATE") &&
        i_event.GetVarValue("PlayerNumber").equals(m_PlayerNum))
    {
      m_mancount = Integer.parseInt(i_event.GetVarValue("NumberPeople"));
      m_robotcount = Integer.parseInt(i_event.GetVarValue("NumberRobots"));
      m_maxmen = Integer.parseInt(i_event.GetVarValue("PeopleLimit"));
      m_maxrobots = Integer.parseInt(i_event.GetVarValue("RobotLimit"));
      UpdateCounts();
      return;
    }

    if (i_event.GetVarValue("eventname").equals("PLAYERFACTORIES") &&
        i_event.GetVarValue("PlayerNumber").equals(m_PlayerNum))
    {
      String facstring = i_event.GetVarValue("FactoryString");

      Vector<FactoryWidget> tfac;
      try
      {
        tfac = FactoryWidget.ParseFactoryString(facstring,m_DataLoc,m_IAM,m_fmm);
        m_factories = tfac;
        RealizeFactories();
        UpdateCounts();
        if (m_fmm != null) m_fmm.ResetBaseline();
      }
      catch(NodeInterfacePackageException nipe)
      {
        return;
      }
    }
  }

  public void Refresh()
  {
  }

  public void Reset()
  {
    m_mancount = 0;
    m_robotcount = 0;
    m_usedmen = 0;
    m_usedrobots = 0;
    m_maxmen = 0;
    m_maxrobots = 0;

    UpdateCounts();

    m_itempanel.removeAll();
    m_factories.clear();

    m_itempanel.revalidate();
    m_itempanel.repaint();

    odispose();

    m_IAM = false;
    m_fmm = null;
  }


  public void odispose()
  {
    if (m_IAM)
    {
      m_atm.RemoveVariableGenerator(s_actionvarname,m_fmm);
      m_atm.RemoveButtonPresser(s_actionname,m_fmm);
      m_manjl.setListener(null);
      m_robotjl.setListener(null);
    }
  }    
}
