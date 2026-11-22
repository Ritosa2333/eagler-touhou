using System;
using System.Drawing;

namespace Shooting;

public class Particle3D4 : BaseParticle3D
{
	private float maxScale;

	public Particle3D4(StageDataPackage StageData, string textureName, PointF OriginalPosition, float Velocity, double Direction, int LifeTime)
		: base(StageData, textureName, OriginalPosition, Velocity, Direction)
	{
		Active = true;
		StageData.Particle3D.ParticleList.Add(this);
		MaxTransparent = 128;
		TransparentValueF = MaxTransparent;
		base.LifeTime = LifeTime;
		Accelerate = (float)Ran.Next(10) / 100f;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Time == 1)
		{
			maxScale = Scale;
		}
		else if (LifeTime * 3 / 4 < Time && Time <= LifeTime)
		{
			Scale -= maxScale * 4f / (float)LifeTime;
		}
		float num = 8f / Vy;
		Vx = 10f * (float)Math.Sin((float)Time / num);
		Depth += 10f * (float)Math.Cos((float)Time / num);
	}
}
