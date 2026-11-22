using System.Drawing;

namespace Shooting;

internal class Boss_Seiryuu02 : BaseBossTouhou
{
	public Boss_Seiryuu02(StageDataPackage StageData)
		: base(StageData)
	{
		TxtureObjects = new TextureObject[3, 4]
		{
			{
				TextureObjectDictionary["BossSeiryuu-00"],
				TextureObjectDictionary["BossSeiryuu-01"],
				TextureObjectDictionary["BossSeiryuu-02"],
				TextureObjectDictionary["BossSeiryuu-03"]
			},
			{
				TextureObjectDictionary["BossSeiryuu-10"],
				TextureObjectDictionary["BossSeiryuu-11"],
				TextureObjectDictionary["BossSeiryuu-12"],
				TextureObjectDictionary["BossSeiryuu-13"]
			},
			{
				TextureObjectDictionary["BossSeiryuu-20"],
				TextureObjectDictionary["BossSeiryuu-21"],
				TextureObjectDictionary["BossSeiryuu-22"],
				TextureObjectDictionary["BossSeiryuu-23"]
			}
		};
		TxtureObject = TxtureObjects[0, 0];
		OriginalPosition = new PointF(BoundRect.Width / 2, 150f);
		DestPoint = new Point(BoundRect.Width / 2, 150);
		MaxHP = 10000;
		HealthPoint = MaxHP;
		Life = 1;
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
		Region = 0;
		StageData.SoundPlay("se_tan01.wav");
		LifeTime = Time + 150;
		new EndBoss_Touhou03(StageData, OriginalPosition, Color.ForestGreen, Color.Green);
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
		else
		{
			Boss.Armon = 0.4f;
			if (Time == 2)
			{
				new SpellCard_SSS03_FSC(StageData);
			}
		}
	}

	public override void GiveItems()
	{
		base.GiveItems();
		new SpellItem_Touhou(StageData, OriginalPosition);
	}
}
