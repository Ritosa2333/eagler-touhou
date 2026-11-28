using System;
using System.Collections.Generic;
using System.Drawing;

namespace Shooting;

public class MyPlane_Koishi : BaseMyPlane_Touhou
{
	private bool GodMode;

	public MyPlane_Koishi(StageDataPackage StageData, Point OriginalPosition)
		: base(StageData, OriginalPosition, "Koishi")
	{
		WeaponType = "A";
		SubPlaneList = new List<BaseSubPlane>
		{
			new SubPlane_Koishi(StageData, Position),
			new SubPlane_Koishi(StageData, Position),
			new SubPlane_Koishi(StageData, Position),
			new SubPlane_Koishi(StageData, Position)
		};
		HighSpeed = 5f;
		LowSpeed = 2f;
	}

	public override void Shoot()
	{
		if (Time % 3 == 0)
		{
			StageData.SoundPlay("se_plst00.wav", OriginalPosition.X / (float)BoundRect.Width);
			float x = OriginalPosition.X;
			float y = OriginalPosition.Y;
			new BaseMyBullet_Touhou(StageData, "KoishiBullet", new PointF(x + 7f, y), 30f, -Math.PI / 2.0).Damage = 13;
			new BaseMyBullet_Touhou(StageData, "KoishiBullet", new PointF(x - 7f, y), 30f, -Math.PI / 2.0).Damage = 13;
		}
		if (Time % 10 == 0)
		{
			for (int i = 0; i < PowerLevel; i++)
			{
				SubPlaneList[i].Shoot();
			}
		}
	}

	public override void SpellShoot()
	{
		if (!GodMode)
		{
			new Spell_Koishi(StageData);
			GodMode = true;
		}
	}

	public override void Ctrl()
	{
		base.Ctrl();
		TransparentVelocity = (GodMode ? (-10) : 10);
		TransparentValueF += TransparentVelocity;
		SpellEnabled = !GodMode;
	}

	public override void SubPlaneCtrl()
	{
		if (KClass.Key_Shift)
		{
			switch (PowerLevel)
			{
			case 1:
				SubPlanePoint[0] = new PointF(OriginalPosition.X, OriginalPosition.Y - 44f);
				SubPlaneList[0].ShootDirection = -Math.PI / 2.0;
				break;
			case 2:
				SubPlanePoint[0] = new PointF(OriginalPosition.X - 7f, OriginalPosition.Y - 36f);
				SubPlanePoint[1] = new PointF(OriginalPosition.X + 7f, OriginalPosition.Y - 36f);
				SubPlaneList[0].ShootDirection = -Math.PI / 2.0;
				SubPlaneList[1].ShootDirection = -Math.PI / 2.0;
				break;
			case 3:
				SubPlanePoint[2] = new PointF(OriginalPosition.X, OriginalPosition.Y - 36f);
				SubPlanePoint[1] = new PointF(OriginalPosition.X - 16f, OriginalPosition.Y - 29f);
				SubPlanePoint[0] = new PointF(OriginalPosition.X + 16f, OriginalPosition.Y - 29f);
				SubPlaneList[2].ShootDirection = -Math.PI / 2.0;
				SubPlaneList[1].ShootDirection = -Math.PI / 2.0;
				SubPlaneList[0].ShootDirection = -Math.PI / 2.0;
				break;
			case 4:
				SubPlanePoint[3] = new PointF(OriginalPosition.X - 7f, OriginalPosition.Y - 36f);
				SubPlanePoint[2] = new PointF(OriginalPosition.X + 7f, OriginalPosition.Y - 36f);
				SubPlanePoint[1] = new PointF(OriginalPosition.X - 16f, OriginalPosition.Y - 29f);
				SubPlanePoint[0] = new PointF(OriginalPosition.X + 16f, OriginalPosition.Y - 29f);
				SubPlaneList[3].ShootDirection = -Math.PI / 2.0;
				SubPlaneList[2].ShootDirection = -Math.PI / 2.0;
				SubPlaneList[1].ShootDirection = -Math.PI / 2.0;
				SubPlaneList[0].ShootDirection = -Math.PI / 2.0;
				break;
			}
		}
		else
		{
			switch (PowerLevel)
			{
			case 1:
				SubPlanePoint[0] = new PointF(OriginalPosition.X, OriginalPosition.Y - 44f);
				SubPlaneList[0].ShootDirection = -Math.PI / 2.0;
				break;
			case 2:
				SubPlanePoint[0] = new PointF(OriginalPosition.X - 32f, OriginalPosition.Y - 32f);
				SubPlanePoint[1] = new PointF(OriginalPosition.X + 32f, OriginalPosition.Y - 32f);
				SubPlaneList[0].ShootDirection = 3.9269908169872414;
				SubPlaneList[1].ShootDirection = 5.497787143782138;
				break;
			case 3:
				SubPlanePoint[2] = new PointF(OriginalPosition.X, OriginalPosition.Y + 44f);
				SubPlanePoint[1] = new PointF(OriginalPosition.X - 32f, OriginalPosition.Y - 32f);
				SubPlanePoint[0] = new PointF(OriginalPosition.X + 32f, OriginalPosition.Y - 32f);
				SubPlaneList[2].ShootDirection = Math.PI / 2.0;
				SubPlaneList[1].ShootDirection = 3.9269908169872414;
				SubPlaneList[0].ShootDirection = 5.497787143782138;
				break;
			case 4:
				SubPlanePoint[3] = new PointF(OriginalPosition.X - 32f, OriginalPosition.Y + 32f);
				SubPlanePoint[2] = new PointF(OriginalPosition.X + 32f, OriginalPosition.Y + 32f);
				SubPlanePoint[1] = new PointF(OriginalPosition.X - 32f, OriginalPosition.Y - 32f);
				SubPlanePoint[0] = new PointF(OriginalPosition.X + 32f, OriginalPosition.Y - 32f);
				SubPlaneList[3].ShootDirection = Math.PI * 3.0 / 4.0;
				SubPlaneList[2].ShootDirection = Math.PI / 4.0;
				SubPlaneList[1].ShootDirection = 3.9269908169872414;
				SubPlaneList[0].ShootDirection = 5.497787143782138;
				break;
			}
		}
		for (int i = 0; i < PowerLevel; i++)
		{
			SubPlaneList[i].Enabled = true;
			SubPlaneList[i].DestPoint = SubPlanePoint[i];
			SubPlaneList[i].Ctrl();
		}
		for (int j = PowerLevel; j < SubPlaneList.Count; j++)
		{
			SubPlaneList[j].Enabled = false;
			SubPlaneList[j].Position = Position;
		}
	}

	public override void PreMiss()
	{
		if (Time < UnmatchedTime || Time < UnmatchedTime || Time < DeadTime || SpellList.Count != 0)
		{
			return;
		}
		if (EnchantmentState == EnchantmentType.Green)
		{
			new BulletRemover_Small(StageData, OriginalPosition).Region = 500;
			ChangeEnchantment(EnchantmentType.None);
			EnemyPlaneList.ForEach((BaseEnemyPlane x) =>
			{
				if (x is BaseSpellCard)
				{
					((BaseSpellCard)x).Missed = true;
				}
			});
		}
		else if (GodMode)
		{
			GodMode = false;
			StageData.SoundPlay("ice005.wav");
			new BulletRemover_Small(StageData, MyPlane.OriginalPosition).Region = 150;
			Time = UnmatchedTime - 60;
			new ItemGetter(StageData);
			EnemyPlaneList.ForEach((BaseEnemyPlane x) =>
			{
				if (x is BaseSpellCard)
				{
					((BaseSpellCard)x).Missed = true;
				}
			});
		}
		else
		{
			new Emitter(StageData, MyPlane.Position);
			StageData.SoundPlay("se_pldead00.wav", OriginalPosition.X / (float)BoundRect.Width);
			DeadTime = Time + 20;
		}
	}

	public override void Refresh()
	{
		if (GodMode)
		{
			SpellExtand();
			GodMode = false;
		}
		base.Refresh();
	}

	public override void Show()
	{
		if (Time >= DeadTime && Time != 0)
		{
			if (Time < UnmatchedTime && Time / 2 % 2 == 0)
			{
				SpriteMain.Draw2D(TxtureObject, 1f, 0f, Position, Color.FromArgb(TransparentValue, Color.DarkBlue));
			}
			else
			{
				SpriteMain.Draw2D(TxtureObject, 1f, 0f, Position, Color.FromArgb(TransparentValue, Color.White));
			}
			if (GodMode)
			{
				SpriteMain.Draw2D(TextureObjectDictionary["jiejie"], 1f, (0f - (float)Direction) * 4f, Position, Color.FromArgb(255 - TransparentValue, Color.White));
			}
			for (int i = 0; i < PowerLevel; i++)
			{
				SubPlaneList[i].Show();
			}
		}
	}
}
