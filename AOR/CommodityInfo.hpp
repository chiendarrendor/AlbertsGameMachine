#ifndef COMMODITYHPP
#define COMMODITYHPP


#include <string>
#include <map>

class Commodity
{
public:
	Commodity(const std::string &i_Name,int i_Value,int i_SSNum);
	virtual ~Commodity();

	const std::string &GetName() const;
	int GetValue() const;
	int GetSSNum() const;

private:
	std::string m_Name;
	int m_Value;
	int m_SSNum;
};


class CommodityInfo
{
public:
	CommodityInfo();
	virtual ~CommodityInfo();

	Commodity *GetCommodityByName(const std::string &i_Name) const;
  
  const std::map<std::string,Commodity *> &GetCommodities() const;

  static const std::string STONE;
  static const std::string WOOL;
  static const std::string TIMBER;
  static const std::string GRAIN;
  static const std::string CLOTH;
  static const std::string WINE;
  static const std::string METAL;
  static const std::string FUR;
  static const std::string SILK;
  static const std::string SPICE;
  static const std::string GOLD;
  static const std::string IVORY;

private:
	std::map<std::string,Commodity*> m_Commodities;
};


#endif
