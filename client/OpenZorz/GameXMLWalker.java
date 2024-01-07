package OpenZorz;

import java.util.*;
import DOMUtilities.*;


public class GameXMLWalker
{
  private GameGui m_gamegui;
  private HashMap<String,org.w3c.dom.Node> m_noderegistry;
  private AttributeChecker m_eventvarnames;
  private ActionTabNode m_actionwindownode;
  private PriorityMap m_CloneVars;
  private String m_FilePrefix;
  private String m_Name;

  private void OpenCloneLevel()
  {
    m_CloneVars.OpenLevel();
  }

  private void CloseCloneLevel() throws GameXMLException
  {
    try
    {
      m_CloneVars.CloseLevel();
    }
    catch(PriorityMapException pme)
    {
      throw new GameXMLException("Caught PriorityMapException: " + pme);
    }
  }

  private void AddCloneVariable(String i_variable,String i_value)
  {
    m_CloneVars.AddVarVal(i_variable,i_value);
  }

  private class CloneVVO implements PriorityMap.VarValOperator
  {
    org.w3c.dom.Node m_Node;
    public CloneVVO(org.w3c.dom.Node i_Node) { m_Node = i_Node; }
    public void OperateVarVal(String i_key,String i_val)
    {
      DOMUtil.RecursiveValueReplace(m_Node,i_key,i_val);
    }
  }

  private void ApplyCloneVariables(org.w3c.dom.Node i_Node)
  {
    CloneVVO cvvo = new CloneVVO(i_Node);
    m_CloneVars.ForEachVarVal(cvvo);
  }



  private void RegisterNode(String i_name,org.w3c.dom.Node i_Node)
  {
    m_noderegistry.put(i_name,i_Node);
  }

  private org.w3c.dom.Node GetRegisteredNode(String i_name)
  {
    return m_noderegistry.get(i_name);
  }

  public GameGui GetGameGui() { return m_gamegui; }

  public GameXMLWalker(org.w3c.dom.Node i_Node,String i_Name,String i_filePrefix,GameGui i_gamegui) throws GameXMLException
  {
    m_Name = i_Name;
    m_gamegui = i_gamegui;
    m_noderegistry = new HashMap<String,org.w3c.dom.Node>();
    m_eventvarnames = m_gamegui.GetAttributeChecker();
    m_CloneVars = new PriorityMap();
    m_FilePrefix = i_filePrefix;

    m_actionwindownode = new ActionTabNode(m_gamegui.GetActionTransferManager(),i_Name,m_gamegui.GetTabbedWindow());
    GetGameGui().AddWindow(m_actionwindownode);

    i_Node = i_Node.getChildNodes().item(0);

    if (!i_Node.getNodeName().equals("gameboard"))
    {
      throw new GameXMLException("Root Element must be gameboard");
    }

    DOMChildIterator dch = new DOMChildIterator(i_Node);
    org.w3c.dom.Node childNode;

    while((childNode = dch.GetNext()) != null)
    {
      String cname = childNode.getNodeName();

      if      (cname.equals("event"))   { WalkEventType(childNode); }
      else if (cname.equals("actions")) { WalkActions(childNode); }
      else if (cname.equals("window"))  { WalkWindow(childNode); }
      else if (cname.equals("tab"))     { WalkTabNode(childNode); }
      else if (cname.equals("define"))  { WalkDefine(childNode); }
      else if (cname.equals("#text") || cname.equals("#comment"))   { }
      else
      {
        throw new GameXMLException("Unknown Top-level node " + cname);
      }
    }
  }

  private void WalkEventType(org.w3c.dom.Node i_Node) throws GameXMLException
  {
    AttributeChecker acheck = new AttributeChecker();
    acheck.AddAttribute("name",true);
    acheck.ValidateAttributes(i_Node);

    String evname = DOMUtil.GetAttributeWithName(i_Node,"name");

    if (m_gamegui.GetEventType(evname) != null)
    {
      throw new GameXMLException("Duplicate Event Type " + evname);
    }

    GameEventType et = m_gamegui.CreateEventType(evname);
	
    DOMChildIterator cit = new DOMChildIterator(i_Node);
    org.w3c.dom.Node childNode;

    while((childNode = cit.GetNext()) != null)
    {
      String cname = childNode.getNodeName();
      if      (cname.equals("var")) { WalkEventVar(childNode,et); }
      else if (cname.equals("#text") || cname.equals("#comment")) { }
      else 
      {
        throw new GameXMLException("Unknown event subnode " + cname);
      }
    }
  }

  private void WalkEventVar(org.w3c.dom.Node i_Node,GameEventType i_et) throws GameXMLException
  {
    AttributeChecker acheck = new AttributeChecker();
    acheck.AddAttribute("name",true);
    acheck.ValidateAttributes(i_Node);

    String varname = DOMUtil.GetAttributeWithName(i_Node,"name");

    if (i_et.HasVar(varname))
    {
      throw new GameXMLException("Duplicate var name " + varname);
    }

    if (i_Node.hasChildNodes())
    {
      throw new GameXMLException("Event Var has child nodes!");
    }
    i_et.AddVar(varname);

    // keeping track of event names; this is the set of legal attribute names
    // for an EventHandler Element Node.
    m_eventvarnames.AddAttribute(varname,false);
  }

  private void WalkDefine(org.w3c.dom.Node i_Node) throws GameXMLException
  {
    AttributeChecker acheck = new AttributeChecker();
    acheck.AddAttribute("name",true);
    acheck.ValidateAttributes(i_Node);

    String dname = DOMUtil.GetAttributeWithName(i_Node,"name");
    if (GetRegisteredNode(dname) != null)
    {
      throw new GameXMLException("Duplicate define " + dname);
    }

    RegisterNode(dname,i_Node);
  }

  private void WalkTabNode(org.w3c.dom.Node i_Node) throws GameXMLException
  {
    AttributeChecker acheck = new AttributeChecker();
    acheck.AddAttribute("layout",false);
    acheck.AddAttribute("title",true);
    acheck.ValidateAttributes(i_Node);

    String layout = DOMUtil.GetAttributeWithName(i_Node,"layout");
    if (layout != null &&
        !layout.equals("lock") &&
        !layout.equals("hbox") &&
        !layout.equals("vbox") &&
        !layout.equals("flow") &&
        !layout.equals("border"))
    {
      throw new GameXMLException("Illegal value of layout " + layout + " in panel");
    }



    TabNode tnode = new TabNode(m_gamegui.GetActionTransferManager(),
                                m_gamegui.GetTabbedWindow(),
                                DOMUtil.GetAttributeWithName(i_Node,"title"),
                                layout,
                                false);

    tnode.SetAlerter();

    m_gamegui.AddWindow(tnode);
    WalkPanelChildren(i_Node,tnode);

    m_gamegui.GetActionTransferManager().SetCurrentAlerter(null);
  }

  private void WalkWindow(org.w3c.dom.Node i_Node) throws GameXMLException
  {
    AttributeChecker acheck = new AttributeChecker();
    acheck.AddAttribute("title",false);
    acheck.AddAttribute("hidden",false);
    acheck.AddAttribute("backimage",false);
    acheck.AddAttribute("width",false);
    acheck.AddAttribute("height",false);
    acheck.AddAttribute("x",false);
    acheck.AddAttribute("y",false);
    acheck.AddAttribute("layout",false);
    acheck.AddAttribute("noscroll",false);
    acheck.ValidateAttributes(i_Node);

    String backimage = DOMUtil.GetAttributeWithName(i_Node,"backimage");
    if (backimage != null && backimage.length() > 0)
    {
      backimage = m_FilePrefix + "/" + backimage;
    }

    String layout = DOMUtil.GetAttributeWithName(i_Node,"layout");
    if (layout != null &&
        !layout.equals("lock") &&
        !layout.equals("hbox") &&
        !layout.equals("vbox") &&
        !layout.equals("flow") &&
        !layout.equals("border"))
    {
      throw new GameXMLException("Illegal value of layout " + layout + " in panel");
    }

    WindowNode windownode = 
	    new WindowNode(DOMUtil.GetAttributeWithName(i_Node,"title"),
                     DOMUtil.GetAttributeWithName(i_Node,"hidden"),
                     backimage,
                     DOMUtil.GetAttributeWithName(i_Node,"width"),
                     DOMUtil.GetAttributeWithName(i_Node,"height"),
                     DOMUtil.GetAttributeWithName(i_Node,"x"),
                     DOMUtil.GetAttributeWithName(i_Node,"y"),
                     layout,
                     DOMUtil.GetAttributeWithName(i_Node,"noscroll")
                     );
	
    m_gamegui.AddWindow(windownode);

    WalkPanelChildren(i_Node,windownode);

    m_gamegui.GetActionTransferManager().SetCurrentAlerter(null);
  }
    
  private void WalkPanelChildren(org.w3c.dom.Node i_Node,GameNode i_PanelNode) throws GameXMLException
  {
    DOMChildIterator cit = new DOMChildIterator(i_Node);
    org.w3c.dom.Node childNode;

    while((childNode = cit.GetNext()) != null)
    {
      String cname = childNode.getNodeName();

      if      (cname.equals("image"))     { WalkImageNode(childNode,i_PanelNode); }
      else if (cname.equals("text"))      { WalkTextNode(childNode,i_PanelNode); }
      else if (cname.equals("textbox"))   { WalkTextBoxNode(childNode,i_PanelNode); }
      else if (cname.equals("checkbox")) { WalkCheckBoxNode(childNode,i_PanelNode); }
      else if (cname.equals("fixedselection")) { WalkFixedSelectionNode(childNode,i_PanelNode); }
      else if (cname.equals("varselection")) { WalkVarSelectionNode(childNode,i_PanelNode); }
      else if (cname.equals("minmaxvarselection")) { WalkMinMaxVarSelectionNode(childNode,i_PanelNode); }
      else if (cname.equals("fixedspinner")) { WalkFixedSpinnerNode(childNode,i_PanelNode); }
      else if (cname.equals("varspinner")) { WalkVarSpinnerNode(childNode,i_PanelNode); }
      else if (cname.equals("button"))    { WalkButtonNode(childNode,i_PanelNode); }
      else if (cname.equals("named"))     { WalkNamedNode(childNode,i_PanelNode); }
      else if (cname.equals("panel"))     { WalkPanelNode(childNode,i_PanelNode); }
      else if (cname.equals("forupto"))   { WalkForUpToNode(childNode,i_PanelNode); }
      else if (cname.equals("foreach"))   { WalkForEachNode(childNode,i_PanelNode); }
      else if (cname.equals("eventhandler")) { WalkEventHandlerNode(childNode,i_PanelNode); }
      else if (cname.equals("javaclass")) { WalkUserDefinedNode(childNode,i_PanelNode); }
      else if (cname.equals("#text") || cname.equals("#comment"))   { }
      else
      {
        throw new GameXMLException("Unknown subnode of Window " + cname);
      }
    }
  }

  private void WalkImageNode(org.w3c.dom.Node i_XMLNode,GameNode i_ParentGameNode) throws GameXMLException
  {
    AttributeChecker acheck = new AttributeChecker();
    acheck.AddAttribute("x",false);
    acheck.AddAttribute("y",false);
    acheck.AddAttribute("hidden",false);
    acheck.AddAttribute("image",true);
    acheck.ValidateAttributes(i_XMLNode);

    ImageNode inode = new ImageNode(
            m_gamegui,
            i_ParentGameNode,
            DOMUtil.GetIntAttribute(i_XMLNode,"x"),
            DOMUtil.GetIntAttribute(i_XMLNode,"y"),
            DOMUtil.GetAttributeWithName(i_XMLNode,"hidden"),
            DOMUtil.GetAttributeWithName(i_XMLNode,"image"));

    DOMChildIterator cit = new DOMChildIterator(i_XMLNode);
    org.w3c.dom.Node childNode;

    while((childNode = cit.GetNext()) != null)
    {
      String cname = childNode.getNodeName();
      if      (cname.equals("eventhandler")) { WalkEventHandlerNode(childNode,inode); }
      else if (cname.equals("#text") || cname.equals("#comment"))   { }
      else
      {
        throw new GameXMLException("Unknown subnode of Image " + cname);
      }
    }
  }

  private void WalkTextNode(org.w3c.dom.Node i_XMLNode,GameNode i_ParentGameNode) throws GameXMLException
  {
    AttributeChecker acheck = new AttributeChecker();
    acheck.AddAttribute("x",false);
    acheck.AddAttribute("y",false);
    acheck.AddAttribute("hidden",false);
    acheck.ValidateAttributes(i_XMLNode);

    TextNode tnode = new TextNode(i_ParentGameNode,
                                  DOMUtil.GetIntAttribute(i_XMLNode,"x"),
                                  DOMUtil.GetIntAttribute(i_XMLNode,"y"),
                                  DOMUtil.GetAttributeWithName(i_XMLNode,"hidden"));

    WalkReplacingTextChildren(i_XMLNode,tnode);
  }

  private void WalkReplacingTextChildren(org.w3c.dom.Node i_XMLNode,
                                         ReplaceableTextNode i_GameNode) throws GameXMLException
  {
    DOMChildIterator cit = new DOMChildIterator(i_XMLNode);
    org.w3c.dom.Node childNode;

    while((childNode = cit.GetNext()) != null)
    {
      String cname = childNode.getNodeName();
      if      (cname.equals("#text")) { i_GameNode.AddText(childNode.getNodeValue()); }
      else if (cname.equals("#cdata-section")) { i_GameNode.AddText(childNode.getNodeValue()); }
      else if (cname.equals("eventhandler")) { WalkEventHandlerNode(childNode,i_GameNode); }
      else if (cname.equals("#comment")) { }
      else if (cname.equals("replace"))
      {
        AttributeChecker acheck = new AttributeChecker();
        acheck.AddAttribute("from",true);
        acheck.AddAttribute("eventvar",false);
        acheck.AddAttribute("initial",false);
        acheck.ValidateAttributes(childNode);

        String from = DOMUtil.GetAttributeWithName(childNode,"from");
        String evar = DOMUtil.GetAttributeWithName(childNode,"eventvar");
        String init = DOMUtil.GetAttributeWithName(childNode,"initial");

        if (from == null || (evar == null && init == null))
        {
          throw new GameXMLException("invalid attributes of replace node");
        }

        EventStringReplacer.Replacement rep = i_GameNode.AddReplacer(from,evar,init);
        WalkReplaceChildren(childNode,rep);
      }
      else
      {
        throw new GameXMLException("Unknown subnode of Replaceable Text Node: " + cname);
      }
    }
  }

  private void WalkReplaceChildren(org.w3c.dom.Node i_XMLNode,EventStringReplacer.Replacement i_Rep) throws GameXMLException
  {
    DOMChildIterator cit = new DOMChildIterator(i_XMLNode);
    org.w3c.dom.Node childNode;

    while((childNode = cit.GetNext()) != null)
    {
      String cname = childNode.getNodeName();
      if      (cname.equals("if") || cname.equals("ifnot"))
      {
        if (childNode.hasChildNodes())
        {
          throw new GameXMLException("replace if/ifnot shouldn't have child nodes!");
        }

        int ut;
        if      (cname.equals("if"))       ut = GameEventHandler.UNIT_SHOWHIDE;
        else                               ut = GameEventHandler.UNIT_HIDESHOW;

        i_Rep.GetEventHandler().AddEntry(ut);

        if (!childNode.hasAttributes()) continue;
        int i;
        for (i = 0 ; i < childNode.getAttributes().getLength() ; ++i)
        {
          org.w3c.dom.Node attrnode = childNode.getAttributes().item(i);
          i_Rep.GetEventHandler().AddVarVal(attrnode.getNodeName(),attrnode.getNodeValue());
        }
      }
      else if (cname.equals("#text") || cname.equals("#comment"))   { }
      else
      {
        throw new GameXMLException("Unknown subnode of replace " + cname);
      }
    }
  }



  private void WalkNamedNode(org.w3c.dom.Node i_XMLNode,GameNode i_ParentGameNode) throws GameXMLException
  {
    // this node works a little differently in terms of attributes
    // name is required,
    // hidden, x, and y are optional
    // any other attribute name is considered to be a string that will be replaced in 
    // subordinate nodes by that attributes value
    // as such, the AttributeChecker class isn't used here.

    String name = DOMUtil.GetAttributeWithName(i_XMLNode,"name");
    if (name == null)
    {
      throw new GameXMLException("Missing Required Attribute name in " + i_XMLNode.getNodeName());
    }

    org.w3c.dom.Node nameBodyNode = GetRegisteredNode(name);

    if (nameBodyNode == null)
    {
      throw new GameXMLException("Unknown name " + name + " in named");
    }

    OpenCloneLevel();
    
    int i;
    for (i = 0 ; i < i_XMLNode.getAttributes().getLength() ; ++i)
    {
      String s = i_XMLNode.getAttributes().item(i).getNodeName();
      if (s.equals("name")) continue;
      if (s.equals("x")) continue;
      if (s.equals("y")) continue;
      if (s.equals("hidden")) continue;

      String v = i_XMLNode.getAttributes().item(i).getNodeValue();
      
      AddCloneVariable(s,v);
    }

    org.w3c.dom.Node clonedNode = nameBodyNode.cloneNode(true);

    ApplyCloneVariables(clonedNode);

    NamedNode nnode = new NamedNode(i_ParentGameNode,
                                    DOMUtil.GetIntAttribute(i_XMLNode,"x"),
                                    DOMUtil.GetIntAttribute(i_XMLNode,"y"),
                                    DOMUtil.GetAttributeWithName(i_XMLNode,"hidden"));

    DOMChildIterator cit = new DOMChildIterator(i_XMLNode);
    org.w3c.dom.Node childNode;

    while((childNode = cit.GetNext()) != null)
    {
      String cname = childNode.getNodeName();
      if      (cname.equals("eventhandler")) { WalkEventHandlerNode(childNode,nnode); }
      else if (cname.equals("#text") || cname.equals("#comment"))   { }
      else
      {
        throw new GameXMLException("Unknown subnode of Named " + cname);
      }
    }

    WalkPanelChildren(clonedNode,nnode);
    CloseCloneLevel();
  }

  private void WalkPanelNode(org.w3c.dom.Node i_XMLNode,GameNode i_ParentGameNode) throws GameXMLException
  {
    AttributeChecker acheck = new AttributeChecker();
    acheck.AddAttribute("hidden",false);
    acheck.AddAttribute("x",false);
    acheck.AddAttribute("y",false);
    acheck.AddAttribute("border",false);
    acheck.AddAttribute("layout",false);
    acheck.ValidateAttributes(i_XMLNode);

    String layout = DOMUtil.GetAttributeWithName(i_XMLNode,"layout");
    if (layout != null &&
        !layout.equals("lock") &&
        !layout.equals("hbox") &&
        !layout.equals("vbox") &&
        !layout.equals("flow") &&
        !layout.equals("border"))
    {
      throw new GameXMLException("Illegal value of layout " + layout + " in panel");
    }

    PanelNode pnode = new PanelNode(i_ParentGameNode,
                                    DOMUtil.GetIntAttribute(i_XMLNode,"x"),
                                    DOMUtil.GetIntAttribute(i_XMLNode,"y"),
                                    DOMUtil.GetAttributeWithName(i_XMLNode,"hidden"),
                                    DOMUtil.GetAttributeWithName(i_XMLNode,"border"),
                                    layout);

    WalkPanelChildren(i_XMLNode,pnode);
  }

  private void WalkEventHandlerNode(org.w3c.dom.Node i_XMLNode,EventHandlerNode i_ParentGameNode) throws GameXMLException
  {
    if (i_ParentGameNode.GetEventHandler() != null)
    {
      throw new GameXMLException("node has duplicate eventhandlers!");
    }

    if (i_XMLNode.hasAttributes() && i_XMLNode.getAttributes().getLength() != 0)
    {
      
      

      throw new GameXMLException("eventhandler shouldn't have attributes: (" + 
                                 i_XMLNode.getAttributes().getLength() + ") " + i_XMLNode.toString() );
    }

    GameEventHandler handler = new GameEventHandler();
    i_ParentGameNode.SetEventHandler(handler);

    DOMChildIterator cit = new DOMChildIterator(i_XMLNode);
    org.w3c.dom.Node childNode;

    boolean sawPass = false;
    boolean sawBlock = false;

    while((childNode = cit.GetNext()) != null)
    {
      String cname = childNode.getNodeName();

      if (cname.equals("showif") ||
          cname.equals("hideif") ||
          cname.equals("showifnot") ||
          cname.equals("hideifnot") ||
          cname.equals("showhide") ||
          cname.equals("hideshow"))
      {
        WalkEventHandlerUnit(childNode,handler);
      }
      else if (cname.equals("passif") ||
               cname.equals("passifnot"))
      {
        sawPass = true;
        WalkEventHandlerUnit(childNode,handler);
      }
      else if (cname.equals("blockif") ||
               cname.equals("blockifnot"))
      {
        sawBlock = true;
        WalkEventHandlerUnit(childNode,handler);
      }
      else if (cname.equals("#text") || cname.equals("#comment"))
      {
      }
      else
      {
        throw new GameXMLException("Unknown Event Handler Type: " + cname);
      }
    }
    if (sawPass && sawBlock)
    {
      throw new GameXMLException("Can't have both pass and block in the same event handler.");
    }
  }

  private void WalkEventHandlerUnit(org.w3c.dom.Node i_Node,GameEventHandler i_handler) throws GameXMLException
  {
    m_eventvarnames.ValidateAttributes(i_Node);

    if (i_Node.hasChildNodes())
    {
      throw new GameXMLException("Event Handler Unit " + i_Node.getNodeName() + " shouldn't have child nodes");
    }

    String utype = i_Node.getNodeName();
    int ut;

    if      (utype.equals("showif"))      ut = GameEventHandler.UNIT_SHOW;
    else if (utype.equals("hideif"))      ut = GameEventHandler.UNIT_HIDE;
    else if (utype.equals("showifnot"))   ut = GameEventHandler.UNIT_SHOW_IF_NOT;
    else if (utype.equals("hideifnot"))   ut = GameEventHandler.UNIT_HIDE_IF_NOT;
    else if (utype.equals("showhide"))    ut = GameEventHandler.UNIT_SHOWHIDE;
    else if (utype.equals("hideshow"))    ut = GameEventHandler.UNIT_HIDESHOW;
    else if (utype.equals("passif"))      ut = GameEventHandler.UNIT_PASSIF;
    else if (utype.equals("passifnot"))   ut = GameEventHandler.UNIT_PASSIFNOT;
    else if (utype.equals("blockif"))     ut = GameEventHandler.UNIT_BLOCKIF;
    else if (utype.equals("blockifnot"))  ut = GameEventHandler.UNIT_BLOCKIFNOT;
    else
    {
      throw new GameXMLException("How did we get here?");
    }

    i_handler.AddEntry(ut);

    if (!i_Node.hasAttributes()) return;
    int i;
    for (i = 0 ; i < i_Node.getAttributes().getLength() ; ++i)
    {
      org.w3c.dom.Node attrnode = i_Node.getAttributes().item(i);
      i_handler.AddVarVal(attrnode.getNodeName(),attrnode.getNodeValue());
    }
  }


  private void WalkForUpToNode(org.w3c.dom.Node i_XMLNode,GameNode i_ParentGameNode) throws GameXMLException
  {
    AttributeChecker acheck = new AttributeChecker();
    acheck.AddAttribute("varname",true);
    acheck.AddAttribute("low",true);
    acheck.AddAttribute("high",true);
    acheck.ValidateAttributes(i_XMLNode);

    String varname = DOMUtil.GetAttributeWithName(i_XMLNode,"varname");
    int lowi = DOMUtil.GetIntAttribute(i_XMLNode,"low");
    int highi = DOMUtil.GetIntAttribute(i_XMLNode,"high");
	
    int i;
    for (i = lowi ; i <= highi ; ++i)
    {
      String v = Integer.toString(i);

      OpenCloneLevel();

      AddCloneVariable(varname,v);
      org.w3c.dom.Node clonedNode = i_XMLNode.cloneNode(true);
      ApplyCloneVariables(clonedNode);
      WalkPanelChildren(clonedNode,i_ParentGameNode);
      
      CloseCloneLevel();
    }
  }

  private void WalkForEachNode(org.w3c.dom.Node i_XMLNode,GameNode i_ParentGameNode) throws GameXMLException
  {
    AttributeChecker acheck = new AttributeChecker();
    acheck.AddAttribute("varname",true);
    acheck.AddAttribute("value",true);
    acheck.ValidateAttributes(i_XMLNode);

    String varname = DOMUtil.GetAttributeWithName(i_XMLNode,"varname");
    String[] values = DOMUtil.GetAttributeWithName(i_XMLNode,"value").split(",");
	
    int i;
    for (i = 0 ; i < values.length ; ++i)
    {
      String v = values[i].trim();

      OpenCloneLevel();
      
      AddCloneVariable(varname,v);

      org.w3c.dom.Node clonedNode = i_XMLNode.cloneNode(true);
      ApplyCloneVariables(clonedNode);
      WalkPanelChildren(clonedNode,i_ParentGameNode);

      CloseCloneLevel();
    }
  }

  private void WalkActions(org.w3c.dom.Node i_Node) throws GameXMLException
  {
    AttributeChecker acheck = new AttributeChecker();
    acheck.ValidateAttributes(i_Node);

    DOMChildIterator cit = new DOMChildIterator(i_Node);
    org.w3c.dom.Node childNode;

    m_actionwindownode.SetAlerter();

    while((childNode = cit.GetNext()) != null)
    {
      String cname = childNode.getNodeName();
      if (cname.equals("action")) { WalkActionNode(childNode,m_actionwindownode); }
      else if (cname.equals("#text") || cname.equals("#comment")) { }
      else
      {
        throw new GameXMLException("Unknown node type " + cname + " in actions block");
      }
    }
    m_gamegui.GetActionTransferManager().SetCurrentAlerter(null);

  }
    
  private void WalkActionNode(org.w3c.dom.Node i_XMLNode,GameNode i_ActionWindowNode) throws GameXMLException
  {
    AttributeChecker acheck = new AttributeChecker();
    acheck.AddAttribute("name",true);
    acheck.AddAttribute("onstate",false);
    acheck.AddAttribute("text",false);
    acheck.AddAttribute("color",false);
    acheck.AddAttribute("alerts",false);
    acheck.ValidateAttributes(i_XMLNode);

    ActionNode.VisibilityType vis = ActionNode.VisibilityType.NEWSTATEHIDES;
    String vs = DOMUtil.GetAttributeWithName(i_XMLNode,"onstate");

    if (vs  == null || vs.equals("newstatehides"))
    {
      vis = ActionNode.VisibilityType.NEWSTATEHIDES;
    }
    else if (vs.equals("alwayson"))
    {
      vis = ActionNode.VisibilityType.ALWAYSON;
    }
    else if (vs.equals("statedalwayshidden"))
    {
      vis = ActionNode.VisibilityType.STATEDALWAYSHIDDEN;
    }
    else if (vs.equals("alwayshidden"))
    {
      vis = ActionNode.VisibilityType.ALWAYSHIDDEN;
    }
    else
    {
      throw new GameXMLException("unknown visibility type: " + vs);
    }

    String name = DOMUtil.GetAttributeWithName(i_XMLNode,"name");
    String text = DOMUtil.GetAttributeWithName(i_XMLNode,"text");
    String color = DOMUtil.GetAttributeWithName(i_XMLNode,"color");
    String alerts = DOMUtil.GetAttributeWithName(i_XMLNode,"alerts");

    ActionNode anode = new ActionNode(i_ActionWindowNode,name,text,color,m_gamegui,vis,alerts);

    DOMChildIterator cit = new DOMChildIterator(i_XMLNode);
    org.w3c.dom.Node childNode;

    while((childNode = cit.GetNext()) != null)
    {
      String cname = childNode.getNodeName();

      if (cname.equals("var")) { WalkActionVar(childNode,anode); }
      else if (cname.equals("#text") || cname.equals("#comment")) { }
      else
      {
        throw new GameXMLException("Unknown node type " + cname + " in action " + name);
      }
    }
  }

  private void WalkActionVar(org.w3c.dom.Node i_XMLNode,ActionNode i_ActionNode) throws GameXMLException
  {
    AttributeChecker acheck = new AttributeChecker();
    acheck.AddAttribute("name",true);
    acheck.AddAttribute("text",true);
    acheck.AddAttribute("hidden",false);
    acheck.AddAttribute("noreset",false);
    acheck.AddAttribute("hastextbox",false);
    acheck.ValidateAttributes(i_XMLNode);
	
    ActionVarNode avnode = new ActionVarNode(DOMUtil.GetAttributeWithName(i_XMLNode,"name"),
                                             DOMUtil.GetAttributeWithName(i_XMLNode,"text"),
                                             i_ActionNode,
                                             m_gamegui,
                                             DOMUtil.GetAttributeWithName(i_XMLNode,"hidden"),
                                             DOMUtil.GetAttributeWithName(i_XMLNode,"noreset"),
                                             DOMUtil.GetAttributeWithName(i_XMLNode,"hastextbox"));

    WalkPanelChildren(i_XMLNode,avnode);
  }

  private void WalkTextBoxNode(org.w3c.dom.Node i_XMLNode,GameNode i_ParentNode) throws GameXMLException
  {
    AttributeChecker acheck = new AttributeChecker();
    acheck.AddAttribute("varname",true);
    acheck.AddAttribute("x",false);
    acheck.AddAttribute("y",false);
    acheck.AddAttribute("hidden",false);
    acheck.AddAttribute("size",false);
    acheck.ValidateAttributes(i_XMLNode);

    TextBoxNode tbnode = new TextBoxNode(DOMUtil.GetAttributeWithName(i_XMLNode,"varname"),
                                         DOMUtil.GetIntAttribute(i_XMLNode,"size"),
                                         DOMUtil.GetIntAttribute(i_XMLNode,"x"),
                                         DOMUtil.GetIntAttribute(i_XMLNode,"y"),
                                         DOMUtil.GetAttributeWithName(i_XMLNode,"hidden"),
                                         i_ParentNode,
                                         m_gamegui);


    DOMChildIterator cit = new DOMChildIterator(i_XMLNode);
    org.w3c.dom.Node childNode;

    while((childNode = cit.GetNext()) != null)
    {
      String cname = childNode.getNodeName();
      if      (cname.equals("eventhandler")) { WalkEventHandlerNode(childNode,tbnode); }
      else if (cname.equals("#text") || cname.equals("#comment"))   { }
      else
      {
        throw new GameXMLException("Unknown subnode of TextBox " + cname);
      }
    }
  }

  private void WalkCheckBoxNode(org.w3c.dom.Node i_XMLNode,GameNode i_ParentNode) throws GameXMLException
  {
    AttributeChecker acheck = new AttributeChecker();
    acheck.AddAttribute("varname",true);
    acheck.AddAttribute("x",false);
    acheck.AddAttribute("y",false);
    acheck.AddAttribute("hidden",false);
    acheck.ValidateAttributes(i_XMLNode);

    CheckBoxNode cbnode = new CheckBoxNode(DOMUtil.GetAttributeWithName(i_XMLNode,"varname"),
                                         DOMUtil.GetIntAttribute(i_XMLNode,"x"),
                                         DOMUtil.GetIntAttribute(i_XMLNode,"y"),
                                         DOMUtil.GetAttributeWithName(i_XMLNode,"hidden"),
                                         i_ParentNode,
                                         m_gamegui);


    DOMChildIterator cit = new DOMChildIterator(i_XMLNode);
    org.w3c.dom.Node childNode;

    while((childNode = cit.GetNext()) != null)
    {
      String cname = childNode.getNodeName();
      if      (cname.equals("eventhandler")) { WalkEventHandlerNode(childNode,cbnode); }
      else if (cname.equals("#text") || cname.equals("#comment"))   { }
      else
      {
        throw new GameXMLException("Unknown subnode of TextBox " + cname);
      }
    }
  }



  private void WalkFixedSelectionNode(org.w3c.dom.Node i_XMLNode,GameNode i_ParentNode) throws GameXMLException
  {
    AttributeChecker acheck = new AttributeChecker();
    acheck.AddAttribute("varname",true);
    acheck.AddAttribute("list",true);
    acheck.AddAttribute("x",false);
    acheck.AddAttribute("y",false);
    acheck.AddAttribute("hidden",false);
    acheck.ValidateAttributes(i_XMLNode);

    SelectionNode selnode = new SelectionNode(DOMUtil.GetAttributeWithName(i_XMLNode,"varname"),
                                              DOMUtil.GetIntAttribute(i_XMLNode,"x"),
                                              DOMUtil.GetIntAttribute(i_XMLNode,"y"),
                                              DOMUtil.GetAttributeWithName(i_XMLNode,"hidden"),
                                              i_ParentNode,
                                              m_gamegui,
                                              false,
                                              DOMUtil.GetAttributeWithName(i_XMLNode,"list"));


    DOMChildIterator cit = new DOMChildIterator(i_XMLNode);
    org.w3c.dom.Node childNode;

    while((childNode = cit.GetNext()) != null)
    {
      String cname = childNode.getNodeName();
      if      (cname.equals("eventhandler")) { WalkEventHandlerNode(childNode,selnode); }
      else if (cname.equals("#text") || cname.equals("#comment"))   { }
      else
      {
        throw new GameXMLException("Unknown subnode of Selection " + cname);
      }
    }
  }

  private void WalkVarSelectionNode(org.w3c.dom.Node i_XMLNode,GameNode i_ParentNode) throws GameXMLException
  {
    AttributeChecker acheck = new AttributeChecker();
    acheck.AddAttribute("varname",true);
    acheck.AddAttribute("listvar",true);
    acheck.AddAttribute("x",false);
    acheck.AddAttribute("y",false);
    acheck.AddAttribute("hidden",false);
    acheck.ValidateAttributes(i_XMLNode);

    SelectionNode selnode = new SelectionNode(DOMUtil.GetAttributeWithName(i_XMLNode,"varname"),
                                              DOMUtil.GetIntAttribute(i_XMLNode,"x"),
                                              DOMUtil.GetIntAttribute(i_XMLNode,"y"),
                                              DOMUtil.GetAttributeWithName(i_XMLNode,"hidden"),
                                              i_ParentNode,
                                              m_gamegui,
                                              true,
                                              DOMUtil.GetAttributeWithName(i_XMLNode,"listvar"));


    DOMChildIterator cit = new DOMChildIterator(i_XMLNode);
    org.w3c.dom.Node childNode;

    while((childNode = cit.GetNext()) != null)
    {
      String cname = childNode.getNodeName();
      if      (cname.equals("eventhandler")) { WalkEventHandlerNode(childNode,selnode); }
      else if (cname.equals("#text") || cname.equals("#comment"))   { }
      else
      {
        throw new GameXMLException("Unknown subnode of Selection " + cname);
      }
    }
  }


  private void WalkMinMaxVarSelectionNode(org.w3c.dom.Node i_XMLNode,GameNode i_ParentNode) throws GameXMLException
  {
    AttributeChecker acheck = new AttributeChecker();
    acheck.AddAttribute("varname",true);
    acheck.AddAttribute("min",true);
    acheck.AddAttribute("max",true);
    acheck.AddAttribute("x",false);
    acheck.AddAttribute("y",false);
    acheck.AddAttribute("hidden",false);
    acheck.ValidateAttributes(i_XMLNode);

    MinMaxSelectionNode selnode = new MinMaxSelectionNode(DOMUtil.GetAttributeWithName(i_XMLNode,"varname"),
                                                          DOMUtil.GetIntAttribute(i_XMLNode,"x"),
                                                          DOMUtil.GetIntAttribute(i_XMLNode,"y"),
                                                          DOMUtil.GetAttributeWithName(i_XMLNode,"hidden"),
                                                          i_ParentNode,
                                                          m_gamegui,
                                                          DOMUtil.GetAttributeWithName(i_XMLNode,"min"),
                                                          DOMUtil.GetAttributeWithName(i_XMLNode,"max"));


    DOMChildIterator cit = new DOMChildIterator(i_XMLNode);
    org.w3c.dom.Node childNode;

    while((childNode = cit.GetNext()) != null)
    {
      String cname = childNode.getNodeName();
      if      (cname.equals("eventhandler")) { WalkEventHandlerNode(childNode,selnode); }
      else if (cname.equals("#text") || cname.equals("#comment"))   { }
      else
      {
        throw new GameXMLException("Unknown subnode of MinMax Var Selection " + cname);
      }
    }
  }



  private void WalkFixedSpinnerNode(org.w3c.dom.Node i_XMLNode,GameNode i_ParentNode) throws GameXMLException
  {
    AttributeChecker acheck = new AttributeChecker();
    acheck.AddAttribute("varname",true);
    acheck.AddAttribute("list",true);
    acheck.AddAttribute("x",false);
    acheck.AddAttribute("y",false);
    acheck.AddAttribute("hidden",false);
    acheck.ValidateAttributes(i_XMLNode);

    SpinnerNode selnode = new SpinnerNode(DOMUtil.GetAttributeWithName(i_XMLNode,"varname"),
                                          DOMUtil.GetIntAttribute(i_XMLNode,"x"),
                                          DOMUtil.GetIntAttribute(i_XMLNode,"y"),
                                          DOMUtil.GetAttributeWithName(i_XMLNode,"hidden"),
                                          i_ParentNode,
                                          m_gamegui,
                                          false,
                                          DOMUtil.GetAttributeWithName(i_XMLNode,"list"));


    DOMChildIterator cit = new DOMChildIterator(i_XMLNode);
    org.w3c.dom.Node childNode;

    while((childNode = cit.GetNext()) != null)
    {
      String cname = childNode.getNodeName();
      if      (cname.equals("eventhandler")) { WalkEventHandlerNode(childNode,selnode); }
      else if (cname.equals("#text") || cname.equals("#comment"))   { }
      else
      {
        throw new GameXMLException("Unknown subnode of Selection " + cname);
      }
    }
  }

  private void WalkVarSpinnerNode(org.w3c.dom.Node i_XMLNode,GameNode i_ParentNode) throws GameXMLException
  {
    AttributeChecker acheck = new AttributeChecker();
    acheck.AddAttribute("varname",true);
    acheck.AddAttribute("listvar",true);
    acheck.AddAttribute("x",false);
    acheck.AddAttribute("y",false);
    acheck.AddAttribute("hidden",false);
    acheck.ValidateAttributes(i_XMLNode);

    SpinnerNode selnode = new SpinnerNode(DOMUtil.GetAttributeWithName(i_XMLNode,"varname"),
                                          DOMUtil.GetIntAttribute(i_XMLNode,"x"),
                                          DOMUtil.GetIntAttribute(i_XMLNode,"y"),
                                          DOMUtil.GetAttributeWithName(i_XMLNode,"hidden"),
                                          i_ParentNode,
                                          m_gamegui,
                                          true,
                                          DOMUtil.GetAttributeWithName(i_XMLNode,"listvar"));

    DOMChildIterator cit = new DOMChildIterator(i_XMLNode);
    org.w3c.dom.Node childNode;

    while((childNode = cit.GetNext()) != null)
    {
      String cname = childNode.getNodeName();
      if      (cname.equals("eventhandler")) { WalkEventHandlerNode(childNode,selnode); }
      else if (cname.equals("#text") || cname.equals("#comment"))   { }
      else
      {
        throw new GameXMLException("Unknown subnode of Selection " + cname);
      }
    }
  }

								   
  private void WalkButtonNode(org.w3c.dom.Node i_XMLNode,GameNode i_ParentNode) throws GameXMLException
  {
    AttributeChecker acheck = new AttributeChecker();
    acheck.AddAttribute("varname",true);
    acheck.AddAttribute("varvalue",true);
    acheck.AddAttribute("x",false);
    acheck.AddAttribute("y",false);
    acheck.AddAttribute("hidden",false);
    acheck.AddAttribute("image",false);
    acheck.ValidateAttributes(i_XMLNode);

    ButtonNode bnode = new ButtonNode(DOMUtil.GetAttributeWithName(i_XMLNode,"varname"),
                                      DOMUtil.GetAttributeWithName(i_XMLNode,"varvalue"),
                                      DOMUtil.GetAttributeWithName(i_XMLNode,"hidden"),
                                      DOMUtil.GetIntAttribute(i_XMLNode,"x"),
                                      DOMUtil.GetIntAttribute(i_XMLNode,"y"),
                                      DOMUtil.GetAttributeWithName(i_XMLNode,"image"),
                                      i_ParentNode,
                                      m_gamegui);

    WalkReplacingTextChildren(i_XMLNode,bnode);
  }

  private void WalkUserDefinedNode(org.w3c.dom.Node i_XMLNode,GameNode i_ParentNode) throws GameXMLException
  {
    String name = DOMUtil.GetAttributeWithName(i_XMLNode,"classname");
    int x = DOMUtil.GetIntAttribute(i_XMLNode,"x");
    int y = DOMUtil.GetIntAttribute(i_XMLNode,"y");
    String hidden = DOMUtil.GetAttributeWithName(i_XMLNode,"hidden");

    if (name == null)
    {
      throw new GameXMLException("classname is required for javaclass node");
    }

    name = m_Name + "." + name;

    HashMap<String,String> args = new HashMap<String,String>();

    int i;

    for (i = 0 ; i < i_XMLNode.getAttributes().getLength() ; ++i)
    {
      org.w3c.dom.Node attrnode = i_XMLNode.getAttributes().item(i);
      
      if (attrnode.getNodeName().equals("classname") ||
          attrnode.getNodeName().equals("x") ||
          attrnode.getNodeName().equals("y") ||
          attrnode.getNodeName().equals("hidden"))
      {
        continue;
      }

      args.put(attrnode.getNodeName(),attrnode.getNodeValue());
    }

    UserDefinedNode udn = new UserDefinedNode(i_ParentNode,m_gamegui,
                                              x,y,hidden,
                                              name,args,m_FilePrefix);

    DOMChildIterator cit = new DOMChildIterator(i_XMLNode);
    org.w3c.dom.Node childNode;

    while((childNode = cit.GetNext()) != null)
    {
      String cname = childNode.getNodeName();
      if      (cname.equals("eventhandler")) { WalkEventHandlerNode(childNode,udn); }
      else if (cname.equals("#text") || cname.equals("#comment"))   { }
      else
      {
        throw new GameXMLException("Unknown subnode of javaclass " + cname);
      }
    }



  }
                                              
    

}


