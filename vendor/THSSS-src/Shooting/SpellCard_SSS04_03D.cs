using System.Drawing;

namespace Shooting;

internal class SpellCard_SSS04_03D : BaseSpellCard
{
	public SpellCard_SSS04_03D(StageDataPackage StageData)
		: base(StageData)
	{
		if (Difficulty < DifficultLevel.Hard)
		{
			SC_Name = "空想「恋之宣告」";
		}
		else
		{
			SC_Name = "空想「恋之宣告」";
		}
		BaseScore = 25000000L;
		Boss.DestPoint = new Point(BoundRect.Width / 2, 142);
		Boss.Velocity = 4f;
		StageData.SoundPlay("se_cat00.wav");
		new BackgroundBoss04(StageData);
		new BulletTitle(StageData, SC_Name);
		new SpellCardAttack(StageData);
		new FullPic2(StageData, "FaceKage_ct");
	}

	public override void Shoot()
	{
		Boss.Armon = Boss.ArmonArray[4];
		if (Time > 100)
		{
			Boss.MoveUpDown();
		}
		if (Time == 150)
		{
			string fileName = (int)Difficulty switch
			{
				0 => ".\\CS\\St04\\关底Boss\\3符E3.mbg", 
				1 => ".\\CS\\St04\\关底Boss\\3符N3.mbg", 
				2 => ".\\CS\\St04\\关底Boss\\3符H3.mbg", 
				3 => ".\\CS\\St04\\关底Boss\\3符L3.mbg", 
				_ => ".\\CS\\St04\\关底Boss\\3符U3.mbg", 
			};
			CS_Data cSData = StageData.LoadCS(fileName);
			new CSEmitterController(StageData, cSData);
		}
		if ((Time - 160) % 363 == 0)
		{
			Boss.OnAction = 250;
		}
	}
}
