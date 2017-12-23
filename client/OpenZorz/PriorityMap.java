package OpenZorz;


import java.util.*;
// this class is designed to store a set of variables and their values
// at multiple 'priority' levels, indicated by OpenLevel() and CloseLevel().
// a variable can be given a value at a particular level by calling
// AddVarVal(name,value); the most recent call to AddVarVal() for a given
// variable while a level is active will be the value used at that level.
// retrieval of values is mediated by the interface VarValOperator; an
// object realizing that interface passed to ForEachVarVal() will have
// OperateVarVal called once for every variable Added at or below
// the current priority level, with the value of that variable being
// whatever was added at the highest priority level at or below
// the current priority level.  
// CloseLevel() removes from the class all reference to variable/value
// pairs added at that level.




public class PriorityMap
{
  private HashMap<String,PriorityEntry> m_PMap;
  private int m_CurLevel;

  public interface VarValOperator
  {
    public void OperateVarVal(String i_key,String i_val);
  }


  private class PriorityEntry
  {
    private TreeMap<Integer,String> m_TMap;

    public PriorityEntry()
    {
      m_TMap = new TreeMap<Integer,String>();
    }

    public boolean IsEmpty()
    {
      return m_TMap.isEmpty();
    }

    public void CloseLevel(int i_Level) throws PriorityMapException
    {
      if (m_TMap.size() == 0)
      {
        return;
      }

      int maxlevel = (m_TMap.lastKey()).intValue();

      if (i_Level > maxlevel)
      {
        return;
      }

      if (i_Level < maxlevel)
      {
        throw new PriorityMapException("Closing levels out of order!");
      }

      m_TMap.remove(new Integer(maxlevel));

    }

    public void AddEntry(int i_Level,String i_Val)
    {
      m_TMap.put(new Integer(i_Level),i_Val);

    }

    public String GetHighestPriorityEntry()
    {
      if (m_TMap.size() == 0)
      {
        return null;
      }
      return (String)m_TMap.get(m_TMap.lastKey());
    }
  }

  public PriorityMap()
  {
    m_PMap = new HashMap<String,PriorityEntry>();
    m_CurLevel = 0;
  }

  public void OpenLevel()
  {
    ++m_CurLevel;
  }

  public void CloseLevel() throws PriorityMapException
  {
    ArrayList<Map.Entry<String,PriorityEntry>> doomed = new ArrayList<Map.Entry<String,PriorityEntry>>();
    Iterator<Map.Entry<String,PriorityEntry>> mapit = m_PMap.entrySet().iterator();
    while(mapit.hasNext())
    {
      Map.Entry<String,PriorityEntry> mapent = mapit.next();
      PriorityEntry pent = mapent.getValue();
      pent.CloseLevel(m_CurLevel);
      if (pent.IsEmpty())
      {
        doomed.add(mapent);
      }
    }
    
    --m_CurLevel;
    if (m_CurLevel < 0)
    {
      throw new PriorityMapException("Can't Close base level");
    }

    int i;
    for (i = 0 ; i < doomed.size() ; ++i)
    {
      m_PMap.entrySet().remove((Map.Entry<String,PriorityEntry>)doomed.get(i));
    }
  }

  public void AddVarVal(String i_VarName,String i_VarVal)
  {
    
    PriorityEntry pent;

    if (!m_PMap.containsKey(i_VarName))
    {
      pent = new PriorityEntry();
      m_PMap.put(i_VarName,pent);
    }
    else
    {
      pent = m_PMap.get(i_VarName);
    }

    pent.AddEntry(m_CurLevel,i_VarVal);
  }

  public void ForEachVarVal(VarValOperator i_vvo)
  {
    Iterator<Map.Entry<String, PriorityEntry> > mapit = m_PMap.entrySet().iterator();
    while(mapit.hasNext())
    {
      Map.Entry<String,PriorityEntry> mapent = (Map.Entry<String,PriorityEntry>)mapit.next();
      String key = (String)mapent.getKey();
      PriorityEntry pent = (PriorityEntry)mapent.getValue();
      i_vvo.OperateVarVal(key,pent.GetHighestPriorityEntry());
    }
  }
}
  

