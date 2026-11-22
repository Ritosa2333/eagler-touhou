using System;
using System.Drawing;

namespace Shooting;

public class CharacterMenuItem : BaseMenuItem
{
	public PointF DestPoint1 { get; set; }

	public PointF DestPoint2 { get; set; }

	public CharacterMenuItem(StageDataPackage StageData, string textureName)
		: base(StageData, textureName)
	{
		DestPoint = DestPoint1;
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
		TransparentValueF += TransparentVelocity;
		if (Time == 1)
		{
			OriginalPosition = DestPoint2;
		}
		if (Selected)
		{
			Math.Sin((float)Time / 6f);
			ColorValue = Color.White;
			TransparentVelocity = 25f;
			DestPoint = DestPoint1;
		}
		else
		{
			ColorValue = Color.Gray;
			TransparentVelocity = -25f;
			DestPoint = DestPoint2;
		}
		if (Time < TwinkleTime)
		{
			if (Time % 4 == 0)
			{
				ColorValue = Color.White;
			}
			else if (Time % 4 == 2)
			{
				ColorValue = Color.Gray;
			}
		}
		if (OnRemove)
		{
			MaxTransparent -= 15;
		}
	}

	public override void Select()
	{
		Time = 1;
		Selected = true;
		VibrateTime = 0;
		TwinkleTime = 0;
	}

	public override void Click()
	{
		Time = 1;
		VibrateTime = 0;
		TwinkleTime = Time + 30;
	}
}
