using System.Drawing;

namespace Shooting;

internal class SpellCard_SSS04_03A : BaseSpellCard
{
	public SpellCard_SSS04_03A(StageDataPackage StageData)
		: base(StageData)
	{
		if (Difficulty < DifficultLevel.Hard)
		{
			SC_Name = "阴阳「神鬼一击」";
		}
		else
		{
			SC_Name = "阴阳「神鬼一击」";
		}
		BaseScore = 25000000L;
		Boss.DestPoint = new Point(BoundRect.Width / 2, 96);
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
				0 => ".\\CS\\St04\\关底Boss\\3符E0.mbg", 
				1 => ".\\CS\\St04\\关底Boss\\3符N0.mbg", 
				2 => ".\\CS\\St04\\关底Boss\\3符H0.mbg", 
				3 => ".\\CS\\St04\\关底Boss\\3符L0.mbg", 
				_ => ".\\CS\\St04\\关底Boss\\3符U0.mbg", 
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
