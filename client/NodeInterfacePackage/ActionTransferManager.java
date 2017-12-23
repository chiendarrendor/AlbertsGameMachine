package NodeInterfacePackage;

import java.util.*;



public class ActionTransferManager
{
  private HashMap<String,Vector<ActionVariableConsumer> > m_Consumers;
  private HashMap<String,Vector<ActionVariableGenerator> > m_Generators;
  private HashMap<String,DoActionHandler> m_ActionHandlers;
  private HashMap<String,Vector<ActionGenerator> > m_ButtonPressers;

  public interface Alerter
  {
    public void Alert();
  }

  private Alerter m_currentAlerter;

  public Alerter GetCurrentAlerter()
  {
    return m_currentAlerter;
  }

  public void SetCurrentAlerter(Alerter i_alerter)
  {
    m_currentAlerter = i_alerter;
  }

  public ActionTransferManager()
  {
    m_Consumers = new HashMap<String,Vector<ActionVariableConsumer> >();
    m_Generators = new HashMap<String,Vector<ActionVariableGenerator> > ();
    m_ActionHandlers = new HashMap<String,DoActionHandler>();
    m_ButtonPressers = new HashMap<String,Vector<ActionGenerator> >();
    m_currentAlerter = null;
  }

  public void AddActionHandler(String i_aname,DoActionHandler i_handler)
  {
    m_ActionHandlers.put(i_aname,i_handler);
  }

  public void AddButtonPresser(String i_aname,ActionGenerator i_gen)
  {
    Vector<ActionGenerator> v;

    if (m_ButtonPressers.containsKey(i_aname))
    {
      v = m_ButtonPressers.get(i_aname);
    }
    else
    {
      v = new Vector<ActionGenerator>();
      m_ButtonPressers.put(i_aname,v);
    }

    v.add(i_gen);
  }

  public void RemoveButtonPresser(String i_aname,ActionGenerator i_gen)
  {
    Vector<ActionGenerator> v;

    if (m_ButtonPressers.containsKey(i_aname))
    {
      v = m_ButtonPressers.get(i_aname);
      v.remove(i_gen);
    }
  }

  public void ButtonPressed(String i_aname)
  {
    if (m_ActionHandlers.containsKey(i_aname))
    {
      m_ActionHandlers.get(i_aname).ActionGo();
    }
  }

  public void ActionLegalityChanged(String i_aname,boolean i_islegal)
  {
    Vector<ActionGenerator> v = m_ButtonPressers.get(i_aname);
    if (v == null) return;
    
    int i;
    for (i = 0 ; i < v.size() ; ++i)
    {
      v.get(i).ActionLegalityChanged(i_islegal);
    }
  }

  public void ActionReadinessChanged(String i_aname,boolean i_isready)
  {
    Vector<ActionGenerator> v = m_ButtonPressers.get(i_aname);
    if (v == null) return;
    
    int i;
    for (i = 0 ; i < v.size() ; ++i)
    {
      v.get(i).ActionReadinessChanged(i_isready);
    }
  }






  public void AddVariableConsumer(String i_cname,ActionVariableConsumer i_consumer)
  {
    Vector<ActionVariableConsumer> v;

    if (m_Consumers.containsKey(i_cname))
    {
      v = m_Consumers.get(i_cname);
    }
    else
    {
      v = new Vector<ActionVariableConsumer>();
      m_Consumers.put(i_cname,v);
    }

    v.add(i_consumer);
  }

  public void AddVariableGenerator(String i_cname,ActionVariableGenerator i_generator)
  {
    Vector<ActionVariableGenerator> v;

    if (m_Generators.containsKey(i_cname))
    {
      v = m_Generators.get(i_cname);
    }
    else
    {
      v = new Vector<ActionVariableGenerator>();
      m_Generators.put(i_cname,v);
    }

    v.add(i_generator);
  }

  public void RemoveVariableGenerator(String i_cname,ActionVariableGenerator i_generator)
  {
    Vector<ActionVariableGenerator> v;
    if (m_Generators.containsKey(i_cname))
    {
      v = m_Generators.get(i_cname);
      v.remove(i_generator);
    }
  }

  public void ResetVariableGenerators(String i_cname)
  {
    Vector<ActionVariableGenerator> v = m_Generators.get(i_cname);
    if (v == null) return;

    int i;
    for (i = 0 ; i < v.size() ; ++i)
    {
      v.get(i).ActionVariableReset();
    }
  }

  public String GetVariableGeneratorValue(String i_cname)
  {
    Vector<ActionVariableGenerator> v = m_Generators.get(i_cname);
    if (v == null) return null;

    if (v.size() != 1)
    {
      return null;
    }

    return v.get(0).ActionVariableGetValue();
  }

  public void ReportNewValueToConsumers(String i_cname,String i_newvalue)
  {
    Vector<ActionVariableConsumer> v = m_Consumers.get(i_cname);
    if (v == null) return;

    int i;
    for (i = 0 ; i < v.size() ; ++i)
    {
      v.get(i).ReportNewVariableValue(i_newvalue);
    }
  }
}    
    

  
