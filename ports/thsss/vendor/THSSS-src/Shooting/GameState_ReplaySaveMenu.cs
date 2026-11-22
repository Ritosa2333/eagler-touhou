using System.Drawing;
using SlimDX.Direct3D9;

namespace Shooting;

internal class GameState_ReplaySaveMenu : BaseMenu, IGameState
{
	public GameState_ReplaySaveMenu(GlobalDataPackage GlobalData)
		: base(GlobalData)
	{
		BackgroundLight = 255;
	}

	public GameState_ReplaySaveMenu()
	{
	}

	public override void Init()
	{
		StageData.MenuGroupList.Add(new MenuGroup_ReplaySaver(StageData, new PointF(128f, 16f)));
		StageData.MenuGroupList.Add(new MenuGroup_ScoreSaver(StageData, new PointF(36f, 16f)));
		KClass.Hex2Key(0);
	}

	public override void Render()
	{
		//IL_0055: Unknown result type (might be due to invalid IL or missing references)
		//IL_0064: Unknown result type (might be due to invalid IL or missing references)
		SpriteMain.Begin((SpriteFlags)16);
		StageData.MenuGroupList[StageData.MenuGroupList.Count - 1].Show();
		SpriteMain.End();
		SpriteMain.Begin((SpriteFlags)16);
		DeviceMain.SetRenderState((RenderState)19, 5);
		DeviceMain.SetRenderState((RenderState)20, 2);
		EffectList.ForEach((BaseEffect x) =>
		{
			x.Show();
		});
		SpriteMain.End();
	}

	public override void BGM_Pause()
	{
	}

	public override void BGM_Resume()
	{
	}
}
