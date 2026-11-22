using System;
using System.Drawing;

namespace Shooting;

public class BulletRemover_Small : BaseBackground
{
	public BulletRemover_Small(StageDataPackage StageData, PointF OriginalPosition)
		: base(StageData, null, new Point(0, 0), 0f, Math.PI / 2.0)
	{
		base.OriginalPosition = OriginalPosition;
		LifeTime = 10;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		for (int num = BulletList.Count - 1; num >= 0; num--)
		{
			if (HitCheck(BulletList[num], Region) && !BulletList[num].UnRemoveable)
			{
				BulletList[num].GiveEndEffect();
				BulletList[num].GiveItems();
				BulletList.RemoveAt(num);
			}
		}
	}
}
