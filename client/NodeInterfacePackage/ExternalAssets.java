package NodeInterfacePackage;

// Bundles everything a UserDefinedInterface widget's constructor might need
// from the client-wide/gui-tree-wide world, so that adding a new such thing
// (e.g. a future cross-gui pub/sub manager) doesn't require changing every
// widget constructor's signature a second time -- only this class grows.
// GameGui builds one of these per gui tree (see GetExternalAssets()), the
// same way it already builds one ActionTransferManager per tree.
public class ExternalAssets
{
  private ActionTransferManager m_atm;
  private SequenceManager m_sm;

  public ExternalAssets(ActionTransferManager i_atm, SequenceManager i_sm)
  {
    m_atm = i_atm;
    m_sm = i_sm;
  }

  // temporary until we actually get SequenceManager wired in.
  public ExternalAssets(ActionTransferManager i_atm) { this(i_atm,null); }

  public ActionTransferManager GetActionTransferManager()
  {
    return m_atm;
  }

  public SequenceManager GetSequenceManager()
  {
    return m_sm;
  }
}
