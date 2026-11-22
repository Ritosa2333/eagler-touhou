using System.Drawing;

namespace Shooting;

public class BaseEnemyPlane : BaseObject_CS
{
	public BaseEnemyPlane()
	{
	}

	public BaseEnemyPlane(StageDataPackage StageData, string textureName, PointF p, float v, double drctn)
		: base(StageData, textureName, p, v, drctn)
	{
		Region = 4;
		EnemyPlaneList.Add(this);
	}

	public override void Shoot()
	{
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (ShootEnabled)
		{
			Shoot();
		}
		if (OutBoundary())
		{
			EnemyPlaneList.Remove(this);
		}
	}

	public override void HitCheckAll()
	{
		for (int num = MyBulletList.Count - 1; num >= 0; num--)
		{
			if (HitCheck(MyBulletList[num]))
			{
				if (MyPlane.EnchantmentState == EnchantmentType.Red)
				{
					HealthPoint -= (float)MyBulletList[num].Damage * 1.25f;
				}
				else
				{
					HealthPoint -= MyBulletList[num].Damage;
				}
				MyBulletList[num].GiveEndEffect();
				MyBulletList.RemoveAt(num);
				MyPlane.Score += 10L;
				if (HealthPoint <= 0f)
				{
					GiveEndEffect();
					GiveItems();
					EnemyPlaneList.Remove(this);
					StageData.SoundPlay("se_enep00.wav", OriginalPosition.X / (float)BoundRect.Width);
					break;
				}
				StageData.SoundPlay("se_damage01.wav", OriginalPosition.X / (float)BoundRect.Width);
			}
		}
		if (MyPlane.HitEnabled && HitCheck(MyPlane))
		{
			MyPlane.PreMiss();
		}
	}

	public override void GiveEndEffect()
	{
	}

	public override void GiveItems()
	{
	}
}
