using SlimDX.Direct3D9;

namespace Shooting;

public class BackgroundUUZtree : BackgroundFix
{
	public BackgroundUUZtree(StageDataPackage StageData, string textureName)
		: base(StageData, textureName)
	{
		TransparentValueF = 150f;
	}

	public override void Show()
	{
		//IL_0026: Unknown result type (might be due to invalid IL or missing references)
		//IL_003a: Unknown result type (might be due to invalid IL or missing references)
		//IL_0051: Unknown result type (might be due to invalid IL or missing references)
		SpriteMain.End();
		SpriteMain.Begin((SpriteFlags)16);
		StageData.DeviceMain.SetRenderState((RenderState)19, 5);
		StageData.DeviceMain.SetRenderState((RenderState)20, 2);
		StageData.DeviceMain.SetRenderState<BlendOperation>((RenderState)171, (BlendOperation)3);
		base.Show();
		base.Show();
		SpriteMain.End();
		SpriteMain.Begin((SpriteFlags)16);
	}
}
