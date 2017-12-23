#include "Token.hpp"
#include <vector>
#include <set>

// string representations of Advances:
// YD -- yellow drive
// RD -- red drive
// CD -- combined drive
// AF -- Air Foil
// AP -- Auto Pilot
// JS -- Jump Start
// MG -- Mulligan Gear
// SE -- Spy Eye
// SS -- Switch Switch
// GL -- Gate Lock

class Advances
{
 public:
  Advances(const std::vector<Token>& i_PlayerTokenVector);
  Advances& operator=(const Advances& i_other);

  // this function returns a 
  // '-' separated string of the advances that can must be
  // turned on or off at the beginning of the turn.
  // these are: drives, AP, GL, JS
  std::string GetSwitchables() const;
  // similar to above, but string contains only those
  // advances that _are_ currently turned on.
  std::string GetActiveSwitchables() const;

  // this function takes a string as represented above.
  // items present in PlayerTokenVector that are switchable
  // must also have their switchable string present in order
  // to function.
  // returns true if and only if all switchables in the string are
  // a) legal switchables
  // b) that the player actually owns.
  bool SetSwitchables(const std::string& i_switchables);

  // below are boolean statuses for advances
  bool SkipsYellow() const;
  bool SkipsRed() const;
  int GetShieldStrength() const;
  bool HasAirFoil() const;
  bool HasAutoPilot() const;
  bool HasJumpStart() const;
  bool HasMulliganGear() const;
  bool HasSpyEye() const;
  bool HasSwitchSwitch() const;
  bool HasGateLock() const;

 private:
  const std::vector<Token>& m_rPlayerTokenVector;
  std::set<std::string> m_onswitchables;

  enum DriveType { RED_DRIVE,YELLOW_DRIVE,COMBINED_DRIVE };

  bool HasRelic(const std::string& i_name) const;
  bool HasDrive(const DriveType i_dt) const;


  SERIALIZE_FUNC
  {
    SERIALIZE(m_onswitchables);
  }
};


  
