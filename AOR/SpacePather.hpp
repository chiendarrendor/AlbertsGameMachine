#include <vector>

class SpaceInfoMap;
class SpaceInfo;



class SpacePather
{
public:
  class Finger
  {
  public:
    Finger(const Finger &i_basis,const SpaceInfo &i_CurSpace);
    Finger(const SpaceInfo &i_CurSpace);
    const std::vector<const SpaceInfo *> &GetHistory() const;
    const SpaceInfo &GetCurSpace() const;
  private:
    std::vector<const SpaceInfo *> m_History;
  };
  
  class GrowFilter
  {
  public:
    virtual bool operator()(const SpaceInfo &i_Point) const = 0;
  };

  class PathFilter
  {
  public:
    virtual bool operator()(const Finger &i_Finger) const = 0;
  };

  SpacePather(const SpaceInfoMap &i_SpaceInfoMap);
  virtual ~SpacePather();
  
  void RegisterPathFilter(const PathFilter &i_pf);
  void RegisterFinalPathFilter(const PathFilter &i_fpf);
  void RegisterGrowFilter(const GrowFilter &i_gf);
  void Grow();

  const std::vector<const Finger *> &GetFingers() const;
private:
  bool IsSpaceLegal(const SpaceInfo &i_Space) const;
  bool IsPathLegal(const Finger &i_Finger) const;
  bool IsFinalPathLegal(const Finger &i_Finger) const;

	std::vector<const Finger *>m_Fingers;
	std::vector<const PathFilter *>m_PathFilters;
	std::vector<const GrowFilter *>m_GrowFilters;
	std::vector<const PathFilter *>m_FinalPathFilters;
};

std::ostream &operator<<(std::ostream &o,const SpacePather &);
std::ostream &operator<<(std::ostream &o,const SpacePather::Finger &);
