using System.Drawing;

namespace Shooting;

internal class SpellCard_SSS04_02B : BaseSpellCard
{
	public SpellCard_SSS04_02B(StageDataPackage StageData)
		: base(StageData)
	{
		if (Difficulty < DifficultLevel.Hard)
		{
			SC_Name = "镜符「山吹色波纹裂隙」";
		}
		else
		{
			SC_Name = "镜符「山吹色波纹裂隙」";
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
		Boss.Armon = Boss.ArmonArray[3];
		if (Time > 100)
		{
			Boss.RandomMove(205, 20, 2f, new Rectangle(BoundRect.Width / 2 - 45, 100, 90, 40));
		}
		if (Time == 150)
		{
			Boss.OnAction = 10000;
			string fileName = (int)Difficulty switch
			{
				0 => ".\\CS\\St04\\关底Boss\\2符E1.mbg", 
				1 => ".\\CS\\St04\\关底Boss\\2符N1.mbg", 
				2 => ".\\CS\\St04\\关底Boss\\2符H1.mbg", 
				3 => ".\\CS\\St04\\关底Boss\\2符L1.mbg", 
				_ => ".\\CS\\St04\\关底Boss\\2符U1.mbg", 
			};
			CS_Data cSData = StageData.LoadCS(fileName);
			new CSEmitterController(StageData, cSData);
		}
	}
}
