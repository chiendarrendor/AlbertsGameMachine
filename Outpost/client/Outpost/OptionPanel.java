package Outpost;

import NodeInterfacePackage.*;
import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.HashMap;
import java.util.Map;
import java.util.ArrayList;
import java.util.List;

// Consolidates what used to be eleven separate OptionRow/OptionSelector
// widgets, plus the first 8 "start of game" actions' own ActionNode buttons,
// into one widget with two modes -- see .claude/TODO.md's "Migrate Outpost's
// client GUI away from ActionNode/ActionTabNode-driven buttons" entry for
// the full design this implements.
//
//   mode="passive": read-only, no interactive controls at all, never
//   touches ActionTransferManager. Lives permanently in the "Outpost
//   Options" tab, visible to anyone (player or spectator).
//
//   mode="active": today's editable behavior (registers as the
//   ActionVariableGenerator for each of OPTIONS's 11 vars, same var names
//   actions.xml's <action name="OPTIONS"> already declares) plus "go"
//   buttons for the first 8 actions. All 8 in one row ran wider than the
//   window, and they naturally split into three groups that are never
//   relevant at the same time (JOIN alone before joining; OPTIONS/
//   STANDARDBASIC/STANDARDEXPERT/UNJOIN/VALIDATESETUP for a seated player
//   in InitialState; LIKEOPTIONS/DONTLIKEOPTIONS during ValidateSetup) --
//   so only one group is shown at a time (CardLayout), while buttons
//   *within* the visible group still just enable/disable (never hidden) via
//   ActionTransferManager.AddButtonPresser/ActionLegalityChanged, so their
//   relative position within the group never jumps around. Lives in the
//   "Players" window, declared *first* among its siblings so it paints on
//   top (Swing paints earlier-added siblings over later ones -- see
//   WindowNode/LockLayout), and hides itself (this widget's own i_Panel)
//   once the game is no longer in InitialState or ValidateSetup.
//
// The ActionNode instances for these 8 actions are not removed -- they
// stay exactly as declared in actions.xml (onstate="statedalwayshidden"),
// which keeps them silently computing legality/pushing
// ActionLegalityChanged and handling real submission via ActionGo() even
// though their own button panel never renders. This widget's buttons never
// call SendAction themselves -- just ActionTransferManager.ButtonPressed,
// reusing that unchanged machinery.
public class OptionPanel implements UserDefinedInterface
{
  private static final GuiPacketQuery stateQuery = new GuiPacketQuery("{STATE}");
  private static final GuiPacketQuery myPlayerIdQuery = new GuiPacketQuery("myPlayerId");

  static final String GROUP_JOIN = "join";
  static final String GROUP_INITIAL = "initial";
  static final String GROUP_VALIDATE = "validate";

  boolean m_active;
  JPanel m_panel;
  ActionTransferManager m_atm;
  List<OptionField> m_fields = new ArrayList<OptionField>();
  Map<String,ActionButton> m_actionButtons = new HashMap<String,ActionButton>();
  JPanel m_grid;
  JPanel m_buttonGroups; // CardLayout, active mode only -- null in passive mode

  public OptionPanel(HashMap<String,String> i_Options,String i_DataLoc,JPanel i_Panel,ExternalAssets i_assets)
  {
    if (!i_Options.containsKey("mode")) throw new NodeInterfacePackageException("OptionPanel missing required 'mode' option");
    String mode = i_Options.get("mode");
    if (mode.equals("active")) m_active = true;
    else if (mode.equals("passive")) m_active = false;
    else throw new NodeInterfacePackageException("OptionPanel: unknown mode '" + mode + "'");

    m_panel = i_Panel;
    m_atm = i_assets.GetActionTransferManager();

    // UserDefinedNode hands every widget a non-opaque panel by default (fine
    // for most widgets, which don't need to blot out whatever's behind them)
    // -- but this one is specifically meant to sit on top of and occlude
    // everything else in the Players window while active, and a non-opaque
    // component doesn't paint its own background, which leaves Swing's
    // repaint-damage tracking unable to properly clear stale pixels when a
    // child resizes/moves (confirmed live, 2026-10-29: stale checkbox
    // rendering bled into the Robots row until a full window repaint).
    m_panel.setOpaque(true);
    // A couple of pixels of border so it visually reads as purposefully
    // overlaying whatever's underneath, not just an unexplained edge.
    m_panel.setBorder(BorderFactory.createLineBorder(Color.BLACK,2));
    m_panel.setLayout(new BorderLayout());

    // hgap/vgap give each cell a little breathing room -- without it the
    // longest label ("One Upgrade Purchase Per Player Per Turn") runs right
    // up against its own value text with no gap at all.
    m_grid = new JPanel(new GridLayout(11,m_active ? 3 : 2,8,2));
    // A plain JPanel is opaque by default with its own (Swing-default-gray)
    // background, and BorderLayout.CENTER expands to cover nearly all of
    // m_panel -- without this, grid's own background completely hides
    // m_panel's custom color underneath it (confirmed live, 2026-10-29: the
    // background change was invisible, indistinguishable from the window's
    // own default gray, for exactly this reason).
    m_grid.setOpaque(false);
    m_panel.add(m_grid,BorderLayout.CENTER);

    addCheckboxField(m_grid,"Discard","Discard Before Buy","options.discard");
    addCheckboxField(m_grid,"Refinery","Refinery","options.refinery");
    addCheckboxField(m_grid,"OneUpgrade","One Upgrade Purchase Per Player Per Turn","options.oneupgrade");
    addCheckboxField(m_grid,"ActiveBid","Only Active Players May Bid","options.activebid");
    addCheckboxField(m_grid,"ResearchMega","Research has Mega","options.researchmega");
    addCheckboxField(m_grid,"SmallResearch","Research resources are zero size","options.smallresearch");
    addCheckboxField(m_grid,"SmallMicro","Microbiotics resources are zero size","options.smallmicro");
    addCheckboxField(m_grid,"BlindDraws","Draws only visible at end of draw phase","options.blinddraws");
    addSelectorField(m_grid,"Robots","Robot Mechanism","options.robots",true,
        "First Robot Unlimited","Second Robot Unlimited","All Multiplicative");
    addSelectorField(m_grid,"Stocking","Initial Item Stocking","options.stock",true,
        "Normal Item Stocking","4-10 Items have one Extra");
    addSelectorField(m_grid,"SupplyShipFilling","Ship Supply Loading","options.supplyharshness",false,
        "Normal Ship Fill","One Strike","Two Strikes","Three Strikes","Four Strikes","Five Strikes");

    if (m_active)
    {
      // All 8 buttons in one row ran wider than the window. They also
      // naturally split into three groups that are never relevant at the
      // same time (exactly one is ever legal to be looking at: before
      // joining, as a seated player during InitialState, or confirming
      // setup during ValidateSetup) -- so only one group is ever shown at
      // once, switched by HandleEvent via CardLayout, rather than trying to
      // fit all 8 in a single row.
      m_buttonGroups = new JPanel(new CardLayout());
      m_buttonGroups.setOpaque(false); // see grid's own setOpaque(false) above for why
      m_panel.add(m_buttonGroups,BorderLayout.SOUTH);

      JPanel joinGroup = newButtonGroup();
      addActionButton(joinGroup,"JOIN","Join Game");
      m_buttonGroups.add(joinGroup,GROUP_JOIN);

      JPanel initialGroup = newButtonGroup();
      addActionButton(initialGroup,"OPTIONS","Apply Option Changes");
      addActionButton(initialGroup,"STANDARDBASIC","Select Basic Game Options");
      addActionButton(initialGroup,"STANDARDEXPERT","Select Expert Game Options");
      addActionButton(initialGroup,"UNJOIN","Leave Game");
      addActionButton(initialGroup,"VALIDATESETUP","Start Game");
      m_buttonGroups.add(initialGroup,GROUP_INITIAL);

      JPanel validateGroup = newButtonGroup();
      addActionButton(validateGroup,"DONTLIKEOPTIONS","I don't like the Selected Options");
      addActionButton(validateGroup,"LIKEOPTIONS","I like the Selected Options");
      m_buttonGroups.add(validateGroup,GROUP_VALIDATE);
    }
  }

  private static JPanel newButtonGroup()
  {
    JPanel group = new JPanel();
    group.setOpaque(false); // see grid's own setOpaque(false) above for why
    group.setLayout(new BoxLayout(group,BoxLayout.X_AXIS));
    return group;
  }

  // --- one row: label + read-only value display + (active-mode only) interactive control ---

  abstract class OptionField
  {
    final String m_varname;
    final GuiPacketQuery m_valueQuery;
    final JLabel m_valueLabel = new JLabel();

    OptionField(String i_varname,String i_optionpath)
    {
      m_varname = i_varname;
      m_valueQuery = new GuiPacketQuery(i_optionpath);
      m_fields.add(this);
    }

    abstract void updateFromEvent(GameEvent i_event);
    abstract void resetControl();

    // no-op in passive mode -- there's no control to enable/disable.
    abstract void setControlEnabled(boolean i_enabled);
  }

  class CheckboxField extends OptionField implements ActionVariableGenerator
  {
    final JCheckBox m_checkbox; // null in passive mode

    CheckboxField(String i_varname,String i_optionpath)
    {
      super(i_varname,i_optionpath);
      if (m_active)
      {
        m_checkbox = new JCheckBox();
        m_atm.AddVariableGenerator(m_varname,this);
      }
      else
      {
        m_checkbox = null;
      }
    }

    void updateFromEvent(GameEvent i_event)
    {
      boolean curval = (Boolean)m_valueQuery.evaluate(i_event);
      m_valueLabel.setText(String.valueOf(curval));
      if (m_active) m_checkbox.setSelected(curval);
    }

    void resetControl()
    {
      if (m_active) m_checkbox.setSelected(false);
      m_valueLabel.setText("false");
    }

    void setControlEnabled(boolean i_enabled)
    {
      if (m_active) m_checkbox.setEnabled(i_enabled);
    }

    public void ActionVariableReset() { m_checkbox.setSelected(false); }

    // This var's actions.xml paramtype is "bool" -- ActionVarNode.ParseTypedValue
    // does Boolean.valueOf(rawValue) for that type, which only ever recognizes the
    // literal string "true" (case-insensitively); the old OptionRow convention of
    // reporting "1"/"0" would silently parse as false either way. Report the real
    // boolean's string form instead.
    public String ActionVariableGetValue() { return String.valueOf(m_checkbox.isSelected()); }
  }

  class SelectorField extends OptionField implements ActionVariableGenerator
  {
    final JComboBox<String> m_box; // null in passive mode
    final String[] m_values;
    final boolean m_onecount;

    SelectorField(String i_varname,String i_optionpath,boolean i_onecount,String... i_values)
    {
      super(i_varname,i_optionpath);
      m_onecount = i_onecount;
      m_values = i_values;
      if (m_active)
      {
        m_box = new JComboBox<String>(i_values);
        m_atm.AddVariableGenerator(m_varname,this);
      }
      else
      {
        m_box = null;
      }
    }

    void updateFromEvent(GameEvent i_event)
    {
      int idx = (Integer)m_valueQuery.evaluate(i_event) - (m_onecount ? 1 : 0);
      if (m_active) m_box.setSelectedIndex(idx);
      m_valueLabel.setText(m_values[idx]);
    }

    void resetControl()
    {
      if (m_active) m_box.setSelectedIndex(0);
      m_valueLabel.setText(m_values[0]);
    }

    void setControlEnabled(boolean i_enabled)
    {
      if (m_active) m_box.setEnabled(i_enabled);
    }

    public void ActionVariableReset() { m_box.setSelectedIndex(0); }
    public String ActionVariableGetValue() { return String.valueOf(m_box.getSelectedIndex() + (m_onecount ? 1 : 0)); }
  }

  // --- one of the first-8 actions' "go" buttons (active mode only) ---

  class ActionButton implements ActionGenerator,ActionListener
  {
    final String m_actionname;
    final JButton m_button;

    ActionButton(String i_actionname,String i_text)
    {
      m_actionname = i_actionname;
      m_button = new JButton(i_text);
      m_button.setEnabled(false);
      m_button.addActionListener(this);
      m_atm.AddButtonPresser(m_actionname,this);
    }

    public void ActionLegalityChanged(boolean i_islegal) { m_button.setEnabled(i_islegal); }

    // Every one of these 8 actions is either parameterless or (OPTIONS) backed
    // by generators that always have some value from construction onward, so
    // readiness is always true -- nothing meaningful to do here.
    public void ActionReadinessChanged(boolean i_isready) { }

    public void actionPerformed(ActionEvent i_event) { m_atm.ButtonPressed(m_actionname); }
  }

  // --- wiring helpers ---

  private void addCheckboxField(JPanel i_grid,String i_varname,String i_label,String i_optionpath)
  {
    CheckboxField f = new CheckboxField(i_varname,i_optionpath);
    i_grid.add(new JLabel(i_label));
    i_grid.add(f.m_valueLabel);
    if (m_active) i_grid.add(f.m_checkbox);
  }

  private void addSelectorField(JPanel i_grid,String i_varname,String i_label,String i_optionpath,
                                boolean i_onecount,String... i_values)
  {
    SelectorField f = new SelectorField(i_varname,i_optionpath,i_onecount,i_values);
    i_grid.add(new JLabel(i_label));
    i_grid.add(f.m_valueLabel);
    if (m_active) i_grid.add(f.m_box);
  }

  private void addActionButton(JPanel i_buttonRow,String i_actionname,String i_text)
  {
    ActionButton b = new ActionButton(i_actionname,i_text);
    m_actionButtons.put(i_actionname,b);
    i_buttonRow.add(b.m_button);
  }

  // --- UserDefinedInterface ---

  public void HandleEvent(GameEvent i_event)
  {
    for (OptionField f : m_fields) f.updateFromEvent(i_event);

    if (m_active)
    {
      String state = stateQuery.evaluateString(i_event,"");
      int myPlayerId = myPlayerIdQuery.evaluateInt(i_event,-1);

      // OPTIONS/JOIN/STANDARDBASIC/STANDARDEXPERT/UNJOIN/VALIDATESETUP are
      // only ever legal in InitialState (see OutpostServer.xml); once
      // VALIDATESETUP fires, every player has to confirm the now-frozen
      // setup via LIKEOPTIONS/DONTLIKEOPTIONS, which only exist in
      // ValidateSetup -- so the panel has to stay up through both states
      // (DONTLIKEOPTIONS transits back to InitialState if anyone objects),
      // but the option fields themselves can only be edited in InitialState.
      //
      // The panel's own visibility must NOT depend on myPlayerId: JOIN's own
      // <allowed> is "!IsPlayer(i_PlayerName)" -- it's legal *precisely* for
      // someone who isn't a player yet, so hiding the whole panel for
      // non-players would make JOIN unreachable for the one population that
      // needs it. Per-button legality (already correct, driven by each
      // ActionNode's own {LEGALACTION:name}) is what keeps a non-player from
      // submitting OPTIONS/VALIDATESETUP/etc.; only the option *fields*
      // themselves need gating on actually being a player, since only a
      // player can ever submit OPTIONS.
      boolean inSetup = state.equals("InitialState") || state.equals("ValidateSetup");
      boolean editable = state.equals("InitialState") && myPlayerId != -1;

      m_panel.setVisible(inSetup);
      for (OptionField f : m_fields) f.setControlEnabled(editable);

      // Exactly one button group is ever relevant at a time: not-yet-a-player
      // only ever has JOIN available; a seated player in InitialState has the
      // other 5 setup actions; ValidateSetup only has LIKEOPTIONS/DONTLIKEOPTIONS.
      String group = myPlayerId == -1 ? GROUP_JOIN
          : state.equals("ValidateSetup") ? GROUP_VALIDATE
          : GROUP_INITIAL;
      ((CardLayout)m_buttonGroups.getLayout()).show(m_buttonGroups,group);
    }
  }

  public void Refresh() { }

  public void Reset()
  {
    for (OptionField f : m_fields) f.resetControl();
  }
}
