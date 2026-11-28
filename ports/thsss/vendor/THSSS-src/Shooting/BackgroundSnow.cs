using System;
using System.Drawing;

namespace Shooting;

internal class BackgroundSnow : BaseObject
{
	public BackgroundSnow(StageDataPackage StageData)
	{
		base.StageData = StageData;
		Background.BackgroundList.Add(this);
	}

	public override void Ctrl()
	{
		Time++;
		Shoot();
	}

	public override void Shoot()
	{
		if (Time % 20 == 0)
		{
			for (int i = 0; i < 3; i++)
			{
				new ParticleBack(StageData, "snow", new PointF(Ran.Next(BoundRect.Left, BoundRect.Right), Ran.Next(BoundRect.Top, BoundRect.Bottom / 3)), (float)Ran.Next(10, 20) / 10f, Math.PI / 2.0 + 0.5 * Ran.NextDouble())
				{
					LifeTime = 200,
					AngularVelocity = (float)Ran.Next(20) / 1000f,
					Scale = (float)Ran.Next(2, 10) / 10f,
					Active = true
				};
			}
		}
	}

	public override void Show()
	{
	}
}
