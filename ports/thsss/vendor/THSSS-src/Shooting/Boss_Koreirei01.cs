using System.Drawing;

namespace Shooting;

internal class Boss_Koreirei01 : BaseBossTouhou
{
	public Boss_Koreirei01(StageDataPackage StageData)
		: base(StageData)
	{
		TxtureObjects = new TextureObject[3, 4]
		{
			{
				TextureObjectDictionary["BossKoreirei-00"],
				TextureObjectDictionary["BossKoreirei-01"],
				TextureObjectDictionary["BossKoreirei-02"],
				TextureObjectDictionary["BossKoreirei-03"]
			},
			{
				TextureObjectDictionary["BossKoreirei-10"],
				TextureObjectDictionary["BossKoreirei-11"],
				TextureObjectDictionary["BossKoreirei-12"],
				TextureObjectDictionary["BossKoreirei-13"]
			},
			{
				TextureObjectDictionary["BossKoreirei-20"],
				TextureObjectDictionary["BossKoreirei-21"],
				TextureObjectDictionary["BossKoreirei-22"],
				TextureObjectDictionary["BossKoreirei-23"]
			}
		};
		TxtureObject = TxtureObjects[0, 0];
		OriginalPosition = new PointF(30f, -50f);
		DestPoint = new Point(BoundRect.Width / 2, 120);
		MaxHP = 10000;
		HealthPoint = MaxHP;
		Life = 2;
		Region = 20;
		Velocity = 6f;
		DirectionDegree = 90.0;
		SpellTime = 1200;
		LifeTime = 100000;
		OnSpell = false;
		SpellcardHP = 0;
		SetBossLifeLineTex();
		new ItemGetter(StageData);
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
		case 2:
			Ctrl2();
			break;
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
			OnAction = 0;
			if (HealthPoint <= 0f && Life >= -1)
			{
				GiveItems();
				new ShootingStarShard(StageData, new PointF(BoundRect.Width / 2, 0f));
			}
			if (Life <= 0)
			{
				if (Life == 0)
				{
					Velocity = 0f;
					new BulletRemover2(StageData, OriginalPosition);
					HealthPoint = 0f;
					GiveEndEffect();
					Life = -1;
				}
				else if (Time == LifeTime - 100)
				{
					DestPoint = new Point(Ran.Next((int)OriginalPosition.X - 30, (int)OriginalPosition.X + 30), -200);
					Velocity = 5f;
					Background3D.WarpEnabled = false;
				}
			}
			else
			{
				new BulletRemover2(StageData, OriginalPosition);
				StageData.SoundPlay("se_tan00.wav");
				HealthPoint = MaxHP;
				Time = 0;
				OnSpell = false;
				DestPoint = new Point(BoundRect.Width / 2, 100);
				Velocity = 4f;
			}
		}
	}

	public override void GiveEndEffect()
	{
		new EndBoss_Touhou_Small(StageData, OriginalPosition, Color.Yellow, Color.OrangeRed);
		new Background2DRemover(StageData);
		Background3D.WarpEnabled = false;
		new LifeItem_Touhou(StageData, OriginalPosition);
	}

	private void Ctrl2()
	{
		if (!OnSpell)
		{
			if (Time == 1)
			{
				new Background2DRemover(StageData);
				DestPoint = new Point(BoundRect.Width / 2, 140);
			}
			else
			{
				RandomMove1(150, 4f);
			}
			if (Time == 90)
			{
				OnSpell = true;
			}
		}
		else if (Time == 91)
		{
			new BulletRemover2(StageData, OriginalPosition);
			Armon = 0.4f;
			new SpellCard_SSS03_01(StageData);
		}
	}

	private void Ctrl1()
	{
		if (!OnSpell)
		{
			if (Time == 1)
			{
				new Background2DRemover(StageData);
				DestPoint = new Point(BoundRect.Width / 2, 180);
			}
			else
			{
				RandomMove1(150, 4f);
			}
			if (Time == 100)
			{
				OnSpell = true;
				SpellTime = 2400;
			}
		}
		else if (Time == 101)
		{
			new BulletRemover2(StageData, OriginalPosition);
			Armon = 0.4f;
			new SpellCard_SSS03_02(StageData);
		}
	}
}
