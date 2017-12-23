package MerchantOfVenus;

import java.awt.event.*;
import java.awt.*;
import javax.swing.*;
import java.util.*;
import NodeInterfacePackage.*;

class TradePanel extends JPanel 
{
	private static final long serialVersionUID = 1L;
ActionTransferManager atm;
  PlayerData pdata;
  TradeBaseData tbdata;
  Board board;
  OverrideSpaceData osdata;
  PopAndLockPanel palp;
  JLabel palphandle;

  JLabel playerName;
  JLabel baseRace;
  JLabel playerRace;
  JLabel curPlayerMoney;
  JLabel curPlayerBarter;
  JLabel holdInfo;
  JLabel proprietor;
  JLabel factoryOwner;
  JLabel numBuys;
  JLabel numSells;

  JPanel demandItems;
  JPanel baseItems;
  JPanel playerItems;
  JPanel cupItems;

  JButton endButton;

  private class CupOut
  {
    public ImageIcon ii;
    public String race;
    public CupOut(String encoding,String raceid)
    {
      ii = IconManager.GetTokenIcon(new Token(encoding));
      race = Utilities.GetRaceOfRaceId(raceid) + "(" + raceid + ")";
    }
  }

  public Vector<CupOut> cupouts;
  
  private JPanel GetSeparator(String text)
  {
    JPanel result = new JPanel();
    result.setLayout(new BorderLayout());
    result.setBackground(Color.BLUE);
    result.add(ControlPanel.MakeDecoratedLabel(text));
    return result;
  }


  public TradePanel(ActionTransferManager i_atm,PlayerData i_pdata,
                    TradeBaseData tbdata,Board board,OverrideSpaceData osdata,
                    PopAndLockPanel palp,JLabel palphandle)
  {
    this.atm = i_atm;
    this.pdata = i_pdata;
    this.tbdata = tbdata;
    this.board = board;
    this.osdata = osdata;
    this.palp = palp;
    this.palphandle = palphandle;

    ControlPanel.DecorateMasterPanel(this);
    setLayout(new BorderLayout());

    Reset();

    JPanel statusPanel = ControlPanel.MakeDecoratedPanel();
    add(statusPanel,BorderLayout.NORTH);
    GridLayout gl = new GridLayout(4,3);
    gl.setHgap(10);
    gl.setVgap(4);
    statusPanel.setLayout(gl);
    statusPanel.add(playerName = ControlPanel.MakeDecoratedLabel());
    statusPanel.add(ControlPanel.MakeDecoratedLabel()); // dummy 1
    statusPanel.add(ControlPanel.MakeDecoratedLabel()); // dummy 2
    statusPanel.add(baseRace = ControlPanel.MakeDecoratedLabel());
    statusPanel.add(proprietor = ControlPanel.MakeDecoratedLabel());
    statusPanel.add(factoryOwner = ControlPanel.MakeDecoratedLabel());
    statusPanel.add(playerRace = ControlPanel.MakeDecoratedLabel());
    statusPanel.add(curPlayerMoney = ControlPanel.MakeDecoratedLabel());
    statusPanel.add(curPlayerBarter = ControlPanel.MakeDecoratedLabel());
    statusPanel.add(holdInfo = ControlPanel.MakeDecoratedLabel());
    statusPanel.add(numBuys = ControlPanel.MakeDecoratedLabel());
    statusPanel.add(numSells = ControlPanel.MakeDecoratedLabel());

    JPanel vertPanel = ControlPanel.MakeDecoratedPanel();
    vertPanel.setLayout(new BoxLayout(vertPanel,BoxLayout.Y_AXIS));
    add(vertPanel,BorderLayout.CENTER);

    demandItems = ControlPanel.MakeDecoratedPanel();  
    baseItems = ControlPanel.MakeDecoratedPanel();
    playerItems = ControlPanel.MakeDecoratedPanel();
    cupItems = ControlPanel.MakeDecoratedPanel();

    demandItems.setLayout(new BoxLayout(demandItems,BoxLayout.X_AXIS));
    baseItems.setLayout(new BoxLayout(baseItems,BoxLayout.X_AXIS));
    playerItems.setLayout(new BoxLayout(playerItems,BoxLayout.X_AXIS));
    cupItems.setLayout(new BoxLayout(cupItems,BoxLayout.X_AXIS));

    demandItems.setAlignmentX(Component.LEFT_ALIGNMENT);
    baseItems.setAlignmentX(Component.LEFT_ALIGNMENT);
    playerItems.setAlignmentX(Component.LEFT_ALIGNMENT);
    cupItems.setAlignmentX(Component.LEFT_ALIGNMENT);
                       
    vertPanel.add(GetSeparator("Demands: "));
    vertPanel.add(Box.createRigidArea(new Dimension(0,5)));
    vertPanel.add(demandItems);
    vertPanel.add(Box.createRigidArea(new Dimension(0,5)));

    vertPanel.add(GetSeparator("Items for Purchase:"));
    vertPanel.add(Box.createRigidArea(new Dimension(0,5)));
    vertPanel.add(baseItems);
    vertPanel.add(Box.createRigidArea(new Dimension(0,5)));

    vertPanel.add(GetSeparator("Items Player can Sell:"));
    vertPanel.add(Box.createRigidArea(new Dimension(0,5)));
    vertPanel.add(playerItems);
    vertPanel.add(Box.createRigidArea(new Dimension(0,5)));

    vertPanel.add(GetSeparator("Recent Draws From the Cup:"));
    vertPanel.add(Box.createRigidArea(new Dimension(0,5)));
    vertPanel.add(cupItems);
    vertPanel.add(Box.createRigidArea(new Dimension(0,5)));

    JPanel buttonPanel = ControlPanel.MakeDecoratedPanel();
    add(buttonPanel,BorderLayout.SOUTH);
    endButton = new JButton("End Turn");
    buttonPanel.setLayout(new BoxLayout(buttonPanel,BoxLayout.X_AXIS));
    buttonPanel.add(Box.createHorizontalGlue());
    buttonPanel.add(endButton);
    endButton.addActionListener(new ActionListener() 
      { 
        public void actionPerformed(ActionEvent e) 
        { 
          atm.ButtonPressed("ENDTURN");
        }
      }
      );
    endButton.setVisible(false);
  }

  public void rebuild()
  {
    Player curp = pdata.GetCurPlayer();
    boolean iam = pdata.GetIAm() == curp.GetId();
    endButton.setVisible(iam);

    String ssname = board.GetSpaces().get(curp.GetLocation()).GetRegionName();

    SolarSystem ss = tbdata.GetSolarSystems().get(ssname);
    TradeBase tb = ss.GetBase();
    OverrideSpaceData.Override baseOver = osdata.GetOverrides().get(curp.GetLocation());
    int proprietorId = -1;
    if (baseOver != null) proprietorId = baseOver.icon.playerPortId;
    int factoryOwnerId = tb.GetFactoryOwner();

    Vector<Token> paperwork = new Vector<Token>();
    Vector<Token> hull = new Vector<Token>();
    Vector<Token> holds = new Vector<Token>();
    Token ship = Utilities.SplitPlayerTokens(curp,paperwork,hull,holds);

    playerName.setText("Acting Player: "+curp.GetName());
    baseRace.setText("Base Race: "+Utilities.GetRaceOfRaceId(tb.GetRaceId()) + "(" + tb.GetRaceId() + ")");
    proprietor.setText(proprietorId == -1 ? "Unowned Port" : 
                       ("Proprietor: " + pdata.GetPlayer(proprietorId).GetName()));
    factoryOwner.setText(factoryOwnerId == -1 ? "Factory Not Built" : 
                         ("Factory Owner: " + pdata.GetPlayer(factoryOwnerId).GetName()));
    playerRace.setText("Player Race: " + Utilities.GetRaceOfRaceId(curp.GetRace()) + "(" + curp.GetRace() + ")");
    curPlayerMoney.setText("Money: " + curp.GetMoney());
    curPlayerBarter.setText("Barter: " + curp.GetBarter());

    int usedHolds = curp.GetHoldCount();
    String maxHolds = ship == null ? "?" : (""+ship.GetHoldCount());

    holdInfo.setText("Holds: " + usedHolds + "/" + maxHolds);
    
    int nb = curp.GetBuysLeft();
    numBuys.setText( nb == -1 ? "Unlimited buys" : (nb == 0 ? "no buys left" : "1 buy left") );

    int ns = curp.GetSellsLeft();
    numSells.setText( ns == -1 ? "Unlimited sells" : (ns == 0 ? "no sells left" : "1 sell left") );

    

    // ---- rubber hits the road... all of the buy/sell entities are produced here.
    demandItems.removeAll();
    baseItems.removeAll();
    playerItems.removeAll();
    cupItems.removeAll();

    int xsep = 2;

    TradeItemFactory fac = new TradeItemFactory(atm,tb,curp,iam,
                                                proprietorId == -1 ? null : pdata.GetPlayer(proprietorId).GetName(),
                                                factoryOwnerId == -1 ? null : pdata.GetPlayer(factoryOwnerId).GetName(),
                                                ship);

      
    // trade base tokens
    Vector<TokenHolder.DupToken> tbts = tb.GetDupTokenList();
    for (TokenHolder.DupToken dt : tbts)
    {
      if (dt.token.GetSizeClass().equals("demand"))
      {
        JPanel jp = fac.get(dt.token,TradeItemFactory.TradeType.BASETYPE,dt.count);
        if (jp == null) continue;
        demandItems.add(Box.createRigidArea(new Dimension(xsep,0)));
        demandItems.add(jp);
      }
      else
      {
        JPanel jp = fac.get(dt.token,TradeItemFactory.TradeType.BASETYPE,dt.count);
        if (jp == null) continue;
        baseItems.add(Box.createRigidArea(new Dimension(xsep,0)));
        baseItems.add(jp);
      }
    }

    Vector<TokenHolder.DupToken> pdts = curp.GetDupTokenList();
    for (TokenHolder.DupToken dt : pdts)
    {
      JPanel jp = fac.get(dt.token,TradeItemFactory.TradeType.PLAYERTYPE,dt.count);
      if (jp == null) continue;
      playerItems.add(Box.createRigidArea(new Dimension(xsep,0)));
      playerItems.add(jp);
    }

    
    int cupCount = 0;
    for (CupOut co : cupouts)
    {
      cupItems.add(Box.createRigidArea(new Dimension(xsep,0)));

      JPanel coI = ControlPanel.MakeDecoratedPanel();
      coI.setLayout(new BoxLayout(coI,BoxLayout.Y_AXIS));
      coI.setAlignmentY(Component.TOP_ALIGNMENT);
      JLabel iconLabel = new JLabel(co.ii);
      iconLabel.setAlignmentX(Component.CENTER_ALIGNMENT);
      coI.add(iconLabel);
      coI.add(Box.createRigidArea(new Dimension(0,5)));
      JLabel raceLabel = ControlPanel.MakeDecoratedLabel(co.race);
      raceLabel.setAlignmentX(Component.CENTER_ALIGNMENT);
      coI.add(raceLabel);
      cupItems.add(coI);

      if (cupCount > 4) break;
      ++cupCount;
    }

  }    

  public void HandleEvent(GameEvent ev)
  {
    if (ev.GetVarValue("eventname").equals("NEWSTATE"))
    {
      if (ev.GetVarValue("statename").equals("TradePhase"))
      {
        setVisible(true);
        rebuild();
        palp.reconstrain(palphandle,this);
      }
      else
      {
        setVisible(false);
      }
    }
    else if (ev.GetVarValue("eventname").equals("ADDTOTRADEBASE"))
    {
      if (ev.GetVarValue("ADDTOTRADEBASE.FromCup").equals("1"))
      {
        CupOut co = new CupOut(ev.GetVarValue("ADDTOTRADEBASE.TokenEncoding"),
                               ev.GetVarValue("ADDTOTRADEBASE.RaceId"));
        cupouts.add(0,co);
      }
    }
  }

  public void Reset()
  {
    setVisible(false);
    cupouts = new Vector<CupOut>();
  }
}
