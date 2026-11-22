using System;
using System.Drawing;

namespace Shooting;

internal class ParticleLongerByV : BaseEffect
{
	public ParticleLongerByV(StageDataPackage StageData, string textureName, PointF Position, float Velocity, double Direction)
		: base(StageData, textureName, Position, Velocity, Direction)
	{
		LifeTime = 30;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Time > 0)
		{
			TransparentValueF -= 255 / LifeTime;
			ScaleLength = ScaleWidth + ScaleWidth * Math.Abs(Velocity) / 10f;
		}
	}
}
