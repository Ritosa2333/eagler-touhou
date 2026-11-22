using System.Drawing;

namespace Shooting;

internal class SpellCard_SSS06_03 : BaseSpellCard
{
	public SpellCard_SSS06_03(StageDataPackage StageData)
		: base(StageData)
	{
		if (Difficulty < DifficultLevel.Hard)
		{
			SC_Name = "星雨「故乡之星」";
		}
		else
		{
			SC_Name = "星雨「故乡之星」";
		}
		BaseScore = 35000000L;
		Boss.DestPoint = new Point(BoundRect.Width / 2, 120);
		Boss.Velocity = 4f;
		StageData.SoundPlay("se_cat00.wav");
		new BackgroundBoss06(StageData);
		new BulletTitle(StageData, SC_Name);
		new SpellCardAttack(StageData);
		new FullPic3(StageData, "FaceTensei_ct2");
	}

	public override void Shoot()
	{
		Boss.Armon = Boss.ArmonArray[5];
		if (Time > 100)
		{
			Boss.MoveUpDown();
		}
		if (Time == 150)
		{
			string fileName = (int)Difficulty switch
			{
				0 => ".\\CS\\St06\\关底Boss\\3符E.mbg", 
				1 => ".\\CS\\St06\\关底Boss\\3符N.mbg", 
				2 => ".\\CS\\St06\\关底Boss\\3符H.mbg", 
				3 => ".\\CS\\St06\\关底Boss\\3符L.mbg", 
				_ => ".\\CS\\St06\\关底Boss\\3符L.mbg", 
			};
			CS_Data cSData = StageData.LoadCS(fileName);
			new CSEmitterController(StageData, cSData);
		}
	}
}
