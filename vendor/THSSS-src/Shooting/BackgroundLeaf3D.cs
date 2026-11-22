using System;
using System.Drawing;
using SlimDX;

namespace Shooting;

internal class BackgroundLeaf3D : BaseParticle3D
{
	public BackgroundLeaf3D(StageDataPackage StageData)
	{
		base.StageData = StageData;
		StageData.Particle3D.ParticleList.Add(this);
	}

	public override void Ctrl()
	{
		Time++;
		Shoot();
	}

	public override void Shoot()
	{
		//IL_013e: Unknown result type (might be due to invalid IL or missing references)
		if (Time % 20 == 0)
		{
			for (int i = 0; i < 2; i++)
			{
				new BackgroundParticle3D1(StageData, "Leaf", new PointF(StageData.Ran.Next(130, 700), StageData.Ran.Next(-50, -20)), StageData.Ran.Next(2, 4), Math.PI / 2.0 + (double)((float)StageData.Ran.Next(1, 8) / 10f), 200)
				{
					Angle = (float)StageData.Ran.Next(0, 360) * (float)Math.PI / 180f,
					AngularVelocity3D = (float)StageData.Ran.Next(5) / 100f,
					Scale = (float)StageData.Ran.Next(5, 20) / 10f,
					RotatingAxis = new Vector3((float)StageData.Ran.Next(-100, 100), (float)StageData.Ran.Next(-100, 100), (float)StageData.Ran.Next(-100, 100)),
					ColorValue = Color.FromArgb(StageData.Ran.Next(150, 250), 255, StageData.Ran.Next(150, 255))
				};
			}
		}
	}

	public override void Show()
	{
	}
}
