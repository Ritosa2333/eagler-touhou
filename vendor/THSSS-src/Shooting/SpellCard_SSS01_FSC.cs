using System.Drawing;

namespace Shooting;

internal class SpellCard_SSS01_FSC : BaseFSC
{
	public SpellCard_SSS01_FSC(StageDataPackage StageData)
		: base(StageData)
	{
		if (Difficulty < DifficultLevel.Hard)
		{
			SC_Name = "夜风「轮回的幸运之星」";
		}
		else
		{
			SC_Name = "夜风「轮回的幸运之星」";
		}
		Boss.DestPoint = new Point(BoundRect.Width / 2, 120);
		Boss.Velocity = 4f;
		StageData.SoundPlay("se_cat00.wav");
		new BackgroundMove2(StageData, "Card01b").DirectionDegree = 270.0;
		new BackgroundMove2(StageData, "Card01a")
		{
			DirectionDegree = -80.0,
			Length = 1024,
			TransparentValueF = 80f
		};
		new BulletTitle(StageData, SC_Name);
		new SpellCardAttack(StageData);
		new FullPic2(StageData, "FaceAmi_ct");
	}

	public override void Shoot()
	{
		if (Time > 100)
		{
			Boss.MoveUpDown();
			Boss.Armon = 0f;
		}
		if (Time == 100)
		{
			Boss.OnAction = 10000;
			int difficulty = (int)Difficulty;
			CS_Data cSData = StageData.LoadCS(difficulty switch
			{
				2 => ".\\CS\\St01\\关底Boss\\FSC符H.mbg", 
				3 => ".\\CS\\St01\\关底Boss\\FSC符L.mbg", 
				_ => ".\\CS\\St01\\关底Boss\\FSC符L.mbg", 
			});
			new CSEmitterController(StageData, cSData);
		}
	}
}
