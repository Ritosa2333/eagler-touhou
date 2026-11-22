using System.Drawing;

namespace Shooting;

internal class ParticleBiger : BaseEffect
{
	public ParticleBiger(StageDataPackage StageData, string textureName, PointF Position, float Velocity, double Direction)
		: base(StageData, textureName, Position, Velocity, Direction)
	{
		LifeTime = 30;
		Scale = 0.5f;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Time > 0)
		{
			TransparentValueF -= MaxTransparent / LifeTime;
			Scale += 0.05f;
		}
	}
}
