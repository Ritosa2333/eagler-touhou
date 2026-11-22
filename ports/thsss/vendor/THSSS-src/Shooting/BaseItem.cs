using System;
using System.Drawing;

namespace Shooting;

public class BaseItem : BaseObject
{
	public float AngularVelocity { get; set; }

	public bool Obtain { get; set; }

	public bool Doubled { get; set; }

	public BaseItem(StageDataPackage StageData, string textureName, PointF Position)
		: base(StageData, textureName, Position, 0f, 0.0)
	{
		ItemList.Add(this);
		Velocity = Ran.Next(1, 6);
		Direction = (double)((float)Ran.Next(180, 360) / 360f) * Math.PI * 2.0;
		Angle = 0.0;
		if ((float)Direction - (float)Math.PI / 2f > 0f)
		{
			AngularVelocity = -0.2f;
		}
		else
		{
			AngularVelocity = 0.2f;
		}
	}

	public override void HitCheckAll()
	{
		if (HitCheck(MyPlane, 200f))
		{
			Obtain = true;
		}
	}

	public override void Ctrl()
	{
		base.Ctrl();
		Angle += AngularVelocity;
		if (Time == 12)
		{
			Direction = Math.PI / 2.0;
			AngularVelocity = 0f;
			Angle = 0.0;
		}
		else if (Time > 12)
		{
			if (Obtain)
			{
				Direction = Math.PI + Math.Atan2(Position.Y - MyPlane.Position.Y, Position.X - MyPlane.Position.X);
				Velocity = 8f;
			}
			else
			{
				Velocity = 4f;
			}
		}
		if (OutBoundary())
		{
			ItemList.Remove(this);
		}
	}

	public override bool OutBoundary()
	{
		if (OriginalPosition.X < 0f)
		{
			OriginalPosition = new PointF(0f, OriginalPosition.Y);
		}
		else if (OriginalPosition.X > (float)BoundRect.Width)
		{
			OriginalPosition = new PointF(BoundRect.Width, OriginalPosition.Y);
		}
		if (Time < 30)
		{
			return false;
		}
		if (Position.Y > (float)(BoundRect.Bottom + 30))
		{
			return true;
		}
		return false;
	}
}
