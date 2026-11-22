using System.Drawing;

namespace Shooting;

internal class SpellCard_SSS06_05 : BaseTimeSC
{
	public SpellCard_SSS06_05(StageDataPackage StageData)
		: base(StageData)
	{
		if (Difficulty < DifficultLevel.Hard)
		{
			SC_Name = "「弹幕之海」";
		}
		else
		{
			SC_Name = "「弹幕之海」";
		}
		BaseScore = 35000000L;
		Boss.DestPoint = new Point(BoundRect.Width / 2, 120);
		Boss.Velocity = 4f;
		Boss.SpellTime = Boss.Time + 3000 - 30;
		StageData.SoundPlay("se_cat00.wav");
		new BackgroundBoss06(StageData);
		new BulletTitle(StageData, SC_Name);
		new SpellCardAttack(StageData);
		new FullPic3(StageData, "FaceTensei_ct3");
	}

	public override void Shoot()
	{
		Boss.Armon = Boss.ArmonArray[8];
		Boss.HitEnabled = false;
		if (Time > 100)
		{
			Boss.MoveUpDown();
		}
		if (Time == 150)
		{
			string fileName = (int)Difficulty switch
			{
				0 => ".\\CS\\St06\\关底Boss\\5符E.mbg", 
				1 => ".\\CS\\St06\\关底Boss\\5符N.mbg", 
				2 => ".\\CS\\St06\\关底Boss\\5符H.mbg", 
				3 => ".\\CS\\St06\\关底Boss\\5符L.mbg", 
				_ => ".\\CS\\St06\\关底Boss\\5符L.mbg", 
			};
			CS_Data cSData = StageData.LoadCS(fileName);
			new CSEmitterController(StageData, cSData);
		}
	}

	public override void GiveEndEffect()
	{
		base.GiveEndEffect();
		Boss.HitEnabled = true;
	}
}
