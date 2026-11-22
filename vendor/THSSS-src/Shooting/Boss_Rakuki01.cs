using System.Drawing;

namespace Shooting;

internal class Boss_Rakuki01 : BaseBossTouhou
{
	public Boss_Rakuki01(StageDataPackage StageData)
		: base(StageData)
	{
		TxtureObjects = new TextureObject[3, 4]
		{
			{
				TextureObjectDictionary["BossRakuki-00"],
				TextureObjectDictionary["BossRakuki-01"],
				TextureObjectDictionary["BossRakuki-02"],
				TextureObjectDictionary["BossRakuki-03"]
			},
			{
				TextureObjectDictionary["BossRakuki-10"],
				TextureObjectDictionary["BossRakuki-11"],
				TextureObjectDictionary["BossRakuki-12"],
				TextureObjectDictionary["BossRakuki-13"]
			},
			{
				TextureObjectDictionary["BossRakuki-20"],
				TextureObjectDictionary["BossRakuki-21"],
				TextureObjectDictionary["BossRakuki-22"],
				TextureObjectDictionary["BossRakuki-23"]
			}
		};
		TxtureObject = TxtureObjects[0, 0];
		OriginalPosition = new PointF(30f, -50f);
		DestPoint = new Point(BoundRect.Width / 2, 150);
		MaxHP = 10000;
		HealthPoint = MaxHP;
		Life = 1;
		Region = 20;
		Velocity = 6f;
		DirectionDegree = 90.0;
		SpellTime = 1200;
		LifeTime = 100000;
		OnSpell = false;
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
		new Background2DRemover(StageData);
		Background3D.WarpEnabled = false;
		LifeTime = Time + 150;
		Region = 0;
		StageData.SoundPlay("se_tan01.wav");
		new SpellItem_Touhou(StageData, OriginalPosition);
	}

	private void Ctrl1()
	{
		if (!OnSpell)
		{
			Armon = 0f;
			if (Time == 1)
			{
				DestPoint = new Point(BoundRect.Width / 2, 120);
			}
			else if (Time > 100)
			{
				RandomMove(300, 30, 2f, new Rectangle(BoundRect.Width / 2 - 60, 100, 120, 40));
			}
			if (Time == 60)
			{
				string fileName = (int)Difficulty switch
				{
					0 => ".\\CS\\St02\\道中Boss\\1非E.mbg", 
					1 => ".\\CS\\St02\\道中Boss\\1非N.mbg", 
					2 => ".\\CS\\St02\\道中Boss\\1非H.mbg", 
					3 => ".\\CS\\St02\\道中Boss\\1非L.mbg", 
					_ => ".\\CS\\St02\\道中Boss\\1非L.mbg", 
				};
				CS_Data cSData = StageData.LoadCS(fileName);
				new CSEmitterController(StageData, cSData)
				{
					BossBinding = true,
					BaseOriginalPosition = OriginalPosition
				};
			}
		}
		else
		{
			Armon = 0.8f;
			if (Time == 1)
			{
				new BulletRemover2(StageData, OriginalPosition);
				Boss.DestPoint = new Point(BoundRect.Width / 2, 120);
				Boss.Velocity = 4f;
				new SpellCard_SSS02_01(StageData);
			}
		}
	}
}
