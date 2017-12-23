package MerchantOfVenus;

import java.util.*;

public class TokenHolder
{
  private Vector<Token> tokens;
  
  public Vector<Token> GetTokens() { return tokens; }

  public TokenHolder()
  {
    tokens = new Vector<Token>();
  }

  public void AddToken(Token t) { tokens.add(t); }
  public void RemoveToken(String encoding)
  {
	System.out.println("removing token: " + encoding);
	System.out.println("before:");
    for (Token t : tokens) { System.out.println("  "+t.GetEncoding()); }
	  
    int i;
    for (i = 0 ; i < tokens.size() ; ++i)
    {
      if (tokens.elementAt(i).GetEncoding().equals(encoding)) break;
    }

    if (i < tokens.size()) tokens.remove(i);

    System.out.println("after:");
    for (Token t : tokens) { System.out.println("  "+t.GetEncoding()); }
    System.out.println("done");
  }

  public void Age()
  {
    for (Token t : tokens) t.Age();
  }

  public class DupToken
  {
    public int count;
    public Token token;
    public DupToken(Token t) { token = t; count = 1; }
  }

  public Vector<DupToken> GetDupTokenList()
  {
    Vector<DupToken> result = new Vector<DupToken>();
    Map<String,DupToken> lookup = new HashMap<String,DupToken>();

    for (Token t : tokens)
    {
      if (lookup.containsKey(t.GetEncoding()))
      {
        lookup.get(t.GetEncoding()).count++;
      }
      else
      {
        DupToken dt = new DupToken(t);
        result.add(dt);
        lookup.put(t.GetEncoding(),dt);;
      }
    }
    return result;
  }
}

  