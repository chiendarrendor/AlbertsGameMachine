package OpenZorz;

import javax.xml.parsers.DocumentBuilder; 
import javax.xml.parsers.DocumentBuilderFactory;    
import javax.xml.parsers.ParserConfigurationException;
 
import org.xml.sax.SAXException;  
import java.io.*;
import java.net.*;

import org.w3c.dom.Document;
import org.w3c.dom.DOMException;



public class GUIUnit
{
  class PassIntoGui implements Runnable
  {
    private String m_contents;
    private GameGui m_gamegui;
    public PassIntoGui(String i_contents,GameGui i_gamegui) { m_contents = i_contents; m_gamegui = i_gamegui;}
    public void run()
    {
      m_gamegui.HandleEvent(m_contents);
    }
  }

  private GameGui m_gamegui;

  public void dispose()
  {
    m_gamegui.dispose();
  }
  
  public GUIUnit(PrintWriter i_out,StatusWindows i_StatusWindows,TabbedWindow i_tabs)
  {
    m_gamegui = new GameGui(i_out,i_StatusWindows,i_tabs);
  }

  private boolean m_invokeok;

  public boolean ParseXML(String i_Name,String i_Location,String i_XMLFileName)
  {
    boolean parsed = false;
    final String xmlloc = i_Location + "/" + i_XMLFileName;
    final String loc = i_Location;
    final String name = i_Name;

    try
    {
      DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
      DocumentBuilder builder = factory.newDocumentBuilder();
      Document thedoc;

      try
      {
        thedoc = builder.parse(xmlloc);
      }
      catch(UnknownHostException uhe)
      {
        System.out.println("URL " + xmlloc + " not working, retrying as file.");

        FileInputStream fis = new FileInputStream(xmlloc);
        thedoc = builder.parse(fis);
      }
      catch(MalformedURLException uhe)
      {
        System.out.println("URL " + xmlloc + " not working, retrying as file.");

        FileInputStream fis = new FileInputStream(xmlloc);
        thedoc = builder.parse(fis);
      }

      final Document document = thedoc;


        
      m_invokeok = false;

      javax.swing.SwingUtilities.invokeAndWait(new Runnable() 
        { public void run()
          {   
            try
            {
              System.out.println("Beginning Parse of XML");
              GameXMLWalker reader = new GameXMLWalker(document,name,loc,m_gamegui);
              System.out.println("Parse done.  Realizing Windows");
              reader.GetGameGui().RealizeWindows();
              m_invokeok = true;
            }
            catch (GameXMLException gxe)
            {
              System.out.println("Parse of " + xmlloc + " failed:" + gxe);
            }
          }
        });

      parsed = m_invokeok;


    } catch (SAXException sxe) {
      // Error generated during parsing)
      Exception  x = sxe;
      if (sxe.getException() != null)
        x = sxe.getException();
      x.printStackTrace();
    } catch (DOMException dme) {
      dme.printStackTrace();
    } catch (ParserConfigurationException pce) {
      // Parser with specified options can't be built
      pce.printStackTrace();

    } catch (IOException ioe) {
      // I/O error
      ioe.printStackTrace();
    }
    catch(InterruptedException ie) {
	    ie.printStackTrace();
    }
    catch(java.lang.reflect.InvocationTargetException ite) {
	    ite.printStackTrace();
    }

    return parsed;
  }

  void HandleLine(String i_line)
  {
    javax.swing.SwingUtilities.invokeLater(new PassIntoGui(i_line,m_gamegui));
  }

}

