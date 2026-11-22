using System.Drawing;

namespace Shooting;

public class Particle3D2 : BaseParticle3D
{
	public Particle3D2(StageDataPackage StageData, string textureName, PointF OriginalPosition, float Velocity, double Direction, int LifeTime)
		: base(StageData, textureName, OriginalPosition, Velocity, Direction)
	{
		Active = true;
		StageData.Particle3D.ParticleList.Add(this);
		MaxTransparent = 128;
		TransparentValueF = MaxTransparent;
		base.LifeTime = LifeTime;
		Accelerate = (0f - Velocity) / (float)LifeTime;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (LifeTime * 2 / 3 < Time && Time <= LifeTime)
		{
			TransparentValueF -= MaxTransparent * 3 / LifeTime;
		}
		if (Time == LifeTime * 2 / 3)
		{
			ScaleVelocity = -0.1f;
		}
	}
}
