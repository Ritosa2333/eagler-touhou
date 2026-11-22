using System.Drawing;

namespace Shooting;

internal class SpellCard_SSS01_01 : BaseSpellCard
{
	public SpellCard_SSS01_01(StageDataPackage StageData)
		: base(StageData)
	{
		if (Difficulty < DifficultLevel.Hard)
		{
			SC_Name = "";
		}
		else
		{
			SC_Name = "微风「摇曳的三叶草」";
		}
		BaseScore = 10000000L;
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
		Boss.Armon = 0.7f;
		if (Time > 100)
		{
			Boss.MoveUpDown();
		}
		if (Time == 150)
		{
			Boss.OnAction = 10000;
			string fileName = (int)Difficulty switch
			{
				0 => "", 
				1 => "", 
				2 => ".\\CS\\St01\\道中Boss\\1符H.mbg", 
				3 => ".\\CS\\St01\\道中Boss\\1符L.mbg", 
				_ => ".\\CS\\St01\\道中Boss\\1符L.mbg", 
			};
			CS_Data cSData = StageData.LoadCS(fileName);
			new CSEmitterController(StageData, cSData);
		}
	}
}
