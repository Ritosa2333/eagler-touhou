using System.Drawing;

namespace Shooting;

internal class SpellCard_SSSEx_04B : BaseSpellCard
{
	public SpellCard_SSSEx_04B(StageDataPackage StageData)
		: base(StageData)
	{
		SC_Name = "荣符「生长与枯萎的循环」";
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
		Boss.Armon = Boss.ArmonArray[7];
		if (Time == 100)
		{
			string fileName = ".\\CS\\StEx\\关底Boss\\4符1.mbg";
			CS_Data cSData = StageData.LoadCS(fileName);
			new CSEmitterController(StageData, cSData);
		}
	}
}
