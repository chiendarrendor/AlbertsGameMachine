#include <string>
#include <vector>
#include <map>

class CardInfo;

class DeckInfo
{
public:
  DeckInfo();
  virtual ~DeckInfo();

  const CardInfo *GetCardInfo(const std::string &i_Name) const;
  void FillCardNameVector(std::vector<std::string> &io_Vector) const;

  static const std::string ALCHEMISTSGOLD;
  static const std::string ANDREASVESALIUS;
  static const std::string ARMOR;
  static const std::string BARTOLOMEDELASCASAS;
  static const std::string BLACKDEATH;
  static const std::string CHARLEMAGNE;
  static const std::string CHRISTOPHERCOLUMBUS;
  static const std::string CIVILWAR;
  static const std::string DESIDERIUSERASMUS;
  static const std::string DIONYSUSEXIGUUS;
  static const std::string ENLIGHTENEDRULER;
  static const std::string FAMINE;
  static const std::string GALILEOGALILEI;
  static const std::string GUNPOWDER;
  static const std::string HENRYOLDENBURG;
  static const std::string IBNMAJID;
  static const std::string JOHANNGUTENBERG;
  static const std::string LEONARDODAVINCI;
  static const std::string LONGBOW;
  static const std::string MARCOPOLO;
  static const std::string MONGOLARMIES;
  static const std::string MYSTICISMABOUNDS;
  static const std::string NICOLAUSCOPERNICUS;
  static const std::string PAPALDECREE;
  static const std::string PIRATESVIKINGS;
  static const std::string PRINCEHENRY;
  static const std::string RASHIDADDIN;
  static const std::string REBELLION;
  static const std::string RELIGIOUSSTRIFE;
  static const std::string REVOLUTIONARYUPRISINGS;
  static const std::string SIRISAACNEWTON;
  static const std::string STBENEDICT;
  static const std::string STIRRUPS;
  static const std::string THECRUSADES;
  static const std::string WALTERTHEPENNILESS;
  static const std::string WAR;
  static const std::string WILLIAMCAXTON;
  
private:
  std::map<std::string,std::pair<int,CardInfo *> >m_Cards;
};
