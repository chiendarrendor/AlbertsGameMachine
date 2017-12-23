#include <sstream>

struct Dest
{
  std::string sname;
  int mpused;
  std::set<size_t> pnumbers;
  bool isjump;

  Dest() : sname(""),mpused(0),pnumbers(),isjump(false) {}

  Dest(const Dest& i_right)
  {
    *this = i_right;
  }

  Dest& operator=(const Dest& i_right)
  {
    if (this==&i_right) return *this;
    sname = i_right.sname;
    isjump = i_right.isjump;
    mpused = i_right.mpused;
    pnumbers = i_right.pnumbers;
    return *this;
  }

  std::string GetPilotNumberString() const
  {
    std::ostringstream oss;
    std::set<size_t>::const_iterator pit;
    for (pit = pnumbers.begin() ; pit != pnumbers.end() ; ++pit)
    {
      oss << *pit;
    }
    return oss.str();
  }

  SERIALIZE_FUNC
  {
    SERIALIZE(sname);
    SERIALIZE(isjump);
    SERIALIZE(mpused);
    SERIALIZE(pnumbers);
  }
};
