using System.Drawing;

namespace Shooting;

internal class BaseParticle : BaseEffect
{
	public BaseParticle()
	{
	}

	public BaseParticle(StageDataPackage StageData, string textureName, PointF Position, float Velocity, double Direction)
		: base(StageData, textureName, Position, Velocity, Direction)
	{
		LifeTime = 100;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Time > 0)
		{
			TransparentValueF -= 255 / LifeTime;
		}
	}
}
