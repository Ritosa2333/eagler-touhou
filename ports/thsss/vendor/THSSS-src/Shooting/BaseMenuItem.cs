using System;
using System.Drawing;

namespace Shooting;

public class BaseMenuItem : BaseObject
{
	public bool Selected { get; set; }

	public string Name { get; set; }

	public bool OnRemove { get; set; }

	public int TwinkleTime { get; set; }

	public int VibrateTime { get; set; }

	public float OffsetX { get; set; }

	public float OffsetY { get; set; }

	public bool Vibrateable { get; set; }

	public bool Twinkleable { get; set; }

	public bool UnSelectVisible { get; set; }

	public BaseMenuItem()
	{
	}

	public BaseMenuItem(StageDataPackage StageData, string textureName)
	{
		base.StageData = StageData;
		TxtureObject = TextureObjectDictionary[textureName];
		Name = textureName;
		MaxVelocity = 10f;
		UnSelectVisible = true;
		Vibrateable = true;
		Twinkleable = true;
		Selected = false;
	}

	public override void Move()
	{
		float num = (float)GetDistance(DestPoint);
		if (num > 0f)
		{
			Velocity = (float)Math.Sqrt(num) / 1f;
		}
		else
		{
			Velocity = 0f;
		}
		Direction = GetDirection(DestPoint);
		base.Move();
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Selected)
		{
			double num = 1.0 + Math.Sin((float)Time / 5f);
			if (!Twinkleable)
			{
				num = 2.0;
			}
			ColorValue = Color.FromArgb(220 + (int)(12.0 * num), 220 + (int)(12.0 * num), 220 + (int)(12.0 * num));
			TransparentValueF = 255f;
		}
		else if (!UnSelectVisible)
		{
			TransparentVelocity = -25f;
		}
		else
		{
			ColorValue = Color.FromArgb(150, 150, 150);
			TransparentValueF = 255f;
		}
		if (Time < VibrateTime)
		{
			OffsetX = Ran.Next(-2, 3);
			OffsetY = Ran.Next(-2, 3);
		}
		else
		{
			OffsetX = 0f;
			OffsetY = 0f;
		}
		if (Time < TwinkleTime)
		{
			if (Time % 4 == 0)
			{
				TransparentValueF = 255f;
			}
			else if (Time % 4 == 2)
			{
				TransparentValueF = 0f;
			}
		}
		if (OnRemove)
		{
			MaxTransparent -= 15;
		}
	}

	public virtual void Select()
	{
		Time = 0;
		Selected = true;
		if (Vibrateable)
		{
			VibrateTime = Time + 10;
		}
		TwinkleTime = 0;
	}

	public virtual void Click()
	{
		Time = 0;
		VibrateTime = 0;
		TwinkleTime = Time + 30;
	}

	public override void Show()
	{
		SizeF destinationSize = new SizeF((float)TxtureObject.Width * Scale, (float)TxtureObject.Height * Scale);
		SpriteMain.Draw2D(TxtureObject.TXTure, TxtureObject.PosRect, destinationSize, new PointF(0f, 0f), (float)Angle, new PointF(Position.X - OffsetX, Position.Y - OffsetY), Color.FromArgb(TransparentValue, ColorValue));
	}
}
