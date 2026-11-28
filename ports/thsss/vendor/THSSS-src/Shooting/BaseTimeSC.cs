namespace Shooting;

internal class BaseTimeSC : BaseSpellCard
{
	public BaseTimeSC(StageDataPackage StageData)
		: base(StageData)
	{
		Boss.Armon = 1f;
		Boss.HitEnabled = false;
		Boss.TransparentValueF = 200f;
	}

	public override void GiveEndEffect()
	{
		if (!Bombed && !Missed)
		{
			StageData.SoundPlay("se_cardget.wav");
			new SpellCardBonus(StageData, GetSpellCardBonus());
			if (!StageData.OnReplay)
			{
				SC_Data.Recorded++;
			}
		}
		Background3D.Enabled3D = true;
		StageData.SCBstring = null;
		Boss.HitEnabled = true;
		Boss.TransparentValueF = 255f;
	}

	public override long GetSpellCardBonus()
	{
		if (Bombed || Missed)
		{
			return 0L;
		}
		return BaseScore + (long)Difficulty * 2000000L;
	}
}
