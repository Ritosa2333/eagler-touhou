using System.Drawing;

namespace Shooting;

internal class Boss_Ami02 : BaseBossTouhou
{
	public Boss_Ami02(StageDataPackage StageData)
		: base(StageData)
	{
		TxtureObjects = new TextureObject[3, 4]
		{
			{
				TextureObjectDictionary["BossAmi-00"],
				TextureObjectDictionary["BossAmi-01"],
				TextureObjectDictionary["BossAmi-02"],
				TextureObjectDictionary["BossAmi-03"]
			},
			{
				TextureObjectDictionary["BossAmi-10"],
				TextureObjectDictionary["BossAmi-11"],
				TextureObjectDictionary["BossAmi-12"],
				TextureObjectDictionary["BossAmi-13"]
			},
			{
				TextureObjectDictionary["BossAmi-20"],
				TextureObjectDictionary["BossAmi-21"],
				TextureObjectDictionary["BossAmi-22"],
				TextureObjectDictionary["BossAmi-23"]
			}
		};
		TxtureObject = TxtureObjects[0, 0];
		OriginalPosition = new PointF(BoundRect.Width / 2, 150f);
		DestPoint = new Point(BoundRect.Width / 2, 150);
		MaxHP = 10000;
		HealthPoint = MaxHP;
		Life = 2;
		Region = 20;
		Velocity = 4f;
		DirectionDegree = 90.0;
		SpellTime = 2400;
		LifeTime = 100000;
		OnSpell = false;
		SetBossLifeLineTex();
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
		Region = 0;
		StageData.SoundPlay("se_tan01.wav");
		if (MyPlane.FscEnabled)
		{
			LifeTime = Time + 50;
			new EndBoss_FSC01(StageData, OriginalPosition, Color.YellowGreen, Color.Green);
		}
		else
		{
			LifeTime = Time + 150;
			new EndBoss_Touhou01(StageData, OriginalPosition, Color.YellowGreen, Color.Green);
		}
	}

	private void Ctrl2()
	{
		if (!OnSpell)
		{
			Armon = 0.3f;
			if (Time == 1)
			{
				new Background2DRemover(StageData);
				DestPoint = new Point(BoundRect.Width / 2, 150);
				Velocity = 4f;
			}
			else if (Time > 50)
			{
				RandomMove(346, 30, 2f, new Rectangle(BoundRect.Width / 2 - 60, 100, 120, 40));
			}
			if (Time == 10)
			{
				new EmitterSaveEnegy3D(StageData, OriginalPosition, Color.FromArgb(255, 255, 155));
				StageData.SoundPlay("se_ch02.wav");
			}
			if (Time == 60)
			{
				string fileName = (int)Difficulty switch
				{
					0 => ".\\CS\\St01\\关底Boss\\1非E.mbg", 
					1 => ".\\CS\\St01\\关底Boss\\1非N.mbg", 
					2 => ".\\CS\\St01\\关底Boss\\1非H.mbg", 
					3 => ".\\CS\\St01\\关底Boss\\1非L.mbg", 
					_ => ".\\CS\\St01\\关底Boss\\1非L.mbg", 
				};
				CS_Data cSData = StageData.LoadCS(fileName);
				new CSEmitterController(StageData, cSData)
				{
					BossBinding = true,
					BaseOriginalPosition = OriginalPosition
				};
			}
		}
		else if (Time == 1)
		{
			new BulletRemover2(StageData, OriginalPosition);
			new SpellCard_SSS01_02(StageData);
		}
	}

	private void Ctrl1()
	{
		if (!OnSpell)
		{
			Armon = 0.32f;
			if (Time == 1)
			{
				new Background2DRemover(StageData);
				DestPoint = new Point(BoundRect.Width / 2, 100);
				Velocity = 4f;
			}
			else if (Time > 150)
			{
				RandomMove(346, 30, 2f, new Rectangle(BoundRect.Width / 2 - 60, 100, 120, 40));
			}
			if (Time == 90)
			{
				new EmitterSaveEnegy3D(StageData, OriginalPosition, Color.FromArgb(255, 255, 155));
				StageData.SoundPlay("se_ch02.wav");
			}
			if (Time == 150)
			{
				string fileName = (int)Difficulty switch
				{
					0 => ".\\CS\\St01\\关底Boss\\2非E.mbg", 
					1 => ".\\CS\\St01\\关底Boss\\2非N.mbg", 
					2 => ".\\CS\\St01\\关底Boss\\2非H.mbg", 
					3 => ".\\CS\\St01\\关底Boss\\2非L.mbg", 
					_ => ".\\CS\\St01\\关底Boss\\2非L.mbg", 
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
			Armon = 0.86f;
			if (Time == 1)
			{
				new BulletRemover2(StageData, OriginalPosition);
				new SpellCard_SSS01_03(StageData);
			}
		}
	}
}
