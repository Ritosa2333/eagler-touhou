using System.Collections.Generic;
using System.Drawing;

namespace Shooting;

internal class MenuOption : BaseMenuGroup
{
	private BaseNum BGMvolumn;

	private BaseNum SEvolumn;

	public MenuOption(StageDataPackage StageData)
		: base(StageData)
	{
		MenuSelectIndex = 0;
		MenuItemList = new List<BaseMenuItem>
		{
			new BaseMenuItem(StageData, "Menu_BGM_Vol"),
			new BaseMenuItem(StageData, "Menu_SE_Vol"),
			new BaseMenuItem(StageData, "Menu_Quit")
		};
		int num = 220;
		int num2 = 220;
		int num3 = -200;
		foreach (BaseMenuItem menuItem in MenuItemList)
		{
			menuItem.DestPoint = new PointF(num, num2);
			menuItem.Position = new PointF(num + num3, 140f);
			num2 += 24;
			num3 -= 50;
		}
		BGMvolumn = new BaseNum(StageData, new PointF(MenuItemList[0].DestPoint.X + 130f, MenuItemList[0].DestPoint.Y + 16f))
		{
			Num = BGM_Player.Volume,
			Count = 3
		};
		SEvolumn = new BaseNum(StageData, new PointF(MenuItemList[1].DestPoint.X + 130f, MenuItemList[1].DestPoint.Y + 16f))
		{
			Num = StageData.GlobalData.SEVolume,
			Count = 3
		};
		MenuItemList[MenuSelectIndex].Selected = true;
	}

	public override void ProcessKeys()
	{
		base.ProcessKeys();
		if (KClass.Key_Z && LastZ == 0)
		{
			switch (MenuItemList[MenuSelectIndex].Name)
			{
			case "Menu_Quit":
				OnRemoveMenu = TimeMain + 20;
				TransparentVelocity = -15f;
				foreach (BaseMenuItem menuItem in MenuItemList)
				{
					menuItem.DestPoint = new PointF(60f, 160f);
					menuItem.OnRemove = true;
				}
				break;
			default:
				StageData.StateSwitchData = new StateSwitchDataPackage
				{
					NextState = "此功能尚未开通",
					NeedInit = true,
					SDPswitch = StageData
				};
				break;
			case "Menu_BGM_Vol":
			case "Menu_SE_Vol":
				break;
			}
			StageData.SoundPlay("se_ok00.wav");
		}
		if ((KClass.Key_X || KClass.Key_ESC) && LastX == 0)
		{
			MenuSelectIndex = MenuItemList.Count - 1;
			SelectItemChanged();
			StageData.SoundPlay("se_cancel00.wav");
		}
		string name;
		if (KClass.ArrowRight && (LastRight == 0 || LastRight > 30))
		{
			name = MenuItemList[MenuSelectIndex].Name;
			if (!(name == "Menu_BGM_Vol"))
			{
				if (name == "Menu_SE_Vol")
				{
					foreach (XAudio2_Player value in StageData.SoundDictionary.Values)
					{
						value.Volume += 5;
						SEvolumn.Num = value.Volume;
					}
					StageData.SoundPlay("se_ok00.wav");
				}
			}
			else
			{
				BGM_Player.Volume += 5;
				BGMvolumn.Num = BGM_Player.Volume;
			}
			LastSelectTime = TimeMain;
			StageData.GlobalData.BGMVolume = BGM_Player.Volume;
			StageData.GlobalData.SEVolume = SEvolumn.Num;
		}
		if (!KClass.ArrowLeft || (LastLeft != 0 && LastLeft <= 30))
		{
			return;
		}
		name = MenuItemList[MenuSelectIndex].Name;
		if (!(name == "Menu_BGM_Vol"))
		{
			if (name == "Menu_SE_Vol")
			{
				foreach (XAudio2_Player value2 in StageData.SoundDictionary.Values)
				{
					value2.Volume -= 5;
					SEvolumn.Num = value2.Volume;
				}
				StageData.SoundPlay("se_ok00.wav");
			}
		}
		else
		{
			BGM_Player.Volume -= 5;
			BGMvolumn.Num = BGM_Player.Volume;
		}
		LastSelectTime = TimeMain;
		StageData.GlobalData.BGMVolume = BGM_Player.Volume;
		StageData.GlobalData.SEVolume = SEvolumn.Num;
	}

	public override void Show()
	{
		base.Show();
		BGMvolumn.Show();
		SEvolumn.Show();
	}
}
