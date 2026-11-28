using System.Collections.Generic;
using System.Drawing;

namespace Shooting;

internal class MenuGroup_WeaponSelect : BaseMenuGroup
{
	private bool StageSelect;

	public MenuGroup_WeaponSelect(StageDataPackage StageData, bool StageSelect, int PlaneType)
		: base(StageData)
	{
		this.StageSelect = StageSelect;
		MenuSelectIndex = 0;
		if (PlaneType == 1)
		{
			MenuItemList = new List<BaseMenuItem>();
			MenuItemList.Add(new CharacterMenuItem(StageData, "Weapon_PlaneA")
			{
				Scale = 0.7f,
				DestPoint1 = new PointF(270f, 150f),
				DestPoint2 = new PointF(200f, 150f),
				TransparentValueF = 0f
			});
			MenuItemList.Add(new CharacterMenuItem(StageData, "Weapon_PlaneB")
			{
				Scale = 0.7f,
				DestPoint1 = new PointF(270f, 150f),
				DestPoint2 = new PointF(340f, 150f),
				TransparentValueF = 0f
			});
		}
		else
		{
			MenuItemList = new List<BaseMenuItem>
			{
				new CharacterMenuItem(StageData, "Weapon_AyaA")
				{
					Scale = 0.7f,
					DestPoint1 = new PointF(270f, 150f),
					DestPoint2 = new PointF(200f, 150f),
					TransparentValueF = 0f
				}
			};
		}
		MenuItemList[MenuSelectIndex].Selected = true;
		MenuTitlePos1 = new PointF(280f, 30f);
		MenuTitlePos2 = new PointF(280f, -100f);
		MenuTilte = new BaseMenuItem(StageData, "MenuTitle_WeaponSelect")
		{
			Position = MenuTitlePos2
		};
		TransparentValueF = 0f;
		MaxTransparent = 255;
		TransparentVelocity = 5f;
		OriginalPosition = new PointF(BoundRect.Width / 2, BoundRect.Height / 2);
		AngleDegree = 90.0;
	}

	public override void ProcessKeys()
	{
		if (KClass.ArrowLeft && LastLeft == 0)
		{
			MenuSelectIndex++;
			StageData.SoundPlay("se_select00.wav");
			SelectItemChanged();
		}
		if (KClass.ArrowRight && LastRight == 0)
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
			menuItem.Selected = false;
			menuItem.OnRemove = true;
		}
		StageData.SoundPlay("se_cancel00.wav");
	}

	public override void ProcessZ()
	{
		Point point = new Point(192, 398);
		switch (MenuItemList[MenuSelectIndex].Name)
		{
		case "Weapon_AyaA":
			StageData.StateSwitchData.SDPswitch.MyPlane = new MyPlane_Aya(StageData.StateSwitchData.SDPswitch, point);
			break;
		case "Weapon_AyaB":
			StageData.StateSwitchData.SDPswitch.MyPlane = new MyPlane_AyaB(StageData.StateSwitchData.SDPswitch, point);
			break;
		case "Weapon_PlaneA":
			StageData.StateSwitchData.SDPswitch.MyPlane = new AutoPlane(StageData.StateSwitchData.SDPswitch, point);
			break;
		case "Weapon_PlaneB":
			StageData.StateSwitchData.SDPswitch.MyPlane = new MyPlane_PlaneB(StageData.StateSwitchData.SDPswitch, point);
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
		if (!StageSelect)
		{
			StageData.Rep.CreatRpy();
			StageData.StateSwitchData.NextState = "St1";
			StageData.StateSwitchData.SDPswitch.SetReplayInfo(StageData.StateSwitchData.NextState);
		}
		else
		{
			StageData.MenuGroupList.Add(new MenuGroup_StageSelect(StageData));
		}
	}

	public override void Show()
	{
		if (StageData.MenuGroupList[StageData.MenuGroupList.Count - 1] == this)
		{
			base.Show();
		}
	}
}
