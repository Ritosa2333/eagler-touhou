namespace Shooting;

internal class BaseSpellCard : BaseBulleEmitter
{
	public SpellCardHistory SC_Data;

	public bool Bombed { get; set; }

	public bool Missed { get; set; }

	public long BaseScore { get; set; }

	public int SpellTime { get; set; }

	public string SC_Name { get; set; }

	public BaseSpellCard(StageDataPackage StageData)
		: base(StageData)
	{
		if (Boss != null)
		{
			BaseScore = 10000000L;
			OriginalPosition = Boss.OriginalPosition;
			SpellTime = Boss.SpellTime - Boss.Time;
			Background3D.Enabled3D = false;
			SC_Name = null;
		}
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Time == 1)
		{
			SC_Data = StageData.PData.SC_History.Find(FindSC);
			if (!StageData.OnReplay)
			{
				SC_Data.History++;
			}
		}
		if (KClass.Key_X && MyPlane.Spell > 0)
		{
			Bombed = true;
		}
		if (MyPlane.Time <= 1)
		{
			Missed = true;
		}
		if (Time > 210 && SpellList.Count > 0)
		{
			Bombed = true;
		}
		if (GetSpellCardBonus() > 0)
		{
			StageData.SCBstring = GetSpellCardBonus().ToString().PadRight(14, ' ') + (SC_Data.Recorded + "/" + SC_Data.History).PadLeft(6);
		}
		else
		{
			StageData.SCBstring = "Failed".PadRight(14, ' ') + (SC_Data.Recorded + "/" + SC_Data.History).PadLeft(6);
		}
	}

	public override void GiveEndEffect()
	{
		if (!Bombed && !Missed && Time < SpellTime)
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
	}

	public virtual long GetSpellCardBonus()
	{
		if (Bombed || Missed)
		{
			return 0L;
		}
		return (BaseScore - BaseScore * Time / 2 / SpellTime) / 10 * 10;
	}

	public bool FindSC(SpellCardHistory SCH)
	{
		if (SCH.Rank == Difficulty && SCH.MyPlaneFullName == MyPlane.FullName)
		{
			return SCH.Name == SC_Name;
		}
		return false;
	}
}
