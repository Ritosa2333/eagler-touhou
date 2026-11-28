using System;
using System.Drawing;

namespace Shooting;

internal class Emitter : BaseEffect
{
	public Emitter(StageDataPackage StageData, PointF Position)
		: base(StageData)
	{
		base.Position = Position;
		Velocity = 0f;
		Direction = 0.0;
	}

	public override void Shoot()
	{
		for (int i = 0; i < 10; i++)
		{
			string textureName = ((MyPlane.Name == "Aya") ? "MyParticle" : "MyParticle01");
			new ParticleSmaller(StageData, textureName, Position, Ran.Next(20, 200) / 10, (double)((float)Ran.Next(360) / 180f) * Math.PI)
			{
				LifeTime = 20,
				Scale = (float)Ran.Next(100) / 100f,
				TransparentValueF = 200f
			};
		}
	}

	public override bool OutBoundary()
	{
		if (Time >= 4)
		{
			return true;
		}
		return false;
	}

	public override void Show()
	{
	}
}
