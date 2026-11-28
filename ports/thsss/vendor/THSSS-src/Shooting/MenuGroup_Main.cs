using System.Collections.Generic;
using System.Drawing;

namespace Shooting;

internal class MenuGroup_Main : BaseMenuGroup
{
	public MenuGroup_Main(StageDataPackage StageData)
		: base(StageData)
	{
		MenuSelectIndex = 0;
		MenuItemList = new List<BaseMenuItem>
		{
			new BaseMenuItem(StageData, "Menu_Start"),
			new BaseMenuItem(StageData, "Menu_Practice"),
			new BaseMenuItem(StageData, "Menu_Replay"),
			new BaseMenuItem(StageData, "Menu_PlayerData"),
			new BaseMenuItem(StageData, "Menu_MusicRoom"),
			new BaseMenuItem(StageData, "Menu_Option"),
			new BaseMenuItem(StageData, "Menu_Manual"),
			new BaseMenuItem(StageData, "Menu_Exit")
		};
		if (StageData.PData.C_History.FindAll((ClearHistory a) => a.NoContinueClearTimes > 0).Count > 0)
		{
			MenuItemList.Insert(1, new BaseMenuItem(StageData, "Menu_ExtraStart"));
		}
		else
		{
			MenuItemList.Insert(1, new BaseMenuItem(StageData, "DMenu_ExtraStart"));
		}
		int num = 50;
		int num2 = 190;
		int num3 = -200;
		foreach (BaseMenuItem menuItem in MenuItemList)
		{
			menuItem.Position = new PointF(num + num3, 140f);
			menuItem.DestPoint = new PointF(num, num2);
			num2 += 24;
			num = num;
			num3 -= 50;
			menuItem.AngleDegree = 0.0;
		}
		MenuItemList[MenuSelectIndex].Selected = true;
	}

	public override void ProcessKeys()
	{
		base.ProcessKeys();
		if (KClass.Key_Z && LastZ == 0)
		{
			MenuItemList[MenuSelectIndex].Click();
			OnChangeMenu = TimeMain + 20;
			StageData.SoundPlay("se_ok00.wav");
		}
		if ((KClass.Key_X || KClass.Key_ESC) && LastX == 0)
		{
			MenuSelectIndex = MenuItemList.Count - 1;
			SelectItemChanged();
			StageData.SoundPlay("se_cancel00.wav");
		}
	}

	public override void ProcessZ()
	{
		switch (MenuItemList[MenuSelectIndex].Name)
		{
		case "Menu_Start":
			StageData.StateSwitchData = new StateSwitchDataPackage
			{
				SDPswitch = new StageDataPackage(StageData.GlobalData),
				NeedInit = true
			};
			StageData.MenuGroupList.Add(new MenuGroup_RankSelect(StageData, StageSelect: false));
			break;
		case "Menu_ExtraStart":
			StageData.StateSwitchData = new StateSwitchDataPackage
			{
				SDPswitch = new StageDataPackage(StageData.GlobalData),
				NeedInit = true
			};
			StageData.MenuGroupList.Add(new MenuGroup_RankSelectEx(StageData));
			break;
		case "Menu_Replay":
			StageData.MenuGroupList.Add(new MenuGroup_ReplayLoader(StageData, new PointF(36f, 16f)));
			break;
		case "Menu_Practice":
			StageData.StateSwitchData = new StateSwitchDataPackage
			{
				SDPswitch = new StageDataPackage(StageData.GlobalData),
				NeedInit = true
			};
			StageData.MenuGroupList.Add(new MenuGroup_RankSelect(StageData, StageSelect: true));
			break;
		case "Menu_SpellPractice":
			StageData.StateSwitchData = new StateSwitchDataPackage
			{
				SDPswitch = new StageDataPackage(StageData.GlobalData),
				NeedInit = true
			};
			StageData.MenuGroupList.Add(new MenuGroup_SCPractice(StageData));
			break;
		case "Menu_MusicRoom":
			StageData.MenuGroupList.Add(new MenuGroup_MusicRoom(StageData));
			break;
		case "Menu_PlayerData":
			StageData.MenuGroupList.Add(new MenuGroup_PlayerData(StageData));
			break;
		case "Menu_Manual":
			StageData.MenuGroupList.Add(new MenuGroup_Manual(StageData));
			break;
		case "Menu_Option":
			StageData.MenuGroupList.Add(new MenuOption(StageData));
			break;
		case "Menu_Exit":
			StageData.StateSwitchData = new StateSwitchDataPackage
			{
				NextState = "GameExit"
			};
			break;
		}
	}
}
