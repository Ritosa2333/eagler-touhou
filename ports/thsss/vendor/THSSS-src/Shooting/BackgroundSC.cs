using System.Drawing;

namespace Shooting;

public class BackgroundSC : BaseObject
{
	public BackgroundSC(StageDataPackage StageData, string textureName)
		: base(StageData, textureName, new PointF(0f, 0f), 0f, 0.0)
	{
		Background.BackgroundList.Add(this);
		OriginalPosition = new PointF(BoundRect.Width / 2, BoundRect.Height / 2);
		DirectionDegree = 90.0;
	}
}
