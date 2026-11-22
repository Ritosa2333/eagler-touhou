using System;
using System.Drawing;

namespace Shooting;

internal class MusicTitle : BaseEffect
{
	public new PointF Position => new PointF(OriginalPosition.X + 36f, OriginalPosition.Y + 16f);

	public MusicTitle(StageDataPackage StageData, string textureName)
		: this(StageData, textureName, new Point(0, 0))
	{
		DestPoint = new Point(BoundRect.Width, BoundRect.Height - 16);
		Scale = 0.5f;
		OriginalPosition = new PointF(BoundRect.Width, BoundRect.Height + 100);
	}

	public MusicTitle(StageDataPackage StageData, string textureName, Point DestPoint)
		: base(StageData)
	{
		TxtureObject = TextureObjectDictionary[textureName];
		OriginalPosition = new PointF(BoundRect.Width - 10, 350f);
		Velocity = 4f;
		Direction = Math.PI / 2.0;
		Scale = 2f;
		base.DestPoint = DestPoint;
		LifeTime = 240;
	}

	public override void Move()
	{
		float num = (float)GetDistance(DestPoint);
		if (num > 50f)
		{
			Velocity = 7f;
		}
		else if (num > 35f)
		{
			Velocity = 5f;
		}
		else if (num > 15f)
		{
			Velocity = 3f;
		}
		else if (num > 1f)
		{
			Velocity = 1f;
		}
		else
		{
			Velocity = 0f;
		}
		Direction = GetDirection(DestPoint);
		Angle = 0f - (float)(Direction - Math.PI / 2.0);
		base.Move();
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Scale > 1f)
		{
			Scale -= 0.03f;
		}
		if (Time >= LifeTime - 35)
		{
			MaxTransparent -= 8;
		}
	}

	public override bool OutBoundary()
	{
		if (Time > LifeTime)
		{
			return true;
		}
		return false;
	}

	public override void Show()
	{
		SizeF destinationSize = new SizeF((float)TxtureObject.Width * Scale, (float)TxtureObject.Height * Scale);
		SpriteMain.Draw2D(TxtureObject.TXTure, TxtureObject.PosRect, destinationSize, TxtureObject.RightTop, 0f, Position, Color.FromArgb(TransparentValue, Color.White));
	}
}
