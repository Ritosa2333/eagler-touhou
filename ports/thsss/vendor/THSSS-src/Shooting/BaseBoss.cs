namespace Shooting;

public class BaseBoss : BaseObject
{
	private int spellcardHP = 2000;

	private const int unmatchedTime = 100;

	private float armor;

	public int SpellcardHP
	{
		get
		{
			return spellcardHP;
		}
		set
		{
			spellcardHP = value;
		}
	}

	public int UnmatchedTime => 100;

	public int Life { get; set; }

	public int MaxHP { get; set; }

	public bool OnSpell { get; set; }

	public int SpellTime { get; set; }

	public float Armon
	{
		get
		{
			return armor;
		}
		set
		{
			if (value < 0f)
			{
				armor = 0f;
			}
			else if (value > 1f)
			{
				armor = 1f;
			}
			else
			{
				armor = value;
			}
		}
	}

	public override void HitCheckAll()
	{
		if (Life < 0 || !HitEnabled)
		{
			return;
		}
		if (MyPlane.HitEnabled && HitCheck(MyPlane))
		{
			MyPlane.PreMiss();
		}
		for (int num = MyBulletList.Count - 1; num >= 0; num--)
		{
			if (HitCheck(MyBulletList[num]))
			{
				if (Time > UnmatchedTime)
				{
					if (MyPlane.EnchantmentState == EnchantmentType.Red)
					{
						HealthPoint -= (float)MyBulletList[num].Damage * 1.25f * (1f - Armon);
					}
					else
					{
						HealthPoint -= (float)MyBulletList[num].Damage * (1f - Armon);
					}
				}
				MyBulletList[num].GiveEndEffect();
				MyBulletList.RemoveAt(num);
				MyPlane.Score += 10L;
				if (HealthPoint <= 200f)
				{
					StageData.SoundPlay("se_damage01.wav");
				}
			}
		}
	}
}
