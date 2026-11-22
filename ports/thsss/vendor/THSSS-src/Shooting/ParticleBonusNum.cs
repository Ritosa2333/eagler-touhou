using System;
using System.Drawing;

namespace Shooting;

internal class ParticleBonusNum : BaseEffect
{
	public ParticleBonusNum(StageDataPackage StageData, string textureName, PointF OriginalPosition)
		: base(StageData, textureName, OriginalPosition, 0f, -1.5707963705062866)
	{
		base.OriginalPosition = OriginalPosition;
		LifeTime = 100;
		Active = false;
		Angle = 0.0 - Direction + Math.PI / 2.0;
	}

	public override void Ctrl()
	{
		Angle = 0.0 - Direction + Math.PI / 2.0;
		if (Time < 24)
		{
			Velocity = (float)Math.Cos((double)Time * Math.PI * 2.0 / 32.0);
		}
		else
		{
			Velocity = 0f;
		}
		base.Ctrl();
		if (Time > LifeTime - 20)
		{
			TransparentValueF -= 12f;
		}
	}
}
