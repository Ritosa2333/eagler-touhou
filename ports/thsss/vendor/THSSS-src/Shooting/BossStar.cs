using System;
using System.Drawing;

namespace Shooting;

public class BossStar : BaseEffect
{
	public int ColorType;

	public BossStar(StageDataPackage StageData, PointF OriginalPosition)
		: base(StageData, null, new Point(0, 0), 0f, Math.PI / 2.0)
	{
		base.OriginalPosition = OriginalPosition;
		LifeTime = 130;
	}

	public override void Shoot()
	{
		if (Time == 1)
		{
			int num = 110;
			double num2 = Math.PI / 2.0;
			for (int i = 0; i < 3; i++)
			{
				PointF originalPosition = new PointF((float)((double)OriginalPosition.X + (double)num * Math.Cos(num2)), (float)((double)OriginalPosition.Y + (double)num * Math.Sin(num2)));
				new BossStarEmitter01(StageData, originalPosition)
				{
					Velocity = 2.8f,
					DirectionDegree = num2 * 180.0 / Math.PI + 45.0,
					DirectionVelocityDegree = 2f,
					ColorType = ColorType
				};
				num2 += Math.PI * 2.0 / 3.0;
			}
		}
		else if (Time == LifeTime)
		{
			new BaseEffect(StageData, "Light0" + ColorType, Position, 0f, 0.0)
			{
				LifeTime = 50,
				ScaleVelocity = 0.4f,
				TransparentVelocityDictionary = { { 20, -5f } }
			};
		}
	}
}
