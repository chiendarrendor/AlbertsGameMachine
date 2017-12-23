package OpenZorz;
import javax.xml.parsers.DocumentBuilder; 
import javax.xml.parsers.DocumentBuilderFactory;   
import javax.xml.parsers.ParserConfigurationException;
 
import org.xml.sax.SAXException;   

import java.io.File;
import java.io.IOException;

import org.w3c.dom.Document;

public class DomReader{
    // Global value so it can be ref'd by the tree-adapter
    static Document document; 
    static String tab="  ";

    public static String nstr(String i_string)
    {
	if (i_string == null)
	{
	    return "<null>";
	}
	return i_string;
    }

    public static void PrintNode(org.w3c.dom.Node i_node,String i_indent)
    {
	int i;
	System.out.println(i_indent + "Node: " + i_node.getNodeName() + 
			   "(" + i_node.getNodeType() + ")");
	if (i_node.getNodeValue() != null)
	{
	    System.out.println(i_indent + tab + "Value: |" + i_node.getNodeValue() + "|");
	}
	
	if (i_node.hasAttributes())
	{
	    System.out.println(i_indent + tab + "Attributes (" + 
			       i_node.getAttributes().getLength() + "):");
	    for (i = 0 ; i < i_node.getAttributes().getLength() ; ++i)
	    {
		PrintNode(i_node.getAttributes().item(i),i_indent + tab + tab);
	    }
	}

	if (i_node.hasChildNodes())
	{
	    System.out.println(i_indent + tab + "Children (" + 
			       i_node.getChildNodes().getLength() + "):");
	    for (i = 0 ; i < i_node.getChildNodes().getLength() ; ++i)
	    {
		PrintNode(i_node.getChildNodes().item(i),i_indent + tab + tab);
	    }
	}
    }

    public static void main(String argv[])
    {
        if (argv.length != 1) {
            System.err.println("Usage: java DomEcho filename");
            System.exit(1);
        }

        DocumentBuilderFactory factory =
            DocumentBuilderFactory.newInstance();
        //factory.setValidating(true);   
        //factory.setNamespaceAware(true);
        try {
           DocumentBuilder builder = factory.newDocumentBuilder();
           document = builder.parse( new File(argv[0]) );

	   PrintNode(document,"");
        } catch (SAXException sxe) {
           // Error generated during parsing)
           Exception  x = sxe;
           if (sxe.getException() != null)
               x = sxe.getException();
           x.printStackTrace();

        } catch (ParserConfigurationException pce) {
            // Parser with specified options can't be built
            pce.printStackTrace();

        } catch (IOException ioe) {
           // I/O error
           ioe.printStackTrace();
        }
    } // main



}
