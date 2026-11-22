using System;
using System.Drawing;

namespace Shooting;

internal class EmitterStageTitleStar : BaseEffect
{
	public EmitterStageTitleStar(StageDataPackage StageData)
		: base(StageData, null, new PointF(0f, 0f), 0f, 0.0)
	{
		OriginalPosition = new PointF(0f, BoundRect.Height / 2 - 50);
		Velocity = 4f;
		MaxVelocity = 8f;
		AccelerateCS = 0.08f;
		AccDirection = 0.0;
	}

	public override void Shoot()
	{
		if (Time % 1 == 0)
		{
			for (int i = 0; i < 4; i++)
			{
				float num = Ran.Next(0, 15);
				double num2 = (double)Ran.Next(360) * Math.PI / 180.0;
				PointF position = new PointF(OriginalPosition.X + num * (float)Math.Sin(num2), OriginalPosition.Y + num * (float)Math.Cos(num2));
				new Particle(StageData, "Star", position, (float)StageData.Ran.Next(1, 110) / 100f, (double)Ran.Next(360) * Math.PI / 180.0)
				{
					Active = true,
					Scale = (float)StageData.Ran.Next(15, 40) / 100f,
					LifeTime = 80
				};
			}
		}
	}
}
