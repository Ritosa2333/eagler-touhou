using System.Drawing;

namespace Shooting;

public class BaseBackground : BaseObject_CS
{
	public BaseBackground()
	{
	}

	public BaseBackground(StageDataPackage StageData)
	{
		base.StageData = StageData;
		Background.BackgroundList.Add(this);
	}

	public BaseBackground(StageDataPackage StageData, string textureName, PointF Position, float Velocity, double Direction)
		: base(StageData, textureName, Position, Velocity, Direction)
	{
		Background.BackgroundList.Add(this);
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (OutBoundary())
		{
			Background.BackgroundList.Remove(this);
		}
	}

	public override bool HitCheck(BaseObject Target)
	{
		return false;
	}
}
