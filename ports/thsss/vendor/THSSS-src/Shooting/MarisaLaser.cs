using System;
using System.Drawing;

namespace Shooting;

public class MarisaLaser : BaseEffect
{
	public MarisaLaser(StageDataPackage StageData, string textureName, PointF OriginalPosition, float v, double drctn)
		: base(StageData, textureName, new PointF(0f, 0f), v, drctn)
	{
		base.StageData = StageData;
		base.OriginalPosition = OriginalPosition;
		Damage = 4;
		Region = 128;
		MaxVelocity = 1000f;
		MinVelocity = -1000f;
		Velocity = v;
		LifeTime = 5;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		Velocity = 192f;
		bool flag = false;
		for (int num = EnemyPlaneList.Count - 1; num >= 0; num--)
		{
			if (HitCheck(EnemyPlaneList[num]))
			{
				flag = true;
				if (MyPlane.EnchantmentState == EnchantmentType.Red)
				{
					EnemyPlaneList[num].HealthPoint -= (float)Damage * 1.25f;
				}
				else
				{
					EnemyPlaneList[num].HealthPoint -= Damage;
				}
				if (EnemyPlaneList[num].HealthPoint < 10000f)
				{
					MyPlane.Score += 10L;
				}
				if (EnemyPlaneList[num].HealthPoint <= 0f)
				{
					EnemyPlaneList[num].GiveEndEffect();
					EnemyPlaneList[num].GiveItems();
					EnemyPlaneList.RemoveAt(num);
					StageData.SoundPlay("se_enep00.wav", OriginalPosition.X / (float)BoundRect.Width);
					break;
				}
			}
		}
		if (Boss != null && HitCheck(Boss))
		{
			flag = true;
			if (Boss.Time > Boss.UnmatchedTime)
			{
				if (MyPlane.EnchantmentState == EnchantmentType.Red)
				{
					Boss.HealthPoint -= (float)Damage * 1.25f * (1f - Boss.Armon);
				}
				else
				{
					Boss.HealthPoint -= (float)Damage * (1f - Boss.Armon);
				}
			}
			MyPlane.Score += 10L;
			if (Boss.HealthPoint <= 200f)
			{
				StageData.SoundPlay("se_damage01.wav");
			}
		}
		if (flag)
		{
			Damage = 0;
		}
	}

	public override bool HitCheck(BaseObject Target)
	{
		double distance = GetDistance(Target);
		double num = GetDirection(Target);
		double num2 = distance * Math.Cos((float)(Direction - num));
		double num3 = distance * Math.Sin((float)(Direction - num));
		double num4 = (float)TxtureObject.Width * ScaleWidth * 0.5f;
		double num5 = (float)TxtureObject.Height * ScaleLength * 0.5f;
		if (num3 * num3 / num5 / num5 + (num2 - 256.0 + 64.0 - (double)((float)TxtureObject.Height * ScaleLength / 2f)) * (num2 - 256.0 + 64.0 - (double)((float)TxtureObject.Height * ScaleLength / 2f)) / num4 / num4 < 1.0)
		{
			return true;
		}
		return false;
	}

	public override void Show()
	{
		int num = TimeMain % 8 * 8;
		SizeF destinationSize = new SizeF((float)(TxtureObject.Width - 64) * ScaleWidth, (float)TxtureObject.Height * ScaleLength);
		SpriteMain.Draw2D(TxtureObject.TXTure, new Rectangle(TxtureObject.PosRect.X + 64 - num, TxtureObject.PosRect.Y, TxtureObject.PosRect.Width - 64, TxtureObject.PosRect.Height), destinationSize, new PointF((TxtureObject.Width - 64) / 2, TxtureObject.Height / 2), (float)(Direction - Math.PI / 2.0 + Angle), Position, Color.FromArgb(TransparentValue, ColorValue));
	}

	public override void ShowRegion()
	{
		SpriteMain.Draw2D(TextureObjectDictionary["Region"], (float)(TxtureObject.Width - 64) * 1f / (float)TextureObjectDictionary["Region"].Width * ScaleWidth, (float)TxtureObject.Height * 1f / (float)TextureObjectDictionary["Region"].Height * ScaleLength, (float)(Direction - Math.PI / 2.0 + Angle), Position, Color.White, Mirrored);
	}
}
