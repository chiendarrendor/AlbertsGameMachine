package NodeInterfacePackage;
import org.json.JSONObject;

public interface ActionSender {
    public void SendAction(String i_ActionName, JSONObject i_Params);
    public void SendAction(String i_ActionName, JSONObject i_Params, ActionContentType i_ContentType);
}
