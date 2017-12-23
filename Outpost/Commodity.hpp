#ifndef COMMODITYHPP
#define COMMODITYHPP

#include "Outpost.hpp"
#include "Serialize.hpp"

class Commodity
{
public:
  Commodity();
  Commodity(CommodityType i_Type,int i_Value,bool i_IsAverage,bool i_IsMega);
  CommodityType GetType() const;
  bool IsAverage() const;
  bool IsMega() const;
  int GetValue() const;

  bool IsNew() const;
  void MakeNew();
  void MakeOld();

private:
  SERIALIZE_FUNC
  {
    SERIALIZE(m_ctype);
    SERIALIZE(m_IsAverage);
    SERIALIZE(m_IsMega);
    SERIALIZE(m_value);

    // version 0 doesn't have the isNew field.
    if (version >= 1) SERIALIZE(m_IsNew);

  }

  CommodityType m_ctype;
  bool m_IsAverage;
  bool m_IsMega;
  int m_value;
  bool m_IsNew;
};

BOOST_CLASS_VERSION(Commodity,1);



#endif
