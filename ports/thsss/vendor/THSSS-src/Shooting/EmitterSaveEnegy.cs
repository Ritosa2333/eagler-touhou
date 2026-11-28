using System;
using System.Drawing;

namespace Shooting;

internal class EmitterSaveEnegy : BaseEffect
{
	public int ColorType { get; set; }

	public EmitterSaveEnegy(StageDataPackage StageData, PointF Position, int colorType)
		: base(StageData)
	{
		base.Position = Position;
		Velocity = 0f;
		Direction = 0.0;
		LifeTime = 200;
		ColorType = colorType;
	}

	public override void Shoot()
	{
		if (Time % 5 == 0)
		{
			for (int i = 0; i < 5; i++)
			{
				double num = (double)((float)Ran.Next(360) / 180f) * Math.PI;
				int num2 = Ran.Next(10, 100);
				PointF position = new PointF((float)((double)Position.X + (double)num2 * Math.Cos(num)), (float)((double)Position.Y + (double)num2 * Math.Sin(num)));
				ParticleSmaller particleSmaller = new ParticleSmaller(StageData, "MyBullet000" + ColorType, position, 0f, num + Math.PI);
				particleSmaller.TransparentValueF = 200f;
				particleSmaller.LifeTime = 50;
				particleSmaller.Velocity = (float)num2 / (float)particleSmaller.LifeTime;
				particleSmaller.Scale = (float)Ran.Next(20, 50) / 100f;
			}
		}
	}

	public override void Show()
	{
	}
}
