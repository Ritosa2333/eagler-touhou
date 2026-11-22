using System;
using System.Collections.Generic;
using System.Drawing;
using Shooting.Planes.Story;

namespace Shooting;

internal class MenuGroup_SCPractice : BaseMenuGroup
{
	private DescriptionBox DescriptionBox;

	private int deltaY = 20;

	public MenuGroup_SCPractice(StageDataPackage StageData)
		: base(StageData)
	{
		MenuSelectIndex = 0;
		MenuItemList = new List<BaseMenuItem>();
		List<SpellCardHistory> list = StageData.PData.SC_History.FindAll((SpellCardHistory a) => a.MyPlaneFullName == "ReimuA");
		for (int num = 0; num < list.Count; num++)
		{
			MenuItemList.Add(new SCMenuItem(StageData, list[num].Index.ToString().PadRight(3))
			{
				Description = (list[num].Rank.ToString().PadLeft(10) + list[num].Recorded.ToString().PadLeft(2) + "/" + list[num].History.ToString().PadLeft(2)).PadLeft(40),
				TxtureObject = StageData.TextureObjectDictionary["欧" + list[num].Name],
				DefaultColor = ((list[num].Recorded > 0) ? Color.SkyBlue : Color.White)
			});
		}
		int num2 = 100;
		int num3 = 100;
		foreach (BaseMenuItem menuItem in MenuItemList)
		{
			menuItem.TransparentValueF = 0f;
			menuItem.Position = new PointF(num2, 90f);
			menuItem.DestPoint = new PointF(num2, num3);
			menuItem.MaxVelocity = 10000f;
			num3 += deltaY;
		}
		MenuItemList[MenuSelectIndex].Selected = true;
		MenuTitlePos1 = new PointF(140f, -30f);
		MenuTitlePos2 = new PointF(140f, -150f);
		MenuTilte = new BaseMenuItem(StageData, "MenuTitle_MusicRoom")
		{
			Position = MenuTitlePos2
		};
		TransparentValueF = 0f;
		TransparentVelocity = 5f;
		TxtureObject = TextureObjectDictionary["MenuBackground"];
		Scale = 0.625f;
		OriginalPosition = new PointF(BoundRect.Width / 2, BoundRect.Height / 2);
		AngleDegree = 90.0;
		ColorValue = Color.SlateBlue;
		DescriptionBox = new DescriptionBox(StageData)
		{
			MaxTransparent = 0,
			OffsetX = 50
		};
	}

	public override void SelectItemChanged()
	{
		base.SelectItemChanged();
		int num = 100;
		int num2 = 100;
		int num3 = (MenuSelectIndex - 8) * deltaY;
		if (num3 < 0)
		{
			num3 = 0;
		}
		foreach (BaseMenuItem menuItem in MenuItemList)
		{
			menuItem.DestPoint = new PointF(num, num2 - num3);
			num2 += deltaY;
		}
	}

	public override void Ctrl()
	{
		base.Ctrl();
		DescriptionBox.Ctrl();
		foreach (BaseMenuItem menuItem in MenuItemList)
		{
			menuItem.TransparentValueF = 400f - Math.Abs(260f - menuItem.Position.Y) * 2f;
		}
	}

	public override void ProcessKeys()
	{
		base.ProcessKeys();
		if (KClass.Key_Z)
		{
			_ = LastZ;
		}
		if ((!KClass.Key_X && !KClass.Key_ESC) || LastX != 0)
		{
			return;
		}
		OnRemoveMenu = TimeMain + 20;
		TransparentVelocity = -15f;
		DescriptionBox.TransparentVelocity = -15f;
		foreach (BaseMenuItem menuItem in MenuItemList)
		{
			menuItem.DestPoint = new PointF(100f, 60f);
			menuItem.OnRemove = true;
		}
		StageData.SoundPlay("se_cancel00.wav");
	}

	public override void Show()
	{
		base.Show();
		DescriptionBox.Show();
	}
}
