
package NodeInterfacePackage;

public interface UserDefinedInterface
{
  // constructor in this form (interfaces do not include constructors)
  // should initialize whatever gui items into i_Panel are required.
  // in addition, should register any ActionGenerators into i_atm
  // i_Options contain whatever XML attributes from the script
  // that were not a recognized attribute of a user defined node
  // (i.e. x,y, and hidden)
  // public UserDefinedInterface(String i_DataLoc,HashMap i_Options,JPanel i_Panel,ActionTransferManager i_atm)
  //   throws NodeInterfacePackageException;

  // should do whatever it is supposed to do when presented with this event.
  public void HandleEvent(GameEvent i_event);

  // if any special processing is required for a refresh(), do that here.
  // (refresh will work automatically correctly as long as all the visual state
  // for the class is stored in Swing/AWT components (i.e. colors, fonts, text states)
  public void Refresh();

  // return the interface to its initial state. (i.e. however it was when 
  // this object was constructed)
  public void Reset();
}
