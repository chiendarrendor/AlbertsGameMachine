package Utilities;



public class StringUtility
{
  public static String GlobalReplaceString(String i_Source,String i_orig,String i_replace)
  {
    if (i_Source == null) return null;

    int olen = i_orig.length();
    String i_result = i_Source;
	
    while(true)
    {
	    int idx = i_result.indexOf(i_orig);
	    if (idx == -1) break;

	    StringBuffer sb = new StringBuffer(i_result);
	    sb.replace(idx,idx+olen,i_replace);
	    i_result = sb.toString();
    }
    return i_result;
  }

  public static String UnComma(String i_Source)
  {
    if (i_Source.equals("")) return i_Source;
    StringBuffer sb = new StringBuffer(i_Source.length() * 2);
    int i;
    for (i = 0 ; i < i_Source.length() ; ++i)
    {
	    char c = i_Source.charAt(i);
	    switch(c)
	    {
	    case ',':
        sb.append('%');
        sb.append('C');
        break;
	    case '%':
        sb.append('%');
        sb.append('%');
        break;
	    default:
        sb.append(c);
        break;
	    }
    }
    return sb.toString();
  }

  public static String ReComma(String i_Source)
  {
    StringBuffer sb = new StringBuffer(i_Source.length());
    int i;
    for (i = 0 ; i < i_Source.length() ; ++i)
    {
	    char c = i_Source.charAt(i);
	    if (c != '%')
	    {
        sb.append(c);
        continue;
	    }
	    if (i == i_Source.length() - 1)
	    {
        sb.append(c);
        continue;
	    }
	    char cplus = i_Source.charAt(i+1);
	    if (cplus == 'C')
	    {
        sb.append(',');
        i++;
        continue;
	    }
	    if (cplus == '%')
	    {
        sb.append('%');
        i++;
        continue;
	    }
	    sb.append(c);
    }
    return sb.toString();
  }

  static public String Mangle(String i_Source)
  {
    if (i_Source == null) return null;
    StringBuffer sb = new StringBuffer(i_Source.length());
    int i;
    for (i = 0 ; i < i_Source.length() ; ++i)
    {
	    char c = i_Source.charAt(i);
	    int v = ((c * (i+1)) % 96) + 32;
	    
	    char mc = (char)v;
	    
	    sb.append(mc);
    }
    return sb.toString();
  }
}





