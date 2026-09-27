#ifndef RECORDINGOUTPUTPORTHPP
#define RECORDINGOUTPUTPORTHPP

#include "OutputPort.hpp"
#include <string>
#include <utility>
#include <vector>

// Test double recording what GameProcessProxy actually sent, instead of a
// real socket -- same spirit as Outpost/tests/TestOutputPort.hpp.
class RecordingOutputPort : public OutputPort
{
public:
  virtual void UniCast(const std::string &i_Name, const std::string &i_Message) const
  {
    m_UniCasts.push_back(std::make_pair(i_Name, i_Message));
  }
  virtual void BroadCast(const std::string &i_Message) const
  {
    m_BroadCasts.push_back(i_Message);
  }

  mutable std::vector<std::pair<std::string, std::string> > m_UniCasts;
  mutable std::vector<std::string> m_BroadCasts;
};

#endif
