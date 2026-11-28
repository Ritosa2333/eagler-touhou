using System.Drawing;

namespace Shooting;

internal class SpellCard_SSS05_FSC : BaseFSC
{
	public SpellCard_SSS05_FSC(StageDataPackage StageData)
		: base(StageData)
	{
		if (Difficulty < DifficultLevel.Hard)
		{
			SC_Name = "烈焰「焚秽净世」";
		}
		else
		{
			SC_Name = "烈焰「焚秽净世」";
		}
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
		if (Time > 100)
		{
			Boss.MoveUpDown();
			Boss.Armon = Boss.ArmonArray[8];
		}
		if (Time == 100)
		{
			int difficulty = (int)Difficulty;
			CS_Data cSData = StageData.LoadCS(difficulty switch
			{
				2 => ".\\CS\\St05\\关底Boss\\FSC符H.mbg", 
				3 => ".\\CS\\St05\\关底Boss\\FSC符L.mbg", 
				_ => ".\\CS\\St05\\关底Boss\\FSC符L.mbg", 
			});
			new CSEmitterController(StageData, cSData);
		}
	}
}
