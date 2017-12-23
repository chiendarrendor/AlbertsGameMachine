package DOMUtilities;

import org.w3c.dom.*;
import java.util.*;

public class AttributeChecker
{
  HashSet<String> m_required;
  HashSet<String> m_allowed;
  public AttributeChecker()
  {
    m_required = new HashSet<String>();
    m_allowed = new HashSet<String>();
  }

  public void AddAttribute(String i_AttrName,boolean i_required)
  {
    m_allowed.add(i_AttrName);
    if (i_required)
    {
	    m_required.add(i_AttrName);
    }
  }
    
  public void ValidateAttributes(org.w3c.dom.Node i_Node) throws DOMException
  {
    String s = DOMUtil.FirstAttributeNotIn(i_Node,m_allowed);
    if (s != null)
    {
	    throw new DOMException(DOMException.SYNTAX_ERR,"Unknown Attribute '" + s + "' in " + DOMUtil.ShowNode(i_Node));
    }
    Iterator<String> it = m_required.iterator();

    while (it.hasNext())
    {
	    String r = it.next();
	
	    if (DOMUtil.GetAttributeWithName(i_Node,r) == null)
	    {
        throw new DOMException(DOMException.SYNTAX_ERR,"Missing Required Attribute '"+r+"' in " + DOMUtil.ShowNode(i_Node));
	    }
    }
  }
}


	
