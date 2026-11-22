using System;
using System.Drawing;

namespace Shooting;

public class Bullet_RadialLaser : BaseBullet_Touhou
{
	private BaseEffect beginEffect;

	public int UnmatchedTime { get; set; }

	public Bullet_RadialLaser(StageDataPackage StageData)
		: base(StageData)
	{
	}

	public Bullet_RadialLaser(StageDataPackage StageData, PointF OriginalPosition, float Velocity, double Direction, byte ColorType, int Length)
		: this(StageData, OriginalPosition, Velocity, Direction, ColorType, Length, 16)
	{
	}

	public Bullet_RadialLaser(StageDataPackage StageData, PointF OriginalPosition, float Velocity, double Direction, byte ColorType, int Length, int Width)
		: this(StageData, "Laser0_" + ColorType, OriginalPosition, Velocity, Direction, Length)
	{
		base.ColorType = ColorType;
		Active = true;
		Angle = Math.PI / 2.0;
		MaxScaleL = (float)Width / (float)TxtureObject.Height;
	}

	public Bullet_RadialLaser(StageDataPackage StageData, string textureName, PointF OriginalPosition, float Velocity, double Direction, int Length)
		: base(StageData, textureName, OriginalPosition, Velocity, Direction)
	{
		UnRemoveable = true;
		Region = 2;
		ScaleVW = 0.5f;
		MaxScaleW = (float)Length / (float)TxtureObject.Width;
		ScaleWidth = MaxScaleW;
		MaxScaleL = 1f;
		ScaleLength = 0.2f;
		UnmatchedTime = 60;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Time == 1)
		{
			beginEffect = new ParticleBeginBullet(StageData, Position, ColorType)
			{
				Scale = MaxScaleW * 0.5f,
				Active = true,
				LifeTime = LifeTime
			};
			beginEffect.SetBinding(this);
		}
		if (Time == UnmatchedTime)
		{
			ScaleVL = 0.1f;
		}
		if (Time == LifeTime - 8)
		{
			TransparentVelocity = -MaxTransparent / 8;
			ScaleVL = (0f - ScaleLength) / 8f;
		}
		if (Time % 8 == 0)
		{
			Grazed = false;
		}
	}

	public override bool OutBoundary()
	{
		if (LifeTime != 0 && Time > LifeTime)
		{
			return true;
		}
		return false;
	}

	public override bool HitCheck(BaseObject Target, float Radius)
	{
		if (Time < UnmatchedTime)
		{
			return false;
		}
		double distance = GetDistance(Target);
		double num = GetDirection(Target);
		double num2 = distance * Math.Cos((float)(Direction - num));
		double num3 = distance * Math.Sin((float)(Direction - num));
		double num4 = Radius * ScaleLength + (float)Target.Region * Target.Scale;
		double num5 = ((float)TxtureObject.Width + Radius) * ScaleWidth * 0.45f + (float)Target.Region * Target.Scale;
		if (num3 * num3 / num4 / num4 + (num2 - (double)((float)TxtureObject.Width * ScaleWidth / 2f)) * (num2 - (double)((float)TxtureObject.Width * ScaleWidth / 2f)) / num5 / num5 < 1.0)
		{
			return true;
		}
		return false;
	}

	public override void GiveEndEffect()
	{
		beginEffect.LifeTime = beginEffect.Time + 1;
	}

	public override void Show()
	{
		if (TxtureObject != null)
		{
			SizeF destinationSize = new SizeF((float)TxtureObject.Width * ScaleWidth, (float)TxtureObject.Height * ScaleLength);
			SpriteMain.Draw2D(TxtureObject.TXTure, TxtureObject.PosRect, destinationSize, new PointF(0f, TxtureObject.Height / 2), (float)(Direction - Math.PI / 2.0 + Angle), Position, Color.FromArgb(TransparentValue, ColorValue));
		}
	}

	public override void ShowRegion()
	{
		PointF originalPosition = OriginalPosition;
		OriginalPosition = new PointF(OriginalPosition.X + (float)TxtureObject.Width * ScaleWidth * 0.5f * (float)Math.Cos(Direction), OriginalPosition.Y + (float)TxtureObject.Width * ScaleWidth * 0.5f * (float)Math.Sin(Direction));
		SpriteMain.Draw2D(TextureObjectDictionary["Region"], (float)TxtureObject.Width * 0.9f / (float)TextureObjectDictionary["Region"].Height * ScaleWidth, 2f * (float)Region / (float)TextureObjectDictionary["Region"].Width * ScaleLength, (float)(Direction - Math.PI / 2.0 + Angle), Position, Color.Red, Mirrored);
		OriginalPosition = originalPosition;
	}
}
