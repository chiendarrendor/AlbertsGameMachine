package OpenZorz;

import java.util.*;

class GUIUnitMap
{
  public interface GUIUnitFunctor
  {
    public void Do(GUIUnit i_guiunit);
  };

  private TreeMap<String,GUIUnit> m_guiunits;

  public GUIUnitMap()
  {
    m_guiunits = new TreeMap<String,GUIUnit>();
  }

  public boolean IsInMap(String i_key)
  {
    return m_guiunits.containsKey(i_key);
  }


  public boolean AddToMap(String i_key,GUIUnit i_unit)
  {
    if (IsInMap(i_key))
    {
      return false;
    }
    m_guiunits.put(i_key,i_unit);
    return true;
  }

  public GUIUnit Get(String i_key)
  {
    if (!IsInMap(i_key))
    {
      return null;
    }
    GUIUnit res = (GUIUnit)m_guiunits.get(i_key);
    return res;
  }

  public GUIUnit PullFromMap(String i_key)
  {
    if (!IsInMap(i_key))
    {
      return null;
    }
    
    GUIUnit res = (GUIUnit)m_guiunits.remove(i_key);

    return res;
  }

  public void ForEach(GUIUnitFunctor i_guf)
  {
    Iterator<GUIUnit> gif = m_guiunits.values().iterator();

    while(gif.hasNext())
    {
      i_guf.Do((GUIUnit)gif.next());
    }
  }

  public void dispose()
  {
    Iterator<GUIUnit> gif = m_guiunits.values().iterator();

    while(gif.hasNext())
    {
      ((GUIUnit)gif.next()).dispose();
    }
  }

}
