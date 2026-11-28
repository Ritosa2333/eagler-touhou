using System;
using System.Collections.Generic;
using System.Drawing;

namespace Shooting;

public class MyPlane_AyaB : BaseMyPlane_Touhou
{
	public MyPlane_AyaB(StageDataPackage StageData, Point OriginalPosition)
		: base(StageData, OriginalPosition, "Aya")
	{
		WeaponType = "B";
		SubPlaneList = new List<BaseSubPlane>
		{
			new SubPlane_AyaB(StageData, Position),
			new SubPlane_AyaB(StageData, Position),
			new SubPlane_AyaB(StageData, Position),
			new SubPlane_AyaB(StageData, Position)
		};
		HighSpeed = 5f;
		LowSpeed = 2f;
	}

	public override void Shoot()
	{
		if (SpellList.Count > 0)
		{
			return;
		}
		if (Time % 3 == 0)
		{
			StageData.SoundPlay("se_plst00.wav", OriginalPosition.X / (float)BoundRect.Width);
			float x = OriginalPosition.X;
			float y = OriginalPosition.Y;
			new MyBullet_Aya(StageData, new PointF(x + 7f, y));
			new MyBullet_Aya(StageData, new PointF(x - 7f, y));
		}
		if (Time % 5 == 0)
		{
			for (int i = 0; i < PowerLevel; i++)
			{
				SubPlaneList[i].Shoot();
			}
		}
	}

	public override void Ctrl()
	{
		if (SpellList.Count <= 0)
		{
			base.Ctrl();
		}
	}

	public override void SpellShoot()
	{
		new Spell_AyaB(StageData);
		StageData.SoundPlay("se_cat00.wav");
		StageData.SoundPlay("se_nep00.wav");
	}

	public override void SubPlaneCtrl()
	{
		if (KClass.Key_Shift)
		{
			switch (PowerLevel)
			{
			case 1:
				SubPlanePoint[0] = new PointF(OriginalPosition.X, OriginalPosition.Y - 25f);
				SubPlaneList[0].ShootDirection = -Math.PI / 2.0;
				break;
			case 2:
				SubPlanePoint[0] = new PointF(OriginalPosition.X - 6f, OriginalPosition.Y - 30f);
				SubPlanePoint[1] = new PointF(OriginalPosition.X + 6f, OriginalPosition.Y - 30f);
				SubPlaneList[0].ShootDirection = -Math.PI / 2.0;
				SubPlaneList[1].ShootDirection = -Math.PI / 2.0;
				break;
			case 3:
				SubPlanePoint[0] = new PointF(OriginalPosition.X, OriginalPosition.Y - 28f);
				SubPlanePoint[1] = new PointF(OriginalPosition.X - 10f, OriginalPosition.Y - 22f);
				SubPlanePoint[2] = new PointF(OriginalPosition.X + 10f, OriginalPosition.Y - 22f);
				SubPlaneList[0].ShootDirection = -Math.PI / 2.0;
				SubPlaneList[1].ShootDirection = -Math.PI / 2.0;
				SubPlaneList[2].ShootDirection = -Math.PI / 2.0;
				break;
			case 4:
				SubPlanePoint[0] = new PointF(OriginalPosition.X - 6f, OriginalPosition.Y - 30f);
				SubPlanePoint[1] = new PointF(OriginalPosition.X + 6f, OriginalPosition.Y - 30f);
				SubPlanePoint[2] = new PointF(OriginalPosition.X - 15f, OriginalPosition.Y - 22f);
				SubPlanePoint[3] = new PointF(OriginalPosition.X + 15f, OriginalPosition.Y - 22f);
				SubPlaneList[0].ShootDirection = -Math.PI / 2.0;
				SubPlaneList[1].ShootDirection = -Math.PI / 2.0;
				SubPlaneList[2].ShootDirection = -Math.PI / 2.0;
				SubPlaneList[3].ShootDirection = -Math.PI / 2.0;
				break;
			}
		}
		else
		{
			switch (PowerLevel)
			{
			case 1:
				SubPlanePoint[0] = new PointF(OriginalPosition.X, OriginalPosition.Y - 35f);
				SubPlaneList[0].ShootDirection = -Math.PI / 2.0;
				break;
			case 2:
				SubPlanePoint[0] = new PointF(OriginalPosition.X - 30f, OriginalPosition.Y - 0f);
				SubPlanePoint[1] = new PointF(OriginalPosition.X + 30f, OriginalPosition.Y - 0f);
				SubPlaneList[0].ShootDirection = -1.8207963267948966;
				SubPlaneList[1].ShootDirection = -1.3207963267948966;
				break;
			case 3:
				SubPlanePoint[0] = new PointF(OriginalPosition.X, OriginalPosition.Y + 30f);
				SubPlanePoint[1] = new PointF(OriginalPosition.X - 25f, OriginalPosition.Y + 20f);
				SubPlanePoint[2] = new PointF(OriginalPosition.X + 25f, OriginalPosition.Y + 20f);
				SubPlaneList[0].ShootDirection = -Math.PI / 2.0;
				SubPlaneList[1].ShootDirection = -1.920796320834432;
				SubPlaneList[2].ShootDirection = -1.220796332755361;
				break;
			case 4:
				SubPlanePoint[0] = new PointF(OriginalPosition.X - 15f, OriginalPosition.Y + 26f);
				SubPlanePoint[1] = new PointF(OriginalPosition.X + 15f, OriginalPosition.Y + 26f);
				SubPlanePoint[2] = new PointF(OriginalPosition.X - 30f, OriginalPosition.Y + 15f);
				SubPlanePoint[3] = new PointF(OriginalPosition.X + 30f, OriginalPosition.Y + 15f);
				SubPlaneList[0].ShootDirection = -1.8207963267948966;
				SubPlaneList[1].ShootDirection = -1.3207963267948966;
				SubPlaneList[2].ShootDirection = -2.0707963267948966;
				SubPlaneList[3].ShootDirection = -1.0707963267948966;
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

	public override void ShowCenter()
	{
		if (SpellList.Count <= 0)
		{
			base.ShowCenter();
		}
	}

	public override void Show()
	{
		if (SpellList.Count <= 0)
		{
			base.Show();
		}
	}
}
