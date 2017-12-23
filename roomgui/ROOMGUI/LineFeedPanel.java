package ROOMGUI;

import javax.swing.JPanel;
import javax.swing.BoxLayout;
import javax.swing.Box;
import javax.swing.JComponent;


public class LineFeedPanel extends JPanel
{
  JPanel m_activesub;

  public LineFeedPanel()
  {
    super();
    m_activesub = null;
    BoxLayout lmgr = new BoxLayout(this,BoxLayout.Y_AXIS);
    setLayout(lmgr);

    NewSubPanel();
  }

  private void NewSubPanel()
  {
    m_activesub = new JPanel();
    BoxLayout lmgr = new BoxLayout(m_activesub,BoxLayout.X_AXIS);
    m_activesub.setLayout(lmgr);
    super.add(m_activesub);
  }

  public void add(JComponent i_newcomponent)
  {
    m_activesub.add(i_newcomponent);
  }    

  public void LineFeed()
  {
    if (m_activesub != null) m_activesub.add(Box.createHorizontalGlue());
    NewSubPanel();
  }
}
      
