using System.Drawing;
using SlimDX.Direct3D9;

namespace Shooting;

public class BackgroundParticle3D2 : BaseParticle3D
{
	public BackgroundParticle3D2(StageDataPackage StageData, string textureName, PointF OriginalPosition, float Velocity, double Direction, int LifeTime)
		: base(StageData, textureName, OriginalPosition, Velocity, Direction)
	{
		Background3D.BackgroundParticleList.Add(this);
		TransparentValueF = 0f;
		base.LifeTime = LifeTime;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (0 < Time && Time < LifeTime / 3)
		{
			TransparentValueF += MaxTransparent * 3 / LifeTime;
		}
		else if (LifeTime * 2 / 3 < Time && Time < LifeTime)
		{
			TransparentValueF -= MaxTransparent * 3 / LifeTime;
		}
	}

	public override void Show()
	{
		//IL_0009: Unknown result type (might be due to invalid IL or missing references)
		//IL_0018: Unknown result type (might be due to invalid IL or missing references)
		//IL_0042: Unknown result type (might be due to invalid IL or missing references)
		//IL_0055: Unknown result type (might be due to invalid IL or missing references)
		//IL_0068: Unknown result type (might be due to invalid IL or missing references)
		//IL_007d: Unknown result type (might be due to invalid IL or missing references)
		DeviceMain.SetRenderState((RenderState)28, true);
		DeviceMain.SetRenderState<FogMode>((RenderState)35, (FogMode)3);
		DeviceMain.SetRenderState((RenderState)34, Color.FromArgb(160, 190, 230).ToArgb());
		DeviceMain.SetRenderState((RenderState)36, 0f);
		DeviceMain.SetRenderState((RenderState)37, 400f);
		base.Show();
		DeviceMain.SetRenderState((RenderState)28, false);
	}
}
