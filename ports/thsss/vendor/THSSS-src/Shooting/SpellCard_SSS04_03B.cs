using System.Drawing;

namespace Shooting;

internal class SpellCard_SSS04_03B : BaseSpellCard
{
	public SpellCard_SSS04_03B(StageDataPackage StageData)
		: base(StageData)
	{
		if (Difficulty < DifficultLevel.Hard)
		{
			SC_Name = "流光「幻影魔炮」";
		}
		else
		{
			SC_Name = "流光「幻影魔炮」";
		}
		BaseScore = 25000000L;
		Boss.DestPoint = new Point(BoundRect.Width / 2, 128);
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
				0 => ".\\CS\\St04\\关底Boss\\3符E1.mbg", 
				1 => ".\\CS\\St04\\关底Boss\\3符N1.mbg", 
				2 => ".\\CS\\St04\\关底Boss\\3符H1.mbg", 
				3 => ".\\CS\\St04\\关底Boss\\3符L1.mbg", 
				_ => ".\\CS\\St04\\关底Boss\\3符U1.mbg", 
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
