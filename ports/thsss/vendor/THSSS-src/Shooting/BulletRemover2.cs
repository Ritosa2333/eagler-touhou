using System;
using System.Drawing;

namespace Shooting;

public class BulletRemover2 : BaseBackground
{
	public BulletRemover2(StageDataPackage StageData, PointF OriginalPosition)
		: base(StageData, null, new Point(0, 0), 0f, Math.PI / 2.0)
	{
		base.OriginalPosition = OriginalPosition;
		LifeTime = 30;
		Region = 2;
		foreach (BaseEnemyPlane enemyPlane in EnemyPlaneList)
		{
			enemyPlane.GiveEndEffect();
		}
		EnemyPlaneList.Clear();
	}

	public override void Ctrl()
	{
		base.Ctrl();
		Region += 25;
		for (int num = BulletList.Count - 1; num >= 0; num--)
		{
			if (HitCheck(BulletList[num], Region))
			{
				BulletList[num].GiveEndEffect();
				BulletList[num].GiveItems();
				BulletList.RemoveAt(num);
			}
		}
	}
}
