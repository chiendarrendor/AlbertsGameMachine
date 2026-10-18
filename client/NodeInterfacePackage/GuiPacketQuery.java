package NodeInterfacePackage;

import org.doghouse.jsonpath.JsonPath;
import java.util.regex.Pattern;
import java.util.regex.Matcher;
import java.util.List;



// This class joins the facilities of a org.doghouse.jsonpath query object
// and the hard-coded  special attributes of the GuiPAcketParser.  If one of the 
// special cases is not mentioned, the given JSONPath-format query string operates
// against the GPP getBody() json object.
public class GuiPacketQuery
{
    // for the special-case markers, they have a singular predefined type that must
    // be matched at runtime.
    private enum RetType { QRET_INT, QRET_STRING, QRET_BOOLEAN, QRET_STRINGLIST, QRET_ANY };

    private enum QType { 
        QTYPE_STATE(RetType.QRET_STRING), 
        QTYPE_DESC(RetType.QRET_STRING), 
        QTYPE_ME(RetType.QRET_STRING), 
        QTYPE_LEGALACTION(RetType.QRET_BOOLEAN), 
        QTYPE_QUERY(RetType.QRET_ANY);

        private final RetType rettype;
        private QType(RetType rettype) { this.rettype = rettype; }
        public RetType getRetType() { return this.rettype; }
        };
        

    private QType type;
    private JsonPath query = null;
    private Pattern legalActionPattern = Pattern.compile("\\{LEGALACTION:([A-Za-z]+)\\}");
    private String actionName = null;
    private String rawquery = null;

    public GuiPacketQuery(String queryString) {
        rawquery = queryString;
        Matcher m = legalActionPattern.matcher(queryString);

        if (queryString.equals("{STATE}")) type = QType.QTYPE_STATE;
        else if (queryString.equals("{STATEDESC}")) type = QType.QTYPE_DESC;
        else if (queryString.equals("{ME}")) type = QType.QTYPE_ME;
        else if (m.matches()) {
            type = QType.QTYPE_LEGALACTION;
            actionName = m.group(1);
        } else {
            query = JsonPath.compile(queryString);
            type = QType.QTYPE_QUERY;
        }
    }

    public int evaluateInt(GameEvent event, int defVal) {
        GuiPacketParser gpp = event.getGPP();
        if (gpp == null) throw new NodeInterfacePackageException("Uh oh...GameEvent with no GPP inside...what did you do?");

        if (type.getRetType() != RetType.QRET_INT && type.getRetType() != RetType.QRET_ANY) 
            throw new NodeInterfacePackageException("illegal toInt call for " + rawquery);
        // only one legal result for an int.
        return query.evaluateInt(gpp.getBody(), defVal);
    }
    
    public boolean evaluateBoolean(GameEvent event, boolean defVal) {
        GuiPacketParser gpp = event.getGPP();
        if (gpp == null) throw new NodeInterfacePackageException("Uh oh...GameEvent with no GPP inside...what did you do?");

        if (type.getRetType() != RetType.QRET_BOOLEAN && type.getRetType() != RetType.QRET_ANY) 
            throw new NodeInterfacePackageException("illegal toBoolean call for " + rawquery);

        // by definition, if we have a LEGALACTION, actionName has been set.
        if (type == QType.QTYPE_LEGALACTION) return gpp.hasLegalAction(actionName);
        // otherwise, we are in a QUERY type and query is set.
        return query.evaluateBoolean(gpp.getBody(), defVal);
    }

    public String evaluateString(GameEvent event, String defVal) {
        GuiPacketParser gpp = event.getGPP();
        if (gpp == null) throw new NodeInterfacePackageException("Uh oh...GameEvent with no GPP inside...what did you do?");

        if (type.getRetType() != RetType.QRET_STRING && type.getRetType() != RetType.QRET_ANY) 
            throw new NodeInterfacePackageException("illegal toString call for " + rawquery);
        
        switch(type) {
            case QTYPE_STATE: return gpp.getStateName();
            case QTYPE_DESC: return gpp.getStateDesc();
            case QTYPE_ME: return gpp.getMyName();
            case QTYPE_QUERY: return query.evaluateString(gpp.getBody(), defVal);
            default:
                throw new NodeInterfacePackageException("Should never have gotten here!");
        }
    }

    public List<String> evaluateStringList(GameEvent event) {
        GuiPacketParser gpp = event.getGPP();
        if (gpp == null) throw new NodeInterfacePackageException("Uh oh...GameEvent with no GPP inside...what did you do?");

        if (type.getRetType() != RetType.QRET_STRINGLIST && type.getRetType() != RetType.QRET_ANY) 
            throw new NodeInterfacePackageException("illegal toStringList call for " + rawquery);
        // only stringlist operator is query itself.
        return query.evaluateStringList(gpp.getBody());
    }

    public Object evaluate(GameEvent event) {
        GuiPacketParser gpp = event.getGPP();
        if (gpp == null) throw new NodeInterfacePackageException("Uh oh...GameEvent with no GPP inside...what did you do?");

        switch(type) {
            case QTYPE_DESC:  return gpp.getStateDesc();
            case QTYPE_LEGALACTION: return gpp.hasLegalAction(actionName);
            case QTYPE_ME: return gpp.getMyName();
            case QTYPE_STATE: return gpp.getStateName();
            case QTYPE_QUERY: return query.evaluate(gpp.getBody());
            default:
                throw new NodeInterfacePackageException("Should never have gotten here!");
        }
    }
}