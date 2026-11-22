namespace Shooting;

public class BaseSpell : BaseObject
{
	public BaseSpell()
	{
	}

	public BaseSpell(StageDataPackage StageData)
	{
		base.StageData = StageData;
		Position = MyPlane.Position;
		Damage = 12;
		LifeTime = 300;
		SpellList.Add(this);
		StageData.VibrateStart(LifeTime);
		new BackgroundBlack(StageData, Position);
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Time < 30)
		{
			for (int num = ItemList.Count - 1; num >= 0; num--)
			{
				if (HitCheck(ItemList[num]) && !(ItemList[num] is ShootingStarShard))
				{
					ItemList[num].Obtain = true;
					ItemList[num].Doubled = true;
				}
			}
		}
		MyPlane.DeadTime = -1;
		if (OutBoundary())
		{
			SpellList.Remove(this);
			MyPlane.Time = MyPlane.UnmatchedTime - 60;
		}
	}

	public override bool OutBoundary()
	{
		if (Time > LifeTime)
		{
			return true;
		}
		return false;
	}

	public override void HitCheckAll()
	{
		for (int num = BulletList.Count - 1; num >= 0; num--)
		{
			if (HitCheck(BulletList[num]) && !BulletList[num].UnRemoveable)
			{
				BulletList[num].GiveEndEffect();
				BulletList[num].GiveItems();
				BulletList.RemoveAt(num);
			}
		}
		for (int num2 = EnemyPlaneList.Count - 1; num2 >= 0; num2--)
		{
			if (HitCheck(EnemyPlaneList[num2]))
			{
				EnemyPlaneList[num2].HealthPoint -= Damage;
				if (EnemyPlaneList[num2].HealthPoint <= 0f)
				{
					EnemyPlaneList[num2].GiveEndEffect();
					EnemyPlaneList[num2].GiveItems();
					EnemyPlaneList.RemoveAt(num2);
					StageData.SoundPlay("se_enep00.wav");
					break;
				}
			}
		}
		if (Boss != null && Boss.Time > Boss.UnmatchedTime && HitCheck(Boss))
		{
			Boss.HealthPoint -= (int)((float)Damage * (1f - Boss.Armon));
			if (Boss.HealthPoint <= 200f)
			{
				StageData.SoundPlay("se_damage01.wav");
			}
		}
	}
}
