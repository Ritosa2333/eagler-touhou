using System.Drawing;

namespace Shooting;

internal class SpellCard_SSS04_FSC : BaseFSC
{
	public SpellCard_SSS04_FSC(StageDataPackage StageData)
		: base(StageData)
	{
		SC_Name = "境界「三原色」";
		Boss.DestPoint = new Point(BoundRect.Width / 2, 120);
		Boss.Velocity = 4f;
		StageData.SoundPlay("se_cat00.wav");
		new BackgroundBoss04(StageData);
		new BulletTitle(StageData, SC_Name);
		new SpellCardAttack(StageData);
		new FullPic2(StageData, "FaceKage_ct");
	}

	public override void Shoot()
	{
		if (Time > 100)
		{
			Boss.MoveUpDown();
			Boss.Armon = Boss.ArmonArray[7];
		}
		if (Time == 100)
		{
			int difficulty = (int)Difficulty;
			CS_Data cSData = StageData.LoadCS(difficulty switch
			{
				2 => ".\\CS\\St04\\关底Boss\\FSC符H.mbg", 
				3 => ".\\CS\\St04\\关底Boss\\FSC符L.mbg", 
				_ => ".\\CS\\St04\\关底Boss\\FSC符L.mbg", 
			});
			new CSEmitterController(StageData, cSData);
		}
	}
}
