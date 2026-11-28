using System.Drawing;

namespace Shooting;

internal class ParticleSmaller : BaseParticle
{
	public ParticleSmaller(StageDataPackage StageData, string textureName, PointF Position, float Velocity, double Direction)
		: base(StageData, textureName, Position, Velocity, Direction)
	{
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Time > 0)
		{
			Scale -= 1f / (float)LifeTime;
		}
		if (Scale < 0.2f)
		{
			Scale = 0.2f;
		}
	}
}
