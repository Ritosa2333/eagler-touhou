using System;
using System.Drawing;
using SlimDX;

namespace Shooting;

public class BossPetalEmitter01 : BaseEffect
{
	public BossPetalEmitter01(StageDataPackage StageData, PointF OriginalPosition)
		: base(StageData, null, new Point(0, 0), 0f, Math.PI / 2.0)
	{
		base.OriginalPosition = OriginalPosition;
		LifeTime = 130;
	}

	public override void Shoot()
	{
		//IL_016c: Unknown result type (might be due to invalid IL or missing references)
		if (Time % 1 == 0)
		{
			for (int i = 0; i < 2; i++)
			{
				new Particle3D3(StageData, "Petal30", new PointF(StageData.Ran.Next((int)OriginalPosition.X - 20, (int)OriginalPosition.X + 20), StageData.Ran.Next((int)OriginalPosition.Y - 20, (int)OriginalPosition.Y + 20)), (float)StageData.Ran.Next(10, 30) / 10f, Direction, 100 + Ran.Next(60))
				{
					Angle = (float)StageData.Ran.Next(0, 360) * (float)Math.PI / 180f,
					AngularVelocity3D = (float)StageData.Ran.Next(10) / 100f,
					Scale = (float)StageData.Ran.Next(5, 8) / 10f,
					RotatingAxis = new Vector3((float)StageData.Ran.Next(-100, 100), (float)StageData.Ran.Next(-100, 100), (float)StageData.Ran.Next(-100, 100)),
					MaxTransparent = 255
				};
			}
		}
	}
}
