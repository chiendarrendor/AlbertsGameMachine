#include "Serialize.hpp"
#include <fstream>
#include <iostream>

template<class T>
class foo
{
public:
  foo(int a,int b,int c) :
    m_a(a),m_b(b),m_c(c) 
  {
    int i;
    for (i = 0 ; i < m_a ; ++i)
    {
      m_v.push_back(i * m_b * m_c);
    }
  }
  foo() : m_a(-1),m_b(-1),m_c(-1) {}

  int GetA() { return m_a; }
  int GetB() { return m_b; }
  int GetC() { return m_c; }
  std::vector<int> &GetV() { return m_v; }
private:
  SERIALIZE_FUNC
  {
    SERIALIZE(m_a);
    SERIALIZE(m_b);
    SERIALIZE(m_c);
    SERIALIZE(m_v);
  }
  int m_a;
  int m_b;
  int m_c;
  std::vector<int> m_v;
};

typedef foo<int> fooint;


class bar : public fooint
{
public:
  bar(int a,int b,int c,int d) :
    fooint(a,b,c),
    m_d(d)
  {}

  bar() : fooint(),m_d(-1) {}

  int GetD() { return m_d;}
  
private:
  int m_d;
  SERIALIZE_FUNC
  {
    SERIALIZE_PARENT(fooint);
    SERIALIZE(m_d);
  }
};



int main(int argc,char **argv)
{
  try
  {
    bar f(8,3,1,3);

    {
      std::ofstream ofile("sertest");
      ofile << "One Line" << std::endl;
      SaveArchive i_ar(ofile);
      SERIALIZE(f);
      ofile.close();
    }

    bar g;

    {
      std::ifstream ifile("sertest");
      std::string s;
      std::getline(ifile,s);
      LoadArchive i_ar(ifile);
      SERIALIZE(g);
      ifile.close();
    }

    std::cout << "A: " << g.GetA() << "  B: " << g.GetB() 
              << "  C: " << g.GetC() << " D: " << g.GetD() << std::endl;

    for (int i = 0 ; i < g.GetV().size() ; i++)
    {
      std::cout << i << "  " << g.GetV()[i] << std::endl;
    }
  }
  catch(...)
  {
    std::cout << "Caught exception!" << std::endl;
  }
}

  
