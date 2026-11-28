using System.Drawing;

namespace Shooting;

internal class Boss_Rika01 : BaseBossTouhou
{
	public Boss_Rika01(StageDataPackage StageData)
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
		Life = 10;
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
		case 10:
			Ctrl10();
			break;
		case 9:
			Ctrl9();
			break;
		case 8:
			Ctrl8();
			break;
		case 7:
			Ctrl7();
			break;
		case 6:
			Ctrl6();
			break;
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
			new EndBoss_FSCEx(StageData, OriginalPosition, Color.White, Color.Blue);
		}
		else
		{
			LifeTime = Time + 150;
			new EndBoss_TouhouEx(StageData, OriginalPosition, Color.White, Color.Blue);
		}
	}

	private void Ctrl10()
	{
		if (!OnSpell)
		{
			Armon = ArmonArray[0];
			if (Time == 1)
			{
				new Background2DRemover(StageData);
				DestPoint = new Point(BoundRect.Width / 2, 160);
			}
			else
			{
				_ = Time;
				_ = 50;
			}
			if (Time == 60)
			{
				string fileName = ".\\CS\\StEx\\关底Boss\\1非.mbg";
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
				new SpellCard_SSSEx_01A(StageData);
				break;
			case "Marisa":
				new SpellCard_SSSEx_01B(StageData);
				break;
			case "Sanae":
				new SpellCard_SSSEx_01C(StageData);
				break;
			case "Koishi":
				new SpellCard_SSSEx_01D(StageData);
				break;
			default:
				new SpellCard_SSSEx_01A(StageData);
				break;
			}
		}
	}

	private void Ctrl9()
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
				RandomMove(346, 30, 2f, new Rectangle(BoundRect.Width / 2 - 60, 100, 120, 40));
			}
			if (Time == 60)
			{
				string fileName = ".\\CS\\StEx\\关底Boss\\2非.mbg";
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
				new SpellCard_SSSEx_02A(StageData);
				break;
			case "Marisa":
				new SpellCard_SSSEx_02B(StageData);
				break;
			case "Sanae":
				new SpellCard_SSSEx_02C(StageData);
				break;
			case "Koishi":
				new SpellCard_SSSEx_02D(StageData);
				break;
			default:
				new SpellCard_SSSEx_02A(StageData);
				break;
			}
		}
	}

	private void Ctrl8()
	{
		if (!OnSpell)
		{
			Armon = ArmonArray[4];
			if (Time == 1)
			{
				new Background2DRemover(StageData);
				DestPoint = new Point(BoundRect.Width / 2, 160);
			}
			else
			{
				_ = Time;
				_ = 150;
			}
			if (Time == 160)
			{
				string fileName = ".\\CS\\StEx\\关底Boss\\3非.mbg";
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
				new SpellCard_SSSEx_03A(StageData);
				break;
			case "Marisa":
				new SpellCard_SSSEx_03B(StageData);
				break;
			case "Sanae":
				new SpellCard_SSSEx_03C(StageData);
				break;
			case "Koishi":
				new SpellCard_SSSEx_03D(StageData);
				break;
			default:
				new SpellCard_SSSEx_03A(StageData);
				break;
			}
		}
	}

	private void Ctrl7()
	{
		if (!OnSpell)
		{
			Armon = ArmonArray[6];
			if (Time == 1)
			{
				new Background2DRemover(StageData);
				DestPoint = new Point(BoundRect.Width / 2, 160);
			}
			else if (Time > 50)
			{
				RandomMove(346, 30, 2f, new Rectangle(BoundRect.Width / 2 - 60, 100, 120, 40));
			}
			if (Time == 60)
			{
				string fileName = ".\\CS\\StEx\\关底Boss\\4非.mbg";
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
				new SpellCard_SSSEx_04A(StageData);
				break;
			case "Marisa":
				new SpellCard_SSSEx_04B(StageData);
				break;
			case "Sanae":
				new SpellCard_SSSEx_04C(StageData);
				break;
			case "Koishi":
				new SpellCard_SSSEx_04D(StageData);
				break;
			default:
				new SpellCard_SSSEx_04A(StageData);
				break;
			}
		}
	}

	private void Ctrl6()
	{
		if (!OnSpell)
		{
			Armon = ArmonArray[8];
			if (Time == 1)
			{
				new Background2DRemover(StageData);
				DestPoint = new Point(BoundRect.Width / 2, 160);
			}
			else if (Time > 50)
			{
				RandomMove(346, 30, 2f, new Rectangle(BoundRect.Width / 2 - 60, 100, 120, 40));
			}
			if (Time == 60)
			{
				string fileName = ".\\CS\\StEx\\关底Boss\\5非.mbg";
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
				new SpellCard_SSSEx_05A(StageData);
				break;
			case "Marisa":
				new SpellCard_SSSEx_05B(StageData);
				break;
			case "Sanae":
				new SpellCard_SSSEx_05C(StageData);
				break;
			case "Koishi":
				new SpellCard_SSSEx_05D(StageData);
				break;
			default:
				new SpellCard_SSSEx_05A(StageData);
				break;
			}
		}
	}

	private void Ctrl5()
	{
		if (!OnSpell)
		{
			Armon = ArmonArray[10];
			if (Time == 1)
			{
				new Background2DRemover(StageData);
				DestPoint = new Point(BoundRect.Width / 2, 160);
			}
			else if (Time > 50)
			{
				RandomMove(346, 30, 2f, new Rectangle(BoundRect.Width / 2 - 60, 100, 120, 40));
			}
			if (Time == 60)
			{
				string fileName = ".\\CS\\StEx\\关底Boss\\6非.mbg";
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
				new SpellCard_SSSEx_06A(StageData);
				break;
			case "Marisa":
				new SpellCard_SSSEx_06B(StageData);
				break;
			case "Sanae":
				new SpellCard_SSSEx_06C(StageData);
				break;
			case "Koishi":
				new SpellCard_SSSEx_06D(StageData);
				break;
			default:
				new SpellCard_SSSEx_06A(StageData);
				break;
			}
		}
	}

	private void Ctrl4()
	{
		if (!OnSpell)
		{
			Armon = ArmonArray[12];
			if (Time == 1)
			{
				new Background2DRemover(StageData);
				DestPoint = new Point(BoundRect.Width / 2, 160);
			}
			else if (Time > 150)
			{
				RandomMove(346, 30, 2f, new Rectangle(BoundRect.Width / 2 - 60, 100, 120, 40));
			}
			if (Time == 160)
			{
				string fileName = ".\\CS\\StEx\\关底Boss\\7非.mbg";
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
				new SpellCard_SSSEx_07A(StageData);
				break;
			case "Marisa":
				new SpellCard_SSSEx_07B(StageData);
				break;
			case "Sanae":
				new SpellCard_SSSEx_07C(StageData);
				break;
			case "Koishi":
				new SpellCard_SSSEx_07D(StageData);
				break;
			default:
				new SpellCard_SSSEx_07A(StageData);
				break;
			}
		}
	}

	private void Ctrl3()
	{
		if (!OnSpell)
		{
			Armon = ArmonArray[14];
			if (Time == 1)
			{
				new Background2DRemover(StageData);
				DestPoint = new Point(BoundRect.Width / 2, 160);
			}
			else if (Time > 50)
			{
				RandomMove(346, 30, 2f, new Rectangle(BoundRect.Width / 2 - 60, 100, 120, 40));
			}
			if (Time == 60)
			{
				string fileName = ".\\CS\\StEx\\关底Boss\\8非.mbg";
				CS_Data cSData = StageData.LoadCS(fileName);
				new CSEmitterController(StageData, cSData).BossBinding = true;
			}
		}
		else if (Time == 1)
		{
			new BulletRemover2(StageData, OriginalPosition);
			new SpellCard_SSSEx_08(StageData);
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
				SpellTime -= 1200;
			}
			if (Time == 100)
			{
				OnSpell = true;
			}
		}
		else if (Time == 101)
		{
			new BulletRemover2(StageData, OriginalPosition);
			new SpellCard_SSSEx_09(StageData);
		}
	}

	private void Ctrl1()
	{
		if (!OnSpell)
		{
			if (Time == 1)
			{
				new Background2DRemover(StageData);
				DestPoint = new Point(BoundRect.Width / 2, 140);
			}
			if (Time == 100)
			{
				OnSpell = true;
				SpellTime = 7200;
			}
		}
		else if (Time == 101)
		{
			new BulletRemover2(StageData, OriginalPosition);
			new SpellCard_SSSEx_10(StageData);
		}
	}

	public override void HitCheckAll()
	{
		if (OnSpell && StageData.SpellList.Count > 0 && MyPlane.Name != "Koishi")
		{
			StageData.SpellList.ForEach((BaseObject x) =>
			{
				x.Damage = 0;
			});
		}
		else
		{
			base.HitCheckAll();
		}
	}
}
