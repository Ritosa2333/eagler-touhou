using System;
using System.Collections.Generic;
using System.Drawing;

namespace Shooting;

internal class MenuGroup_ScoreSaver : BaseMenuGroup
{
	private int NameIndex;

	private string[] PlayerName = new string[8];

	private int HistorySelectIndex = -1;

	private List<BaseMenuItem> ScoreHistorys;

	public MenuGroup_ScoreSaver(StageDataPackage StageData, PointF OriginalPosition)
		: base(StageData)
	{
		base.OriginalPosition = OriginalPosition;
		ScoreHistorys = new List<BaseMenuItem>();
		for (int i = 0; i < 10; i++)
		{
			ScoreHistorys.Add(new DescriptionMenuItem(StageData, $"{i + 1:00}"));
		}
		StageDataPackage stageData = StageData.GlobalData.LastState.StageData;
		for (int j = 0; j < stageData.S_History.Count; j++)
		{
			if (stageData.MyPlane.Score > stageData.S_History[j].Score)
			{
				ScoreHistory sH = new ScoreHistory
				{
					MyPlaneFullName = stageData.MyPlane.FullName,
					Rank = stageData.Difficulty,
					PlayerName = "",
					Score = stageData.MyPlane.Score,
					Date = DateTime.Now.Date.ToString("yyyy'/'MM'/'dd"),
					Time = DateTime.Now.ToShortTimeString(),
					Stage = stageData.CurrentStageName,
					SlowRate = stageData.SlowRate
				};
				for (int num = stageData.S_History.Count - 1; num > j; num--)
				{
					stageData.S_History[num].Copy(stageData.S_History[num - 1]);
				}
				stageData.S_History[j].Copy(sH);
				HistorySelectIndex = j;
				break;
			}
		}
		float x = OriginalPosition.X + 12f;
		float num2 = OriginalPosition.Y + 30f;
		int num3 = 0;
		foreach (BaseMenuItem scoreHistory in ScoreHistorys)
		{
			scoreHistory.Position = new PointF(x, BoundRect.Top + 40);
			scoreHistory.DestPoint = new PointF(x, num2);
			num2 += 17f;
			if (stageData.CurrentStageName.Contains("St"))
			{
				((DescriptionMenuItem)scoreHistory).Description = stageData.S_History[num3++].Data2DrawStringSimple();
			}
			else
			{
				((DescriptionMenuItem)scoreHistory).Description = stageData.S_History[num3++].Data2DrawString();
			}
		}
		if (HistorySelectIndex >= 0)
		{
			ScoreHistorys[HistorySelectIndex].Selected = true;
		}
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
		MenuItemList = new List<BaseMenuItem>();
		for (int k = 0; k < 7; k++)
		{
			for (int l = 0; l < 13; l++)
			{
				BaseMenuItem item = new DescriptionMenuItem(StageData, array[k, l])
				{
					OriginalPosition = new PointF(stageData.BoundRect.Left + stageData.BoundRect.Width / 2 - 138, 300f),
					DestPoint = new PointF(stageData.BoundRect.Left + stageData.BoundRect.Width / 2 - 138 + l * 22, 270 + k * 22)
				};
				MenuItemList.Add(item);
			}
		}
		MenuSelectIndex = MenuItemList.Count - 1;
		if (MenuSelectIndex >= 0)
		{
			MenuItemList[MenuSelectIndex].Selected = true;
		}
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
		ScoreHistorys.ForEach((BaseMenuItem x) =>
		{
			x.Ctrl();
		});
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
	}

	public override void ProcessZ()
	{
		if (MenuSelectIndex == MenuItemList.Count - 1)
		{
			OnRemoveMenu = TimeMain + 20;
			TransparentVelocity = -15f;
			foreach (BaseMenuItem scoreHistory in ScoreHistorys)
			{
				scoreHistory.DestPoint = new PointF(OriginalPosition.X + 12f, OriginalPosition.Y + 30f);
				scoreHistory.OnRemove = true;
			}
			StageData.SoundPlay("se_ok00.wav");
		}
		else if (MenuSelectIndex == MenuItemList.Count - 2)
		{
			if (NameIndex > 0)
			{
				NameIndex--;
				PlayerName[NameIndex] = " ";
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
		StageDataPackage stageData = StageData.GlobalData.LastState.StageData;
		if (HistorySelectIndex >= 0)
		{
			stageData.S_History[HistorySelectIndex].PlayerName = text;
			stageData.PData.PlayerName = text;
			if (stageData.CurrentStageName.Contains("St"))
			{
				((DescriptionMenuItem)ScoreHistorys[HistorySelectIndex]).Description = stageData.S_History[HistorySelectIndex].Data2DrawStringSimple();
			}
			else
			{
				((DescriptionMenuItem)ScoreHistorys[HistorySelectIndex]).Description = stageData.S_History[HistorySelectIndex].Data2DrawString();
			}
		}
	}

	public override void Show()
	{
		base.Show();
		if (ScoreHistorys.Count > 0)
		{
			ScoreHistorys.ForEach((BaseMenuItem x) =>
			{
				x.Show();
			});
		}
	}
}
