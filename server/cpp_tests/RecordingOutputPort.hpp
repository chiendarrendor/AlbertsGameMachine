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
  virtual void BroadCastFullState(const std::string &i_GuiName,
                                  const std::string &i_XMLLoc,const std::string &i_XMLFile,
                                  const boost::json::object &i_Envelope) const
  {
    m_BroadCastFullStates.push_back(i_Envelope);
  }

  mutable std::vector<std::pair<std::string, std::string> > m_UniCasts;
  mutable std::vector<boost::json::object> m_BroadCastFullStates;
};

#endif
