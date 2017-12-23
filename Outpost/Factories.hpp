#include "Outpost.hpp"
#include "Serialize.hpp"


class Factories
{
public:
  Factories();
  void AddFactory(FactoryType i_FType);

  std::string GetFactoryDescription() const;

  // -1 = invalid manninglist...not the same size as the Factory List
  // -2 = invalid manninglist...too many robots or people.
  // -3 = invalid manninglist...illegal character
  // -4 = invalid manninglist...moving a human off of a mannable item
  //    -- obsoleting this error case.
  //  0 = valid manning alteration

  int AlterManning(const std::string &i_ManningList,int i_NPeople,int i_NRobots);

  int GetVPs() const;
  int GetPeopleIncrease() const;

  int GetMannedFactoryCount(FactoryType i_FType) const;
  int GetUnmannedFactoryCount(FactoryType i_FType) const;

  int GetUsedMen() const;
  int GetUsedRobots() const;


  size_t size() const; 

  typedef std::pair<FactoryType,ManningType> FactoryDatum;

private:

  SERIALIZE_FUNC
  {
    SERIALIZE(m_Factories);
  }
  std::vector<FactoryDatum>m_Factories;
};

