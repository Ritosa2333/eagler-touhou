using System;
using System.Drawing;

namespace Shooting;

public class SubPlane_Sanae : BaseSubPlane
{
	public SubPlane_Sanae(StageDataPackage StageData, PointF Position)
		: base(StageData, "SanaeSubPlane", Position)
	{
		AngleDegree = 90.0;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		Scale = 1f + 0.1f * (float)Math.Sin((double)((float)TimeMain / 40f * 2f) * Math.PI);
	}

	public override void Shoot()
	{
		if (KClass.Key_Shift)
		{
			new BaseMyBullet_Touhou(StageData, "SanaeSubBullet02", OriginalPosition, 40f, ShootDirection - 0.029999999329447746)
			{
				Angle = 1.5707963705062866,
				Damage = 3
			};
			new BaseMyBullet_Touhou(StageData, "SanaeSubBullet02", OriginalPosition, 40f, ShootDirection + 0.029999999329447746)
			{
				Angle = 1.5707963705062866,
				Damage = 3
			};
			return;
		}
		new BaseMyBullet_Touhou(StageData, "SanaeSubBullet00", OriginalPosition, 30f, ShootDirection - 0.15000000596046448)
		{
			Angle = 3.1415927410125732,
			Damage = 7,
			Mirrored = Mirrored
		};
		new BaseMyBullet_Touhou(StageData, "SanaeSubBullet00", OriginalPosition, 30f, ShootDirection + 0.15000000596046448)
		{
			Angle = 3.1415927410125732,
			Damage = 7,
			Mirrored = Mirrored
		};
		new BaseMyBullet_Touhou(StageData, "SanaeSubBullet00", OriginalPosition, 30f, ShootDirection)
		{
			Angle = 3.1415927410125732,
			Damage = 7,
			Mirrored = Mirrored
		};
	}

	public override void Show()
	{
		if (Enabled)
		{
			if (MyPlane.Power >= 400)
			{
				float scale = Scale;
				float transparentValueF = TransparentValueF;
				Scale = 1.3f;
				TransparentValueF = 150f;
				base.Show();
				TransparentValueF = transparentValueF;
				Scale = scale;
			}
			base.Show();
		}
	}
}
