using System.Drawing;

namespace Shooting;

internal class EmitterTitleStarFall : BaseEffect
{
	public EmitterTitleStarFall(StageDataPackage StageData)
		: base(StageData, null, new PointF(0f, 0f), 0f, 0.0)
	{
		OriginalPosition = new PointF(BoundRect.Width - 100, 0f);
	}

	public override void Shoot()
	{
		if (Time % 80 == 0)
		{
			new EmitterStarFall01(StageData, new PointF(OriginalPosition.X + 50f + (float)Ran.Next(-200, 200), Ran.Next(30)));
		}
	}
}
