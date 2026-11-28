using System.Drawing;

namespace Shooting;

internal class SpellCard_SSS06_FSC02 : BaseFSC
{
	public SpellCard_SSS06_FSC02(StageDataPackage StageData)
		: base(StageData)
	{
		SC_Name = "第二个愿望「无穷之力」";
		Boss.DestPoint = new Point(BoundRect.Width / 2, 120);
		Boss.Velocity = 4f;
		StageData.SoundPlay("se_cat00.wav");
		new BackgroundBoss06(StageData);
		new BulletTitle(StageData, SC_Name);
		new SpellCardAttack(StageData);
		new FullPic3(StageData, "FaceTensei_ct3");
	}

	public override void Shoot()
	{
		if (Time > 100)
		{
			Boss.MoveUpDown();
			Boss.Armon = Boss.ArmonArray[11];
		}
		if (Time == 100)
		{
			int difficulty = (int)Difficulty;
			CS_Data cSData = StageData.LoadCS(difficulty switch
			{
				2 => ".\\CS\\St06\\关底Boss\\FSC2H.mbg", 
				3 => ".\\CS\\St06\\关底Boss\\FSC2L.mbg", 
				_ => ".\\CS\\St06\\关底Boss\\FSC2L.mbg", 
			});
			new CSEmitterController(StageData, cSData);
		}
	}
}
