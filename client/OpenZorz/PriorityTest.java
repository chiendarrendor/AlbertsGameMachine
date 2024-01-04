package OpenZorz;



class PriorityTest
{
  static class TestVVO implements PriorityMap.VarValOperator
  {
    public void OperateVarVal(String i_key,String i_val)
    {
      System.out.println("Key: " + i_key + "    Value: " + i_val);
    }
  }

  public static void main(String argv[])
  {
    PriorityMap pmap = new PriorityMap();

    TestVVO vvo = new TestVVO();

    try
    {
      pmap.OpenLevel();
      pmap.AddVarVal("foo","fooval1");
      pmap.AddVarVal("bar","barval1");

      System.out.println("Level 1---foo and bar set");
      pmap.ForEachVarVal(vvo);

      pmap.OpenLevel();
      pmap.AddVarVal("foo","fooval2");
      pmap.AddVarVal("baz","bazval2");


      System.out.println("Level 2---foo overridden, baz set");
      pmap.ForEachVarVal(vvo);

      pmap.OpenLevel();
      pmap.AddVarVal("bar","barval3");
      pmap.AddVarVal("baz","bazval3");

      System.out.println("Level 3---bar overridden, baz overrriden");
      pmap.ForEachVarVal(vvo);

      pmap.CloseLevel();

      System.out.println("---close level three");
      pmap.ForEachVarVal(vvo);


      pmap.CloseLevel();

      System.out.println("---close level 2");
      pmap.ForEachVarVal(vvo);

    }
    catch(PriorityMapException pme)
    {
      System.out.println("Exception Thrown" + pme);
    }
  }
}
