using System.Drawing;

namespace Shooting;

public class BackgroundMySC : BaseObject
{
	public BackgroundMySC(StageDataPackage StageData, string textureName)
		: base(StageData, textureName, new PointF(0f, 0f), 0f, 0.0)
	{
		Background.BackgroundList.Add(this);
		OriginalPosition = new PointF(BoundRect.Width / 2, BoundRect.Height / 2);
		Scale = 2.5f;
		AngularVelocityDegree = 0.5f;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Time >= LifeTime - 50)
		{
			TransparentVelocity = -5f;
		}
	}
}
