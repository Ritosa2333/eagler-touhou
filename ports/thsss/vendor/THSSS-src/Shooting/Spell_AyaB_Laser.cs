using System;
using System.Drawing;

namespace Shooting;

public class Spell_AyaB_Laser : BaseSpell
{
	private BaseObject Target;

	public Spell_AyaB_Laser(StageDataPackage StageData, PointF OriginalPosition, bool Left)
	{
		Spell_AyaB_Laser spell_AyaB_Laser = this;
		base.StageData = StageData;
		SpellList.Add(this);
		base.OriginalPosition = OriginalPosition;
		Damage = 50;
		Region = 25;
		LifeTime = 20;
		Velocity = 50f;
		ScaleLength = 2f;
		ScaleWidth = 1f;
		TxtureObject = TextureObjectDictionary["Laser"];
		if (Boss != null && Boss.OriginalPosition.X > 0f && Boss.OriginalPosition.X < (float)BoundRect.Width && Boss.OriginalPosition.Y > 0f && Boss.OriginalPosition.Y < (float)BoundRect.Height)
		{
			Target = Boss;
		}
		if (Target == null)
		{
			double dist = 500.0;
			EnemyPlaneList.ForEach((BaseEnemyPlane x) =>
			{
				double distance = spell_AyaB_Laser.GetDistance(x);
				if (distance < dist)
				{
					spell_AyaB_Laser.Target = x;
					dist = distance;
				}
			});
		}
		if (Target == null)
		{
			Target = new BaseObject
			{
				OriginalPosition = new PointF(OriginalPosition.X + (float)Ran.Next(-80, 80), OriginalPosition.Y + (float)Ran.Next(-80, 80))
			};
			if (Target.OriginalPosition.Y < 0f)
			{
				Target.OriginalPosition = new PointF(Target.OriginalPosition.X, 0f);
			}
			else if (Target.OriginalPosition.Y > (float)BoundRect.Height)
			{
				Target.OriginalPosition = new PointF(Target.OriginalPosition.X, BoundRect.Height);
			}
		}
		base.OriginalPosition = new PointF(Left ? (-150) : (BoundRect.Width + 150), Ran.Next((int)Target.OriginalPosition.Y - 80, (int)Target.OriginalPosition.Y + 80));
		Direction = GetDirection(Target);
		SpellList[0].OriginalPosition = Target.OriginalPosition;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Target != null)
		{
			if (Target.HealthPoint <= 0f)
			{
				Target = null;
			}
			if (OutBoundary())
			{
				Target = null;
				MyPlane.Time = MyPlane.UnmatchedTime - 90;
			}
		}
	}

	public override bool HitCheck(BaseObject Target)
	{
		double distance = GetDistance(Target);
		double num = GetDirection(Target);
		double num2 = distance * Math.Cos((float)(Direction - num));
		double num3 = distance * Math.Sin((float)(Direction - num));
		double num4 = (float)Region * ScaleWidth;
		double num5 = (float)TxtureObject.Height * ScaleLength * 0.45f;
		if (num3 * num3 / num4 / num4 + (num2 - (double)((float)TxtureObject.Height * ScaleLength / 2f)) * (num2 - (double)((float)TxtureObject.Height * ScaleLength / 2f)) / num5 / num5 < 1.0)
		{
			return true;
		}
		return false;
	}

	public override void Show()
	{
		SizeF destinationSize = new SizeF((float)TxtureObject.Width * ScaleWidth, (float)TxtureObject.Height * ScaleLength);
		SpriteMain.Draw2D(TxtureObject.TXTure, TxtureObject.PosRect, destinationSize, new PointF(TxtureObject.Width / 2, 0f), (float)(Direction - Math.PI / 2.0 + Angle), Position, Color.FromArgb(TransparentValue, ColorValue));
	}

	public override void ShowRegion()
	{
		PointF originalPosition = OriginalPosition;
		OriginalPosition = new PointF(OriginalPosition.X + (float)TxtureObject.Height * ScaleLength * 0.5f * (float)Math.Cos(Direction), OriginalPosition.Y + (float)TxtureObject.Height * ScaleLength * 0.5f * (float)Math.Sin(Direction));
		SpriteMain.Draw2D(TextureObjectDictionary["Region"], 2f * (float)Region / (float)TextureObjectDictionary["Region"].Width * ScaleWidth, (float)TxtureObject.Height * 0.9f / (float)TextureObjectDictionary["Region"].Height * ScaleLength, (float)(Direction - Math.PI / 2.0 + Angle), Position, Color.White, Mirrored);
		OriginalPosition = originalPosition;
	}
}
