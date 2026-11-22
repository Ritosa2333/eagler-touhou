using System.Collections.Generic;
using System.Drawing;

namespace Shooting;

internal class MenuGroup_StageSelect : BaseMenuGroup
{
	public MenuGroup_StageSelect(StageDataPackage StageData)
		: base(StageData)
	{
		MenuSelectIndex = 0;
		MenuItemList = new List<BaseMenuItem>();
		MenuItemList.Add(new BaseMenuItem(StageData, "Stage01"));
		MenuItemList.Add(new BaseMenuItem(StageData, "Boss01"));
		int practiceLevel = 1;
		StageData.PData.C_History.ForEach((ClearHistory a) =>
		{
			if (a.Rank == StageData.StateSwitchData.SDPswitch.Difficulty && a.MyPlaneFullName == StageData.StateSwitchData.SDPswitch.MyPlane.FullName)
			{
				practiceLevel = a.PracticeLevel;
			}
		});
		if (practiceLevel > 1)
		{
			MenuItemList.Add(new BaseMenuItem(StageData, "Stage02"));
			MenuItemList.Add(new BaseMenuItem(StageData, "Boss02"));
		}
		if (practiceLevel > 2)
		{
			MenuItemList.Add(new BaseMenuItem(StageData, "Stage03"));
			MenuItemList.Add(new BaseMenuItem(StageData, "Boss03"));
		}
		if (practiceLevel > 3)
		{
			MenuItemList.Add(new BaseMenuItem(StageData, "Stage04"));
			MenuItemList.Add(new BaseMenuItem(StageData, "Boss04"));
		}
		if (practiceLevel > 4)
		{
			MenuItemList.Add(new BaseMenuItem(StageData, "Stage05"));
			MenuItemList.Add(new BaseMenuItem(StageData, "Boss05"));
		}
		if (practiceLevel > 5)
		{
			MenuItemList.Add(new BaseMenuItem(StageData, "Stage06"));
			MenuItemList.Add(new BaseMenuItem(StageData, "Boss06"));
		}
		int num = 440;
		int num2 = 120;
		int num3 = 0;
		int num4 = 0;
		foreach (BaseMenuItem menuItem in MenuItemList)
		{
			menuItem.Position = new PointF(num + num3, 120f);
			if (num4 % 2 == 0)
			{
				menuItem.DestPoint = new PointF(num, num2);
			}
			else
			{
				menuItem.DestPoint = new PointF(num + 80, num2);
				num2 += 32;
			}
			num4++;
		}
		MenuItemList[MenuSelectIndex].Selected = true;
		MenuTilte = new BaseMenuItem(StageData, "MenuTitle_StageSelect")
		{
			Position = MenuTitlePos2
		};
		TransparentValueF = 0f;
		MaxTransparent = 210;
		TransparentVelocity = 10f;
		AngleDegree = 90.0;
		Scale = 0.75f;
		OriginalPosition = new PointF(BoundRect.Width / 2 + 30 * (int)(StageData.StateSwitchData.SDPswitch.Difficulty - 1), BoundRect.Height / 2 - 20 * (int)(StageData.StateSwitchData.SDPswitch.Difficulty - 1));
	}

	public override void ProcessKeys()
	{
		if (KClass.ArrowDown && (LastDown == 0 || LastDown > 30))
		{
			MenuSelectIndex += 2;
			StageData.SoundPlay("se_select00.wav");
			SelectItemChanged();
		}
		if (KClass.ArrowUp && (LastUp == 0 || LastUp > 30))
		{
			MenuSelectIndex -= 2;
			StageData.SoundPlay("se_select00.wav");
			SelectItemChanged();
		}
		if (KClass.ArrowLeft && (LastLeft == 0 || LastLeft > 30))
		{
			MenuSelectIndex--;
			StageData.SoundPlay("se_select00.wav");
			SelectItemChanged();
		}
		if (KClass.ArrowRight && (LastRight == 0 || LastRight > 30))
		{
			MenuSelectIndex++;
			StageData.SoundPlay("se_select00.wav");
			SelectItemChanged();
		}
		if (KClass.Key_Z && LastZ == 0)
		{
			MenuItemList[MenuSelectIndex].Click();
			OnChangeMenu = TimeMain + 20;
			StageData.SoundPlay("se_ok00.wav");
		}
		if ((!KClass.Key_X && !KClass.Key_ESC) || LastX != 0)
		{
			return;
		}
		OnRemoveMenu = TimeMain + 20;
		TransparentVelocity = -15f;
		foreach (BaseMenuItem menuItem in MenuItemList)
		{
			menuItem.DestPoint = new PointF(480f, 120f);
			menuItem.OnRemove = true;
		}
		StageData.SoundPlay("se_cancel00.wav");
	}

	public override void ProcessZ()
	{
		StageData.StateSwitchData.SDPswitch.MyPlane.Life = 9;
		StageData.StateSwitchData.SDPswitch.OnPractice = true;
		switch (MenuItemList[MenuSelectIndex].Name)
		{
		case "Stage01":
			StageData.Rep.CreatRpy();
			StageData.StateSwitchData.NextState = "St1";
			StageData.StateSwitchData.SDPswitch.SetReplayInfo(StageData.StateSwitchData.NextState);
			break;
		case "Stage02":
			StageData.StateSwitchData.SDPswitch.MyPlane.Power = 300;
			StageData.Rep.CreatRpy();
			StageData.StateSwitchData.NextState = "St2";
			StageData.StateSwitchData.SDPswitch.SetReplayInfo(StageData.StateSwitchData.NextState);
			break;
		case "Stage03":
			StageData.StateSwitchData.SDPswitch.MyPlane.Power = 400;
			StageData.Rep.CreatRpy();
			StageData.StateSwitchData.NextState = "St3";
			StageData.StateSwitchData.SDPswitch.SetReplayInfo(StageData.StateSwitchData.NextState);
			break;
		case "Stage04":
			StageData.StateSwitchData.SDPswitch.MyPlane.Power = 400;
			StageData.Rep.CreatRpy();
			StageData.StateSwitchData.NextState = "St4";
			StageData.StateSwitchData.SDPswitch.SetReplayInfo(StageData.StateSwitchData.NextState);
			break;
		case "Stage05":
			StageData.StateSwitchData.SDPswitch.MyPlane.Power = 400;
			StageData.Rep.CreatRpy();
			StageData.StateSwitchData.NextState = "St5";
			StageData.StateSwitchData.SDPswitch.SetReplayInfo(StageData.StateSwitchData.NextState);
			break;
		case "Stage06":
			StageData.StateSwitchData.SDPswitch.MyPlane.Power = 400;
			StageData.Rep.CreatRpy();
			StageData.StateSwitchData.NextState = "St6";
			StageData.StateSwitchData.SDPswitch.SetReplayInfo(StageData.StateSwitchData.NextState);
			break;
		case "Boss01":
			StageData.StateSwitchData.SDPswitch.MyPlane.Power = 200;
			StageData.Rep.CreatRpy();
			StageData.StateSwitchData.NextState = "Bs1";
			StageData.StateSwitchData.SDPswitch.SetReplayInfo(StageData.StateSwitchData.NextState);
			break;
		case "Boss02":
			StageData.StateSwitchData.SDPswitch.MyPlane.Power = 300;
			StageData.Rep.CreatRpy();
			StageData.StateSwitchData.NextState = "Bs2";
			StageData.StateSwitchData.SDPswitch.SetReplayInfo(StageData.StateSwitchData.NextState);
			break;
		case "Boss03":
			StageData.StateSwitchData.SDPswitch.MyPlane.Power = 400;
			StageData.Rep.CreatRpy();
			StageData.StateSwitchData.NextState = "Bs3";
			StageData.StateSwitchData.SDPswitch.SetReplayInfo(StageData.StateSwitchData.NextState);
			break;
		case "Boss04":
			StageData.StateSwitchData.SDPswitch.MyPlane.Power = 400;
			StageData.Rep.CreatRpy();
			StageData.StateSwitchData.NextState = "Bs4";
			StageData.StateSwitchData.SDPswitch.SetReplayInfo(StageData.StateSwitchData.NextState);
			break;
		case "Boss05":
			StageData.StateSwitchData.SDPswitch.MyPlane.Power = 400;
			StageData.Rep.CreatRpy();
			StageData.StateSwitchData.NextState = "Bs5";
			StageData.StateSwitchData.SDPswitch.SetReplayInfo(StageData.StateSwitchData.NextState);
			break;
		case "Boss06":
			StageData.StateSwitchData.SDPswitch.MyPlane.Power = 400;
			StageData.Rep.CreatRpy();
			StageData.StateSwitchData.NextState = "Bs6";
			StageData.StateSwitchData.SDPswitch.SetReplayInfo(StageData.StateSwitchData.NextState);
			break;
		case "StageEx":
			StageData.StateSwitchData.SDPswitch.MyPlane.Power = 400;
			StageData.Rep.CreatRpy();
			StageData.StateSwitchData.NextState = "Stage_test";
			StageData.StateSwitchData.SDPswitch.SetReplayInfo(StageData.StateSwitchData.NextState);
			break;
		}
	}
}
