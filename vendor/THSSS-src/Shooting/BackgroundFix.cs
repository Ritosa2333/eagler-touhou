using System.Drawing;

namespace Shooting;

public class BackgroundFix : BaseObject
{
	public BackgroundFix(StageDataPackage StageData, string textureName)
		: base(StageData, textureName, new PointF(0f, 0f), 0f, 0.0)
	{
		Background.BackgroundList.Add(this);
		OriginalPosition = new PointF(BoundRect.Width / 2, BoundRect.Height / 2);
		AngleDegree = 90.0;
		OutsideRegion = 1000;
	}
}
