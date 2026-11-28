using System.Drawing;

namespace Shooting;

internal class SpellCard_SSS05_03 : BaseSpellCard
{
	public SpellCard_SSS05_03(StageDataPackage StageData)
		: base(StageData)
	{
		if (Difficulty < DifficultLevel.Hard)
		{
			SC_Name = "混沌「守护的心意」";
		}
		else
		{
			SC_Name = "混沌「守护之心」";
		}
		BaseScore = 30000000L;
		Boss.DestPoint = new Point(BoundRect.Width / 2, 120);
		Boss.Velocity = 4f;
		StageData.SoundPlay("se_cat00.wav");
		new BackgroundBoss05(StageData);
		new BulletTitle(StageData, SC_Name);
		new SpellCardAttack(StageData);
		new FullPic2(StageData, "FaceRakukun_ct");
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
				0 => ".\\CS\\St05\\关底Boss\\3符E.mbg", 
				1 => ".\\CS\\St05\\关底Boss\\3符N.mbg", 
				2 => ".\\CS\\St05\\关底Boss\\3符H.mbg", 
				3 => ".\\CS\\St05\\关底Boss\\3符L.mbg", 
				_ => ".\\CS\\St05\\关底Boss\\3符L.mbg", 
			};
			CS_Data cSData = StageData.LoadCS(fileName);
			new CSEmitterController(StageData, cSData);
		}
	}
}
