using System;
using System.Drawing;

namespace Shooting;

public class Bullet_BendLaser : BaseBullet_Touhou
{
	private Particle3D_BendLaser PL;

	public Bullet_BendLaser(StageDataPackage StageData)
		: base(StageData)
	{
	}

	public Bullet_BendLaser(StageDataPackage StageData, PointF OriginalPosition, float Velocity, double Direction, byte ColorType, int Length)
		: this(StageData, OriginalPosition, Velocity, Direction, ColorType, Length, 16)
	{
	}

	public Bullet_BendLaser(StageDataPackage StageData, PointF OriginalPosition, float Velocity, double Direction, byte ColorType, int Length, int Width)
		: this(StageData, null, OriginalPosition, Velocity, Direction, Length)
	{
		base.ColorType = ColorType;
		new ParticleBeginBullet(StageData, Position, ColorType)
		{
			Scale = 0.5f,
			Active = true
		};
		PL = new Particle3D_BendLaser(StageData, GhostingCount, Width, ColorType)
		{
			OriginalPosition = base.OriginalPosition,
			LastPoints = LastPoints
		};
	}

	public Bullet_BendLaser(StageDataPackage StageData, string textureName, PointF OriginalPosition, float Velocity, double Direction, int Length)
		: base(StageData, textureName, OriginalPosition, Velocity, Direction)
	{
		GhostingCount = Length;
		Region = 2;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Time == 1 && PL == null)
		{
			PL = new Particle3D_BendLaser(StageData, GhostingCount, (int)(ScaleLength * 16f), ColorType)
			{
				OriginalPosition = OriginalPosition,
				LastPoints = LastPoints
			};
		}
		PL.LastPoints = LastPoints;
		PL.TransparentValueF = TransparentValueF;
		if (Time % 8 == 0)
		{
			Grazed = false;
		}
	}

	public override bool OutBoundary()
	{
		if (LifeTime != 0 && Time > LifeTime)
		{
			PL.Dispose();
			StageData.Particle3D.ParticleList.Remove(PL);
			return true;
		}
		if (Reflect <= 0)
		{
			if (Time < 100)
			{
				return false;
			}
			int num = ((OutsideRegion >= 10) ? (OutsideRegion * 2) : 20);
			if ((float)(-num) < LastPoints[LastPoints.Length - 1].X && LastPoints[LastPoints.Length - 1].X < (float)(BoundRect.Width + num) && (float)(-num) < LastPoints[LastPoints.Length - 1].Y && LastPoints[LastPoints.Length - 1].Y < (float)(BoundRect.Height + num))
			{
				return false;
			}
			if ((float)(-num) < OriginalPosition.X && OriginalPosition.X < (float)(BoundRect.Width + num) && (float)(-num) < OriginalPosition.Y && OriginalPosition.Y < (float)(BoundRect.Height + num))
			{
				return false;
			}
			PL.Dispose();
			StageData.Particle3D.ParticleList.Remove(PL);
			return true;
		}
		if (OriginalPosition.X < 0f || OriginalPosition.X >= (float)BoundRect.Width)
		{
			Reflect--;
			Direction -= 2.0 * (Direction - Math.PI / 2.0);
		}
		if (OriginalPosition.Y < 20f)
		{
			Reflect--;
			Direction = 0.0 - Direction;
		}
		return false;
	}

	public override bool HitCheck(BaseObject Target, float Radius)
	{
		for (int i = 3; i < LastPoints.Length - 3; i++)
		{
			PointF pointF = LastPoints[i];
			if (Math.Sqrt((pointF.X - Target.OriginalPosition.X) * (pointF.X - Target.OriginalPosition.X) + (pointF.Y - Target.OriginalPosition.Y) * (pointF.Y - Target.OriginalPosition.Y)) - (double)Target.Region < (double)Radius)
			{
				return true;
			}
		}
		return false;
	}

	public override void GiveEndEffect()
	{
		for (int i = 0; i < LastPoints.Length; i += 8)
		{
			new ParticleEndBullet(StageData, LastPoints[i], ColorType, SizeValue);
		}
		StageData.Particle3D.ParticleList.Remove(PL);
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
