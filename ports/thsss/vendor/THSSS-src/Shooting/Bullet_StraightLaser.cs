using System;
using System.Drawing;

namespace Shooting;

public class Bullet_StraightLaser : BaseBullet_Touhou
{
	public bool LaserHead { get; set; }

	private int HeadTransparent { get; set; }

	public Bullet_StraightLaser(StageDataPackage StageData)
		: base(StageData)
	{
	}

	public Bullet_StraightLaser(StageDataPackage StageData, PointF OriginalPosition, float Velocity, double Direction, byte ColorType, int Length)
		: this(StageData, OriginalPosition, Velocity, Direction, ColorType, Length, 16)
	{
	}

	public Bullet_StraightLaser(StageDataPackage StageData, PointF OriginalPosition, float Velocity, double Direction, byte ColorType, int Length, int Width)
		: this(StageData, "Laser0_" + ColorType, OriginalPosition, Velocity, Direction, Length)
	{
		base.ColorType = ColorType;
		Active = true;
		Angle = Math.PI / 2.0;
		ScaleLength = (float)Width / (float)TxtureObject.Height;
	}

	public Bullet_StraightLaser(StageDataPackage StageData, string textureName, PointF OriginalPosition, float Velocity, double Direction, int Length)
		: base(StageData, textureName, OriginalPosition, Velocity, Direction)
	{
		GhostingCount = Length;
		Region = 2;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		ScaleWidth = (float)GetDistance(LastPoints[LastPoints.Length - 1]) / (float)TxtureObject.Width;
		if (Time == 1)
		{
			new ParticleBeginBullet(StageData, Position, ColorType)
			{
				Scale = 0.5f,
				Active = true,
				LifeTime = GhostingCount
			};
		}
		if (Time % 8 == 0)
		{
			Grazed = false;
		}
		if (LaserHead)
		{
			HeadTransparent = Ran.Next(150, 255);
		}
	}

	public override bool OutBoundary()
	{
		if (LifeTime != 0 && Time > LifeTime)
		{
			return true;
		}
		if (Reflect <= 0)
		{
			if (Time < 100)
			{
				return false;
			}
			int num = ((Region >= 10) ? Region : 10);
			if ((float)(-num) < LastPoints[LastPoints.Length - 1].X && LastPoints[LastPoints.Length - 1].X < (float)(BoundRect.Width + num) && (float)(-num) < LastPoints[LastPoints.Length - 1].Y && LastPoints[LastPoints.Length - 1].Y < (float)(BoundRect.Height + num))
			{
				return false;
			}
			if ((float)(-num) < OriginalPosition.X && OriginalPosition.X < (float)(BoundRect.Width + num) && (float)(-num) < OriginalPosition.Y && OriginalPosition.Y < (float)(BoundRect.Height + num))
			{
				return false;
			}
			return true;
		}
		if (OriginalPosition.X < 0f || OriginalPosition.X >= (float)BoundRect.Width)
		{
			Reflect--;
			Bullet_StraightLaser bullet_StraightLaser = new Bullet_StraightLaser(StageData, new PointF(0f, 0f), 0f, 0.0, 0, 0);
			bullet_StraightLaser.Copy(this);
			bullet_StraightLaser.Direction -= 2.0 * (Direction - Math.PI / 2.0);
		}
		if (OriginalPosition.Y < 20f)
		{
			Reflect--;
			Bullet_StraightLaser bullet_StraightLaser2 = new Bullet_StraightLaser(StageData, new PointF(0f, 0f), 0f, 0.0, 0, 0);
			bullet_StraightLaser2.Copy(this);
			bullet_StraightLaser2.Direction = 0.0 - Direction;
		}
		return false;
	}

	public override bool HitCheck(BaseObject Target, float Radius)
	{
		for (int i = 3; i < LastPoints.Length - 3; i++)
		{
			PointF pointF = LastPoints[i];
			if (Math.Sqrt((pointF.X - Target.OriginalPosition.X) * (pointF.X - Target.OriginalPosition.X) + (pointF.Y - Target.OriginalPosition.Y) * (pointF.Y - Target.OriginalPosition.Y)) - (double)((float)Target.Region * Target.Scale) < (double)(Radius * ScaleLength))
			{
				return true;
			}
		}
		return false;
	}

	public override void GiveEndEffect()
	{
		for (int i = 0; i < LastPoints.Length; i++)
		{
			if (i % 8 == 0)
			{
				new ParticleEndBullet(StageData, LastPoints[i], ColorType, SizeValue);
			}
		}
	}

	public override void GiveItems()
	{
		int num = 5;
		int num2 = Region / num;
		for (int i = 0; i < LastPoints.Length; i += 8)
		{
			for (int j = 0; j <= num2; j++)
			{
				double num3 = Direction;
				for (int k = 0; k < j * 5 + 1; k++)
				{
					new StarItem_Touhou(OriginalPosition: new PointF((float)((double)LastPoints[i].X + (double)j * Math.Cos(num3)), (float)((double)LastPoints[i].Y + (double)j * Math.Sin(num3))), StageData: StageData);
					num3 += Math.PI * 2.0 / (double)(j * 6);
				}
			}
		}
	}

	public override void Show()
	{
		if (TxtureObject == null)
		{
			return;
		}
		PointF position = new PointF((OriginalPosition.X + LastPoints[LastPoints.Length - 1].X) / 2f, (OriginalPosition.Y + LastPoints[LastPoints.Length - 1].Y) / 2f);
		position = new PointF(position.X + (float)BoundRect.Left, position.Y + (float)BoundRect.Top);
		SpriteMain.Draw2D(TxtureObject, ScaleWidth, ScaleLength, (float)(Direction - Math.PI / 2.0 + Angle), position, Color.FromArgb(TransparentValue, ColorValue), Mirrored);
		if (LaserHead)
		{
			TextureObject txtureObject = TextureObjectDictionary["光点"];
			if (0 < ColorType && ColorType <= 2)
			{
				txtureObject = TextureObjectDictionary["FY_Particle6_0"];
			}
			else if (2 < ColorType && ColorType <= 4)
			{
				txtureObject = TextureObjectDictionary["FY_Particle4_0"];
			}
			else if (4 < ColorType && ColorType <= 8)
			{
				txtureObject = TextureObjectDictionary["FY_Particle5_0"];
			}
			SpriteMain.Draw2D(txtureObject, 0.4f, 0.4f, (float)(Direction - Math.PI / 2.0 + Angle + (double)((float)Time / 10f)), Position, Color.FromArgb(HeadTransparent, ColorValue), Mirrored);
		}
	}

	public override void ShowRegion()
	{
		PointF originalPosition = OriginalPosition;
		for (int i = 3; i < LastPoints.Length - 3; i++)
		{
			OriginalPosition = LastPoints[i];
			base.ShowRegion();
		}
		OriginalPosition = originalPosition;
	}
}
