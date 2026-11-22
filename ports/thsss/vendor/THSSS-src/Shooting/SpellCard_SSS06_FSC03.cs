using System.Drawing;

namespace Shooting;

internal class SpellCard_SSS06_FSC03 : BaseTimeSC
{
	public SpellCard_SSS06_FSC03(StageDataPackage StageData)
		: base(StageData)
	{
		SC_Name = "第三个愿望「永生不灭」";
		Boss.DestPoint = new Point(BoundRect.Width / 2, 120);
		Boss.Velocity = 4f;
		StageData.SoundPlay("se_cat00.wav");
		new BackgroundBoss06(StageData);
		new BulletTitle(StageData, SC_Name);
		new SpellCardAttack(StageData);
		new FullPic3(StageData, "FaceTensei_ct3");
	}

	public override void Shoot()
	{
		if (Time > 100)
		{
			Boss.MoveUpDown();
			Boss.Armon = Boss.ArmonArray[12];
		}
		if (Time == 100)
		{
			int difficulty = (int)Difficulty;
			CS_Data cSData = StageData.LoadCS(difficulty switch
			{
				2 => ".\\CS\\St06\\关底Boss\\FSC3H.mbg", 
				3 => ".\\CS\\St06\\关底Boss\\FSC3L.mbg", 
				_ => ".\\CS\\St06\\关底Boss\\FSC3L.mbg", 
			});
			new CSEmitterController(StageData, cSData);
		}
	}

	public override void GiveEndEffect()
	{
		if (!Bombed && !Missed && Time >= SpellTime)
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

	public override long GetSpellCardBonus()
	{
		if (Bombed || Missed)
		{
			return 0L;
		}
		return 99999990L;
	}
}
