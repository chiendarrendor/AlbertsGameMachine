package OpenZorz;

import javax.swing.*;

class SpinnerResettableTextModel extends AbstractSpinnerModel
{
  private String[] m_Entries;
  private int m_ResetIndex;
  private int m_CurIndex;
  private static final String s_ZeroMark = "<mark>";

  public SpinnerResettableTextModel(String i_data)
  {
    m_Entries = i_data.split(",");
    m_CurIndex = 0;
    m_ResetIndex = 0;


    int i;
    for (i = 0 ; i < m_Entries.length ; ++i)
    {
      if (m_Entries[i].startsWith(s_ZeroMark))
      {
        m_CurIndex = i;
        m_ResetIndex = i;
        m_Entries[i] = m_Entries[i].substring(s_ZeroMark.length());
        break;
      }
    }
  }

  public void Reset()
  {
    m_CurIndex = m_ResetIndex;
    fireStateChanged();
  }

  public Object getValue()
  {
    return m_Entries[m_CurIndex];
  }

  public Object getNextValue()
  {
    if (m_CurIndex + 1 == m_Entries.length)
    {
      return null;
    }

    return m_Entries[++m_CurIndex];
  }

  public Object getPreviousValue()
  {
    if (m_CurIndex == 0)
    {
      return null;
    }

    return m_Entries[--m_CurIndex];
  }

  public void setValue(Object value)
  {
    String vs = (String)value;
    if (vs == null)
    {
      throw new IllegalArgumentException();
    }
    int i;
    for (i = 0 ; i < m_Entries.length ; ++i)
    {
      if (vs.equals(m_Entries[i]))
      {
        m_CurIndex = i;
        fireStateChanged();
        return;
      }
    }

    throw new IllegalArgumentException();
  }
  

}

