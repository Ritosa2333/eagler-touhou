using System.Drawing;

namespace Shooting;

public class BackgroundMove : BaseObject_CS
{
	public BackgroundMove(StageDataPackage StageData, string textureName)
		: base(StageData, textureName, new PointF(0f, 0f), 0f, 0.0)
	{
		Background.BackgroundList.Add(this);
		OriginalPosition = new PointF(BoundRect.Height / 2, BoundRect.Height / 2);
		Scale = (float)BoundRect.Height / (float)TxtureObject.Height;
		Velocity = 2f;
		DirectionDegree = 225.0;
		OutsideRegion = 1000;
	}

	public override void Ctrl()
	{
		AngleDegree = 0.0 - DirectionDegree + 90.0;
		base.Ctrl();
		float x = OriginalPosition.X;
		float y = OriginalPosition.Y;
		if (OriginalPosition.X <= (float)(-BoundRect.Height / 2))
		{
			x = OriginalPosition.X + (float)BoundRect.Height;
		}
		else if (OriginalPosition.X >= (float)(BoundRect.Height / 2))
		{
			x = OriginalPosition.X - (float)BoundRect.Height;
		}
		if (OriginalPosition.Y <= (float)(-BoundRect.Height / 2))
		{
			y = OriginalPosition.Y + (float)BoundRect.Height;
		}
		else if (OriginalPosition.Y >= (float)(BoundRect.Height / 2))
		{
			y = OriginalPosition.Y - (float)BoundRect.Height;
		}
		OriginalPosition = new PointF(x, y);
	}

	public override void Show()
	{
		float x = OriginalPosition.X;
		float y = OriginalPosition.Y;
		base.Show();
		x += (float)BoundRect.Height;
		OriginalPosition = new PointF(x, y);
		base.Show();
		y += (float)BoundRect.Height;
		OriginalPosition = new PointF(x, y);
		base.Show();
		x -= (float)BoundRect.Height;
		OriginalPosition = new PointF(x, y);
		base.Show();
		y -= (float)BoundRect.Height;
		OriginalPosition = new PointF(x, y);
	}
}
