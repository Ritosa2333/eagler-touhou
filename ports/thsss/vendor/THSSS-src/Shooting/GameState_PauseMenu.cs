using System.Drawing;
using SlimDX.Direct3D9;

namespace Shooting;

internal class GameState_PauseMenu : BaseMenu, IGameState
{
	private float blurLevel;

	public GameState_PauseMenu(GlobalDataPackage GlobalData)
		: base(GlobalData)
	{
		BackgroundLight = 255;
	}

	public GameState_PauseMenu()
	{
	}

	public override void Init()
	{
		StageData.MenuGroupList.Add(new MenuGroup_Pause(StageData));
		KClass.Hex2Key(0);
	}

	public override void UpdateData()
	{
		base.UpdateData();
		if (TimeMain < 50)
		{
			blurLevel = (float)TimeMain * 25f / 500000f;
		}
		else
		{
			blurLevel = 0.0025f;
		}
	}

	public override void Render()
	{
		//IL_00a3: Unknown result type (might be due to invalid IL or missing references)
		//IL_00b5: Unknown result type (might be due to invalid IL or missing references)
		//IL_0116: Unknown result type (might be due to invalid IL or missing references)
		//IL_0127: Unknown result type (might be due to invalid IL or missing references)
		//IL_01ee: Unknown result type (might be due to invalid IL or missing references)
		//IL_01fd: Unknown result type (might be due to invalid IL or missing references)
		GlobalData.ScreenTexMan.Begin();
		GlobalData.LastState.Render();
		GlobalData.ScreenTexMan.End();
		SpriteMain.Begin((SpriteFlags)16);
		GlobalData.EffectMain = GlobalData.EffectDictionary["Blur"];
		GlobalData.EffectMain.Technique = EffectHandle.op_Implicit("technique1");
		GlobalData.EffectMain.Begin();
		((BaseEffect)GlobalData.EffectMain).SetValue<float>(EffectHandle.op_Implicit("BlurLevel"), blurLevel);
		GlobalData.EffectMain.BeginPass(0);
		GlobalData.SpriteMain.Draw2D(GlobalData.ScreenTexMan.RenderTexture, new Point(0, 0), 0f, new PointF(0f, 0f), Color.White);
		SpriteMain.End();
		GlobalData.EffectMain.EndPass();
		GlobalData.EffectMain.End();
		SpriteMain.Begin((SpriteFlags)16);
		Background.Show(Active: false);
		if (GlobalData.LastState is BaseStage)
		{
			((BaseStage)GlobalData.LastState).ShowInterface();
			((BaseStage)GlobalData.LastState).InterfaceList.ForEach((BaseObject x) =>
			{
				x.Show();
			});
		}
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
