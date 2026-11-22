using System.Drawing;

namespace Shooting;

internal class SpellCard_SSS04_05 : BaseTimeSC
{
	public SpellCard_SSS04_05(StageDataPackage StageData)
		: base(StageData)
	{
		if (Difficulty < DifficultLevel.Hard)
		{
			SC_Name = "虹奥义「光幕结界」";
		}
		else
		{
			SC_Name = "虹奥义「光幕结界」";
		}
		BaseScore = 25000000L;
		Boss.DestPoint = new Point(BoundRect.Width / 2, 120);
		Boss.Velocity = 4f;
		StageData.SoundPlay("se_cat00.wav");
		new BackgroundBoss04(StageData);
		new BulletTitle(StageData, SC_Name);
		new SpellCardAttack(StageData);
		new FullPic2(StageData, "FaceKage_ct");
		Boss.TransparentValueF = 0f;
	}

	public override void Shoot()
	{
		Boss.Armon = Boss.ArmonArray[6];
		Boss.HitEnabled = false;
		if (Time > 100)
		{
			Boss.MoveUpDown();
		}
		if (Time == 150)
		{
			string fileName = (int)Difficulty switch
			{
				0 => ".\\CS\\St04\\关底Boss\\5符E.mbg", 
				1 => ".\\CS\\St04\\关底Boss\\5符N.mbg", 
				2 => ".\\CS\\St04\\关底Boss\\5符H.mbg", 
				3 => ".\\CS\\St04\\关底Boss\\5符L.mbg", 
				_ => ".\\CS\\St04\\关底Boss\\5符U.mbg", 
			};
			CS_Data cSData = StageData.LoadCS(fileName);
			new CSEmitterController(StageData, cSData);
		}
	}
}
