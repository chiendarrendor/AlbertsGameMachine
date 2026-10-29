package Outpost;

import NodeInterfacePackage.*;
import org.json.JSONObject;
import org.json.JSONArray;
import javax.swing.JButton;
import javax.swing.JPanel;
import java.awt.Component;
import java.util.HashMap;

/**
 * Plain, dependency-free test runner -- see
 * client/org/doghouse/jsonpath/JsonPathTest.java for the project's Java
 * test convention. No GUIUnit/GameXMLWalker involved -- OptionPanel is
 * exercised directly, the same way SequenceManagerTest drives SequenceManager,
 * with hand-assembled envelopes matching OutpostSchema.json's real shape.
 */
public class OptionPanelTest
{
  private static int s_failures = 0;

  private static void check(String i_label,boolean i_condition)
  {
    if (i_condition)
    {
      System.out.println("ok:   " + i_label);
    }
    else
    {
      s_failures++;
      System.out.println("FAIL: " + i_label);
    }
  }

  public static void main(String[] args)
  {
    testPanelIsOpaqueInBothModes();
    testPassiveModeHasNoControlsAndNeverTouchesActionTransferManager();
    testPassiveModeDisplaysCurrentValues();
    testActiveModeCheckboxReportsLiteralBooleanStringNotOneZero();
    testActiveModeSelectorRoundTrips();
    testActiveModeButtonEnabledFollowsActionLegalityChanged();
    testActiveModeButtonClickCallsButtonPressed();
    testActiveModeButtonGroupSwitchesWithPlayerStatusAndState();
    testActiveModePanelVisibleThroughInitialStateAndValidateSetupRegardlessOfPlayerStatus();
    testActiveModeFieldsOnlyEditableInInitialStateForASeatedPlayer();
    testResetReturnsControlsToDefaults();
    testUnknownModeThrows();

    System.out.println();
    System.out.println(s_failures == 0 ? "ALL PASS" : s_failures + " FAILURE(S)");
    System.exit(s_failures == 0 ? 0 : 1);
  }

  // --- envelope/fixture builders ---

  private static GameEvent gameGuiEvent(String i_state,Integer i_myPlayerId,JSONObject i_options)
  {
    JSONObject body = new JSONObject();
    body.put("options",i_options);
    body.put("players",new JSONArray());
    body.put("myPlayerId",i_myPlayerId == null ? JSONObject.NULL : i_myPlayerId);

    JSONObject fullState = new JSONObject();
    fullState.put("state",i_state);
    fullState.put("stateDesc","D");
    fullState.put("legalActions",new JSONArray());
    fullState.put("gameInstanceId","00000000-0000-0000-0000-000000000000");
    fullState.put("type","transition");
    fullState.put("sequences",new JSONObject());
    fullState.put("body",body);

    JSONObject envelope = new JSONObject();
    envelope.put("gui","Outpost");
    envelope.put("recipient","alice");
    envelope.put("XMLLoc","loc");
    envelope.put("XMLFile","file.xml");
    envelope.put("fullState",fullState);

    return new GameEvent(new GuiPacketParser(envelope.toString()));
  }

  private static JSONObject defaultOptions()
  {
    JSONObject o = new JSONObject();
    o.put("discard",false);
    o.put("refinery",false);
    o.put("oneupgrade",false);
    o.put("activebid",false);
    o.put("researchmega",false);
    o.put("smallresearch",false);
    o.put("smallmicro",false);
    o.put("blinddraws",false);
    o.put("robots",1);
    o.put("stock",1);
    o.put("supplyharshness",0);
    return o;
  }

  private static HashMap<String,String> modeOptions(String i_mode)
  {
    HashMap<String,String> m = new HashMap<String,String>();
    m.put("mode",i_mode);
    return m;
  }

  private static OptionPanel newPanel(String i_mode,JPanel i_panel)
  {
    return new OptionPanel(modeOptions(i_mode),"loc",i_panel,new ExternalAssets(new ActionTransferManager(),null));
  }

  private static OptionPanel.CheckboxField findCheckbox(OptionPanel p,String i_varname)
  {
    for (OptionPanel.OptionField f : p.m_fields)
    {
      if (f instanceof OptionPanel.CheckboxField cf && cf.m_varname.equals(i_varname)) return cf;
    }
    return null;
  }

  private static OptionPanel.SelectorField findSelector(OptionPanel p,String i_varname)
  {
    for (OptionPanel.OptionField f : p.m_fields)
    {
      if (f instanceof OptionPanel.SelectorField sf && sf.m_varname.equals(i_varname)) return sf;
    }
    return null;
  }

  // --- tests ---

  private static void testPanelIsOpaqueInBothModes()
  {
    // UserDefinedNode hands every widget a non-opaque panel by default; a
    // non-opaque panel doesn't paint its own background, which left stale
    // rendering behind when this panel's children resized (confirmed live,
    // 2026-10-29) -- OptionPanel has to override that itself.
    JPanel activePanel = new JPanel();
    OptionPanel active = newPanel("active",activePanel);
    check("the active-mode panel overrides UserDefinedNode's default non-opaque panel",
        activePanel.isOpaque());
    check("the active-mode panel has a visible border, so it reads as purposefully overlaying things",
        activePanel.getBorder() != null);

    // A plain JPanel is opaque with its own default background by default --
    // if grid/m_buttonGroups didn't override that, they'd completely hide
    // m_panel's own background underneath them (confirmed live, 2026-10-29:
    // the background change was invisible for exactly this reason).
    check("the inner grid panel is transparent, so m_panel's background shows through it",
        !active.m_grid.isOpaque());
    check("the inner button-group panel is transparent too",
        !active.m_buttonGroups.isOpaque());

    JPanel passivePanel = new JPanel();
    OptionPanel passive = newPanel("passive",passivePanel);
    check("the passive-mode panel is opaque too",
        passivePanel.isOpaque());
    check("the passive-mode panel's inner grid is transparent too (no button-group panel to check)",
        !passive.m_grid.isOpaque());
  }

  private static void testPassiveModeHasNoControlsAndNeverTouchesActionTransferManager()
  {
    OptionPanel p = newPanel("passive",new JPanel());

    check("passive mode builds no checkbox control", findCheckbox(p,"Discard").m_checkbox == null);
    check("passive mode builds no selector control", findSelector(p,"Robots").m_box == null);
    check("passive mode never registers as an ActionVariableGenerator",
        p.m_atm.GetVariableGeneratorValue("Discard") == null);
    check("passive mode builds no action buttons", p.m_actionButtons.isEmpty());
  }

  private static void testPassiveModeDisplaysCurrentValues()
  {
    OptionPanel p = newPanel("passive",new JPanel());

    JSONObject opts = defaultOptions();
    opts.put("discard",true);
    opts.put("robots",2);

    p.HandleEvent(gameGuiEvent("InitialState",0,opts));

    check("passive checkbox field's value label reflects the FSS",
        "true".equals(findCheckbox(p,"Discard").m_valueLabel.getText()));
    check("passive selector field's value label reflects the FSS",
        "Second Robot Unlimited".equals(findSelector(p,"Robots").m_valueLabel.getText()));
  }

  private static void testActiveModeCheckboxReportsLiteralBooleanStringNotOneZero()
  {
    OptionPanel p = newPanel("active",new JPanel());
    OptionPanel.CheckboxField cf = findCheckbox(p,"Discard");

    cf.m_checkbox.setSelected(true);
    // ActionVarNode.ParseTypedValue does Boolean.valueOf(raw) for paramtype="bool" --
    // that only ever recognizes the literal string "true"; the old OptionRow
    // convention of "1"/"0" would silently always parse as false regardless.
    check("a checked box reports the literal string \"true\", not \"1\"",
        "true".equals(p.m_atm.GetVariableGeneratorValue("Discard")));

    cf.m_checkbox.setSelected(false);
    check("an unchecked box reports \"false\", not \"0\"",
        "false".equals(p.m_atm.GetVariableGeneratorValue("Discard")));
  }

  private static void testActiveModeSelectorRoundTrips()
  {
    OptionPanel p = newPanel("active",new JPanel());

    OptionPanel.SelectorField robots = findSelector(p,"Robots");
    robots.m_box.setSelectedIndex(2); // "All Multiplicative"
    check("a one-counted selector reports index+1",
        "3".equals(p.m_atm.GetVariableGeneratorValue("Robots")));

    OptionPanel.SelectorField ship = findSelector(p,"SupplyShipFilling");
    ship.m_box.setSelectedIndex(2); // "Two Strikes"
    check("a zero-counted selector reports the raw index",
        "2".equals(p.m_atm.GetVariableGeneratorValue("SupplyShipFilling")));
  }

  private static void testActiveModeButtonEnabledFollowsActionLegalityChanged()
  {
    OptionPanel p = newPanel("active",new JPanel());
    JButton joinButton = p.m_actionButtons.get("JOIN").m_button;

    check("a go-button starts disabled", !joinButton.isEnabled());

    p.m_atm.ActionLegalityChanged("JOIN",true);
    check("ActionLegalityChanged(true) enables the button", joinButton.isEnabled());

    p.m_atm.ActionLegalityChanged("JOIN",false);
    check("ActionLegalityChanged(false) disables it again", !joinButton.isEnabled());
  }

  private static void testActiveModeButtonClickCallsButtonPressed()
  {
    OptionPanel p = newPanel("active",new JPanel());

    final boolean[] fired = new boolean[1];
    p.m_atm.AddActionHandler("JOIN", () -> fired[0] = true);

    // A disabled JButton won't fire its ActionListener even via doClick() --
    // make it legal first, same as a real ActionLegalityChanged push would.
    p.m_atm.ActionLegalityChanged("JOIN",true);
    p.m_actionButtons.get("JOIN").m_button.doClick();

    check("clicking a go-button reaches ActionTransferManager.ButtonPressed and the real handler",
        fired[0]);
  }

  private static void testActiveModeButtonGroupSwitchesWithPlayerStatusAndState()
  {
    OptionPanel p = newPanel("active",new JPanel());

    Component joinGroup = p.m_actionButtons.get("JOIN").m_button.getParent();
    Component initialGroup = p.m_actionButtons.get("OPTIONS").m_button.getParent();
    Component validateGroup = p.m_actionButtons.get("LIKEOPTIONS").m_button.getParent();

    p.HandleEvent(gameGuiEvent("InitialState",null,defaultOptions()));
    check("non-player sees only the JOIN group",
        joinGroup.isVisible() && !initialGroup.isVisible() && !validateGroup.isVisible());

    p.HandleEvent(gameGuiEvent("InitialState",0,defaultOptions()));
    check("a seated player in InitialState sees only the initial-setup group",
        !joinGroup.isVisible() && initialGroup.isVisible() && !validateGroup.isVisible());

    p.HandleEvent(gameGuiEvent("ValidateSetup",0,defaultOptions()));
    check("a seated player in ValidateSetup sees only the LIKEOPTIONS/DONTLIKEOPTIONS group",
        !joinGroup.isVisible() && !initialGroup.isVisible() && validateGroup.isVisible());
  }

  private static void testActiveModePanelVisibleThroughInitialStateAndValidateSetupRegardlessOfPlayerStatus()
  {
    JPanel panel = new JPanel();
    OptionPanel p = newPanel("active",panel);

    p.HandleEvent(gameGuiEvent("InitialState",0,defaultOptions()));
    check("active panel is visible for a seated player in InitialState", panel.isVisible());

    p.HandleEvent(gameGuiEvent("ValidateSetup",0,defaultOptions()));
    check("active panel stays visible in ValidateSetup -- LIKEOPTIONS/DONTLIKEOPTIONS live there",
        panel.isVisible());

    // JOIN's own <allowed> is "!IsPlayer(i_PlayerName)" -- it's legal
    // *precisely* for someone who isn't a player yet, so the panel must stay
    // visible for a non-player too, or JOIN would be unreachable for the one
    // population that needs it (everyone, at the start of every game).
    p.HandleEvent(gameGuiEvent("InitialState",null,defaultOptions()));
    check("active panel stays visible for a non-player in InitialState, so JOIN stays reachable",
        panel.isVisible());

    p.HandleEvent(gameGuiEvent("TurnOrder",0,defaultOptions()));
    check("active panel is hidden once setup is fully done (neither InitialState nor ValidateSetup)",
        !panel.isVisible());
  }

  private static void testActiveModeFieldsOnlyEditableInInitialStateForASeatedPlayer()
  {
    OptionPanel p = newPanel("active",new JPanel());
    OptionPanel.CheckboxField cf = findCheckbox(p,"Discard");
    OptionPanel.SelectorField sf = findSelector(p,"Robots");

    p.HandleEvent(gameGuiEvent("InitialState",0,defaultOptions()));
    check("option fields are enabled in InitialState for a seated player",
        cf.m_checkbox.isEnabled() && sf.m_box.isEnabled());

    // OPTIONS itself is only ever legal in InitialState (OutpostServer.xml's
    // <cyclic state="InitialState"/>) -- once VALIDATESETUP moves the game to
    // ValidateSetup, the setup is frozen pending LIKEOPTIONS/DONTLIKEOPTIONS,
    // so the fields must stop being editable even though the panel itself
    // stays up.
    p.HandleEvent(gameGuiEvent("ValidateSetup",0,defaultOptions()));
    check("option fields are disabled in ValidateSetup even though the panel is still visible",
        !cf.m_checkbox.isEnabled() && !sf.m_box.isEnabled());

    // OPTIONS requires GetPlayers().IsPlayer(i_PlayerName) server-side, so a
    // non-player's edits could never actually be submitted -- the fields
    // (not the panel, and not JOIN's own button) are what should reflect that.
    p.HandleEvent(gameGuiEvent("InitialState",null,defaultOptions()));
    check("option fields are disabled for a non-player even in InitialState",
        !cf.m_checkbox.isEnabled() && !sf.m_box.isEnabled());

    p.HandleEvent(gameGuiEvent("InitialState",0,defaultOptions()));
    check("option fields re-enable if DONTLIKEOPTIONS sends a seated player back to InitialState",
        cf.m_checkbox.isEnabled() && sf.m_box.isEnabled());
  }

  private static void testResetReturnsControlsToDefaults()
  {
    OptionPanel p = newPanel("active",new JPanel());

    OptionPanel.CheckboxField cf = findCheckbox(p,"Discard");
    cf.m_checkbox.setSelected(true);
    OptionPanel.SelectorField sf = findSelector(p,"Robots");
    sf.m_box.setSelectedIndex(2);

    p.Reset();

    check("Reset() clears a checkbox field", !cf.m_checkbox.isSelected());
    check("Reset() returns a selector field to its first entry", sf.m_box.getSelectedIndex() == 0);
  }

  private static void testUnknownModeThrows()
  {
    boolean threw = false;
    try
    {
      newPanel("sideways",new JPanel());
    }
    catch (NodeInterfacePackageException e)
    {
      threw = true;
    }
    check("an unrecognized mode throws rather than silently picking one", threw);
  }
}
