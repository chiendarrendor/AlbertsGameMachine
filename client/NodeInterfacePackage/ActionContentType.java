package NodeInterfacePackage; 

public enum ActionContentType { 
    TRANSITION("transition"),
    SEQUENCE("sequence");
    private String m_name;
    private ActionContentType(String i_name) {m_name = i_name; }
    public String getName() { return m_name; }

    public static ActionContentType getType(String s) {
        if (s.equals("transition")) return TRANSITION;
        if (s.equals("sequence")) return SEQUENCE;
        return null;
    }
}