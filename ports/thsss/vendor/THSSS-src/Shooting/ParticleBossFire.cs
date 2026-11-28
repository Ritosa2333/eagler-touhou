using System.Drawing;

namespace Shooting;

internal class ParticleBossFire : BaseObject
{
	public ParticleBossFire(StageDataPackage StageData, string textureName, PointF OriginalPosition, float Velocity, double Direction, int LifeTime)
	{
		base.StageData = StageData;
		base.Velocity = Velocity;
		base.Direction = Direction;
		base.OriginalPosition = OriginalPosition;
		base.LifeTime = LifeTime;
		if (textureName != null)
		{
			TxtureObject = TextureObjectDictionary[textureName];
		}
		Active = true;
		Background.BackgroundList.Add(this);
		ScaleLength = 0.5f;
		ScaleWidth = 0.3f;
		DirectionDegree = 90 + Ran.Next(-4, 5);
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Time > 0)
		{
			TransparentValueF -= 255 / LifeTime;
		}
		if (Boss != null)
		{
			OriginalPosition = Boss.OriginalPosition;
		}
		if (Time > 0)
		{
			ScaleLength += 0.07f;
			ScaleWidth += 0.03f;
			TransparentValueF -= MaxTransparent / LifeTime;
		}
		if (OutBoundary())
		{
			Background.BackgroundList.Remove(this);
		}
	}
}
