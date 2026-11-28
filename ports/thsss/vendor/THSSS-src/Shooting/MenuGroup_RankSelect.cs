using System.Collections.Generic;
using System.Drawing;

namespace Shooting;

internal class MenuGroup_RankSelect : BaseMenuGroup
{
	private bool StageSelect;

	public MenuGroup_RankSelect(StageDataPackage StageData, bool StageSelect)
		: base(StageData)
	{
		this.StageSelect = StageSelect;
		MenuSelectIndex = 1;
		MenuItemList = new List<BaseMenuItem>
		{
			new BaseMenuItem(StageData, "Menu_Easy"),
			new BaseMenuItem(StageData, "Menu_Normal"),
			new BaseMenuItem(StageData, "Menu_Hard"),
			new BaseMenuItem(StageData, "Menu_Lunatic")
		};
		int num = 220 + MenuSelectIndex * 150;
		int num2 = 200 - MenuSelectIndex * 100;
		int num3 = -200;
		int num4 = 0;
		foreach (BaseMenuItem menuItem in MenuItemList)
		{
			menuItem.Position = new PointF(num + num3, 48f);
			menuItem.DestPoint = new PointF(num, num2);
			menuItem.Scale = 0.8f;
			menuItem.Vibrateable = false;
			menuItem.MaxVelocity = 30f;
			num2 += 100;
			num -= 150;
			num3 -= 100;
			num4++;
		}
		MenuItemList[MenuSelectIndex].Selected = true;
		MenuTitlePos1 = new PointF(0f, 0f);
		MenuTitlePos2 = new PointF(0f, -150f);
		MenuTilte = new BaseMenuItem(StageData, "MenuTitle_RankSelect")
		{
			Position = MenuTitlePos2
		};
		TransparentValueF = 0f;
		TransparentVelocity = 20f;
		TxtureObject = TextureObjectDictionary["MenuBackground"];
		OriginalPosition = new PointF(BoundRect.Width / 2, BoundRect.Height / 2);
		AngleDegree = 90.0;
		Scale = 0.625f;
		MaxScale = 0.75f;
		ScaleVelocity = 0.005f;
		DestPoint = new PointF(BoundRect.Width / 2 + 30 * (MenuSelectIndex - 1), BoundRect.Height / 2 - 20 * (MenuSelectIndex - 1));
		Velocity = 4f;
		AngleWithDirection = false;
	}

	public override void SelectItemChanged()
	{
		base.SelectItemChanged();
		int num = 220 + MenuSelectIndex * 150;
		int num2 = 200 - MenuSelectIndex * 100;
		foreach (BaseMenuItem menuItem in MenuItemList)
		{
			menuItem.DestPoint = new PointF(num, num2);
			num2 += 100;
			num -= 150;
		}
		DestPoint = new PointF(BoundRect.Width / 2 + 30 * (MenuSelectIndex - 1), BoundRect.Height / 2 - 20 * (MenuSelectIndex - 1));
		Velocity = 4f;
	}

	public override void ProcessKeys()
	{
		if ((KClass.ArrowDown && (LastDown == 0 || LastDown > 30)) || (KClass.ArrowLeft && (LastLeft == 0 || LastLeft > 30)))
		{
			MenuSelectIndex++;
			StageData.SoundPlay("se_select00.wav");
			SelectItemChanged();
		}
		if ((KClass.ArrowUp && (LastUp == 0 || LastUp > 30)) || (KClass.ArrowRight && (LastRight == 0 || LastRight > 30)))
		{
			MenuSelectIndex--;
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
			menuItem.DestPoint = new PointF(0f, 0f);
			menuItem.OnRemove = true;
		}
		StageData.SoundPlay("se_cancel00.wav");
	}

	public override void ProcessZ()
	{
		switch (MenuItemList[MenuSelectIndex].Name)
		{
		case "Menu_Easy":
			StageData.StateSwitchData.SDPswitch.Difficulty = DifficultLevel.Easy;
			break;
		case "Menu_Normal":
			StageData.StateSwitchData.SDPswitch.Difficulty = DifficultLevel.Normal;
			break;
		case "Menu_Hard":
			StageData.StateSwitchData.SDPswitch.Difficulty = DifficultLevel.Hard;
			break;
		case "Menu_Lunatic":
			StageData.StateSwitchData.SDPswitch.Difficulty = DifficultLevel.Lunatic;
			break;
		case "Ultra":
			StageData.StateSwitchData.SDPswitch.Difficulty = DifficultLevel.Ultra;
			break;
		default:
			StageData.StateSwitchData = new StateSwitchDataPackage
			{
				NextState = "此功能尚未开通",
				NeedInit = true,
				SDPswitch = StageData
			};
			break;
		}
		StageData.MenuGroupList.Add(new MenuGroup_PlayerSelect(StageData, StageSelect));
	}

	public override void Show()
	{
		base.Show();
		MenuItemList[MenuSelectIndex].Show();
	}
}
