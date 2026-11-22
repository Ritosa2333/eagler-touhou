using System.Drawing;

namespace Shooting;

internal class BulletYukaLaser : BulletLaser
{
	public BulletYukaLaser(StageDataPackage StageData, string textureName, PointF OriginalPosition, float Velocity, double Direction)
		: base(StageData, textureName, OriginalPosition, Velocity, Direction)
	{
		base.OriginalPosition = OriginalPosition;
		ScaleWidth = 0.05f;
		Active = true;
		LifeTime = 500;
		ColorValue = Color.FromArgb(255, 120, 80);
		UnRemoveable = true;
		Angle = 0.0;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		Region = (int)((float)TxtureObject.Width / 2f * 0.7f);
		if (ScaleLength < DesScaleLength)
		{
			ScaleLength += 0.05f;
		}
		else if (ScaleWidth < DesScaleWidth)
		{
			ScaleWidth += 0.05f;
		}
		if (Time < LifeTime - 50)
		{
			TransparentValueF = 150 + Ran.Next(40);
		}
		else
		{
			TransparentValueF = (LifeTime - Time) * 2 - 20 + Ran.Next(40);
		}
	}

	public override void HitCheckAll()
	{
		if (MyPlane.HitEnabled && HitCheck(MyPlane) && Time < LifeTime - 50)
		{
			MyPlane.PreMiss();
		}
	}
}
