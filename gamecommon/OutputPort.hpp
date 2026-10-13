#ifndef OUTPUTPORTHPP
#define OUTPUTPORTHPP

#include <string>
#include <boost/json.hpp>

// Shared by OutputPort::UniCastFullState (per-game full-state, called from
// GameProcessProxy::DispatchEvent) and RoomManager's own ROOMGUI broadcast
// (which has no OutputPort of its own -- it talks to
// GameServerConnectionHandlerFactory directly). Both need the exact same
// layers 3+4 wrapping around a pure content object, so there's one place
// that knows this shape instead of two. See
// .claude/server_game_interface_spec.md's Phase 3 envelope design.
inline boost::json::object WrapFullStateEnvelope(const std::string &i_Name,const std::string &i_GuiName,
                                                 const std::string &i_XMLLoc,const std::string &i_XMLFile,
                                                 const boost::json::object &i_Content)
{
  boost::json::object outer;
  outer["recipient"] = i_Name;
  outer["gui"] = i_GuiName;
  outer["XMLLoc"] = i_XMLLoc;
  outer["XMLFile"] = i_XMLFile;
  outer["fullState"] = i_Content;
  return outer;
}

class OutputPort
{
public:
  virtual void UniCast(const std::string &i_Name,const std::string &i_Message) const = 0;

  // Sends a native JSON value as one event, rather than a pre-serialized
  // opaque string -- for anything that's already structured JSON, so it
  // nests rather than getting double-encoded. Default behavior serializes
  // and forwards through the existing string-based UniCast, which is
  // correct for any subclass whose job ends at "produce one opaque text
  // line" (e.g. RoomOutputPort, writing straight to a real socket).
  // GameServerMainOutputPort overrides this to nest the value natively
  // instead, since its job is to produce one JSON-RPC notification object
  // for the server to unwrap, not a client-facing text line -- see
  // .claude/server_game_interface_spec.md's Phase 3 envelope design.
  virtual void UniCastJson(const std::string &i_Name,const boost::json::value &i_Value) const
  {
    UniCast(i_Name,boost::json::serialize(i_Value));
  }

  // Layers 3+4 of the full-state envelope (see the spec doc): added "at the
  // one point it's actually writing to a specific socket" -- i.e. called by
  // the server's own OutputPort (RoomOutputPort), not by the game process.
  // The server already has Name/XMLLoc/XMLFile authoritatively via its own
  // GetServerGameInfo() (it's literally what spawned the game process with
  // this exact data), so it adds all of it here in one place rather than
  // having the game process redundantly echo static data back on every
  // single message. Wraps a full-state envelope (state/legalActions/body-
  // or-error) with the things only this layer knows: whose connection it's
  // for, which GUI it's for, and how to bootstrap that GUI if the client
  // doesn't have it yet (enabling NEWGUI retirement). Nested under its own
  // key rather than merged into i_Envelope's fields, so these can never
  // collide with whatever's inside it -- same sandboxing principle as
  // "body" never colliding with "error" one layer down. Every OutputPort
  // subclass gets this for free, rather than each concrete subclass having
  // to remember to add it itself.
  void UniCastFullState(const std::string &i_Name,const std::string &i_GuiName,
                        const std::string &i_XMLLoc,const std::string &i_XMLFile,
                        const boost::json::object &i_Envelope) const
  {
    UniCast(i_Name,boost::json::serialize(WrapFullStateEnvelope(i_Name,i_GuiName,i_XMLLoc,i_XMLFile,i_Envelope)));
  }

  // The broadcast counterpart to UniCastFullState above -- every currently
  // known recipient gets the same envelope. Unlike UniCastFullState, this
  // can't be a concrete default here: enumerating "every recipient" is
  // necessarily subclass-specific knowledge the base class doesn't have
  // (RoomOutputPort tracks its own broadcast-target set; a game process's
  // own OutputPort has no such list and no legitimate reason to broadcast
  // a full-state envelope at all -- see GameServerMainOutputPort's
  // implementation). Currently only called by GameProcessProxy::Terminate,
  // to notify everyone in the room a game process died -- see
  // .claude/server_game_interface_spec.md.
  virtual void BroadCastFullState(const std::string &i_GuiName,
                                  const std::string &i_XMLLoc,const std::string &i_XMLFile,
                                  const boost::json::object &i_Envelope) const = 0;
};

#endif
