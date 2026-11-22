using System;
using System.Drawing;

namespace Shooting;

internal class Boss_Freya07 : BaseBossTouhou
{
	private bool ShowWings { get; set; }

	private TextureObject TxtureObject2 { get; set; }

	private TextureObject[] TxtureObjects2 { get; set; }

	private int IndexX2 { get; set; }

	public Boss_Freya07(StageDataPackage StageData)
		: base(StageData)
	{
		TxtureObjects = new TextureObject[3, 4]
		{
			{
				TextureObjectDictionary["BossFreya-00"],
				TextureObjectDictionary["BossFreya-01"],
				TextureObjectDictionary["BossFreya-02"],
				TextureObjectDictionary["BossFreya-03"]
			},
			{
				TextureObjectDictionary["BossFreya-10"],
				TextureObjectDictionary["BossFreya-11"],
				TextureObjectDictionary["BossFreya-12"],
				TextureObjectDictionary["BossFreya-13"]
			},
			{
				TextureObjectDictionary["BossFreya-00"],
				TextureObjectDictionary["BossFreya-01"],
				TextureObjectDictionary["BossFreya-02"],
				TextureObjectDictionary["BossFreya-03"]
			}
		};
		TxtureObjects2 = new TextureObject[8]
		{
			TextureObjectDictionary["BossFreyaWing-00"],
			TextureObjectDictionary["BossFreyaWing-01"],
			TextureObjectDictionary["BossFreyaWing-02"],
			TextureObjectDictionary["BossFreyaWing-03"],
			TextureObjectDictionary["BossFreyaWing-04"],
			TextureObjectDictionary["BossFreyaWing-05"],
			TextureObjectDictionary["BossFreyaWing-06"],
			TextureObjectDictionary["BossFreyaWing-07"]
		};
		TxtureObject = TxtureObjects[0, 0];
		TxtureObject2 = TxtureObjects2[0];
		OriginalPosition = new PointF(BoundRect.Width / 2, 150f);
		DestPoint = new Point(BoundRect.Width / 2, 150);
		MaxHP = 10000;
		HealthPoint = MaxHP;
		Life = 3;
		Region = 20;
		Velocity = 4f;
		DirectionDegree = 90.0;
		SpellTime = 2400;
		LifeTime = 100000;
		OnSpell = false;
		ShowWings = true;
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
			IndexX2 = TimeMain % 32 / 8;
		}
		if (num != (Mirrored ? IndexY : (-IndexY)))
		{
			IndexX = 0f;
			IndexX2 = 0;
		}
		if (IndexX > 3f)
		{
			IndexX = 3f;
		}
		if (IndexX2 > 7)
		{
			IndexX2 = 7;
		}
		TxtureObject = TxtureObjects[IndexY, (int)IndexX];
		TxtureObject2 = TxtureObjects2[IndexX2];
	}

	public override void GiveEndEffect()
	{
		TransparentValueF = 255f;
		new Background2DRemover(StageData);
		Background3D.WarpEnabled = false;
		LifeTime = Time + 150;
		Region = 0;
		StageData.SoundPlay("se_tan01.wav");
		new EndBoss_Touhou01(StageData, OriginalPosition, Color.LightPink, Color.Pink);
	}

	private void Ctrl3()
	{
		if (!OnSpell)
		{
			Armon = 0f;
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
				string fileName = (int)Difficulty switch
				{
					0 => ".\\CS\\St02\\关底Boss\\1非E.mbg", 
					1 => ".\\CS\\St02\\关底Boss\\1非N.mbg", 
					2 => ".\\CS\\St02\\关底Boss\\1非H.mbg", 
					3 => ".\\CS\\St02\\关底Boss\\1非L.mbg", 
					_ => ".\\CS\\St02\\关底Boss\\1非L.mbg", 
				};
				CS_Data cSData = StageData.LoadCS(fileName);
				new CSEmitterController(StageData, cSData).BossBinding = true;
			}
		}
		else if (Time == 1)
		{
			new BulletRemover2(StageData, OriginalPosition);
			new SpellCard_SSS02_02(StageData);
		}
	}

	private void Ctrl2()
	{
		if (!OnSpell)
		{
			ShowWings = false;
			if (Time == 100)
			{
				StageData.SoundPlay("se_boon01.wav");
				new EmitterGiveOutEnegy3D(StageData, Boss.OriginalPosition, Color.SkyBlue);
				new BackgroundWing(StageData, "Wing", Boss.OriginalPosition, 0f, 0.0, 0);
				new BackgroundWing(StageData, "Wing", Boss.OriginalPosition, 0f, Math.PI, 0).Mirrored = true;
				StageData.VibrateStart(50);
			}
			Armon = 0.3f;
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
				string fileName = (int)Difficulty switch
				{
					0 => ".\\CS\\St02\\关底Boss\\2非E.mbg", 
					1 => ".\\CS\\St02\\关底Boss\\2非N.mbg", 
					2 => ".\\CS\\St02\\关底Boss\\2非H.mbg", 
					3 => ".\\CS\\St02\\关底Boss\\2非L.mbg", 
					_ => ".\\CS\\St02\\关底Boss\\2非L.mbg", 
				};
				CS_Data cSData = StageData.LoadCS(fileName);
				new CSEmitterController(StageData, cSData).BossBinding = true;
			}
		}
		else if (Time == 1)
		{
			new BulletRemover2(StageData, OriginalPosition);
			new SpellCard_SSS02_03(StageData);
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
			if (Time == 150)
			{
				OnSpell = true;
				SpellTime = 3000;
			}
		}
		else if (Time == 151)
		{
			new BulletRemover2(StageData, OriginalPosition);
			new SpellCard_SSS02_04(StageData);
		}
	}

	public override void GiveItems()
	{
		PointF originalPosition;
		for (int i = 0; i < 15; i++)
		{
			originalPosition = new PointF(OriginalPosition.X + (float)Ran.Next(-49, 50), OriginalPosition.Y + (float)Ran.Next(-49, 50));
			new PowerItem_Touhou(StageData, originalPosition);
		}
		for (int j = 0; j < 20; j++)
		{
			originalPosition = new PointF(OriginalPosition.X + (float)Ran.Next(-49, 50), OriginalPosition.Y + (float)Ran.Next(-49, 50));
			new ScoreItem_Touhou(StageData, originalPosition);
		}
	}

	public override void Show()
	{
		if (ShowWings && TxtureObject2 != null)
		{
			SpriteMain.Draw2D(TxtureObject2, ScaleWidth, ScaleLength, (float)(Angle - Math.PI / 2.0), Position, Color.FromArgb(TransparentValue, ColorValue), Mirrored);
		}
		base.Show();
	}
}
