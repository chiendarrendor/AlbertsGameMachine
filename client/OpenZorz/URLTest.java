package OpenZorz;

import java.net.*;
import java.io.*;

class URLTest
{

  public static void main(String argv[])
  {
    try
    {
      URL url = new URL("http://albert.best.vwh.net/games/Aor/Icons/London_New_Token.JPG");
      System.out.println(url);
      URLConnection urlc = url.openConnection();
      InputStream is = urlc.getInputStream();
      BufferedReader br = new BufferedReader(new InputStreamReader(is));

      String line;

      while (true)
      {
        line = br.readLine();
        if (line == null) 
        {
          break;
        }

        System.out.println("> " + line);
      }
    }
    catch(MalformedURLException mex)
    {
      System.out.println("Malformed URL Exception Caught: " + mex);
    }
    catch(IOException iox)
    {
      System.out.println("IO Exception Caught: " + iox);
    }
  }
}
