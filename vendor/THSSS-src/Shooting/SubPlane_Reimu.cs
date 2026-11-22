using System;
using System.Drawing;

namespace Shooting;

public class SubPlane_Reimu : BaseSubPlane
{
	public SubPlane_Reimu(StageDataPackage StageData, PointF Position)
		: base(StageData, "ReimuSubPlane", Position)
	{
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (KClass.Key_Shift)
		{
			TransparentVelocity = -20f;
		}
		else
		{
			TransparentVelocity = 20f;
		}
	}

	public override void Shoot()
	{
		if (KClass.Key_Shift)
		{
			new BaseMyBullet_Touhou(StageData, "MaruBullet", OriginalPosition, 18f, ShootDirection + 0.0523598802906103)
			{
				Angle = 1.5707963705062866,
				Damage = 5
			};
			new BaseMyBullet_Touhou(StageData, "MaruBullet", OriginalPosition, 18f, ShootDirection - 0.0523598802906103)
			{
				Angle = 1.5707963705062866,
				Damage = 5
			};
		}
		else
		{
			new BulletGuidance(StageData, "ReimuSubBullet", Position, 8f, ShootDirection)
			{
				TransparentValueF = 180f,
				Angle = 1.5707963705062866,
				Damage = 9
			};
		}
	}

	public override void Show()
	{
		if (Enabled && TxtureObject != null)
		{
			double num = (double)(-Time * 5 % 360) * Math.PI / 180.0;
			if (MyPlane.Power >= 400)
			{
				SpriteMain.Draw2D(TxtureObject, 1.3f, 1.3f, 0f - (float)num, Position, Color.FromArgb(TransparentValue * 6 / 10, ColorValue), Mirrored);
			}
			SpriteMain.Draw2D(TxtureObject, ScaleWidth, ScaleLength, (float)num, Position, Color.FromArgb(TransparentValue, ColorValue), Mirrored);
		}
	}
}
