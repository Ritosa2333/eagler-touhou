using System.Drawing;

namespace Shooting;

internal class Boss_Kage01 : BaseBossTouhou
{
	public Boss_Kage01(StageDataPackage StageData)
		: base(StageData)
	{
		TxtureObjects = new TextureObject[3, 4]
		{
			{
				TextureObjectDictionary["BossKage-00"],
				TextureObjectDictionary["BossKage-01"],
				TextureObjectDictionary["BossKage-02"],
				TextureObjectDictionary["BossKage-03"]
			},
			{
				TextureObjectDictionary["BossKage-10"],
				TextureObjectDictionary["BossKage-11"],
				TextureObjectDictionary["BossKage-12"],
				TextureObjectDictionary["BossKage-13"]
			},
			{
				TextureObjectDictionary["BossKage-20"],
				TextureObjectDictionary["BossKage-21"],
				TextureObjectDictionary["BossKage-22"],
				TextureObjectDictionary["BossKage-23"]
			}
		};
		TxtureObject = TxtureObjects[0, 0];
		OriginalPosition = new PointF(BoundRect.Width / 2, 150f);
		DestPoint = new Point(BoundRect.Width / 2, 150);
		MaxHP = 10000;
		HealthPoint = MaxHP;
		Life = 5;
		Region = 20;
		Velocity = 4f;
		DirectionDegree = 90.0;
		SpellTime = 2400;
		LifeTime = 100000;
		OnSpell = false;
		SetBossLifeLineTex();
		LoadArmon(".\\CS\\St04\\关底Boss\\ctrl.csv");
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
		case 5:
			Ctrl5();
			break;
		case 4:
			Ctrl4();
			break;
		case 3:
			Ctrl3();
			break;
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

	public override void TextureCtrl()
	{
		int num = (Mirrored ? IndexY : (-IndexY));
		if (OnAction > 0)
		{
			IndexY = 2;
			IndexX += 0.125f;
			OnAction--;
		}
		else if (Vx < -0.05f)
		{
			Mirrored = false;
			IndexY = 1;
			if (Vx < -1f)
			{
				if (IndexX < 2f)
				{
					IndexX += 0.125f;
				}
				else
				{
					IndexX = 2 + Time % 16 / 8;
				}
			}
			else if (Vx < -0.5f)
			{
				IndexX = 1f;
			}
			else
			{
				IndexX = 0f;
			}
		}
		else if (Vx > 0.05f)
		{
			Mirrored = true;
			IndexY = 1;
			if (Vx > 1f)
			{
				if (IndexX < 2f)
				{
					IndexX += 0.125f;
				}
				else
				{
					IndexX = 2 + Time % 16 / 8;
				}
			}
			else if (Vx > 0.5f)
			{
				IndexX = 1f;
			}
			else
			{
				IndexX = 0f;
			}
		}
		else
		{
			Mirrored = false;
			IndexY = 0;
			IndexX = TimeMain % 32 / 8;
		}
		if (num != (Mirrored ? IndexY : (-IndexY)))
		{
			IndexX = 0f;
		}
		if (IndexX > 3f)
		{
			IndexX = 3f;
		}
		TxtureObject = TxtureObjects[IndexY, (int)IndexX];
	}

	public override void GiveEndEffect()
	{
		TransparentValueF = 255f;
		new Background2DRemover(StageData);
		Background3D.WarpEnabled = false;
		LifeTime = Time + 150;
		Region = 0;
		StageData.SoundPlay("se_tan01.wav");
		if (MyPlane.FscEnabled)
		{
			LifeTime = Time + 50;
			new EndBoss_FSC04(StageData, OriginalPosition, Color.Pink, Color.Violet);
		}
		else
		{
			LifeTime = Time + 150;
			new EndBoss_Touhou04(StageData, OriginalPosition, Color.Pink, Color.Violet);
		}
	}

	private void Ctrl5()
	{
		if (!OnSpell)
		{
			Armon = ArmonArray[0];
			if (Time == 1)
			{
				new Background2DRemover(StageData);
				DestPoint = new Point(BoundRect.Width / 2, 160);
			}
			else if (Time > 50)
			{
				RandomMove(138, 20, 2f, new Rectangle(BoundRect.Width / 2 - 45, 100, 90, 40));
			}
			if (Time == 60)
			{
				string fileName = (int)Difficulty switch
				{
					0 => ".\\CS\\St04\\关底Boss\\1非E.mbg", 
					1 => ".\\CS\\St04\\关底Boss\\1非N.mbg", 
					2 => ".\\CS\\St04\\关底Boss\\1非H.mbg", 
					3 => ".\\CS\\St04\\关底Boss\\1非L.mbg", 
					_ => ".\\CS\\St04\\关底Boss\\1非L.mbg", 
				};
				CS_Data cSData = StageData.LoadCS(fileName);
				new CSEmitterController(StageData, cSData).BossBinding = true;
			}
		}
		else if (Time == 1)
		{
			new BulletRemover2(StageData, OriginalPosition);
			new SpellCard_SSS04_01(StageData);
		}
	}

	private void Ctrl4()
	{
		if (!OnSpell)
		{
			Armon = ArmonArray[2];
			if (Time == 1)
			{
				new Background2DRemover(StageData);
				DestPoint = new Point(BoundRect.Width / 2, 160);
			}
			else if (Time > 50)
			{
				RandomMove(138, 20, 2f, new Rectangle(BoundRect.Width / 2 - 45, 100, 90, 40));
			}
			if (Time == 60)
			{
				string fileName = (int)Difficulty switch
				{
					0 => ".\\CS\\St04\\关底Boss\\2非E.mbg", 
					1 => ".\\CS\\St04\\关底Boss\\2非N.mbg", 
					2 => ".\\CS\\St04\\关底Boss\\2非H.mbg", 
					3 => ".\\CS\\St04\\关底Boss\\2非L.mbg", 
					_ => ".\\CS\\St04\\关底Boss\\2非L.mbg", 
				};
				CS_Data cSData = StageData.LoadCS(fileName);
				new CSEmitterController(StageData, cSData).BossBinding = true;
			}
		}
		else if (Time == 1)
		{
			new BulletRemover2(StageData, OriginalPosition);
			switch (MyPlane.Name)
			{
			case "Reimu":
				new SpellCard_SSS04_02A(StageData);
				break;
			case "Marisa":
				new SpellCard_SSS04_02B(StageData);
				break;
			case "Sanae":
				new SpellCard_SSS04_02C(StageData);
				break;
			case "Koishi":
				new SpellCard_SSS04_02D(StageData);
				break;
			default:
				new SpellCard_SSS04_02A(StageData);
				break;
			}
		}
	}

	private void Ctrl3()
	{
		if (!OnSpell)
		{
			if (Time == 1)
			{
				new Background2DRemover(StageData);
				DestPoint = new Point(BoundRect.Width / 2, 180);
			}
			if (Time == 100)
			{
				OnSpell = true;
				SpellTime = 3000;
			}
		}
		else if (Time == 101)
		{
			new BulletRemover2(StageData, OriginalPosition);
			switch (MyPlane.Name)
			{
			case "Reimu":
				new SpellCard_SSS04_03A(StageData);
				break;
			case "Marisa":
				new SpellCard_SSS04_03B(StageData);
				break;
			case "Sanae":
				new SpellCard_SSS04_03C(StageData);
				break;
			case "Koishi":
				new SpellCard_SSS04_03D(StageData);
				break;
			default:
				new SpellCard_SSS04_03A(StageData);
				break;
			}
		}
	}

	private void Ctrl2()
	{
		if (!OnSpell)
		{
			if (Time == 1)
			{
				new Background2DRemover(StageData);
				DestPoint = new Point(BoundRect.Width / 2, 180);
			}
			if (Time == 100)
			{
				OnSpell = true;
				SpellTime = 2100;
			}
		}
		else if (Time == 101)
		{
			new BulletRemover2(StageData, OriginalPosition);
			switch (MyPlane.Name)
			{
			case "Reimu":
				new SpellCard_SSS04_04A(StageData);
				break;
			case "Marisa":
				new SpellCard_SSS04_04B(StageData);
				break;
			case "Sanae":
				new SpellCard_SSS04_04C(StageData);
				break;
			case "Koishi":
				new SpellCard_SSS04_04D(StageData);
				break;
			default:
				new SpellCard_SSS04_04A(StageData);
				break;
			}
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
			if (Time == 100)
			{
				OnSpell = true;
				SpellTime = 1680;
			}
		}
		else if (Time == 101)
		{
			new BulletRemover2(StageData, OriginalPosition);
			new SpellCard_SSS04_05(StageData);
		}
	}
}
