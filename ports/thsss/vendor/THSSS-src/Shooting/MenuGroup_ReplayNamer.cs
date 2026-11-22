using System;
using System.Collections.Generic;
using System.Drawing;

namespace Shooting;

internal class MenuGroup_ReplayNamer : BaseMenuGroup
{
	private int NameIndex;

	private string[] PlayerName = new string[8];

	private string RepDescription;

	private DescriptionMenuItem RepMenuItem;

	public MenuGroup_ReplayNamer(StageDataPackage StageData, PointF OriginalPosition, DescriptionMenuItem RepMenuItem)
		: base(StageData)
	{
		this.RepMenuItem = RepMenuItem;
		MenuItemList = new List<BaseMenuItem>();
		StageDataPackage stageData = StageData.GlobalData.LastState.StageData;
		string[,] array = new string[7, 13]
		{
			{
				"A", "B", "C", "D", "E", "F", "G", "H", "I", "J",
				"K", "L", "M"
			},
			{
				"N", "O", "P", "Q", "R", "S", "T", "U", "V", "W",
				"X", "Y", "Z"
			},
			{
				"a", "b", "c", "d", "e", "f", "g", "h", "i", "j",
				"k", "l", "m"
			},
			{
				"n", "o", "p", "q", "r", "s", "t", "u", "v", "w",
				"x", "y", "z"
			},
			{
				"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
				"+", "-", "="
			},
			{
				".", ",", "!", "?", "@", ":", ";", "[", "]", "(",
				")", "_", "/"
			},
			{
				"{", "}", "|", "~", "^", "#", "$", "%", "&", "*",
				" ", "BS", "OK"
			}
		};
		for (int i = 0; i < 7; i++)
		{
			for (int j = 0; j < 13; j++)
			{
				BaseMenuItem item = new DescriptionMenuItem(StageData, array[i, j])
				{
					OriginalPosition = new PointF(stageData.BoundRect.Left + stageData.BoundRect.Width / 2 - 138, 300f),
					DestPoint = new PointF(stageData.BoundRect.Left + stageData.BoundRect.Width / 2 - 138 + j * 22, 270 + i * 22)
				};
				MenuItemList.Add(item);
			}
		}
		MenuSelectIndex = MenuItemList.Count - 1;
		ReplayInfo repInfo = StageData.GlobalData.LastState.StageData.RepInfo;
		RepDescription += DateTime.Now.Date.ToString("yy'/'MM'/'dd").PadRight(9);
		RepDescription += repInfo.MyPlaneName.PadRight(7);
		switch (repInfo.Rank)
		{
		case DifficultLevel.Easy:
			RepDescription += "E  ";
			break;
		case DifficultLevel.Normal:
			RepDescription += "N  ";
			break;
		case DifficultLevel.Hard:
			RepDescription += "H  ";
			break;
		case DifficultLevel.Lunatic:
			RepDescription += "L  ";
			break;
		case DifficultLevel.Ultra:
			RepDescription += "U  ";
			break;
		case DifficultLevel.Extra:
			RepDescription += "EX ";
			break;
		}
		RepDescription += ((repInfo.LastStage == null) ? "   " : repInfo.LastStage.PadRight(4));
		MenuItemList[MenuSelectIndex].Selected = true;
		MenuTilte = new DescriptionMenuItem(StageData, RepMenuItem.Name)
		{
			Description = "        " + RepDescription,
			OriginalPosition = RepMenuItem.OriginalPosition,
			Selected = true
		};
		MenuTitlePos1 = new PointF(OriginalPosition.X, OriginalPosition.Y);
		MenuTitlePos2 = RepMenuItem.OriginalPosition;
		char[] array2 = stageData.PData.PlayerName.ToCharArray();
		while (NameIndex < array2.Length)
		{
			PlayerName[NameIndex] = array2[NameIndex].ToString();
			NameIndex++;
		}
		ShowPlayerName();
	}

	public override void ProcessKeys()
	{
		if (KClass.ArrowDown && (LastDown == 0 || LastDown > 30))
		{
			MenuSelectIndex += 13;
			StageData.SoundPlay("se_select00.wav");
			SelectItemChanged();
		}
		if (KClass.ArrowUp && (LastUp == 0 || LastUp > 30))
		{
			MenuSelectIndex -= 13;
			StageData.SoundPlay("se_select00.wav");
			SelectItemChanged();
		}
		if (KClass.ArrowLeft && (LastLeft == 0 || LastLeft > 30))
		{
			MenuSelectIndex--;
			if ((MenuSelectIndex + 1) % 13 == 0)
			{
				MenuSelectIndex += 13;
			}
			StageData.SoundPlay("se_select00.wav");
			SelectItemChanged();
		}
		if (KClass.ArrowRight && (LastRight == 0 || LastRight > 30))
		{
			MenuSelectIndex++;
			if (MenuSelectIndex % 13 == 0)
			{
				MenuSelectIndex -= 13;
			}
			StageData.SoundPlay("se_select00.wav");
			SelectItemChanged();
		}
		if (KClass.Key_Z && LastZ == 0)
		{
			MenuItemList[MenuSelectIndex].Click();
			OnChangeMenu = TimeMain + 1;
		}
		if (KClass.Key_X && LastX == 0)
		{
			if (NameIndex > 0)
			{
				NameIndex--;
				PlayerName[NameIndex] = null;
			}
			StageData.SoundPlay("se_cancel00.wav");
			ShowPlayerName();
		}
		if (!KClass.Key_ESC)
		{
			return;
		}
		OnRemoveMenu = TimeMain + 20;
		TransparentVelocity = -15f;
		foreach (BaseMenuItem menuItem in MenuItemList)
		{
			menuItem.DestPoint = new PointF(MenuItemList[0].OriginalPosition.X, MenuItemList[0].OriginalPosition.Y);
			menuItem.OnRemove = true;
		}
		StageData.SoundPlay("se_cancel00.wav");
	}

	public override void ProcessZ()
	{
		if (MenuSelectIndex == MenuItemList.Count - 1)
		{
			ReplayInfo repInfo = StageData.GlobalData.LastState.StageData.RepInfo;
			repInfo.PlayerName = null;
			for (int i = 0; i < 7; i++)
			{
				repInfo.PlayerName += PlayerName[i];
			}
			StageData.Rep.SaveRpy(".\\Replay\\thSSS_" + RepMenuItem.Name + ".rpy", repInfo);
			RepMenuItem.Description = ((DescriptionMenuItem)MenuTilte).Description;
			OnRemoveMenu = TimeMain + 20;
			TransparentVelocity = -15f;
			foreach (BaseMenuItem menuItem in MenuItemList)
			{
				menuItem.DestPoint = new PointF(MenuItemList[0].OriginalPosition.X, MenuItemList[0].OriginalPosition.Y);
				menuItem.OnRemove = true;
			}
			StageData.SoundPlay("se_extend.wav");
		}
		else if (MenuSelectIndex == MenuItemList.Count - 2)
		{
			if (NameIndex > 0)
			{
				NameIndex--;
				PlayerName[NameIndex] = null;
			}
			StageData.SoundPlay("se_cancel00.wav");
		}
		else
		{
			PlayerName[NameIndex] = MenuItemList[MenuSelectIndex].Name;
			StageData.SoundPlay("se_ok00.wav");
			if (NameIndex < 7)
			{
				NameIndex++;
			}
		}
		ShowPlayerName();
	}

	private void ShowPlayerName()
	{
		string text = null;
		for (int i = 0; i < 7; i++)
		{
			text += PlayerName[i];
		}
		((DescriptionMenuItem)MenuTilte).Description = text.PadRight(8) + RepDescription;
		StageData.GlobalData.LastState.StageData.PData.PlayerName = text;
	}
}
