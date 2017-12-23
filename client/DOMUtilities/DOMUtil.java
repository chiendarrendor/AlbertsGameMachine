package DOMUtilities;

import java.util.*;
import Utilities.StringUtility;

public class DOMUtil
{
  // returns true if all the node's attributes are named by an entry in i_Strings
  public static String FirstAttributeNotIn(org.w3c.dom.Node i_Node,HashSet<String> i_Strings)
  {
    if (!i_Node.hasAttributes()) return null;
    int i;
    for (i = 0 ; i < i_Node.getAttributes().getLength() ; ++i)
    {
	    String s = i_Node.getAttributes().item(i).getNodeName();
	    if (!i_Strings.contains(s)) return s;
    }
    return null;
  }

  public static int GetIntAttribute(org.w3c.dom.Node i_Node,String i_Name)
  {
    String s=GetAttributeWithName(i_Node,i_Name);
    if (s == null) return 0;
    return Integer.parseInt(s);
  }

  public static String GetAttributeWithName(org.w3c.dom.Node i_Node,String i_Name)
  {
    if (!i_Node.hasAttributes()) return null;
    org.w3c.dom.Node attr = i_Node.getAttributes().getNamedItem(i_Name);
    if (attr == null) return null;
    return attr.getNodeValue();
  }

  // recursively looks at all nodes under this one (attributes and children)
  // and replaces any instance of i_text in the Node Value with i_replace.
  public static void RecursiveValueReplace(org.w3c.dom.Node i_Node,String i_text,String i_replace)
  {
    // look for an attribute with name equal to the string to replace.
    org.w3c.dom.NamedNodeMap nnm = i_Node.getAttributes();
    if (nnm != null)
    {
      org.w3c.dom.Node attrnode = nnm.getNamedItem(i_text);
      if (attrnode != null)
      {
        // if we find it, make a new node with a new name identical to the prior node with the new name
        // and replace it
        org.w3c.dom.Document doc = i_Node.getOwnerDocument();
        if (doc == null)
        {
          System.out.println("wtf?");
        }

        org.w3c.dom.Attr newattr = doc.createAttribute(i_replace);
        newattr.setValue(attrnode.getNodeValue());
        nnm.removeNamedItem(i_text);
        nnm.setNamedItem(newattr);
      }
    }

    // apply the text replace to this node's contents.
    i_Node.setNodeValue(StringUtility.GlobalReplaceString(i_Node.getNodeValue(),i_text,i_replace));
    int i;
	
    // if node has attributes, replace their text, too (note that this mechanism doesn't change attr names)
    if (i_Node.hasAttributes())
    {
	    for(i = 0 ; i < i_Node.getAttributes().getLength() ; ++i)
	    {
        RecursiveValueReplace(i_Node.getAttributes().item(i),i_text,i_replace);
	    }
    }

    // if this node has children, replace their text too.
    DOMChildIterator dch = new DOMChildIterator(i_Node);
    org.w3c.dom.Node inode;

    while((inode = dch.GetNext()) != null)
    {
	    RecursiveValueReplace(inode,i_text,i_replace);
    }
  }

  public static String ShowNode(org.w3c.dom.Node i_Node)
  {
    int i;
    String result = new String();
    result += "(";

    if (i_Node == null) 
    {
      result += ")";
      return result;
    }
    
    result += i_Node.getNodeName() + ": ";
    
    if (i_Node.hasAttributes())
    {
	    for(i = 0 ; i < i_Node.getAttributes().getLength() ; ++i)
	    {
        result += i_Node.getAttributes().item(i).getNodeName() + " = " + 
          i_Node.getAttributes().item(i).getNodeValue() + " -- ";
      }
    }

    result += ")";

    result = ShowNode(i_Node.getParentNode()) + result;
    
    return result;
  }

}

    
