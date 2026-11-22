using System;
using System.Collections.Generic;
using System.Drawing;

namespace Shooting;

public class MyPlane_Sanae : BaseMyPlane_Touhou
{
	public MyPlane_Sanae(StageDataPackage StageData, Point OriginalPosition)
		: base(StageData, OriginalPosition, "Sanae")
	{
		WeaponType = "A";
		SubPlaneList = new List<BaseSubPlane>
		{
			new SubPlane_Sanae(StageData, Position),
			new SubPlane_Sanae(StageData, Position),
			new SubPlane_Sanae(StageData, Position)
			{
				Mirrored = true
			},
			new SubPlane_Sanae(StageData, Position)
			{
				Mirrored = true
			}
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
			new BaseMyBullet_Touhou(StageData, "SanaeBullet00", new PointF(x + 7f, y), 30f, -Math.PI / 2.0);
			new BaseMyBullet_Touhou(StageData, "SanaeBullet00", new PointF(x - 7f, y), 30f, -Math.PI / 2.0);
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
		new Spell_Sanae(StageData);
	}

	public override void SubPlaneCtrl()
	{
		if (KClass.Key_Shift)
		{
			switch (PowerLevel)
			{
			case 1:
				SubPlanePoint[0] = new PointF(OriginalPosition.X, OriginalPosition.Y - 30f);
				SubPlaneList[0].ShootDirection = -Math.PI / 2.0;
				break;
			case 2:
				SubPlanePoint[0] = new PointF(OriginalPosition.X - 10f, OriginalPosition.Y - 30f);
				SubPlanePoint[1] = new PointF(OriginalPosition.X + 10f, OriginalPosition.Y - 30f);
				SubPlaneList[0].ShootDirection = -Math.PI / 2.0;
				SubPlaneList[1].ShootDirection = -Math.PI / 2.0;
				break;
			case 3:
				SubPlanePoint[2] = new PointF(OriginalPosition.X, OriginalPosition.Y - 30f);
				SubPlanePoint[1] = new PointF(OriginalPosition.X - 7f, OriginalPosition.Y - 30f);
				SubPlanePoint[0] = new PointF(OriginalPosition.X + 7f, OriginalPosition.Y - 30f);
				SubPlaneList[2].ShootDirection = -Math.PI / 2.0;
				SubPlaneList[1].ShootDirection = -Math.PI / 2.0;
				SubPlaneList[0].ShootDirection = -Math.PI / 2.0;
				break;
			case 4:
				SubPlanePoint[3] = new PointF(OriginalPosition.X - 6f, OriginalPosition.Y - 30f);
				SubPlanePoint[2] = new PointF(OriginalPosition.X + 6f, OriginalPosition.Y - 30f);
				SubPlanePoint[1] = new PointF(OriginalPosition.X - 12f, OriginalPosition.Y - 30f);
				SubPlanePoint[0] = new PointF(OriginalPosition.X + 12f, OriginalPosition.Y - 30f);
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
				SubPlanePoint[0] = new PointF(OriginalPosition.X, OriginalPosition.Y + 30f);
				SubPlaneList[0].ShootDirection = -Math.PI / 2.0;
				break;
			case 2:
				SubPlanePoint[0] = new PointF(OriginalPosition.X - 36f, OriginalPosition.Y + 0f);
				SubPlanePoint[1] = new PointF(OriginalPosition.X + 36f, OriginalPosition.Y - 0f);
				SubPlaneList[0].ShootDirection = -1.6607963303711752;
				SubPlaneList[1].ShootDirection = -1.4807963232186179;
				break;
			case 3:
				SubPlanePoint[2] = new PointF(OriginalPosition.X, OriginalPosition.Y + 30f);
				SubPlanePoint[1] = new PointF(OriginalPosition.X - 36f, OriginalPosition.Y + 0f);
				SubPlanePoint[0] = new PointF(OriginalPosition.X + 36f, OriginalPosition.Y + 0f);
				SubPlaneList[2].ShootDirection = -Math.PI / 2.0;
				SubPlaneList[1].ShootDirection = -1.6607963303711752;
				SubPlaneList[0].ShootDirection = -1.4807963232186179;
				break;
			case 4:
				SubPlanePoint[3] = new PointF(OriginalPosition.X - 36f, OriginalPosition.Y + 0f);
				SubPlanePoint[2] = new PointF(OriginalPosition.X + 36f, OriginalPosition.Y + 0f);
				SubPlanePoint[1] = new PointF(OriginalPosition.X - 66f, OriginalPosition.Y + 0f);
				SubPlanePoint[0] = new PointF(OriginalPosition.X + 66f, OriginalPosition.Y + 0f);
				SubPlaneList[3].ShootDirection = -Math.PI / 2.0;
				SubPlaneList[2].ShootDirection = -Math.PI / 2.0;
				SubPlaneList[1].ShootDirection = -1.6607963303711752;
				SubPlaneList[0].ShootDirection = -1.4807963232186179;
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
