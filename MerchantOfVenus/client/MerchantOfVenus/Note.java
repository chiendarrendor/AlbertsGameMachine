package MerchantOfVenus;

public class Note
{
  public static final int BASENOTE = 1;
  public static final int DISCOUNTNOTE = 2;
  public static final int BONUSNOTE = 3;
  public static final int FINALNOTE = 4;
  public static final int NOTNOTE = 5;
  public static final int COMMISSIONNOTE = 6;

  public String note;
  public int type;
  
  public Note(int t,String n) { note = n ; type = t; }
}
