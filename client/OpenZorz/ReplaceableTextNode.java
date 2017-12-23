package OpenZorz;

public interface ReplaceableTextNode extends EventHandlerNode
{
  void AddText(String i_text);
  EventStringReplacer.Replacement AddReplacer(String i_replacestring,
                                              String i_eventvarname,
                                              String i_initialvarvalue);
}
