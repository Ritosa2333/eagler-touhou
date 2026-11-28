using System;
using System.Drawing;

namespace Shooting;

internal class EmitterBossFire : BaseEffect
{
	public EmitterBossFire(StageDataPackage StageData, PointF OriginalPosition, Color ColorValue)
		: base(StageData, null, OriginalPosition, 0f, 0.0)
	{
		base.OriginalPosition = OriginalPosition;
		base.ColorValue = ColorValue;
	}

	public override void Move()
	{
		if (Boss != null)
		{
			OriginalPosition = Boss.OriginalPosition;
		}
	}

	public override void Shoot()
	{
		if (Time % 10 == 0)
		{
			new ParticleBossFire(StageData, "Effect01_1", OriginalPosition, 0f, 0.0, 60).ColorValue = ColorValue;
			new ParticleBossFire2(StageData, "Effect01_2", OriginalPosition, 0f, (double)Ran.Next(360) * Math.PI / 180.0, 80).ColorValue = ColorValue;
			new ParticleBossFire2(StageData, "Effect01_2", OriginalPosition, 0f, (double)Ran.Next(360) * Math.PI / 180.0, 100).ColorValue = ColorValue;
		}
	}

	public override bool OutBoundary()
	{
		if (Boss == null)
		{
			return true;
		}
		return false;
	}
}
