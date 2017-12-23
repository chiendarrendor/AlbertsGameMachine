package Outpost;

import javax.swing.JPanel;
import javax.swing.JLabel;
import java.util.Vector;
import NodeInterfacePackage.*;
import java.awt.Point;
import javax.swing.BoxLayout;

public class CommodityNotesPanel extends JPanel
{
  private boolean m_IAM;
  //  private JLabel m_evlabel;
  private JLabel m_vlabel;
  private JLabel m_wlabel;
  private JLabel m_mwlabel;
  private JLabel m_dvlabel;
  private JLabel m_dwlabel;
  private Vector<CommodityInfo> m_vci;
  private int m_MaxWeight;
  private boolean m_SmallResearch;
  private boolean m_SmallMicro;

  public void SetMaxWeight(int i_Weight) { m_MaxWeight = i_Weight; }

  public void SetSmalls(boolean i_SmallResearch,boolean i_SmallMicro)
  {
    m_SmallResearch = i_SmallResearch;
    m_SmallMicro = i_SmallMicro;
  }


  public CommodityNotesPanel(Vector<CommodityInfo> i_vci)
  {
    super();
    setLayout(new BoxLayout(this,BoxLayout.Y_AXIS));
    m_IAM = false;
    m_vci = i_vci;
    m_MaxWeight = 0;
    m_SmallResearch = false;
    m_SmallMicro = false;

    MakeNormalLabels();
  }

  private void MakeNormalLabels()
  {
    removeAll();
    m_mwlabel = new JLabel("Hand Limit: 0");
    m_wlabel = new JLabel("Hand Size: 0");
    add(m_mwlabel);
    add(m_wlabel);
    revalidate();
    repaint();
  }

  private void MakeIAMLabels()
  {
    removeAll();
    m_mwlabel = new JLabel("Hand Limit: 0");
    m_vlabel = new JLabel("Actual Value: 0");
    m_wlabel = new JLabel("Hand Size: 0");
    m_dvlabel = new JLabel("Discard Value: 0");
    m_dwlabel = new JLabel("Discard Hand Size: 0");

    add(m_vlabel);
    add(m_mwlabel);
    add(m_wlabel);
    add(m_dvlabel);
    add(m_dwlabel);
    revalidate();
    repaint();
  }

  public void SetIAM()
  {
    m_IAM = true;
    MakeIAMLabels();
  }

  public void Reset()
  {
    m_IAM = false;
    MakeNormalLabels();
  }

  public void Redraw()
  {
    int i;
    int tv = 0;
    int tw = 0;
    int dv = 0;
    int dw = 0;

    int ev = 0;


    for (i = 0 ; i < m_vci.size() ; ++i)
    {
      ev += m_vci.get(i).GetMeanValue();

      tv += m_vci.get(i).GetValue();
      if (m_vci.get(i).Discarded())
      {
        dv += m_vci.get(i).GetValue();
      }

      if (m_vci.get(i).GetCommType() == 4 && m_SmallResearch) continue;
      if (m_vci.get(i).GetCommType() == 5 && m_SmallMicro) continue;

      tw += m_vci.get(i).GetWeight();
      if (m_vci.get(i).Discarded())
      {
        dw += m_vci.get(i).GetWeight();
      }
    }


    m_mwlabel.setText("Hand Limit: " + m_MaxWeight);

    if (m_IAM)
    {
      String wcolor;
      String dcolor;

      wcolor = (tw > m_MaxWeight) ? "Red" : "#00C000";
      dcolor = (tw - dw > m_MaxWeight) ? "Red" : "#00C000";

      m_vlabel.setText("Actual Value: " + tv);
      m_wlabel.setText("<HTML><FONT COLOR=" + wcolor + ">Hand Size: " + tw + "</FONT></HTML>");
      m_dvlabel.setText("Discarded Value: " + dv );
      m_dwlabel.setText("<HTML><FONT COLOR=" + dcolor + ">Discarded Hand Size: " + dw + "</FONT></HTML>");
    }
    else
    {
      m_wlabel.setText("Hand Size: " + tw);
    }
  }
}

