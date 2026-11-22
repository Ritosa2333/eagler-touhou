using System;
using System.Drawing;

namespace Shooting;

public class Spell_AyaA : BaseSpell
{
	public Spell_AyaA(StageDataPackage StageData)
	{
		base.StageData = StageData;
		OriginalPosition = MyPlane.OriginalPosition;
		Damage = 10;
		Region = 30;
		LifeTime = 300;
		Direction = Math.PI / 2.0;
		SpellList.Add(this);
		new BackgroundBlack(StageData, Position);
		new BackgroundMySC(StageData, "76-4")
		{
			LifeTime = 350,
			MaxTransparent = 180,
			TransparentValueF = 0f,
			TransparentVelocity = 4f,
			ColorValue = Color.Green
		};
		new BackgroundMySC(StageData, "76-4")
		{
			LifeTime = 350,
			MaxTransparent = 150,
			TransparentValueF = 0f,
			TransparentVelocity = 4f,
			ColorValue = Color.Green,
			AngularVelocityDegree = -0.5f,
			Active = true
		};
		StageData.VibrateStart(LifeTime);
		MyPlane.HighSpeed = 2f;
		MyPlane.LowSpeed = 1f;
		TransparentValueF = 0f;
		TransparentVelocity = 4f;
		ScaleLength = 0.2f;
		ScaleVelocity = 0.05f;
		MaxScale = 2f;
	}

	public override void Move()
	{
		OriginalPosition = MyPlane.OriginalPosition;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Time < 16)
		{
			TxtureObject = TextureObjectDictionary["wind" + $"{Time % 16 / 2:00}"];
		}
		else if (Time < 284)
		{
			TxtureObject = TextureObjectDictionary["wind" + $"{7 + (Time - 16) % 36 / 2:00}"];
		}
		else
		{
			TxtureObject = TextureObjectDictionary["wind" + $"{25 + (Time - 300 + 16) % 16 / 2:00}"];
		}
		TxtureObject.OffsetY = 100;
		if (Time == 240)
		{
			TransparentVelocity = -4f;
		}
	}

	public override bool OutBoundary()
	{
		bool flag = base.OutBoundary();
		if (flag)
		{
			MyPlane.HighSpeed = 5f;
			MyPlane.LowSpeed = 2f;
		}
		return flag;
	}

	public override bool HitCheck(BaseObject Target)
	{
		if (Target.OriginalPosition.Y - OriginalPosition.Y < 2.8f * (Target.OriginalPosition.X - OriginalPosition.X) + 136f && Target.OriginalPosition.Y - OriginalPosition.Y < -2.8f * (Target.OriginalPosition.X - OriginalPosition.X) + 136f)
		{
			return true;
		}
		return false;
	}

	public override void HitCheckAll()
	{
		for (int num = BulletList.Count - 1; num >= 0; num--)
		{
			if (HitCheck(BulletList[num]) && !BulletList[num].UnRemoveable)
			{
				BulletList[num].GiveEndEffect();
				BulletList[num].GiveItems();
				BulletList.RemoveAt(num);
			}
		}
		for (int num2 = EnemyPlaneList.Count - 1; num2 >= 0; num2--)
		{
			if (HitCheck(EnemyPlaneList[num2]))
			{
				int num3 = Damage + (int)((400.0 - EnemyPlaneList[num2].GetDistance(MyPlane)) / 50.0);
				if (num3 < Damage)
				{
					num3 = Damage;
				}
				else if (num3 > 19)
				{
					num3 = 19;
				}
				EnemyPlaneList[num2].HealthPoint -= num3;
				if (EnemyPlaneList[num2].HealthPoint <= 0f)
				{
					EnemyPlaneList[num2].GiveEndEffect();
					EnemyPlaneList[num2].GiveItems();
					EnemyPlaneList.RemoveAt(num2);
					StageData.SoundPlay("se_enep00.wav");
					break;
				}
			}
		}
		if (Boss != null && Boss.Time > Boss.UnmatchedTime && HitCheck(Boss))
		{
			int num4 = Damage + (int)((400.0 - Boss.GetDistance(MyPlane)) / 40.0);
			if (num4 < Damage)
			{
				num4 = Damage;
			}
			else if (num4 > 19)
			{
				num4 = 19;
			}
			Boss.HealthPoint -= (int)((float)num4 * (1f - Boss.Armon));
			if (Boss.HealthPoint <= 200f)
			{
				StageData.SoundPlay("se_damage01.wav");
			}
		}
	}
}
