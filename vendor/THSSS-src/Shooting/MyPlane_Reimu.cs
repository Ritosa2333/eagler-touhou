using System;
using System.Collections.Generic;
using System.Drawing;

namespace Shooting;

public class MyPlane_Reimu : BaseMyPlane_Touhou
{
	public MyPlane_Reimu(StageDataPackage StageData, Point OriginalPosition)
		: base(StageData, OriginalPosition, "Reimu")
	{
		WeaponType = "A";
		SubPlaneList = new List<BaseSubPlane>
		{
			new Shinmyoumaru(StageData, Position),
			new SubPlane_Reimu(StageData, Position),
			new SubPlane_Reimu(StageData, Position),
			new SubPlane_Reimu(StageData, Position)
		};
		HighSpeed = 4.5f;
		LowSpeed = 2f;
	}

	public override void Shoot()
	{
		if (Time % 3 == 0)
		{
			StageData.SoundPlay("se_plst00.wav", OriginalPosition.X / (float)BoundRect.Width);
			float x = OriginalPosition.X;
			float y = OriginalPosition.Y;
			new BaseMyBullet_Touhou(StageData, "ReimuBullet", new PointF(x - 7f, y), 30f, -Math.PI / 2.0).Damage = 14;
			new BaseMyBullet_Touhou(StageData, "ReimuBullet", new PointF(x + 7f, y), 30f, -Math.PI / 2.0).Damage = 14;
		}
		if (Time % 4 == 0)
		{
			for (int i = 0; i < PowerLevel; i++)
			{
				SubPlaneList[i].Shoot();
			}
		}
	}

	public override void SpellShoot()
	{
		if (KClass.Key_Shift)
		{
			new Spell_Reimu(StageData);
		}
		else
		{
			new Spell_Shinmyoumaru(StageData);
		}
	}

	public override void SubPlaneCtrl()
	{
		if (KClass.Key_Shift)
		{
			switch (PowerLevel)
			{
			case 1:
				SubPlanePoint[0] = new PointF(OriginalPosition.X, OriginalPosition.Y - 70f);
				SubPlaneList[0].ShootDirection = -Math.PI / 2.0;
				break;
			case 2:
				SubPlanePoint[0] = new PointF(OriginalPosition.X - 0f, OriginalPosition.Y - 70f);
				SubPlanePoint[1] = new PointF(OriginalPosition.X + 0f, OriginalPosition.Y - 70f);
				SubPlaneList[0].ShootDirection = -Math.PI / 2.0;
				SubPlaneList[1].ShootDirection = -Math.PI / 2.0;
				break;
			case 3:
				SubPlanePoint[0] = new PointF(OriginalPosition.X, OriginalPosition.Y - 70f);
				SubPlanePoint[1] = new PointF(OriginalPosition.X - 24f, OriginalPosition.Y - 0f);
				SubPlanePoint[2] = new PointF(OriginalPosition.X + 24f, OriginalPosition.Y - 0f);
				SubPlaneList[0].ShootDirection = -Math.PI / 2.0;
				SubPlaneList[1].ShootDirection = 4.75602204523698;
				SubPlaneList[2].ShootDirection = 4.668755915532399;
				break;
			case 4:
				SubPlanePoint[0] = new PointF(OriginalPosition.X - 0f, OriginalPosition.Y - 70f);
				SubPlanePoint[1] = new PointF(OriginalPosition.X + 0f, OriginalPosition.Y - 70f);
				SubPlanePoint[2] = new PointF(OriginalPosition.X - 24f, OriginalPosition.Y - 0f);
				SubPlanePoint[3] = new PointF(OriginalPosition.X + 24f, OriginalPosition.Y - 0f);
				SubPlaneList[0].ShootDirection = -Math.PI / 2.0;
				SubPlaneList[1].ShootDirection = -Math.PI / 2.0;
				SubPlaneList[2].ShootDirection = 4.75602204523698;
				SubPlaneList[3].ShootDirection = 4.668755915532399;
				break;
			}
		}
		else
		{
			switch (PowerLevel)
			{
			case 1:
				SubPlanePoint[0] = new PointF(OriginalPosition.X, OriginalPosition.Y - 48f);
				SubPlaneList[0].ShootDirection = -Math.PI / 2.0;
				break;
			case 2:
				SubPlanePoint[0] = new PointF(OriginalPosition.X - 0f, OriginalPosition.Y - 48f);
				SubPlanePoint[1] = new PointF(OriginalPosition.X + 0f, OriginalPosition.Y + 48f);
				SubPlaneList[0].ShootDirection = -Math.PI / 2.0;
				SubPlaneList[1].ShootDirection = -Math.PI / 2.0;
				break;
			case 3:
				SubPlanePoint[0] = new PointF(OriginalPosition.X, OriginalPosition.Y - 48f);
				SubPlanePoint[1] = new PointF(OriginalPosition.X - 48f, OriginalPosition.Y + 0f);
				SubPlanePoint[2] = new PointF(OriginalPosition.X + 48f, OriginalPosition.Y + 0f);
				SubPlaneList[0].ShootDirection = -Math.PI / 2.0;
				SubPlaneList[1].ShootDirection = -Math.PI / 2.0;
				SubPlaneList[2].ShootDirection = -Math.PI / 2.0;
				break;
			case 4:
				SubPlanePoint[0] = new PointF(OriginalPosition.X, OriginalPosition.Y - 48f);
				SubPlanePoint[1] = new PointF(OriginalPosition.X, OriginalPosition.Y + 48f);
				SubPlanePoint[2] = new PointF(OriginalPosition.X - 48f, OriginalPosition.Y + 0f);
				SubPlanePoint[3] = new PointF(OriginalPosition.X + 48f, OriginalPosition.Y + 0f);
				SubPlaneList[0].ShootDirection = -Math.PI / 2.0;
				SubPlaneList[1].ShootDirection = -Math.PI / 2.0;
				SubPlaneList[2].ShootDirection = -Math.PI / 2.0;
				SubPlaneList[3].ShootDirection = -Math.PI / 2.0;
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
}
