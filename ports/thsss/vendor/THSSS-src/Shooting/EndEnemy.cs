using System.Drawing;

namespace Shooting;

internal class EndEnemy : BaseEffect
{
	public EndEnemy(StageDataPackage StageData, string textureName, PointF Position, float Velocity, double Direction)
		: base(StageData)
	{
		base.Position = Position;
		base.Velocity = Velocity;
		base.Direction = Direction;
		Active = true;
		TransparentVelocity = -15f;
		ScaleVelocity = 0.1f;
		TxtureObject = TextureObjectDictionary[textureName];
	}

	public override bool OutBoundary()
	{
		if (TransparentValueF <= 0f)
		{
			return true;
		}
		return false;
	}
}
