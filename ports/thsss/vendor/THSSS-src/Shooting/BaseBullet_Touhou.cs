using System;
using System.Drawing;

namespace Shooting;

public class BaseBullet_Touhou : BaseObject_CS
{
	public int Layer { get; set; }

	public byte ColorType { get; set; }

	public int SizeValue { get; set; }

	public byte Reflect { get; set; }

	public bool Grazed { get; set; }

	public BaseBullet_Touhou(StageDataPackage StageData)
		: base(StageData)
	{
	}

	public BaseBullet_Touhou(StageDataPackage StageData, string textureName, PointF OriginalPosition, float Velocity, double Direction, byte ColorType)
		: this(StageData, textureName, OriginalPosition, Velocity, Direction)
	{
		this.ColorType = ColorType;
		BeginningEffect = true;
	}

	public BaseBullet_Touhou(StageDataPackage StageData, string textureName, PointF OriginalPosition, float Velocity, double Direction)
		: base(StageData, textureName, OriginalPosition, Velocity, Direction)
	{
		BulletList.Add(this);
		base.OriginalPosition = OriginalPosition;
		AngleDegree = 180.0;
		SizeValue = 32;
		SetRegion(textureName);
	}

	public void SetRegion(string textureName)
	{
		if (textureName != null)
		{
			char[] separator = new char[1] { '_' };
			switch (textureName.Split(separator)[0])
			{
			case "bullet0":
				Region = 1;
				break;
			case "bullet1":
				Region = 1;
				break;
			case "bullet2":
				Region = 2;
				break;
			case "bullet3":
				Region = 2;
				break;
			case "bullet4":
				Region = 1;
				break;
			case "bullet5":
				Region = 1;
				break;
			case "bullet6":
				Region = 1;
				break;
			case "bullet7":
				Region = 1;
				break;
			case "bullet8":
				Region = 1;
				break;
			case "bullet9":
				Region = 1;
				break;
			case "bullet10":
				Region = 2;
				break;
			case "bullet11":
				Region = 2;
				break;
			case "bullet12":
				Region = 1;
				break;
			case "bullet13":
				Region = 3;
				break;
			case "bullet15":
				Region = 1;
				break;
			case "bullet20":
				Region = 6;
				break;
			case "bullet21":
				Region = 6;
				break;
			case "bullet22":
				Region = 3;
				break;
			case "bullet23":
				Region = 3;
				break;
			case "bullet24":
				Region = 3;
				break;
			case "bullet26":
				Region = 16;
				AngularVelocityDegree = 10f;
				Active = true;
				SizeValue = 80;
				OutsideRegion = 32;
				break;
			case "bullet50":
				Region = 12;
				Active = true;
				SizeValue = 64;
				OutsideRegion = 64;
				break;
			case "bullet40":
				Region = 6;
				SizeValue = 32;
				OutsideRegion = 32;
				break;
			case "bullet46":
				Region = 1;
				break;
			case "bullet61":
				Region = 2;
				break;
			case "bullet62":
				Region = 3;
				break;
			case "bullet64":
				Region = 3;
				break;
			case "bullet65":
				Region = 3;
				break;
			case "bullet66":
				Region = 3;
				break;
			case "bullet67":
				Region = 3;
				break;
			}
		}
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Time == 1 && BeginningEffect)
		{
			new ParticleBeginBullet(StageData, Position, ColorType).Active = Active;
		}
		if (Time == LifeTime && EndingEffect)
		{
			GiveEndEffect();
		}
		if (OutBoundary())
		{
			BulletList.Remove(this);
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
			return base.OutBoundary();
		}
		if (OriginalPosition.X < 0f)
		{
			Reflect--;
			Direction -= 2.0 * (Direction - Math.PI / 2.0);
			OriginalPosition = new PointF(0f - OriginalPosition.X, OriginalPosition.Y);
		}
		else if (OriginalPosition.X >= (float)BoundRect.Width)
		{
			Reflect--;
			Direction -= 2.0 * (Direction - Math.PI / 2.0);
			OriginalPosition = new PointF((float)(2 * BoundRect.Width) - OriginalPosition.X, OriginalPosition.Y);
		}
		if (OriginalPosition.Y < 20f && Reflect > 0)
		{
			Reflect--;
			Direction = 0.0 - Direction;
			OriginalPosition = new PointF(OriginalPosition.X, 20f - OriginalPosition.Y);
		}
		return false;
	}

	public override void HitCheckAll()
	{
		if (!MyPlane.HitEnabled)
		{
			return;
		}
		if (HitCheck(MyPlane))
		{
			MyPlane.PreMiss();
			if (!UnRemoveable)
			{
				GiveEndEffect();
				BulletList.Remove(this);
			}
		}
		else if (!Grazed && HitCheck(MyPlane, 24f / ScaleLength + (float)Region))
		{
			Grazed = true;
			MyPlane.Graze++;
			if (MyPlane.Graze % 10 == 0)
			{
				MyPlane.HighItemScore += 10;
			}
			StageData.SoundPlay("se_graze.wav");
			new BaseEffect(StageData, "GrazeG", MyPlane.Position, 5f, Ran.NextDouble() * Math.PI * 2.0)
			{
				Scale = 1f,
				TransparentVelocity = -20f,
				ScaleVelocity = -0.05f,
				AngleWithDirection = false
			};
		}
	}

	public override void GiveEndEffect()
	{
		new ParticleEndBullet(StageData, OriginalPosition, ColorType, SizeValue);
	}

	public override void GiveItems()
	{
		new StarItem_Touhou(StageData, OriginalPosition);
	}
}
