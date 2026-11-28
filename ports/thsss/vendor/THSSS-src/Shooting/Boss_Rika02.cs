using System.Drawing;

namespace Shooting;

internal class Boss_Rika02 : BaseBossTouhou
{
	public Boss_Rika02(StageDataPackage StageData)
		: base(StageData)
	{
		TxtureObjects = new TextureObject[3, 4]
		{
			{
				TextureObjectDictionary["BossRika-00"],
				TextureObjectDictionary["BossRika-01"],
				TextureObjectDictionary["BossRika-02"],
				TextureObjectDictionary["BossRika-03"]
			},
			{
				TextureObjectDictionary["BossRika-10"],
				TextureObjectDictionary["BossRika-11"],
				TextureObjectDictionary["BossRika-12"],
				TextureObjectDictionary["BossRika-13"]
			},
			{
				TextureObjectDictionary["BossRika-20"],
				TextureObjectDictionary["BossRika-21"],
				TextureObjectDictionary["BossRika-22"],
				TextureObjectDictionary["BossRika-23"]
			}
		};
		TxtureObject = TxtureObjects[0, 0];
		OriginalPosition = new PointF(BoundRect.Width / 2, 120f);
		DestPoint = new Point(BoundRect.Width / 2, 120);
		MaxHP = 10000;
		HealthPoint = MaxHP;
		Life = 1;
		Region = 20;
		Velocity = 4f;
		DirectionDegree = 90.0;
		SpellTime = 3600;
		LifeTime = 100000;
		OnSpell = false;
		SetBossLifeLineTex();
		LoadArmon(".\\CS\\StEx\\关底Boss\\ctrl.csv");
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (!OnSpell)
		{
			Armon = 0f;
		}
		switch (Life)
		{
		case 1:
			Ctrl1();
			break;
		case 0:
			Velocity = 0f;
			DestPoint = new Point(Ran.Next(100, BoundRect.Width - 100), Ran.Next(70, 200));
			break;
		}
		MoveToPoint(DestPoint);
		if (!OnSpell)
		{
			if (HealthPoint < (float)SpellcardHP || Time > SpellTime)
			{
				HealthPoint = SpellcardHP;
				Time = 0;
				OnSpell = true;
				new ShootingStarShard(StageData, new PointF(BoundRect.Width / 2, 0f));
			}
		}
		else
		{
			if (!(HealthPoint <= 0f) && Time <= SpellTime)
			{
				return;
			}
			Life--;
			if (HealthPoint <= 0f && Life >= -1)
			{
				GiveItems();
				new ShootingStarShard(StageData, new PointF(BoundRect.Width / 2, 0f));
			}
			if (Life <= 0)
			{
				if (Life == 0)
				{
					DestPoint = new Point(Ran.Next((int)OriginalPosition.X - 30, (int)OriginalPosition.X + 30), Ran.Next((int)OriginalPosition.Y + 30, (int)OriginalPosition.Y + 50));
					Velocity = 0.5f;
					new BulletRemover2(StageData, OriginalPosition);
					HealthPoint = 0f;
					GiveEndEffect();
					Life = -1;
				}
			}
			else
			{
				new BulletRemover2(StageData, OriginalPosition);
				StageData.SoundPlay("se_tan00.wav");
				HealthPoint = MaxHP;
				Time = 0;
				OnSpell = false;
				DestPoint = new Point(BoundRect.Width / 2, 120);
				Velocity = 4f;
			}
		}
	}

	public override void GiveEndEffect()
	{
		TransparentValueF = 255f;
		new Background2DRemover(StageData);
		Background3D.WarpEnabled = false;
		LifeTime = Time + 150;
		Region = 0;
		StageData.SoundPlay("se_tan01.wav");
		new EndBoss_TouhouEx(StageData, OriginalPosition, Color.White, Color.Blue);
	}

	private void Ctrl1()
	{
		if (!OnSpell)
		{
			if (Time == 1)
			{
				OnSpell = true;
				SpellTime = 3000;
			}
		}
		else if (Time == 2)
		{
			new SpellCard_SSSEx_FSC(StageData);
		}
	}
}
