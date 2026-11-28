using System.Drawing;

namespace Shooting;

internal class SpellCard_SSS05_02 : BaseSpellCard
{
	public SpellCard_SSS05_02(StageDataPackage StageData)
		: base(StageData)
	{
		if (Difficulty < DifficultLevel.Hard)
		{
			SC_Name = "机关「火焰发射装置」";
		}
		else
		{
			SC_Name = "焰机「星夜的火焰舞曲」";
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
		Boss.Armon = Boss.ArmonArray[3];
		if (Time > 100)
		{
			Boss.MoveUpDown();
		}
		if (Time == 150)
		{
			string fileName = (int)Difficulty switch
			{
				0 => ".\\CS\\St05\\关底Boss\\2符E.mbg", 
				1 => ".\\CS\\St05\\关底Boss\\2符N.mbg", 
				2 => ".\\CS\\St05\\关底Boss\\2符H.mbg", 
				3 => ".\\CS\\St05\\关底Boss\\2符L.mbg", 
				_ => ".\\CS\\St05\\关底Boss\\2符L.mbg", 
			};
			CS_Data cSData = StageData.LoadCS(fileName);
			new CSEmitterController(StageData, cSData);
		}
	}
}
