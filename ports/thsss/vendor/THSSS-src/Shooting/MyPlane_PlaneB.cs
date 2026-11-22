using System;
using System.Collections.Generic;
using System.Drawing;

namespace Shooting;

public class MyPlane_PlaneB : BaseMyPlane
{
	public MyPlane_PlaneB(StageDataPackage StageData, Point OriginalPosition)
		: base(StageData, OriginalPosition)
	{
		WeaponType = "B";
		SubPlaneList = new List<BaseSubPlane>
		{
			new SubPlane_PlaneB(StageData, Position),
			new SubPlane_PlaneB(StageData, Position),
			new SubPlane_PlaneB(StageData, Position),
			new SubPlane_PlaneB(StageData, Position)
		};
		HighSpeed = 4.5f;
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
			float x = Position.X;
			float y = Position.Y;
			new BaseMyBullet(StageData, "自机子弹", new PointF(x - 7f, y), 30f, -Math.PI / 2.0);
			new BaseMyBullet(StageData, "自机子弹", new PointF(x + 7f, y), 30f, -Math.PI / 2.0);
		}
		if (Time % 5 == 0)
		{
			for (int i = 0; i < PowerLevel; i++)
			{
				SubPlaneList[i].Shoot();
			}
		}
	}

	public override void SpellShoot()
	{
		new Spell_PlaneB(StageData);
	}

	public override void SubPlaneCtrl()
	{
		if (KClass.Key_Shift)
		{
			switch (PowerLevel)
			{
			case 1:
				SubPlanePoint[0] = new PointF(OriginalPosition.X, OriginalPosition.Y - 30f);
				break;
			case 2:
				SubPlanePoint[0] = new PointF(OriginalPosition.X - 12f, OriginalPosition.Y - 30f);
				SubPlanePoint[1] = new PointF(OriginalPosition.X + 12f, OriginalPosition.Y - 30f);
				break;
			case 3:
				SubPlanePoint[0] = new PointF(OriginalPosition.X, OriginalPosition.Y - 30f);
				SubPlanePoint[1] = new PointF(OriginalPosition.X + 15f, OriginalPosition.Y - 20f);
				SubPlanePoint[2] = new PointF(OriginalPosition.X - 15f, OriginalPosition.Y - 20f);
				break;
			case 4:
				SubPlanePoint[0] = new PointF(OriginalPosition.X - 8f, OriginalPosition.Y - 35f);
				SubPlanePoint[1] = new PointF(OriginalPosition.X + 8f, OriginalPosition.Y - 35f);
				SubPlanePoint[2] = new PointF(OriginalPosition.X - 22f, OriginalPosition.Y - 25f);
				SubPlanePoint[3] = new PointF(OriginalPosition.X + 22f, OriginalPosition.Y - 25f);
				break;
			}
		}
		else
		{
			switch (PowerLevel)
			{
			case 1:
				SubPlanePoint[0] = new PointF(OriginalPosition.X, OriginalPosition.Y - 40f);
				break;
			case 2:
				SubPlanePoint[0] = new PointF(OriginalPosition.X - 25f, OriginalPosition.Y - 20f);
				SubPlanePoint[1] = new PointF(OriginalPosition.X + 25f, OriginalPosition.Y - 20f);
				break;
			case 3:
				SubPlanePoint[0] = new PointF(OriginalPosition.X, OriginalPosition.Y - 40f);
				SubPlanePoint[1] = new PointF(OriginalPosition.X + 30f, OriginalPosition.Y - 20f);
				SubPlanePoint[2] = new PointF(OriginalPosition.X - 30f, OriginalPosition.Y - 20f);
				break;
			case 4:
				SubPlanePoint[0] = new PointF(OriginalPosition.X - 25f, OriginalPosition.Y - 0f);
				SubPlanePoint[1] = new PointF(OriginalPosition.X + 25f, OriginalPosition.Y - 0f);
				SubPlanePoint[2] = new PointF(OriginalPosition.X - 50f, OriginalPosition.Y + 5f);
				SubPlanePoint[3] = new PointF(OriginalPosition.X + 50f, OriginalPosition.Y + 5f);
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
