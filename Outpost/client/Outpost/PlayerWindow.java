package Outpost;

import NodeInterfacePackage.*;
import javax.swing.*;
import java.awt.*;
import java.util.HashMap;

// The sole <javaclass> in the "Players" window -- constructs every other
// widget that used to be separate <javaclass>/<text>/<panel> entries in
// Players.xml directly as plain Java objects, and fans out
// HandleEvent/Refresh/Reset to them by hand, since they're no longer part
// of the GameNode tree GameXMLWalker would otherwise walk automatically.
//
// Exists specifically to give OptionPanel (mode="active") a way to sit on
// top of, and correctly occlude, the rest of this window's content --
// plain Container z-order (what every other <window> still uses, via
// LockLayout) doesn't reliably support deliberately-overlapping siblings
// once something *inside* the overlap triggers its own narrow repaint (a
// checkbox's rollover highlight, confirmed live, 2026-10-29: ItemStockGrid's
// Robots card kept bleeding through underneath OptionPanel's own checkboxes
// even though OptionPanel's own panel was opaque and genuinely on top).
// JLayeredPane is the standard Swing-sanctioned fix for exactly this -- it
// already has a precedent in this codebase
// (MerchantOfVenus/client/MerchantOfVenus/PopAndLockPanel.java). Sizing the
// two layers uses a small custom LayoutManager (LayerLayout, below) rather
// than OverlayLayout: OverlayLayout stretches every layer to fill the whole
// pane by default (confirmed live, 2026-10-29 -- the active-mode OptionPanel
// started rendering window-filling, when the design was always "as big as
// it needs to be, not window-filling"), and neither OverlayLayout's own
// alignment nor GridBagLayout's anchor (both tried and empirically checked)
// actually pin an unstretched component to a corner when the other layer's
// zero weight/alignment leaves it centered instead -- a plain custom
// LayoutManager sidesteps that family of surprises entirely.
//
// See .claude/TODO.md's "Migrate Outpost's client GUI away from
// ActionNode/ActionTabNode-driven buttons" entry for the rest of this
// arc's design, and docs/ for any further write-up.
public class PlayerWindow implements UserDefinedInterface
{
  private static final GuiPacketQuery turnNumQuery = new GuiPacketQuery("turnNum");

  private JLabel m_turnNumberLabel;
  private ItemStockGrid m_itemStockGrid;
  private ShipList m_shipList;
  private TurnOrder m_turnOrder;
  private PlayerDisplay m_playerDisplay;
  private OptionPanel m_optionPanel;
  private JPanel m_lowerContent;

  public PlayerWindow(HashMap<String,String> i_Options,String i_DataLoc,JPanel i_Panel,ExternalAssets i_assets)
  {
    JLayeredPane layers = new JLayeredPane();

    m_lowerContent = buildLowerContent(i_DataLoc,i_assets);
    layers.add(m_lowerContent,JLayeredPane.DEFAULT_LAYER);

    JPanel optionPanelPanel = new JPanel();
    m_optionPanel = new OptionPanel(activeModeOptions(),i_DataLoc,optionPanelPanel,i_assets);
    layers.add(optionPanelPanel,JLayeredPane.PALETTE_LAYER);

    layers.setLayout(new LayerLayout(m_lowerContent,optionPanelPanel));

    i_Panel.setLayout(new BorderLayout());
    i_Panel.add(layers,BorderLayout.CENTER);
  }

  // lowerContent fills the whole layered pane, same as before; optionPanel
  // stays pinned to its own natural (unstretched) size in the top-left
  // corner, correctly re-laid-out both on resize and whenever optionPanel's
  // own preferred size changes (e.g. a value label's text changing width),
  // since this participates in Swing's normal revalidate/layoutContainer
  // pipeline rather than being a one-off computation.
  private static class LayerLayout implements LayoutManager
  {
    private final JPanel m_lower;
    private final JPanel m_optionPanel;

    LayerLayout(JPanel i_lower,JPanel i_optionPanel)
    {
      m_lower = i_lower;
      m_optionPanel = i_optionPanel;
    }

    public void layoutContainer(Container i_parent)
    {
      m_lower.setBounds(0,0,i_parent.getWidth(),i_parent.getHeight());
      Dimension pref = m_optionPanel.getPreferredSize();
      m_optionPanel.setBounds(0,0,pref.width,pref.height);
    }

    public Dimension preferredLayoutSize(Container i_parent) { return m_lower.getPreferredSize(); }
    public Dimension minimumLayoutSize(Container i_parent) { return m_lower.getMinimumSize(); }
    public void addLayoutComponent(String i_name,Component i_comp) { }
    public void removeLayoutComponent(Component i_comp) { }
  }

  private JPanel buildLowerContent(String i_DataLoc,ExternalAssets i_assets)
  {
    JPanel lower = new JPanel();
    lower.setLayout(new BoxLayout(lower,BoxLayout.Y_AXIS));

    m_turnNumberLabel = new JLabel("Turn Number: 0");
    addLeftJustified(lower,m_turnNumberLabel);

    JPanel itemStockPanel = new JPanel();
    m_itemStockGrid = new ItemStockGrid(new HashMap<String,String>(),i_DataLoc,itemStockPanel,i_assets);
    addLeftJustified(lower,itemStockPanel);

    JPanel shipRow = new JPanel();
    shipRow.setLayout(new BoxLayout(shipRow,BoxLayout.X_AXIS));
    shipRow.add(new JLabel("Supply Ship:"));
    JPanel shipListPanel = new JPanel();
    m_shipList = new ShipList(new HashMap<String,String>(),i_DataLoc,shipListPanel,i_assets);
    shipRow.add(shipListPanel);
    addLeftJustified(lower,shipRow);

    JPanel turnOrderPanel = new JPanel();
    m_turnOrder = new TurnOrder(new HashMap<String,String>(),i_DataLoc,turnOrderPanel,i_assets);
    addLeftJustified(lower,turnOrderPanel);

    JPanel playerDisplayPanel = new JPanel();
    m_playerDisplay = new PlayerDisplay(new HashMap<String,String>(),i_DataLoc,playerDisplayPanel,i_assets);
    addLeftJustified(lower,playerDisplayPanel);

    return lower;
  }

  // A plain JComponent's default maximum size is unbounded, so BoxLayout.Y_AXIS
  // stretches each cross-axis dimension to fill lower's full (now window-wide)
  // width instead of honoring LEFT_ALIGNMENT at the component's own natural
  // size (confirmed live, 2026-10-29: every sub-widget rendered centered
  // across the whole window instead of left-justified at its own compact
  // size, for exactly this reason).
  //
  // A one-time setMaximumSize(getPreferredSize()) snapshot isn't enough,
  // though -- ShipList/TurnOrder/PlayerDisplay all build their real content
  // *lazily*, inside their own HandleEvent, not at construction time (their
  // constructors just create an empty content panel), so a snapshot taken
  // right after construction freezes them at their near-empty starting size
  // forever (confirmed live, 2026-10-29: this is exactly what reduced
  // everything below "Supply Ship" to "a tiny box" or nothing at all).
  // ShrinkWrapPanel's getMaximumSize() override tracks getPreferredSize()
  // live instead, so it grows correctly as real content arrives later.
  private static void addLeftJustified(JPanel i_parent,Component i_child)
  {
    ShrinkWrapPanel wrapper = new ShrinkWrapPanel(i_child);
    wrapper.setAlignmentX(Component.LEFT_ALIGNMENT);
    i_parent.add(wrapper);
  }

  private static class ShrinkWrapPanel extends JPanel
  {
    ShrinkWrapPanel(Component i_content)
    {
      super(new BorderLayout());
      setOpaque(false);
      add(i_content,BorderLayout.CENTER);
    }

    public Dimension getMaximumSize() { return getPreferredSize(); }
  }

  private static HashMap<String,String> activeModeOptions()
  {
    HashMap<String,String> options = new HashMap<String,String>();
    options.put("mode","active");
    return options;
  }

  public void HandleEvent(GameEvent i_event)
  {
    m_turnNumberLabel.setText("Turn Number: " + turnNumQuery.evaluateInt(i_event,0));
    m_itemStockGrid.HandleEvent(i_event);
    m_shipList.HandleEvent(i_event);
    m_turnOrder.HandleEvent(i_event);
    m_playerDisplay.HandleEvent(i_event);
    m_optionPanel.HandleEvent(i_event);

    // BoxLayout caches each child's size requirements and, confirmed
    // empirically 2026-10-29, doesn't reliably notice a grandchild's
    // preferred size growing (e.g. ShipList adding real ShipItems above)
    // just from the normal invalidate() cascade -- force it explicitly
    // rather than relying on that cascade reaching m_lowerContent's own
    // BoxLayout on its own.
    m_lowerContent.invalidate();
    m_lowerContent.doLayout();
    m_lowerContent.repaint();
  }

  public void Refresh()
  {
    m_itemStockGrid.Refresh();
    m_shipList.Refresh();
    m_turnOrder.Refresh();
    m_playerDisplay.Refresh();
    m_optionPanel.Refresh();
  }

  public void Reset()
  {
    m_turnNumberLabel.setText("Turn Number: 0");
    m_itemStockGrid.Reset();
    m_shipList.Reset();
    m_turnOrder.Reset();
    m_playerDisplay.Reset();
    m_optionPanel.Reset();
  }
}
