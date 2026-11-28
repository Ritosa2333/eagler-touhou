using System.Collections.Generic;
using System.Drawing;
using System.IO;

namespace Shooting;

internal class MenuGroup_ReplaySaver : BaseMenuGroup
{
	public MenuGroup_ReplaySaver(StageDataPackage StageData, PointF OriginalPosition)
		: base(StageData)
	{
		base.OriginalPosition = OriginalPosition;
		MenuSelectIndex = 0;
		MenuItemList = new List<BaseMenuItem>();
		for (int i = 0; i < 25; i++)
		{
			MenuItemList.Add(new DescriptionMenuItem(StageData, $"{i + 1:00}"));
		}
		float x = OriginalPosition.X + 12f;
		float num = OriginalPosition.Y + 12f;
		foreach (BaseMenuItem menuItem in MenuItemList)
		{
			menuItem.Position = new PointF(x, BoundRect.Top + 10);
			menuItem.DestPoint = new PointF(x, num);
			num += 17f;
			((DescriptionMenuItem)menuItem).Description = SetDescroption(".\\Replay\\thSSS_" + menuItem.Name + ".rpy");
		}
		MenuItemList[MenuSelectIndex].Selected = true;
	}

	public virtual string SetDescroption(string fileName)
	{
		string result = "------- -------- ------ -- --- ";
		if (File.Exists(fileName))
		{
			try
			{
				ReplayInfo replayInfo = Replay.ReadTitle(fileName);
				if (replayInfo.Version != "ver 1.00")
				{
					return result;
				}
				string text = null;
				if (replayInfo != null)
				{
					text += ((replayInfo.PlayerName == null) ? "        " : replayInfo.PlayerName.PadRight(8));
					text += ((replayInfo.Date == null) ? "       " : replayInfo.Date.PadRight(9));
					text += ((replayInfo.MyPlaneName == null) ? "               " : replayInfo.MyPlaneName.PadRight(7));
					switch (replayInfo.Rank)
					{
					case DifficultLevel.Easy:
						text += "E  ";
						break;
					case DifficultLevel.Normal:
						text += "N  ";
						break;
					case DifficultLevel.Hard:
						text += "H  ";
						break;
					case DifficultLevel.Lunatic:
						text += "L  ";
						break;
					case DifficultLevel.Ultra:
						text += "U  ";
						break;
					case DifficultLevel.Extra:
						text += "EX ";
						break;
					}
					text += ((replayInfo.LastStage == null) ? "   " : replayInfo.LastStage.PadRight(4));
				}
				return text;
			}
			catch
			{
				return result;
			}
		}
		return result;
	}

	public override void ProcessKeys()
	{
		base.ProcessKeys();
		ProcessKeys2();
	}

	public virtual void ProcessKeys2()
	{
		if (KClass.Key_Z && LastZ == 0)
		{
			MenuItemList[MenuSelectIndex].Click();
			OnChangeMenu = TimeMain + 1;
		}
		if ((KClass.Key_X || KClass.Key_ESC) && LastX == 0)
		{
			StageData.SoundPlay("se_cancel00.wav");
			if (StageData.Rep.CanRead)
			{
				ReplayInfo repInfo = StageData.GlobalData.LastState.StageData.RepInfo;
				repInfo.PlayerName = "autosave";
				StageData.Rep.SaveRpy(".\\Replay\\AutoSave.rpy", repInfo);
			}
			StageData.StateSwitchData = new StateSwitchDataPackage
			{
				NextState = "MainMenu",
				NeedInit = true,
				SDPswitch = new StageDataPackage(StageData.GlobalData)
			};
			StageData.Rep.CloseRpy();
		}
	}

	public override void ProcessZ()
	{
		if (StageData.Rep.CanRead)
		{
			StageData.SoundPlay("se_ok00.wav");
			StageData.GlobalData.LastState.StageData.SetReplayStageInfo();
			StageData.MenuGroupList.Add(new MenuGroup_ReplayNamer(StageData, MenuItemList[0].OriginalPosition, (DescriptionMenuItem)MenuItemList[MenuSelectIndex]));
		}
	}
}
