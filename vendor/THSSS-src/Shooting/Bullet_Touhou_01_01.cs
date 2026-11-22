using System;
using System.Drawing;

namespace Shooting;

public class Bullet_Touhou_01_01 : Bullet_Touhou_01
{
	public string SubBulletName { get; set; }

	public Bullet_Touhou_01_01(StageDataPackage StageData, string textureName, PointF OriginalPosition, float Velocity, double Direction, byte ColorType)
		: base(StageData, textureName, OriginalPosition, Velocity, Direction, ColorType)
	{
	}

	public override bool OutBoundary()
	{
		if (LifeTime != 0 && Time > LifeTime)
		{
			return true;
		}
		if (Reflect <= 0)
		{
			if (Time < 100)
			{
				return false;
			}
			int num = ((Region >= 10) ? Region : 10);
			if ((float)(-num) < OriginalPosition.X && OriginalPosition.X < (float)(BoundRect.Width + num) && (float)(-num) < OriginalPosition.Y && OriginalPosition.Y < (float)(BoundRect.Height + num))
			{
				return false;
			}
			return true;
		}
		int num2 = 5;
		if (OriginalPosition.X < (float)num2 || OriginalPosition.X >= (float)(BoundRect.Width - num2))
		{
			Reflect--;
			Direction -= 2.0 * (Direction - Math.PI / 2.0);
			new BaseBullet_Touhou(StageData, SubBulletName, OriginalPosition, Velocity, Direction, ColorType);
			GiveEndEffect();
			return true;
		}
		if (OriginalPosition.Y > (float)(BoundRect.Bottom + num2))
		{
			return true;
		}
		return false;
	}
}
