using System;
using System.Collections.Generic;
using System.Drawing;

namespace Shooting;

public class MyPlane_Aya : BaseMyPlane_Touhou
{
	public MyPlane_Aya(StageDataPackage StageData, Point OriginalPosition)
		: base(StageData, OriginalPosition, "Aya")
	{
		WeaponType = "A";
		SubPlaneList = new List<BaseSubPlane>
		{
			new SubPlane_Aya(StageData, Position),
			new SubPlane_Aya(StageData, Position),
			new SubPlane_Aya(StageData, Position),
			new SubPlane_Aya(StageData, Position)
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
			new MyBullet_Aya(StageData, new PointF(x + 7f, y));
			new MyBullet_Aya(StageData, new PointF(x - 7f, y));
		}
		if (!KClass.Key_Shift)
		{
			if (Time % 4 == 0)
			{
				for (int i = 0; i < PowerLevel; i++)
				{
					SubPlaneList[i].Shoot();
				}
			}
		}
		else if (PowerLevel > 0 && Time % (13 - (PowerLevel - 1) * 2) == 0)
		{
			new MyBullet_AyaSub(StageData, OriginalPosition);
		}
	}

	public override void SpellShoot()
	{
		new Spell_AyaA(StageData);
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
				SubPlaneList[0].ShootDirection = -1.7707963297751288;
				SubPlaneList[1].ShootDirection = -1.3707963238146643;
				break;
			case 3:
				SubPlanePoint[0] = new PointF(OriginalPosition.X, OriginalPosition.Y - 28f);
				SubPlanePoint[1] = new PointF(OriginalPosition.X - 10f, OriginalPosition.Y - 22f);
				SubPlanePoint[2] = new PointF(OriginalPosition.X + 10f, OriginalPosition.Y - 22f);
				SubPlaneList[0].ShootDirection = -Math.PI / 2.0;
				SubPlaneList[1].ShootDirection = -1.7707963297751288;
				SubPlaneList[2].ShootDirection = -1.3707963238146643;
				break;
			case 4:
				SubPlanePoint[0] = new PointF(OriginalPosition.X - 6f, OriginalPosition.Y - 30f);
				SubPlanePoint[1] = new PointF(OriginalPosition.X + 6f, OriginalPosition.Y - 30f);
				SubPlanePoint[2] = new PointF(OriginalPosition.X - 15f, OriginalPosition.Y - 22f);
				SubPlanePoint[3] = new PointF(OriginalPosition.X + 15f, OriginalPosition.Y - 22f);
				SubPlaneList[0].ShootDirection = -1.7707963297751288;
				SubPlaneList[1].ShootDirection = -1.3707963238146643;
				SubPlaneList[2].ShootDirection = -1.7707963297751288;
				SubPlaneList[3].ShootDirection = -1.3707963238146643;
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
				SubPlanePoint[0] = new PointF(OriginalPosition.X - 25f, OriginalPosition.Y - 0f);
				SubPlanePoint[1] = new PointF(OriginalPosition.X + 25f, OriginalPosition.Y - 0f);
				SubPlaneList[0].ShootDirection = -1.7707963297751288;
				SubPlaneList[1].ShootDirection = -1.3707963238146643;
				break;
			case 3:
				SubPlanePoint[0] = new PointF(OriginalPosition.X, OriginalPosition.Y - 35f);
				SubPlanePoint[1] = new PointF(OriginalPosition.X - 30f, OriginalPosition.Y + 5f);
				SubPlanePoint[2] = new PointF(OriginalPosition.X + 30f, OriginalPosition.Y + 5f);
				SubPlaneList[0].ShootDirection = -Math.PI / 2.0;
				SubPlaneList[1].ShootDirection = -1.920796320834432;
				SubPlaneList[2].ShootDirection = -1.220796332755361;
				break;
			case 4:
				SubPlanePoint[0] = new PointF(OriginalPosition.X - 25f, OriginalPosition.Y + 5f);
				SubPlanePoint[1] = new PointF(OriginalPosition.X + 25f, OriginalPosition.Y + 5f);
				SubPlanePoint[2] = new PointF(OriginalPosition.X - 40f, OriginalPosition.Y + 10f);
				SubPlanePoint[3] = new PointF(OriginalPosition.X + 40f, OriginalPosition.Y + 10f);
				SubPlaneList[0].ShootDirection = -1.720796332755361;
				SubPlaneList[1].ShootDirection = -1.420796320834432;
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

	public override void TextureCtrl()
	{
		base.TextureCtrl();
		if (SpellList.Count > 0)
		{
			TxtureObject = TextureObjectDictionary["Aya36"];
		}
	}
}
