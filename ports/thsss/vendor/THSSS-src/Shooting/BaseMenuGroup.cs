using System.Collections.Generic;
using System.Drawing;

namespace Shooting;

public class BaseMenuGroup : BaseObject
{
	public int LastDown = 1;

	public int LastUp = 1;

	public int LastLeft = 1;

	public int LastRight = 1;

	public int LastZ = 1;

	public int LastX = 1;

	public int OnChangeMenu;

	public int OnRemoveMenu;

	public PointF MenuTitlePos1 = new PointF(280f, -30f);

	public PointF MenuTitlePos2 = new PointF(280f, -150f);

	public int MenuSelectIndex { get; set; }

	public int LastSelectTime { get; set; }

	public List<BaseMenuItem> MenuItemList { get; set; }

	public BaseMenuItem MenuTilte { get; set; }

	public BaseMenuGroup()
	{
	}

	public BaseMenuGroup(StageDataPackage StageData)
	{
		base.StageData = StageData;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (MenuItemList.Count <= 0)
		{
			return;
		}
		MenuItemList.ForEach((BaseMenuItem x) =>
		{
			x.Ctrl();
		});
		if (MenuTilte != null)
		{
			if (StageData.MenuGroupList[StageData.MenuGroupList.Count - 1] == this && TimeMain > OnRemoveMenu)
			{
				MenuTilte.DestPoint = MenuTitlePos1;
			}
			else
			{
				MenuTilte.DestPoint = MenuTitlePos2;
			}
			MenuTilte.Ctrl();
			MenuTilte.ColorValue = Color.White;
			MenuTilte.TransparentValueF = 255f;
		}
	}

	public virtual void MenuSelect()
	{
		if (TimeMain - LastSelectTime < 7 || TimeMain < OnChangeMenu)
		{
			return;
		}
		if (TimeMain == OnChangeMenu)
		{
			ProcessZ();
		}
		else if (TimeMain >= OnRemoveMenu)
		{
			if (TimeMain == OnRemoveMenu)
			{
				ProcessX();
				return;
			}
			ProcessKeys();
			LastDown = (KClass.ArrowDown ? (LastDown + 1) : 0);
			LastUp = (KClass.ArrowUp ? (LastUp + 1) : 0);
			LastLeft = (KClass.ArrowLeft ? (LastLeft + 1) : 0);
			LastRight = (KClass.ArrowRight ? (LastRight + 1) : 0);
			LastZ = (KClass.Key_Z ? (LastZ + 1) : 0);
			LastX = ((KClass.Key_X || KClass.Key_ESC) ? (LastX + 1) : 0);
		}
	}

	public virtual void ProcessKeys()
	{
		if (KClass.ArrowDown && (LastDown == 0 || LastDown > 30))
		{
			MenuSelectIndex++;
			StageData.SoundPlay("se_select00.wav");
			SelectItemChanged();
		}
		if (KClass.ArrowUp && (LastUp == 0 || LastUp > 30))
		{
			MenuSelectIndex--;
			StageData.SoundPlay("se_select00.wav");
			SelectItemChanged();
		}
	}

	public virtual void ProcessZ()
	{
	}

	public virtual void ProcessX()
	{
		StageData.MenuGroupList.Remove(this);
		StageData.MenuGroupList[StageData.MenuGroupList.Count - 1].LastSelectTime = TimeMain;
	}

	public virtual void SelectItemChanged()
	{
		if (MenuSelectIndex < 0)
		{
			MenuSelectIndex += MenuItemList.Count;
		}
		else if (MenuSelectIndex > MenuItemList.Count - 1)
		{
			MenuSelectIndex -= MenuItemList.Count;
		}
		LastSelectTime = TimeMain;
		MenuItemList.ForEach((BaseMenuItem x) =>
		{
			x.Selected = false;
		});
		MenuItemList[MenuSelectIndex].Select();
	}

	public override void Show()
	{
		base.Show();
		if (MenuItemList.Count > 0)
		{
			MenuItemList.ForEach((BaseMenuItem x) =>
			{
				x.Show();
			});
			if (MenuTilte != null)
			{
				MenuTilte.Show();
			}
		}
	}
}
