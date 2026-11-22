using System;
using System.Drawing;

namespace Shooting;

public class BossStarEmitter01 : BaseEffect
{
	public int ColorType;

	public BossStarEmitter01(StageDataPackage StageData, PointF OriginalPosition)
		: base(StageData, null, new Point(0, 0), 0f, Math.PI / 2.0)
	{
		base.OriginalPosition = OriginalPosition;
		LifeTime = 130;
	}

	public override void Shoot()
	{
		if (Time % 1 == 0)
		{
			for (int i = 0; i < 2; i++)
			{
				float num = Ran.Next(0, 15);
				double num2 = (double)Ran.Next(360) * Math.PI / 180.0;
				PointF position = new PointF(Position.X + num * (float)Math.Sin(num2), Position.Y + num * (float)Math.Cos(num2));
				new Particle(StageData, "Star0" + ColorType, position, (float)StageData.Ran.Next(1, 15) / 10f, Direction)
				{
					Scale = (float)StageData.Ran.Next(15, 40) / 100f,
					MaxTransparent = 255,
					LifeTime = 40 + Ran.Next(60)
				};
			}
		}
	}
}
