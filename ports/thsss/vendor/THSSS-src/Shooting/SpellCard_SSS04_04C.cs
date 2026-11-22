using System.Drawing;

namespace Shooting;

internal class SpellCard_SSS04_04C : BaseTimeSC
{
	public SpellCard_SSS04_04C(StageDataPackage StageData)
		: base(StageData)
	{
		if (Difficulty < DifficultLevel.Hard)
		{
			SC_Name = "秘术「遗忘之封印」";
		}
		else
		{
			SC_Name = "秘术「遗忘之封印」";
		}
		BaseScore = 25000000L;
		Boss.DestPoint = new Point(BoundRect.Width / 2, 100);
		Boss.Velocity = 4f;
		StageData.SoundPlay("se_cat00.wav");
		new BackgroundBoss04(StageData);
		new BulletTitle(StageData, SC_Name);
		new SpellCardAttack(StageData);
		new FullPic2(StageData, "FaceKage_ct");
	}

	public override void Shoot()
	{
		Boss.Armon = Boss.ArmonArray[5];
		Boss.HitEnabled = false;
		if (Time > 100)
		{
			Boss.MoveUpDown();
		}
		if (Time == 150)
		{
			string fileName = (int)Difficulty switch
			{
				0 => ".\\CS\\St04\\关底Boss\\4符E2.mbg", 
				1 => ".\\CS\\St04\\关底Boss\\4符N2.mbg", 
				2 => ".\\CS\\St04\\关底Boss\\4符H2.mbg", 
				3 => ".\\CS\\St04\\关底Boss\\4符L2.mbg", 
				_ => ".\\CS\\St04\\关底Boss\\4符U2.mbg", 
			};
			CS_Data cSData = StageData.LoadCS(fileName);
			new CSEmitterController(StageData, cSData);
		}
		if ((Time - 100) % 200 == 0)
		{
			Boss.OnAction = 150;
		}
	}
}
