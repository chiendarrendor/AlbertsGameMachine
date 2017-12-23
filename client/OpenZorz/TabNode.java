package OpenZorz;

import NodeInterfacePackage.ActionTransferManager;
import java.awt.Dimension;
import java.awt.Insets;

public class TabNode extends PanelNode
{
  TabbedWindow m_tabloc;
  String m_title;
  ActionTransferManager m_atm;

  private class TabAlerter implements ActionTransferManager.Alerter
  {
    public void Alert()
    {
      m_tabloc.Alert(GetVisibilityComponent());
    }
  }

  public TabNode(ActionTransferManager i_atm,TabbedWindow i_tabloc,String i_title,String i_layout,boolean i_switchto)
  {
    super(null,0,0,null,null,i_layout);
    m_tabloc = i_tabloc;
    m_title = i_title;
    m_atm = i_atm;

    m_tabloc.addTab(i_title,GetVisibilityComponent(),i_switchto);
  }

  public void SetAlerter()
  {
    m_atm.SetCurrentAlerter(new TabAlerter());
  }

  public void RefreshMe()
  {
    Dimension d = GetVisibilityComponent().getParent().getSize();
    Insets ins = GetVisibilityComponent().getParent().getInsets();
    GetVisibilityComponent().setSize(d.width-ins.left-ins.right,d.height-ins.top-ins.bottom);
  }


  public void AlterVisibility(boolean i_visibility)
  {
    m_IsVisible = i_visibility;
  }

  public void dispose()
  {
    m_tabloc.removeTab(GetVisibilityComponent());
  }





}

  
  


    
  
