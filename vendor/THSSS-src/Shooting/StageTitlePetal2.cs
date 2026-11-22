using System;
using System.Drawing;
using SlimDX;

namespace Shooting;

public class StageTitlePetal2 : BaseParticle3D
{
	public StageTitlePetal2(StageDataPackage StageData)
	{
		base.StageData = StageData;
		Background3D.BackgroundParticleList.Add(this);
		OriginalPosition = new PointF(BoundRect.Width / 2 - 45, 40f);
		Direction = Math.PI / 2.0;
		Velocity = 3f;
		LifeTime = 80;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		Shoot();
		float num = 5f;
		Vx = 10f * (float)Math.Sin((float)Time / num);
		Depth += 10f * (float)Math.Cos((float)Time / num);
	}

	public override void Shoot()
	{
		//IL_014a: Unknown result type (might be due to invalid IL or missing references)
		if (Time % 1 == 0)
		{
			for (int i = 0; i < 8; i++)
			{
				new Particle3D4(StageData, "Petal00", new PointF(StageData.Ran.Next((int)OriginalPosition.X - 60, (int)OriginalPosition.X - 35), (int)OriginalPosition.Y), (float)StageData.Ran.Next(10, 30) / 10f, Math.PI / 2.0, 100 + Ran.Next(40))
				{
					Angle = (float)StageData.Ran.Next(0, 360) * (float)Math.PI / 180f,
					AngularVelocity3D = (float)StageData.Ran.Next(10) / 100f,
					Scale = (float)StageData.Ran.Next(5, 8) / 10f,
					RotatingAxis = new Vector3((float)StageData.Ran.Next(-100, 100), (float)StageData.Ran.Next(-100, 100), (float)StageData.Ran.Next(-100, 100)),
					MaxTransparent = 255,
					Depth = Ran.Next((int)Depth - 50, (int)Depth - 50)
				};
			}
		}
	}

	public override void Show()
	{
	}
}
