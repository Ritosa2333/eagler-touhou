using System;
using System.Drawing;

namespace Shooting;

public class SubPlane_Koishi : BaseSubPlane
{
	public SubPlane_Koishi(StageDataPackage StageData, PointF Position)
		: base(StageData, "KoishiSubPlane01", Position)
	{
		AngleDegree = 90.0;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		Scale = 1f + 0.1f * (float)Math.Sin((double)((float)TimeMain / 40f * 2f) * Math.PI);
		if (KClass.Key_Shift)
		{
			TxtureObject = TextureObjectDictionary["KoishiSubPlane00"];
		}
		else
		{
			TxtureObject = TextureObjectDictionary["KoishiSubPlane01"];
		}
	}

	public override void Shoot()
	{
		if (KClass.Key_Shift)
		{
			int num = 5;
			double num2 = 24.0 * Math.PI / 180.0 / (double)(num - 1);
			double num3 = ShootDirection - (double)(num - 1) * num2 / 2.0;
			for (int i = 0; i < num; i++)
			{
				new BaseMyBullet_Touhou(StageData, "KoishiSubBullet00", OriginalPosition, 15f, num3)
				{
					Angle = 1.5707963705062866,
					Damage = 6
				};
				num3 += num2;
			}
		}
		else
		{
			int num4 = 15;
			double num5 = 120.0 * Math.PI / 180.0 / (double)(num4 - 1);
			double num6 = ShootDirection - (double)(num4 - 1) * num5 / 2.0;
			for (int j = 0; j < num4; j++)
			{
				new BaseMyBullet_Touhou(StageData, "KoishiSubBullet01", OriginalPosition, 8f, num6)
				{
					Angle = 1.5707963705062866,
					Damage = 15
				};
				num6 += num5;
			}
		}
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
