using System;
using System.Drawing;

namespace Shooting;

public class ItemBonus : BaseEffect
{
	public ItemBonus(StageDataPackage StageData)
		: base(StageData, null, new Point(0, 0), 0f, Math.PI / 2.0)
	{
		OriginalPosition = new PointF(BoundRect.Width / 2, BoundRect.Height / 2);
		LifeTime = 72;
		TransparentValueF = 0f;
		Accelerate = 0.1f;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		Direction = GetDirection(MyPlane);
	}

	public override void Shoot()
	{
		int num = 160 - Time;
		int num2 = 10;
		for (int i = 0; i < num2; i++)
		{
			PointF originalPosition = new PointF(OriginalPosition.X + (float)num * (float)Math.Sin(Angle) + (float)Ran.Next(-15, 15), OriginalPosition.Y + (float)num * (float)Math.Cos(Angle) + (float)Ran.Next(-15, 15));
			new StarItem_Touhou(StageData, originalPosition);
			originalPosition = new PointF(OriginalPosition.X - (float)num * (float)Math.Sin(Angle) + (float)Ran.Next(-15, 15), OriginalPosition.Y - (float)num * (float)Math.Cos(Angle) + (float)Ran.Next(-15, 15));
			new StarItem_Touhou(StageData, originalPosition);
		}
		Angle += 0.30000001192092896;
	}
}
