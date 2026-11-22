using System;
using System.Drawing;

namespace Shooting;

public class BulletRemover : BaseBackground
{
	public BulletRemover(StageDataPackage StageData, PointF OriginalPosition)
		: base(StageData, null, new Point(0, 0), 0f, Math.PI / 2.0)
	{
		base.OriginalPosition = OriginalPosition;
		LifeTime = 30;
		Region = 2;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		Region += 20;
		for (int num = BulletList.Count - 1; num >= 0; num--)
		{
			if (HitCheck(BulletList[num], Region) && !BulletList[num].UnRemoveable)
			{
				BulletList[num].GiveEndEffect();
				BulletList.RemoveAt(num);
			}
		}
		for (int num2 = EnemyPlaneList.Count - 1; num2 >= 0; num2--)
		{
			if (HitCheck(EnemyPlaneList[num2], Region))
			{
				EnemyPlaneList[num2].HealthPoint -= 20f;
				if (EnemyPlaneList[num2].HealthPoint <= 0f)
				{
					EnemyPlaneList[num2].GiveEndEffect();
					EnemyPlaneList[num2].GiveItems();
					EnemyPlaneList.RemoveAt(num2);
					StageData.SoundPlay("se_enep00.wav");
				}
			}
		}
	}
}
