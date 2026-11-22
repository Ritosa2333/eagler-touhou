using System.Drawing;

namespace Shooting;

internal class GameState_GameSetMenu : GameState_PauseMenu
{
	public GameState_GameSetMenu(GlobalDataPackage GlobalData)
		: base(GlobalData)
	{
	}

	public override void Init()
	{
		StageData.MenuGroupList.Add(new MenuGroup_GameSet(StageData)
		{
			OriginalPosition = new PointF(36f, 16f)
		});
		KClass.Hex2Key(0);
	}
}
