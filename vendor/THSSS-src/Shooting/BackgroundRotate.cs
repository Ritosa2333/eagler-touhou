using System.Drawing;

namespace Shooting;

public class BackgroundRotate : BaseObject_CS
{
	public BackgroundRotate(StageDataPackage StageData, string textureName)
		: base(StageData, textureName, new PointF(0f, 0f), 0f, 0.0)
	{
		Background.BackgroundList.Add(this);
		OriginalPosition = new PointF(BoundRect.Width / 2, BoundRect.Height / 2);
		Scale = 2.5f;
		AngularVelocityDegree = 0.5f;
	}
}
