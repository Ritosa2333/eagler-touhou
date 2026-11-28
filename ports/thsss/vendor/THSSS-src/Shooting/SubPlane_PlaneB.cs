using System;
using System.Drawing;

namespace Shooting;

public class SubPlane_PlaneB : BaseSubPlane
{
	public SubPlane_PlaneB(StageDataPackage StageData, PointF Position)
		: base(StageData, "MySubPlane", Position)
	{
		ShootDirection = -Math.PI / 2.0;
	}

	public override void Shoot()
	{
		new BaseMyBullet_Touhou(StageData, "子机子弹", OriginalPosition, 4f, ShootDirection)
		{
			Angle = 3.1415927410125732,
			Damage = 5,
			Accelerate = 0.5f
		};
	}
}
