using System.Drawing;

namespace Shooting;

public class Particle3D1 : BaseParticle3D
{
	public Particle3D1(StageDataPackage StageData, string textureName, PointF OriginalPosition, float Velocity, double Direction, int LifeTime)
		: base(StageData, textureName, OriginalPosition, Velocity, Direction)
	{
		StageData.Particle3D.ParticleList.Add(this);
		Active = true;
		MaxTransparent = 128;
		TransparentValueF = 0f;
		base.LifeTime = LifeTime;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (0 < Time && Time < LifeTime / 3)
		{
			TransparentValueF += MaxTransparent * 3 / LifeTime;
		}
		else if (LifeTime * 2 / 3 < Time && Time < LifeTime)
		{
			TransparentValueF -= MaxTransparent * 3 / LifeTime;
		}
	}
}
