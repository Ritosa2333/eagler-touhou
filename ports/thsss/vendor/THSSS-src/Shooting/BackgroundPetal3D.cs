using System;
using System.Drawing;
using SlimDX;

namespace Shooting;

internal class BackgroundPetal3D : BaseParticle3D
{
	public BackgroundPetal3D(StageDataPackage StageData)
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
		//IL_016e: Unknown result type (might be due to invalid IL or missing references)
		if (Time % 5 == 0)
		{
			for (int i = 0; i < 3; i++)
			{
				new BackgroundParticle3D1(StageData, "Petal3" + Ran.Next(4), new PointF(StageData.Ran.Next(280, BoundRect.Width + 160), StageData.Ran.Next(-20, 0)), (float)StageData.Ran.Next(15, 25) / 10f, Math.PI / 2.0 + (double)((float)StageData.Ran.Next(1, 8) / 10f), 200)
				{
					Angle = (float)StageData.Ran.Next(0, 360) * (float)Math.PI / 180f,
					AngularVelocity3D = (float)StageData.Ran.Next(10) / 100f,
					Scale = (float)StageData.Ran.Next(35, 50) / 100f,
					RotatingAxis = new Vector3((float)StageData.Ran.Next(-100, 100), (float)StageData.Ran.Next(-100, 100), (float)StageData.Ran.Next(-100, 100)),
					MaxTransparent = 200,
					Depth = Ran.Next(0, 30)
				};
			}
		}
	}

	public override void Show()
	{
	}
}
