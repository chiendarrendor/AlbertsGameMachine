// hpp
public:
  void Visualize();

private:
  bool HasLoop(const std::string& region);
  void LoopVisualize(const std::string& region,NextInfo& nexti);
  void MGSVisualize(const std::string& region,NextInfo& nexti);
  void CloudVisualize(const std::string& region,NextInfo& nexti);
  void AsteroidVisualize(const std::string& region,NextInfo& nexti);
  void GalBaseVisualize(const std::string& region,NextInfo& nexti);
  void LineVisualize(const std::string& region,const std::string& priorspace,
                     NextInfo& nexti);

  void SpaceVisualize(XYPrint& xyp,int x,int y,int width,MapSpace *pSpace);
  void SplitPrint(XYPrint& xyp,int x,int y,int width,const std::string& name);

  int m_width;
  int m_height;
  int m_xsep;
  int m_mainy;

// in ctor
  m_width = 119;
  m_height = 40;
  m_xsep = 10;
  m_mainy=17;



class NextInfo
{
public:

  std::pair<std::string,std::string> GetNext()
  {
    size_t choice;

    while(1)
    {
      std::cout << visualized;
      for(size_t i = 0 ; i < outs.size(); ++i)
      {
        std::cout << i << ": " << outs[i].first << "\n";
      }
      static const boost::regex r("^\\d$");
      std::cout << "Which? ";
      std::string lin;
      getline(std::cin,lin);
      if (!boost::regex_match(lin,r))
      {
        std::cout << "illegal input" << "\n";
        continue;
      }
      choice = boost::lexical_cast<int>(lin);
      if (choice >= outs.size())
      {
        std::cout << "out-of-bounds input" << "\n";
        continue;
      }
      break;
    }

    std::pair<std::string,std::string> result(outs[choice].first,outs[choice].second);
    return result;
  }

  std::string visualized;
  // next region, space in current region that leaves to that next region.
  std::vector<std::pair<std::string,std::string> > outs;
};

void MapReader::SplitPrint(XYPrint& xyp,int x,int y,
                int width,const std::string &name)
{
  std::vector<std::string> splits;
  boost::split(splits,name,boost::is_any_of(" "));

  int xoff = 0;
  size_t num = splits.size() - 1;
  if (splits[num].size() < (size_t)width) xoff = (width - splits[num].size())/2;
  xyp.AddString(splits[num],x+xoff,y+1);
  std::ostringstream oss;
  for (size_t i = 0 ; i < splits.size()-1 ; ++i)
  {
    if (i != 0) oss << ' ';
    oss << splits[i];
  }

  xoff = 0;
  if (oss.str().size() < (size_t)width) xoff = (width-oss.str().size())/2;
  xyp.AddString(oss.str(),x+xoff,y-1);
}
    

      


// x = left most x, y = center y (+/-1 available)
// 
void MapReader::SpaceVisualize(XYPrint& xyp,int x,int y,
                               int width,MapSpace *pSpace)
{
  Color c = pSpace->m_color;
  int p = pSpace->m_cost;
  char cc = (c == BLUE ? 'b' : (c == RED ? 'r' : 'y')); 
  int xoff = width/2;

  switch (pSpace->m_type)
  {
  case CITY: 
    SplitPrint(xyp,x,y,width,pSpace->m_name);
    xyp.SetChar(x+xoff,y,'#'); 
    break;
  case STATION: 
    SplitPrint(xyp,x,y,width,pSpace->m_name);
    xyp.SetChar(x+xoff,y,'O'); 
    break;
  case ASTEROID: xyp.SetChar(x+xoff,y,'*'); break;
  case QBOX: xyp.SetChar(x+xoff,y,'?'); break;
  case PENALTY: 
    {
      std::ostringstream oss;
      oss << cc << '(' << p << ')';
      xyp.AddString(oss.str(),x+xoff-3,y);
    }
    break;
  case DOT:
    if (pSpace->m_orbit.size() > 0) goto ORBIT;
    xyp.SetChar(x+xoff-1,y,cc);
    xyp.SetChar(x+xoff,y,'@');
    break;
  case NAVCIRCLE:
    if (pSpace->m_orbit.size() > 0) goto ORBIT;
    xyp.SetChar(x+xoff-1,y,'(');
    xyp.SetChar(x+xoff+1,y,')');
    break;
  ORBIT:
    SplitPrint(xyp,x,y,width,pSpace->m_orbit);
    xyp.AddString("(X)",x+xoff-1,y);
    break;
  default:
    xyp.AddString("???",x+xoff-1,y);
  }
}




void MapReader::LoopVisualize(const std::string& curRegion,NextInfo& ni)
{
  ni.outs.clear();

  // find all spaces belonging to that region, organized by loop id if they are in a loop,
  // and all other spaces.
  std::map<std::string,MapSpace*>::iterator sit;
  std::map<size_t,MapSpace*> loops;
  std::vector<MapSpace*>others;
  for (sit = m_spaces.begin() ; sit != m_spaces.end() ; ++sit)
  {
    MapSpace* pSpace = sit->second;
    if (pSpace->m_region.m_name != curRegion) continue;
    static const boost::regex r("(\\S+)_loop_(\\d+)");
    boost::smatch what;
    if (boost::regex_match(pSpace->m_name,what,r))
    {
      loops[boost::lexical_cast<size_t>(what[2])] = pSpace;
    }
    else
    {
      others.push_back(pSpace);
    }
  }
  XYPrint xyp(m_width,m_height);
  //  int ysep=5;

  std::map<size_t,MapSpace*>::iterator loopit;
  int i = 0;
  for (loopit = loops.begin() ; loopit != loops.end() ; ++loopit,++i)
  {
    MapSpace *curspace = loopit->second;
    int curx = m_xsep * i;

    SpaceVisualize(xyp,curx,m_mainy,m_xsep,curspace);

    // find if this space
    // a) has a single connection to another region
    // b) has a connection to a city or station.
    std::vector<std::pair<std::string,bool> >::iterator adjit;
    for (adjit = curspace->m_adjacentnames.begin() ; adjit != curspace->m_adjacentnames.end() ; ++adjit)
    {
      MapSpace *pNewSpace = m_spaces[adjit->first];
      if (pNewSpace->m_region.m_name !=
          curspace->m_region.m_name)
      {
        xyp.SetChar(curx+m_xsep/2-1,m_mainy-2,'|');
        std::ostringstream oss;
        oss << pNewSpace->m_region.m_name << " " << '(' <<
          ni.outs.size() << ')';
        SplitPrint(xyp,curx,m_mainy-4,m_xsep,oss.str());
        ni.outs.push_back(std::pair<std::string,std::string>(pNewSpace->m_region.m_name,curspace->m_name));
      }
      else if (pNewSpace->m_type == CITY ||
               pNewSpace->m_type == STATION)
      {
        xyp.SetChar(curx+m_xsep/2-1,m_mainy+2,'|');
        SpaceVisualize(xyp,curx,m_mainy+4,m_xsep,pNewSpace);
      }
    }
  }
  ni.visualized = xyp.Show();
}




void MapReader::Visualize()
{
  std::string curRegion = "Trapped Rogue Planet";
  std::string priorSpace = "";
  
  while(1)
  {
    // cases
    // a) regions with loops
    // a') exception: interstellar biosphere (maybe?)
    // b) Multi-Generation Ship (acts like loop, but doesn't have 'loop' spaces
    // c) The Cloud
    // d) Asteroid System
    // e) everything else is a line.

    NextInfo ni;

    if (curRegion == "Interstellar Biosphere") LoopVisualize(curRegion,ni);
    else if (curRegion == "Multi-Generation Ship") MGSVisualize(curRegion,ni);
    else if (curRegion == "The Cloud") CloudVisualize(curRegion,ni);
    else if (curRegion == "Asteroid System") AsteroidVisualize(curRegion,ni);
    else if (curRegion == "Galactic Base") GalBaseVisualize(curRegion,ni);
    else if (HasLoop(curRegion)) LoopVisualize(curRegion,ni);
    else LineVisualize(curRegion,priorSpace,ni);

    std::pair<std::string,std::string> result = ni.GetNext();
    curRegion = result.first;
    priorSpace = result.second;
  }
}

bool MapReader::HasLoop(const std::string& region)
{
  std::map<std::string,MapSpace*>::iterator sit;
  for (sit = m_spaces.begin() ; sit != m_spaces.end() ; ++sit)
  {
    MapSpace* pSpace = sit->second;
    if (pSpace->m_region.m_name != region) continue;
    static const boost::regex r("(\\S+)_loop_(\\d+)");
    if (boost::regex_match(pSpace->m_name,r))
    {
      return true;
    }
  }
  return false;
}

void MapReader::LineVisualize(const std::string& region,const std::string& priorspace,NextInfo& nexti)
{
  // first, find the name of the adjacent in priorspace
  // that is in this region
  MapSpace* priorSpace = m_spaces[priorspace];
  MapSpace* curSpace;
  MapSpace* nextSpace;

  std::vector<std::pair<std::string,bool> >::iterator adjit;
  for (adjit = priorSpace->m_adjacentnames.begin() ; adjit != priorSpace->m_adjacentnames.end() ; ++adjit)
  {
    curSpace = m_spaces[adjit->first];
    if (curSpace->m_region.m_name == region) break;
  }

  XYPrint xyp(m_width,m_height);

  int curidx = 0;
  std::ostringstream oss;
  oss << priorSpace->m_region.m_name << ' ' << '(' <<
    nexti.outs.size() << ')';
  
  SplitPrint(xyp,curidx*m_xsep,m_mainy,m_xsep,oss.str());

  nexti.outs.push_back(std::pair<std::string,std::string>(priorSpace->m_region.m_name,curSpace->m_name));


  while(1)
  {
    // curidx points to the previous cell
    // priorSpace points to the direction we don't want to go
    // curSpace points to the Space we want to draw.
    ++curidx;

    if (curSpace->m_region.m_name == region)
    {
      SpaceVisualize(xyp,curidx*m_xsep,m_mainy,m_xsep,curSpace);
    }
    else
    {
      std::ostringstream oss;
      oss << curSpace->m_region.m_name << ' ' << '(' <<
        nexti.outs.size() << ')';
      SplitPrint(xyp,curidx*m_xsep,m_mainy,m_xsep,oss.str());
      nexti.outs.push_back(std::pair<std::string,std::string>(curSpace->m_region.m_name,priorSpace->m_name));
      break;
    }

    for (adjit = curSpace->m_adjacentnames.begin() ; adjit != curSpace->m_adjacentnames.end() ; ++adjit)
    {
      nextSpace = m_spaces[adjit->first];
      if (nextSpace != priorSpace) break;
    }


    priorSpace = curSpace;
    curSpace = nextSpace;
  }
  nexti.visualized = xyp.Show();
}


// ----------------



void MapReader::MGSVisualize(const std::string& region,NextInfo& nexti)
{
}
void MapReader::CloudVisualize(const std::string& region,NextInfo& nexti)
{
}
void MapReader::AsteroidVisualize(const std::string& region,NextInfo& nexti)
{
}
void MapReader::GalBaseVisualize(const std::string& region,NextInfo& nexti)
{
}
