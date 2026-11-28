using System.Drawing;

namespace Shooting;

internal class SpellCard_SSS06_02 : BaseSpellCard
{
	public SpellCard_SSS06_02(StageDataPackage StageData)
		: base(StageData)
	{
		if (Difficulty < DifficultLevel.Hard)
		{
			SC_Name = "耀星「白夜永昼之光」";
		}
		else
		{
			SC_Name = "耀星「白夜永昼之光」";
		}
		BaseScore = 35000000L;
		Boss.DestPoint = new Point(BoundRect.Width / 2, 120);
		Boss.Velocity = 4f;
		StageData.SoundPlay("se_cat00.wav");
		new BackgroundBoss06(StageData);
		new BulletTitle(StageData, SC_Name);
		new SpellCardAttack(StageData);
		new FullPic3(StageData, "FaceTensei_ct1");
	}

	public override void Shoot()
	{
		Boss.Armon = Boss.ArmonArray[3];
		if (Time > 100)
		{
			Boss.MoveUpDown();
		}
		if (Time == 150)
		{
			string fileName = (int)Difficulty switch
			{
				0 => ".\\CS\\St06\\关底Boss\\2符E.mbg", 
				1 => ".\\CS\\St06\\关底Boss\\2符N.mbg", 
				2 => ".\\CS\\St06\\关底Boss\\2符H.mbg", 
				3 => ".\\CS\\St06\\关底Boss\\2符L.mbg", 
				_ => ".\\CS\\St06\\关底Boss\\2符L.mbg", 
			};
			CS_Data cSData = StageData.LoadCS(fileName);
			new CSEmitterController(StageData, cSData);
		}
	}
}
