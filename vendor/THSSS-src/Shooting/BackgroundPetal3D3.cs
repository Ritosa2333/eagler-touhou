using System;
using System.Drawing;
using SlimDX;

namespace Shooting;

internal class BackgroundPetal3D3 : BaseParticle3D
{
	public BackgroundPetal3D3(StageDataPackage StageData)
	{
		base.StageData = StageData;
		Background3D.BackgroundParticleList.Add(this);
	}

	public override void Ctrl()
	{
		Time++;
		Shoot();
	}

	public override void Shoot()
	{
		//IL_0173: Unknown result type (might be due to invalid IL or missing references)
		if (Time % 4 == 0)
		{
			for (int i = 0; i < 3; i++)
			{
				new BackgroundParticle3D2(StageData, "Petal0" + Ran.Next(4), new PointF(StageData.Ran.Next(0, BoundRect.Width), StageData.Ran.Next(-150, (Time > 600) ? 100 : 300)), StageData.Ran.Next(3, 6), Math.PI / 2.0 + (double)((float)StageData.Ran.Next(-4, 3) / 10f), 250)
				{
					Angle = (float)StageData.Ran.Next(0, 360) * (float)Math.PI / 180f,
					AngularVelocity3D = (float)StageData.Ran.Next(10) / 100f,
					Scale = (float)StageData.Ran.Next(4, 6) / 10f,
					RotatingAxis = new Vector3((float)StageData.Ran.Next(-100, 100), (float)StageData.Ran.Next(-100, 100), (float)StageData.Ran.Next(-100, 100)),
					MaxTransparent = 160,
					Depth = Ran.Next(-300, 0),
					MaxDepth = 1000f,
					Direction3D = ((Time > 600) ? (Math.PI / 6.0) : (Math.PI / 4.0))
				};
			}
		}
	}

	public override void Show()
	{
	}
}
