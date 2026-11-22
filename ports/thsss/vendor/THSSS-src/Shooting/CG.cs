using System;
using System.Drawing;

namespace Shooting;

internal class CG : BaseStoryItem
{
	public CG(StageDataPackage StageData, string textureName)
		: base(StageData)
	{
		OriginalPosition = new PointF(BoundRect.Width / 2, BoundRect.Height / 2);
		Velocity = 0f;
		Direction = 0.0;
		Active = false;
		TransparentValueF = 0f;
		TransparentVelocity = 20f;
		Angle = 0f - (float)(Direction - Math.PI / 2.0);
	}
}
