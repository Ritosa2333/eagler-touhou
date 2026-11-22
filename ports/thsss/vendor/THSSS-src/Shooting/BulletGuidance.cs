using System;
using System.Drawing;

namespace Shooting;

public class BulletGuidance : BaseObject
{
	private bool targetFlag;

	public BaseObject Target { get; set; }

	public BulletGuidance(StageDataPackage StageData, string textureName, PointF Position, float Velocity, double Direction)
		: base(StageData, textureName, Position, Velocity, Direction)
	{
		Damage = 3;
		Region = 15;
		LifeTime = 100;
		MyBulletList.Add(this);
		if (Boss != null && Boss.OriginalPosition.X > 0f && Boss.OriginalPosition.X < (float)BoundRect.Width && Boss.OriginalPosition.Y > 0f && Boss.OriginalPosition.Y < (float)BoundRect.Height && Boss.HitEnabled)
		{
			Target = Boss;
		}
		GhostingCount = 5;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Target == null && !targetFlag)
		{
			double dist = 300.0;
			EnemyPlaneList.ForEach((BaseEnemyPlane x) =>
			{
				if (x.HitEnabled)
				{
					double distance = GetDistance(x);
					if (distance < dist)
					{
						Target = x;
						dist = distance;
					}
				}
			});
		}
		else
		{
			targetFlag = true;
		}
		if (Target != null)
		{
			if (Target.HealthPoint > 0f)
			{
				double num = GetDirection(Target);
				double num2 = Direction;
				if (num > Math.PI)
				{
					num -= Math.PI * 2.0;
				}
				if (num2 > Math.PI)
				{
					num2 -= Math.PI * 2.0;
				}
				double num3 = num - num2;
				if (num3 > Math.PI)
				{
					num3 -= Math.PI * 2.0;
				}
				if (Math.Abs(num3) > 0.019999999552965164 && GetDistance(Target) > 50.0)
				{
					Direction += num3 / 5.0;
				}
				else
				{
					Direction += num3;
				}
			}
			else
			{
				Target = null;
			}
		}
		if (OutBoundary())
		{
			MyBulletList.Remove(this);
		}
	}

	public override bool OutBoundary()
	{
		if (LifeTime != 0 && Time > LifeTime)
		{
			return true;
		}
		if (Time < 10)
		{
			return false;
		}
		int num = 2;
		if ((float)(-num) < OriginalPosition.X && OriginalPosition.X < (float)(BoundRect.Width + num) && (float)(-num) < OriginalPosition.Y && OriginalPosition.Y < (float)(BoundRect.Height + num))
		{
			return false;
		}
		return true;
	}

	public override void GiveEndEffect()
	{
		new ParticleBiger(StageData, null, Position, 2f, Direction)
		{
			Scale = 1.2f,
			LifeTime = 15,
			TxtureObject = TxtureObject,
			Angle = Angle
		};
	}
}
