#include "Options.hpp"

Options::Options() :
  m_earlydiscard(false),
  m_refineries(false),
  m_oneupgradeperturn(false),
  m_mustbeactivetobid(false),
  m_supplyharshness(0),
  m_robotmechanism(FIRST_UNLIMITED),
  m_researchmega(false),
  m_researchissmall(false),
  m_microissmall(false),
  m_blinddraws(false),
  m_stockmechanism(NORMAL_STOCK)
{
}

void Options::SetEarlyDiscard(bool i_earlydiscard) {  m_earlydiscard =   i_earlydiscard; }
void Options::SetRefineries(bool i_refineries) {  m_refineries =   i_refineries; }
void Options::SetOneUpgradePerTurn(bool i_oneupgradeperturn) {  m_oneupgradeperturn =   i_oneupgradeperturn; }
void Options::SetMustBeActiveToBid(bool i_mustbeactivetobid) {  m_mustbeactivetobid =   i_mustbeactivetobid; }
void Options::SetSupplyHarshness(int i_supplyharshness) {  m_supplyharshness =  i_supplyharshness; }
void Options::SetRobotMechanism(RobotMechanism i_robotmechanism) {  m_robotmechanism =   i_robotmechanism; }
void Options::SetResearchMega(bool i_researchmega) {  m_researchmega =   i_researchmega; }
void Options::SetResearchIsSmall(bool i_researchissmall) {  m_researchissmall =   i_researchissmall; }
void Options::SetMicroIsSmall(bool i_microissmall) {  m_microissmall =   i_microissmall; }
void Options::SetBlindDraws(bool i_blinddraws) {  m_blinddraws = i_blinddraws; }
void Options::SetStockMechanism(StockMechanism i_stockmechanism) { m_stockmechanism = i_stockmechanism; }

bool Options::GetEarlyDiscard() const {  return m_earlydiscard; }
bool Options::GetRefineries() const {   return m_refineries; }
bool Options::GetOneUpgradePerTurn() const {   return m_oneupgradeperturn; }
bool Options::GetMustBeActiveToBid() const {   return m_mustbeactivetobid; }
int Options::GetSupplyHarshness() const {   return m_supplyharshness; }
bool Options::GetResearchMega() const {   return m_researchmega; }
bool Options::GetResearchIsSmall() const {   return m_researchissmall; }
bool Options::GetMicroIsSmall() const {   return m_microissmall; }
bool Options::GetBlindDraws() const { return m_blinddraws; }
RobotMechanism Options::GetRobotMechanism() const { return m_robotmechanism; }
StockMechanism Options::GetStockMechanism() const { return m_stockmechanism; }


void Options::SetStandardBasic()
{
  m_earlydiscard =   false;
  m_refineries =   false;
  m_oneupgradeperturn =   false;
  m_mustbeactivetobid =   false;
  m_supplyharshness = 0;
  m_robotmechanism =  FIRST_UNLIMITED;
  m_researchmega =   false;
  m_researchissmall =   false;
  m_microissmall =   false;
  m_blinddraws = false;
  m_stockmechanism = NORMAL_STOCK;
}

void Options::SetStandardExpert()
{
  m_earlydiscard =   true;
  m_refineries =   false;
  m_oneupgradeperturn =   false;
  m_mustbeactivetobid =   false;
  m_supplyharshness = 0;
  m_robotmechanism =   FIRST_UNLIMITED;
  m_researchmega =   false;
  m_researchissmall =   true;
  m_microissmall =   true;
  m_blinddraws = false;
  m_stockmechanism = NORMAL_STOCK;
}
