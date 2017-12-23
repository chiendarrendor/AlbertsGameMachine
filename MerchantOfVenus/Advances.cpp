#include "Advances.hpp"
#include "StringUtilities.hpp"

Advances::Advances(const std::vector<Token>& i_PlayerTokenVector) :
  m_rPlayerTokenVector(i_PlayerTokenVector),
  m_onswitchables()
{
}

// an advances object depends on the PlayerTokenVector it 
// was instanciated with.
Advances& Advances::operator=(const Advances& i_right)
{
  if (this == &i_right) return *this;
  m_onswitchables = i_right.m_onswitchables;
  return *this;
}

bool Advances::HasRelic(const std::string& i_RelicName) const
{
  for (std::vector<Token>::const_iterator tit = m_rPlayerTokenVector.begin();
       tit != m_rPlayerTokenVector.end(); ++tit)
  {
    if (tit->GetSizeClass() != RELIC_SIZE) continue;
    if (tit->GetName() != i_RelicName) continue;
    return true;
  }
  return false;
}

bool Advances::HasDrive(DriveType i_dt) const
{
  for (std::vector<Token>::const_iterator tit = m_rPlayerTokenVector.begin();
       tit != m_rPlayerTokenVector.end(); ++tit)
  {
    switch(i_dt)
    {
    case RED_DRIVE:
      if (tit->GetSkipColors() == 2) return true;
      break;
    case YELLOW_DRIVE:
      if (tit->GetSkipColors() == 1) return true;
      break;
    case COMBINED_DRIVE:
      if (tit->GetSkipColors() == 3) return true;
    }
  }
  return false;
}

void Append(std::string& i_result,const std::string& i_new)
{
  if (i_result.size() != 0)
  {
    i_result += std::string("-");
  }
  i_result += i_new;
}

std::string Advances::GetSwitchables() const
{
  std::string result;
  if (HasDrive(RED_DRIVE)) Append(result,"RD");
  if (HasDrive(YELLOW_DRIVE)) Append(result,"YD");
  if (HasDrive(COMBINED_DRIVE)) Append(result,"CD");
  if (HasRelic("Auto Pilot")) Append(result,"AP");
  if (HasRelic("Gate Lock")) Append(result,"GL");
  if (HasRelic("Jump Start")) Append(result,"JS");

  return result;
}

std::string Advances::GetActiveSwitchables() const
{
  std::string result;
  std::set<std::string>::const_iterator swit;
  for (swit = m_onswitchables.begin() ; swit != m_onswitchables.end() ; ++swit)
  {
    Append(result,*swit);
  }
  return result;
}

bool Advances::SetSwitchables(const std::string& i_switchables)
{
  std::vector<std::string> switchar;

  if (i_switchables != "-") // our representation of the 'empty' switch list
  {
    Tokenize(switchar,i_switchables,"-",false);
    // validate
    for (std::vector<std::string>::iterator switchit = switchar.begin() ;  switchit != switchar.end() ; ++switchit)
    {
      if (*switchit == "RD" && HasDrive(RED_DRIVE)) continue;
      if (*switchit == "YD" && HasDrive(YELLOW_DRIVE)) continue;
      if (*switchit == "CD" && HasDrive(COMBINED_DRIVE)) continue;
      if (*switchit == "AP" && HasRelic("Auto Pilot")) continue;
      if (*switchit == "GL" && HasRelic("Gate Lock")) continue;
      if (*switchit == "JS" && HasRelic("Jump Start")) continue;
      return false;
    }
  }
  
  // if we get here, then all items in switchar are valid.
  m_onswitchables.clear();
  for (std::vector<std::string>::iterator switchit = switchar.begin() ; 
       switchit != switchar.end() ; ++switchit)
  {
    m_onswitchables.insert(*switchit);
  }
  return true;
}

bool Advances::SkipsYellow() const
{
  return ((HasDrive(YELLOW_DRIVE) && 
           m_onswitchables.find("YD") != m_onswitchables.end()) ||
          (HasDrive(COMBINED_DRIVE) && 
           m_onswitchables.find("CD") != m_onswitchables.end())
          );
}

bool Advances::SkipsRed() const
{
  return ((HasDrive(RED_DRIVE) && 
           m_onswitchables.find("RD") != m_onswitchables.end()) ||
          (HasDrive(COMBINED_DRIVE) && 
           m_onswitchables.find("CD") != m_onswitchables.end())
          );
}

int Advances::GetShieldStrength() const
{
  int result = 0;
  for (std::vector<Token>::const_iterator tit = m_rPlayerTokenVector.begin();
       tit != m_rPlayerTokenVector.end(); ++tit)
  {
    result += tit->GetDefenseValue();
  }

  return result;
}

bool Advances::HasAirFoil() const
{
  return HasRelic("Air Foil");
}

bool Advances::HasAutoPilot() const
{
  return HasRelic("Auto Pilot") && 
    m_onswitchables.find("AP") != m_onswitchables.end();
}

bool Advances::HasJumpStart() const
{
  return HasRelic("Jump Start") &&
    m_onswitchables.find("JS") != m_onswitchables.end();
}

bool Advances::HasMulliganGear() const
{
  return HasRelic("Mulligan Gear");
}

bool Advances::HasSpyEye() const
{
  return HasRelic("Spy Eye");
}

bool Advances::HasSwitchSwitch() const
{
  return HasRelic("Switch Switch");
}

bool Advances::HasGateLock() const
{
  return HasRelic("Gate Lock") && 
    m_onswitchables.find("GL") != m_onswitchables.end();
}
  


// switchables are Auto Pilot AP and Gate Lock GL
  
    


  
