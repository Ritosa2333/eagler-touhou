using System.Drawing;

namespace Shooting;

public class SubPlane_AyaB : BaseSubPlane
{
	public SubPlane_AyaB(StageDataPackage StageData, PointF Position)
		: base(StageData, "MarisaSubPlane10", Position)
	{
	}

	public override void Refresh()
	{
		base.Refresh();
	}

	public override void Shoot()
	{
		if (KClass.Key_Shift)
		{
			new BaseMyBullet_Touhou(StageData, "AyaBullet10", OriginalPosition, 12f, ShootDirection)
			{
				Angle = 1.5707963705062866,
				Damage = 4
			};
		}
		else
		{
			new BulletGuidance(StageData, "AyaBullet10", Position, 8f, ShootDirection)
			{
				TransparentValueF = 180f,
				Angle = 1.5707963705062866
			};
		}
	}
}
