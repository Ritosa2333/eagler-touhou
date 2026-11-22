using System;
using System.Drawing;

namespace Shooting;

internal class ParticleScore : BaseEffect
{
	public ParticleScore(StageDataPackage StageData, string textureName, PointF Position)
		: base(StageData, textureName, Position, 1f, -1.5707963705062866)
	{
		LifeTime = 50;
		Active = false;
	}

	public override void Ctrl()
	{
		Angle = 0.0 - Direction + Math.PI / 2.0;
		base.Ctrl();
		if (Time > 0)
		{
			TransparentValueF -= 255 / LifeTime;
		}
	}
}
