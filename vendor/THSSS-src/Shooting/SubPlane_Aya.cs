using System.Drawing;

namespace Shooting;

public class SubPlane_Aya : BaseSubPlane
{
	private int shootFlag;

	public SubPlane_Aya(StageDataPackage StageData, PointF Position)
		: base(StageData, "Center", Position)
	{
	}

	public override void Refresh()
	{
		base.Refresh();
		shootFlag = 0;
	}

	public override void Shoot()
	{
		new BaseMyBullet_Touhou(StageData, "ReimuAyaBullet01", OriginalPosition, 8f, ShootDirection)
		{
			TransparentValueF = 150f,
			Angle = 1.5707963705062866,
			Damage = 6
		};
		if (shootFlag++ % 2 == 0)
		{
			new BaseMyBullet_Touhou(StageData, "ReimuAyaBullet01", OriginalPosition, 8f, ShootDirection + (double)((float)Ran.Next(-3, 4) / 20f))
			{
				TransparentValueF = 150f,
				Angle = 1.5707963705062866,
				Damage = 6
			};
		}
	}
}
