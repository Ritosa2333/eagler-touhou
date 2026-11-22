using System.Drawing;
using System.IO;

namespace Shooting;

internal class MenuGroup_ReplayLoader : MenuGroup_ReplaySaver
{
	private const string NullDescription = "------- -------- ----- ------- ------- --- ----";

	public MenuGroup_ReplayLoader(StageDataPackage StageData, PointF OriginalPosition)
		: base(StageData, OriginalPosition)
	{
		TransparentValueF = 0f;
		TransparentVelocity = 5f;
		TxtureObject = TextureObjectDictionary["MenuBackground"];
		Scale = 0.625f;
		base.OriginalPosition = new PointF(BoundRect.Width / 2, BoundRect.Height / 2);
		AngleDegree = 90.0;
		ColorValue = Color.SkyBlue;
	}

	public override string SetDescroption(string fileName)
	{
		if (File.Exists(fileName))
		{
			try
			{
				ReplayInfo replayInfo = Replay.ReadTitle(fileName);
				if (replayInfo.Version != "ver 1.00")
				{
					return "------- -------- ----- ------- ------- --- ----";
				}
				string text = null;
				if (replayInfo != null)
				{
					text += ((replayInfo.PlayerName == null) ? "        " : replayInfo.PlayerName.PadRight(8));
					text += ((replayInfo.Date == null) ? "        " : replayInfo.Date.PadRight(9));
					text += ((replayInfo.Time == null) ? "        " : replayInfo.Time.PadRight(6));
					text += ((replayInfo.MyPlaneName == null) ? "               " : replayInfo.MyPlaneName.PadRight(8));
					text += replayInfo.Rank.ToString().PadRight(8);
					text += ((replayInfo.LastStage == null) ? "   " : replayInfo.LastStage.PadRight(4));
					text += ((replayInfo.SlowRate == null) ? "   " : replayInfo.SlowRate.PadRight(4));
				}
				return text;
			}
			catch
			{
				return "------- -------- ----- ------- ------- --- ----";
			}
		}
		return "------- -------- ----- ------- ------- --- ----";
	}

	public override void ProcessKeys2()
	{
		if (KClass.Key_Z && LastZ == 0)
		{
			MenuItemList[MenuSelectIndex].Click();
			OnChangeMenu = TimeMain + 1;
		}
		if ((!KClass.Key_X && !KClass.Key_ESC) || LastX != 0)
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
		if (((DescriptionMenuItem)MenuItemList[MenuSelectIndex]).Description != "------- -------- ----- ------- ------- --- ----")
		{
			StageData.SoundPlay("se_ok00.wav");
			StageData.MenuGroupList.Add(new MenuGroup_ReplayStageSelect(StageData, new PointF(200f, 150f), (DescriptionMenuItem)MenuItemList[MenuSelectIndex]));
		}
	}
}
