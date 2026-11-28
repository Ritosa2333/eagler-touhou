using System.Drawing;

namespace Shooting;

internal class GameState_GameOverMenu : GameState_PauseMenu
{
	public GameState_GameOverMenu(GlobalDataPackage GlobalData)
		: base(GlobalData)
	{
	}

	public override void Init()
	{
		StageData.MenuGroupList.Add(new MenuGroup_GameOver(StageData)
		{
			OriginalPosition = new PointF(36f, 16f)
		});
		StageDataPackage stageData = StageData.GlobalData.LastState.StageData;
		if (!stageData.OnReplay && !stageData.OnPractice && stageData.MyPlane.Score > stageData.S_History[9].Score)
		{
			StageData.MenuGroupList.Add(new MenuGroup_ScoreSaver(StageData, new PointF(36f, 16f)));
		}
		KClass.Hex2Key(0);
	}
}
