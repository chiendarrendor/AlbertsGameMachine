package OpenZorz;

import NodeInterfacePackage.*;


abstract public class ActionGeneratorNode extends GameNode implements ActionVariableGenerator
{
  private ActionTransferManager m_actionmanager;
  private String m_varname;

  public ActionGeneratorNode(GameNode i_ParentNode,String i_Hidden,
                             GameGui i_gamegui,String i_varname)
  {
    super(i_ParentNode,i_Hidden);
    m_actionmanager = i_gamegui.GetActionTransferManager();
    m_varname = i_varname;

    m_actionmanager.AddVariableGenerator(i_varname,this);
  }

  public String GetName()
  {
    return m_varname;
  }

  public void VarSet(String i_Value)
  {
    m_actionmanager.ReportNewValueToConsumers(m_varname,i_Value);
  }

  public void ActionVariableReset()
  {
    DoActionReset();
  }

  abstract void DoReset();
  abstract void DoActionReset();

}

	
