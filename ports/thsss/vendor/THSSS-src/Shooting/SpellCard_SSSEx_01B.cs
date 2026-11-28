using System.Drawing;

namespace Shooting;

internal class SpellCard_SSSEx_01B : BaseSpellCard
{
	public SpellCard_SSSEx_01B(StageDataPackage StageData)
		: base(StageData)
	{
		SC_Name = "绽放「爱意的玫瑰」";
		BaseScore = 40000000L;
		Boss.DestPoint = new Point(BoundRect.Width / 2, 160);
		Boss.Velocity = 4f;
		StageData.SoundPlay("se_cat00.wav");
		new BulletTitle(StageData, SC_Name);
		new SpellCardAttack(StageData);
		new FullPic2(StageData, "FaceRika_ct1");
		new BackgroundBossEx(StageData);
	}

	public override void Shoot()
	{
		Boss.Armon = Boss.ArmonArray[1];
		if (Time == 100)
		{
			string fileName = ".\\CS\\StEx\\关底Boss\\1符1.mbg";
			CS_Data cSData = StageData.LoadCS(fileName);
			new CSEmitterController(StageData, cSData);
		}
	}
}
