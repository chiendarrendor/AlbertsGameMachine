#include "Serialize.hpp"
#include "Outpost.hpp"

class Options
{
 public:
  Options();

  void SetStandardBasic();
  void SetStandardExpert();

  void SetEarlyDiscard(bool i_earlydiscard);
  void SetRefineries(bool i_refineries);
  void SetOneUpgradePerTurn(bool i_oneupgradeperturn);
  void SetMustBeActiveToBid(bool i_mustbeactivetobid);
  void SetSupplyHarshness(int i_numOfTurns); // 0 is normal
  void SetRobotMechanism(RobotMechanism i_robotmechanism);
  void SetResearchMega(bool i_researchmega);
  void SetResearchIsSmall(bool i_researchissmall);
  void SetMicroIsSmall(bool i_microissmall);
  void SetBlindDraws(bool i_blinddraws);
  void SetStockMechanism(StockMechanism i_stockmechanism);

  bool GetEarlyDiscard() const;
  bool GetRefineries() const;
  bool GetOneUpgradePerTurn() const;
  bool GetMustBeActiveToBid() const;
  int GetSupplyHarshness() const;
  bool GetResearchMega() const;
  bool GetResearchIsSmall() const;
  bool GetMicroIsSmall() const;
  bool GetBlindDraws() const;
  RobotMechanism GetRobotMechanism() const;
  StockMechanism GetStockMechanism() const;



 private:

  bool m_earlydiscard;
  bool m_refineries;
  bool m_oneupgradeperturn;
  bool m_mustbeactivetobid;
  int m_supplyharshness;
  RobotMechanism m_robotmechanism;
  bool m_researchmega;
  bool m_researchissmall;
  bool m_microissmall;
  bool m_blinddraws;
  StockMechanism m_stockmechanism;

  SERIALIZE_FUNC
  {
    SERIALIZE(m_earlydiscard);
    SERIALIZE(m_refineries);
    SERIALIZE(m_oneupgradeperturn);
    SERIALIZE(m_mustbeactivetobid);
    SERIALIZE(m_supplyharshness);
    SERIALIZE(m_robotmechanism);
    SERIALIZE(m_researchmega);
    SERIALIZE(m_researchissmall);
    SERIALIZE(m_microissmall);
    SERIALIZE(m_blinddraws);
    SERIALIZE(m_stockmechanism);
  }
};

