using System.Drawing;

namespace Shooting;

internal class SpellCard_SSS04_01 : BaseSpellCard
{
	public SpellCard_SSS04_01(StageDataPackage StageData)
		: base(StageData)
	{
		if (Difficulty < DifficultLevel.Hard)
		{
			SC_Name = "魔镜「幻影之境」";
		}
		else
		{
			SC_Name = "魔镜「虚无幽幻之境」";
		}
		BaseScore = 25000000L;
		Boss.DestPoint = new Point(BoundRect.Width / 2, 120);
		Boss.Velocity = 4f;
		StageData.SoundPlay("se_cat00.wav");
		new BackgroundBoss04(StageData);
		new BulletTitle(StageData, SC_Name);
		new SpellCardAttack(StageData);
		new FullPic2(StageData, "FaceKage_ct").Scale = 0.8f;
	}

	public override void Shoot()
	{
		Boss.Armon = Boss.ArmonArray[1];
		if (Time > 50)
		{
			Boss.MoveUpDown();
		}
		if (Time == 100)
		{
			string fileName = (int)Difficulty switch
			{
				0 => ".\\CS\\St04\\关底Boss\\1符E.mbg", 
				1 => ".\\CS\\St04\\关底Boss\\1符N.mbg", 
				2 => ".\\CS\\St04\\关底Boss\\1符H.mbg", 
				3 => ".\\CS\\St04\\关底Boss\\1符L.mbg", 
				_ => ".\\CS\\St04\\关底Boss\\1符U.mbg", 
			};
			CS_Data cSData = StageData.LoadCS(fileName);
			new CSEmitterController(StageData, cSData).BossBinding = true;
		}
	}
}
