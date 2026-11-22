using System;
using System.Drawing;

namespace Shooting;

internal class BackgroundSakura : BaseParticle3D
{
	public BackgroundSakura(StageDataPackage StageData)
	{
		base.StageData = StageData;
		Background3D.BackgroundParticleList.Add(this);
	}

	public override void Ctrl()
	{
		Time++;
		Shoot();
	}

	public override void Shoot()
	{
		int num = 25;
		float num2 = 2.7f;
		if (Boss != null && Boss.Life >= 2)
		{
			num = 50;
			num2 = 1.35f;
		}
		if (Time % num == 0)
		{
			int num3 = Time / 2;
			if (num3 > 110)
			{
				num3 = 110;
			}
			new BackgroundParticle3D1(StageData, "Sakura", new PointF(num3 + Ran.Next(30), -80f), num2, 1.720796332755361, 350)
			{
				Angle = (float)StageData.Ran.Next(-10, 10) * (float)Math.PI / 180f,
				Active = true,
				MaxTransparent = 60,
				Direction3D = 0.75,
				Depth = -100f,
				MaxDepth = 1000f,
				OutsideRegion = 128
			};
		}
	}

	public override void Show()
	{
	}
}
