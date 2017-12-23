
package MerchantOfVenus;

import NodeInterfacePackage.*;
import MySwing.LockLayout;
import javax.swing.*;
import java.util.*;
import java.awt.*;


public class MerchantOfVenus implements UserDefinedInterface
{
  Board b;
  PlayerData plData;
  TradeBaseData baseData;
  OverrideSpaceData spaceData;
  DynamicBoardOverlay dbo;
  DiceDisplay dd;
  ControlPanel cp;
  
  public MerchantOfVenus (HashMap<String,String>i_Options, String i_DataLoc, JPanel i_Panel, ActionTransferManager i_atm)
  {
    IconManager.Initialize(i_DataLoc);
    b = new Board(i_DataLoc);
    plData = new PlayerData(i_DataLoc);
    baseData = new TradeBaseData();
    spaceData = new OverrideSpaceData(b);

    BoardIconMaker bim = new BoardIconMaker(b,i_DataLoc);
    ImageIcon ii = bim.GetBoardIcon();

    SpringLayout springs = new SpringLayout();
    i_Panel.setLayout(springs);

    PopAndLockPanel plp = new PopAndLockPanel();
    cp = new ControlPanel(plp,i_DataLoc,i_atm,plData,spaceData,baseData,b);


    JPanel combo = new JPanel(new LockLayout());
    dbo = new DynamicBoardOverlay(i_DataLoc,b,plData,i_atm,cp,spaceData);

    Point ul = new Point(0,0);

    combo.add(dbo,ul);
    combo.add(new JLabel(ii),ul);
    combo.add(new StarPanel(b),ul);



    ScrollWithGlass swg = new ScrollWithGlass(combo,plp);
    i_Panel.add(swg);
    springs.putConstraint(SpringLayout.NORTH,swg,0,SpringLayout.NORTH,i_Panel);
    springs.putConstraint(SpringLayout.SOUTH,swg,0,SpringLayout.SOUTH,i_Panel);
    springs.putConstraint(SpringLayout.EAST,swg,0,SpringLayout.EAST,i_Panel);
    springs.putConstraint(SpringLayout.WEST,swg,0,SpringLayout.WEST,i_Panel);

    new PlayerPops(plp,plData);
    new BasePops(plp,baseData);
    dd = new DiceDisplay(plp,i_DataLoc);
  }

  //UserDefinedInterface Methods

  public void HandleEvent(GameEvent i_event)
  {
    plData.HandleEvent(i_event);
    baseData.HandleEvent(i_event);
    spaceData.HandleEvent(i_event);
    dbo.HandleEvent(i_event);
    dd.HandleEvent(i_event);
    cp.HandleEvent(i_event);
    
  }

  public void Refresh()
  {
  }

  public void Reset()
  {
    plData.Reset();
    baseData.Reset();
    spaceData.Reset();
    dbo.Reset();
    dd.Reset();
    cp.Reset();
  }



}