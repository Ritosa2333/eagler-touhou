using System.Drawing;

namespace Shooting;

public class BackgroundMove2 : BaseObject_CS
{
	public int Length;

	private float Width;

	public BackgroundMove2(StageDataPackage StageData, string textureName)
		: base(StageData, textureName, new PointF(0f, 0f), 0f, 0.0)
	{
		Background.BackgroundList.Add(this);
		OriginalPosition = new PointF(BoundRect.Height / 2, BoundRect.Height / 2);
		Length = BoundRect.Height * 2;
		Velocity = 2f;
		DirectionDegree = 225.0;
		OutsideRegion = 10000;
	}

	public override void Ctrl()
	{
		Scale = (float)Length / (float)TxtureObject.Height;
		Width = (float)TxtureObject.Width * Scale;
		AngleDegree = 0.0 - DirectionDegree + 90.0;
		base.Ctrl();
		float x = OriginalPosition.X;
		float y = OriginalPosition.Y;
		if (OriginalPosition.X <= (0f - Width) / 2f)
		{
			x = OriginalPosition.X + Width;
		}
		else if (OriginalPosition.X >= Width / 2f)
		{
			x = OriginalPosition.X - Width;
		}
		if (OriginalPosition.Y <= (float)(-Length / 2))
		{
			y = OriginalPosition.Y + (float)Length;
		}
		else if (OriginalPosition.Y >= (float)(Length / 2))
		{
			y = OriginalPosition.Y - (float)Length;
		}
		OriginalPosition = new PointF(x, y);
	}

	public override void Show()
	{
		float x = OriginalPosition.X;
		float y = OriginalPosition.Y;
		base.Show();
		x += Width;
		OriginalPosition = new PointF(x, y);
		base.Show();
		y += (float)Length;
		OriginalPosition = new PointF(x, y);
		base.Show();
		x -= Width;
		OriginalPosition = new PointF(x, y);
		base.Show();
		y -= (float)Length;
		OriginalPosition = new PointF(x, y);
	}
}
