using System;
using System.Drawing;

namespace Shooting;

public class BaseItem_Touhou : BaseItem
{
	public BaseItem_Touhou(StageDataPackage StageData, string textureName, PointF OriginalPosition)
		: base(StageData, textureName, OriginalPosition)
	{
		base.OriginalPosition = OriginalPosition;
		Velocity = 4f;
		Direction = 4.71238898038469;
		Angle = 0.0;
		Accelerate = (0f - Velocity) / 40f;
		AngularVelocity = 0.2f;
	}

	public override void Ctrl()
	{
		if (!Enabled)
		{
			return;
		}
		Time++;
		Move();
		Velocity += Accelerate;
		if (Velocity < 0f)
		{
			Velocity = 0f;
		}
		Angle += AngularVelocity;
		TransparentValueF += TransparentVelocity;
		if (Obtain)
		{
			Direction = Math.PI + Math.Atan2(Position.Y - MyPlane.Position.Y, Position.X - MyPlane.Position.X);
			Accelerate = 1f;
			if (Velocity > 10f)
			{
				Velocity = 10f;
			}
		}
		else if (Time == 40)
		{
			AngularVelocity = 0f;
			Angle = 0.0;
		}
		else if (Time > 40)
		{
			Direction = Math.PI / 2.0;
			Accelerate = 0.15f;
			if (Velocity > 1.5f)
			{
				Velocity = 1.5f;
			}
		}
		if (OriginalPosition.Y < 0f)
		{
			TransparentValueF = 255 + 2 * (int)OriginalPosition.Y;
		}
		else
		{
			TransparentValueF = 255f;
		}
		if (OutBoundary())
		{
			ItemList.Remove(this);
		}
	}

	public override void HitCheckAll()
	{
		if (Time >= 10)
		{
			if (HitCheck(MyPlane, 48f))
			{
				Obtain = true;
			}
			if (MyPlane.OriginalPosition.Y < 128f)
			{
				Obtain = true;
				Doubled = true;
			}
		}
	}
}
